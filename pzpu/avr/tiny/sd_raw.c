/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 *
 * Ported for tiny MCU from Roland Riegel's original code.
 * Copyright (c) 2006-2012 by Roland Riegel <feedback@roland-riegel.de>
 */


#include "sd_raw.h"
#include "avr_config.h"
#include "soft_spi.h"
#include <util/delay.h>

#define sd_raw_send_byte(X) spi(X)
#define sd_raw_rec_byte() spi(0xFF)
#define sd_raw_hispeed_on() GTCCR = (1<<PWM1B) | (1<<COM1B1)
#define sd_raw_hispeed_off() GTCCR = 0

/* card type state */
static uint8_t sd_raw_card_type;

static uint8_t sd_raw_send_command(uint8_t command, uint32_t arg)
{
    uint8_t response;

    /* wait some clock cycles */
    sd_raw_rec_byte();

    /* send command via SPI */
    sd_raw_send_byte(0x40 | command);
    sd_raw_send_byte((arg >> 24) & 0xff);
    sd_raw_send_byte((arg >> 16) & 0xff);
    sd_raw_send_byte((arg >> 8) & 0xff);
    sd_raw_send_byte((arg >> 0) & 0xff);
    switch(command)
    {
        case CMD_GO_IDLE_STATE:
           sd_raw_send_byte(0x95);
           break;
        case CMD_SEND_IF_COND:
           sd_raw_send_byte(0x87);
           break;
        default:
           sd_raw_send_byte(0xff);
           break;
    }

    /* receive response */
    for(uint8_t i = 0; i < 10; ++i)
    {
        response = sd_raw_rec_byte();
        if(response != 0xff)
            break;
    }

    return response;
}

uint8_t sd_raw_init()
{
    SPI_PERIPH;

    /* initialization procedure */
    sd_raw_card_type = 0;

    /* address card */
    SPI_CARD;

    /* reset card */
    uint8_t response;
    for(uint16_t i = 0; ; ++i)
    {
        response = sd_raw_send_command(CMD_GO_IDLE_STATE, 0);
        if(response == (1 << R1_IDLE_STATE))
            break;

        if(i == 0x1ff)
        {
            SPI_PERIPH;
            return 0;
        }
    }

    /* check for version of SD card specification */
    response = sd_raw_send_command(CMD_SEND_IF_COND, 0x100 /* 2.7V - 3.6V */ | 0xaa /* test pattern */);
    if((response & (1 << R1_ILL_COMMAND)) == 0)
    {
        sd_raw_rec_byte();
        sd_raw_rec_byte();
        if((sd_raw_rec_byte() & 0x01) == 0)
            return 0; /* card operation voltage range doesn't match */
        if(sd_raw_rec_byte() != 0xaa)
            return 0; /* wrong test pattern */

        /* card conforms to SD 2 card specification */
        sd_raw_card_type |= (1 << SD_RAW_SPEC_2);
    } else {
        /* determine SD/MMC card type */
        sd_raw_send_command(CMD_APP, 0);
        response = sd_raw_send_command(CMD_SD_SEND_OP_COND, 0);
        if((response & (1 << R1_ILL_COMMAND)) == 0)
        {
            /* card conforms to SD 1 card specification */
            sd_raw_card_type |= (1 << SD_RAW_SPEC_1);
        }
        else
        {
            /* MMC card */
        }
    }

    /* wait for card to get ready */
    for(uint16_t i = 0; ; ++i)
    {
        if(sd_raw_card_type & ((1 << SD_RAW_SPEC_1) | (1 << SD_RAW_SPEC_2)))
        {
            uint32_t arg = 0;

            if(sd_raw_card_type & (1 << SD_RAW_SPEC_2))
                arg = 0x40000000;

            sd_raw_send_command(CMD_APP, 0);
            response = sd_raw_send_command(CMD_SD_SEND_OP_COND, arg);
        }
        else
        {
            response = sd_raw_send_command(CMD_SEND_OP_COND, 0);
        }

        if((response & (1 << R1_IDLE_STATE)) == 0)
            break;

        if(i == 0x7fff)
        {
            SPI_PERIPH;
            return 0;
        }
    }

    if(sd_raw_card_type & (1 << SD_RAW_SPEC_2))
    {
        if(sd_raw_send_command(CMD_READ_OCR, 0))
        {
            SPI_PERIPH;
            return 0;
        }

        if(sd_raw_rec_byte() & 0x40)
            sd_raw_card_type |= (1 << SD_RAW_SPEC_SDHC);

        sd_raw_rec_byte();
        sd_raw_rec_byte();
        sd_raw_rec_byte();
    }

    /* set block size to 512 bytes */
    if(sd_raw_send_command(CMD_SET_BLOCKLEN, 512))
    {
        SPI_PERIPH;
        return 0;
    }

    /* deaddress card */
    SPI_PERIPH;

    /* prepare high-speed signal generator */
    while (!(PLLCSR & (1<<PLOCK))) ; //wait for PLL to sync (must be set already, as the PLL is used for system clock, though)
    PLLCSR |= (1<<PCKE); //enable High-Speed clock
#if PIN_SCK == PB4
    TCCR1 = (1<<CS10); //PLL input clock
    OCR1B = 1; //duty (will be 33%, but I can't do anything with it)
    OCR1C = 2; //period
#else
#error "Please configure Timer1 to match pinout!"
#endif

    return 1;
}

uint8_t sd_raw_read(const uint64_t offset, uint8_t* buffer, uint16_t length)
{
    uint32_t blk = (offset / 4);
	uint16_t n = 0;
	uint8_t rem = offset & 3; //reminder needed for byte-oriented unaligned accesses

	if ((length > 1) && (length % 4)) return 0; //misaligned access

	while (length) {
		/* address card */
		SPI_CARD;

		/* send single block request */
		if(sd_raw_send_command(CMD_READ_SINGLE_BLOCK, (sd_raw_card_type & (1 << SD_RAW_SPEC_SDHC) ? blk : blk * 512)))
		{
			SPI_PERIPH;
			return 0;
		}

		/* wait for data block (start byte 0xfe) */
		while(sd_raw_rec_byte() != 0xfe);

		/* read byte block */
		for (uint16_t i = 0, j = 0; i < 512; i++) {
			uint8_t b = sd_raw_rec_byte();

			if ((i >= img_blk_offset) && (j < 4) && length) {
				if (rem) rem--;
				else {
					buffer[n++] = b;
					length--;
				}
				j++;
			}

			if (j == 4) {
				sd_raw_hispeed_on();
				SD_RAW_HISPEED_WAIT;
				sd_raw_hispeed_off();
				break;
			}
		}

		/* deaddress card */
		SPI_PERIPH;

		/* let card some time to finish */
		SPI_PREAD;
		SPI_PREAD;
//		SPI_PREAD;
//		SPI_PREAD;

		/* shift block */
		blk++;
	}

    return 1;
}
#if 0
uint8_t sd_raw_read(const uint64_t offset, uint8_t* buffer, uint16_t length)
{
    uint32_t blk = (offset / 4);
	uint16_t n = 0;
	uint8_t rem = offset & 3; //reminder needed for byte-oriented unaligned accesses

	if ((length > 1) && (length % 4)) return 0; //misaligned access

	while (length) {
		/* address card */
		SPI_CARD;

		/* send single block request */
		if(sd_raw_send_command(CMD_READ_SINGLE_BLOCK, (sd_raw_card_type & (1 << SD_RAW_SPEC_SDHC) ? blk : blk * 512)))
		{
			SPI_PERIPH;
			return 0;
		}

		/* wait for data block (start byte 0xfe) */
		while(sd_raw_rec_byte() != 0xfe);

		/* read byte block */
		for (uint16_t i = 0, j = 0; i < 512; i++) {
			uint8_t b = sd_raw_rec_byte();

			if ((i >= img_blk_offset) && (j < 4) && length) {
				if (rem) rem--;
				else {
					buffer[n++] = b;
					length--;
				}
				j++;
			}
		}

		/* read crc16 */
		sd_raw_rec_byte();
		sd_raw_rec_byte();

		/* deaddress card */
		SPI_PERIPH;

		/* let card some time to finish */
		SPI_PREAD;

		/* shift block */
		blk++;
	}

    return 1;
}
#endif
uint8_t sd_raw_write(const uint64_t offset, const uint8_t* buffer, uint16_t length)
{
	uint32_t blk = (offset / 4);
	uint16_t n = 0;

	if ((length > 1) && (length % 4)) return 0; //misaligned access

	while (length) {
		/* address card */
		SPI_CARD;

		/* send single block request */
		if(sd_raw_send_command(CMD_WRITE_SINGLE_BLOCK, (sd_raw_card_type & (1 << SD_RAW_SPEC_SDHC) ? blk : blk * 512)))
		{
			SPI_PERIPH;
			return 0;
		}

		/* send start byte */
        sd_raw_send_byte(0xfe);

		/* read byte block */
		for (uint16_t i = 0, j = 0; i < 512; i++) {
			uint8_t b = 0xFF; //empty Flash cell state
			if ((i >= img_blk_offset) && (j < 4) && length) {
				b = buffer[n++];
				j++;
				length--;
			}
			sd_raw_send_byte(b);
		}

		/* write dummy crc16 */
        sd_raw_send_byte(0xff);
        sd_raw_send_byte(0xff);

        /* wait while card is busy */
        while(sd_raw_rec_byte() != 0xff);
        sd_raw_rec_byte();

        /* deaddress card */
        SPI_PERIPH;

		/* shift block */
		blk++;
	}

    return 1;
}

inline uint8_t sd_raw_sync()
{
	return 1; //always successful
}
