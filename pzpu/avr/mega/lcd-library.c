/*
 * lcd_library.c
 *
 * Created: 02.02.2012 14:25:03
 *  Author: Дядя Женя
 *
 *	Библиотека для работы с жидкокристаллическим дисплеем на основе контрол-
 *	лера Hitachi HD44780 или аналогичным.
 *
 *	!!!Вся документация и настройки находятся в lcd-library.h!!!
 *
 *
 */ 
#include "lcd-library.h"

#ifdef LCD_USE_FLASH_FUNCTIONS
	#include <avr/pgmspace.h>
#endif

#ifdef LCD_USE_EEPROM_FUNCTIONS
	#include <avr/eeprom.h>
#endif

#define LCD_STROBDOWN		0
#define LCD_STROBUP			1

void lcdSendNibble(char byte, char state)
/*
	Отправляет младшую половину байта byte в LCD. Если state == 0,
	то передаётся как команда, если нет, то как данные.
*/
{
	LCDCONTROLDDR |=	(1<<LCD_RS)|
						(1<<LCD_RW)|
						(1<<LCD_E);			// Пины управления - на выход
	LCDDATADDR |=		(1<<LCD_D4)|
						(1<<LCD_D5)|
						(1<<LCD_D6)|
						(1<<LCD_D7);		// Пины данных - на выход	
	
	LCDCONTROLPORT &=	~(1<<LCD_RW);		// Режим записи, RW = 0					
	
	if (state)								// Устанавливаем 1 в RS
		LCDCONTROLPORT |=(1<<LCD_RS);		// если отдаём данные
	else
		LCDCONTROLPORT &= ~(1<<LCD_RS);
		
	LCDCONTROLPORT |= (1<<LCD_E);			// Взводим строб
		
	LCDDATAPORT &= ~((1<<LCD_D4)|
					(1<<LCD_D5)|
					(1<<LCD_D6)|
					(1<<LCD_D7));			// Обнуляем пины данных
	
	if (byte & (1<<3))						// Записываем младшую
		LCDDATAPORT |= (1<<LCD_D7);			// половину байта
											// byte в порт вывода данных
	if (byte & (1<<2))
		LCDDATAPORT |= (1<<LCD_D6);

	if (byte & (1<<1))
		LCDDATAPORT |= (1<<LCD_D5);

 	if (byte & (1<<0))
		LCDDATAPORT |= (1<<LCD_D4);
	
	_delay_us(LCD_STROBEDELAY_US);			// Пауза
	
	LCDCONTROLPORT &= ~(1<<LCD_E);			// Опускаем строб. Полубайт ушёл

}

char lcdGetNibble(char state)
/*
	Читает половину байта из LCD. Если state == 0, то читается команда,
	если нет, то данные.
*/
{
	char temp = 0;
	LCDCONTROLDDR |=	((1<<LCD_RS)|
						(1<<LCD_RW)|
						(1<<LCD_E));		// Пины управления - на выход
	LCDCONTROLPORT |=	(1<<LCD_RW);		// Режим чтения
	if (state)								// Устанавливаем 1 в RS
		LCDCONTROLPORT |=(1<<LCD_RS);		// если получаем данные
	else
		LCDCONTROLPORT &= ~(1<<LCD_RS);

	LCDCONTROLPORT |=	(1<<LCD_E);			// Взводим строб

	LCDDATADDR &= ~((1<<LCD_D4)|
					(1<<LCD_D5)|
					(1<<LCD_D6)|
					(1<<LCD_D7));			// Пины данных - на вход	
	LCDDATAPORT |= ((1<<LCD_D4)|
					(1<<LCD_D5)|
					(1<<LCD_D6)|
					(1<<LCD_D7));			// с подтяжкой
				
	_delay_us(LCD_STROBEDELAY_US);			// Пауза	

	LCDCONTROLPORT &= ~(1<<LCD_E);			// Опускаем строб
		
	if (LCDDATAPIN & (1<<LCD_D7))			// Читаем пины
		temp |=(1<<3);						// во временную переменную	
		
	if (LCDDATAPIN & (1<<LCD_D6))
		temp |= (1<<2);

	if (LCDDATAPIN & (1<<LCD_D5))
		temp |= (1<<1);
		
 	if (LCDDATAPIN & (1<<LCD_D4))
		temp |= (1<<0);
	
	return temp;							// возвращаем прочитанное

}


char lcdRawGetByte(char state)


{
	char temp = 0;
	
	temp |= lcdGetNibble(state);
	temp = temp<<4;					
	temp |= lcdGetNibble(state);
	
	return temp;
}



void lcdRawSendByte(char byte, char state)
/*
	Отравляет байт в LCD. Если state == 0, то передаётся как команда,
	если нет, то как данные.
*/
{
	lcdSendNibble((byte>>4), state);					
	lcdSendNibble(byte,state);
}


char lcdIsBusy(void)
/*
	Читает состояние LCD, возвращает 0xff, если флаг занятости установлен,
	и 0x00, если нет.
*/
{
	if (lcdRawGetByte(LCD_COMMAND) & (1<<7))
		return 0xff;
	else
		return 0x00;
}


void lcdInit(void)
/*
	Выполняет начальную инициализацию дисплея. Четырёхбитный режим. 
*/
{
	while (lcdIsBusy()) ;	
	lcdSendNibble(0b0010, LCD_COMMAND);
	while (lcdIsBusy()) ;
	lcdRawSendByte(0b00101000, LCD_COMMAND); 
	while (lcdIsBusy()) ;
	lcdRawSendByte(0b00000001, LCD_COMMAND);
	while (lcdIsBusy()) ;
	lcdRawSendByte(0b00000110, LCD_COMMAND);
	while (lcdIsBusy()) ;
	lcdRawSendByte(0b00001100, LCD_COMMAND);
}

////////////////////////////////////////////////////////////////////////////////////

void lcdInitGraphMode(void)
/*
	������������� � ����������� ������
*/
{
	//_delay_ms(500); // ���� ��� ������

	while (lcdIsBusy()) ;	
	lcdSendNibble(0b0010, LCD_COMMAND);
	while (lcdIsBusy()) ;
	lcdRawSendByte(0b00101000, LCD_COMMAND); //Function SET [0,0,1,DL,N,F,FT1,FT0]
	while (lcdIsBusy()) ;
	lcdRawSendByte(0b00001100, LCD_COMMAND); //Display ON/OFF [0,0,0,0,1,D,C,B]
	while (lcdIsBusy()) ;
	lcdRawSendByte(0b00000001, LCD_COMMAND); //Clear Display
	while (lcdIsBusy()) ;
	lcdRawSendByte(0b00000110, LCD_COMMAND); //Entry Mode Set [0,0,0,0,0,1,ID,S]
	while (lcdIsBusy()) ;
	lcdRawSendByte(0b00011011, LCD_COMMAND); //Mode/POW [0,0,0,1,GC,PWR,1,1]
}

void lcdSetXYbyte (char Byte, char X, char Y)
/*
	������������ ����� � ������������ � � Y � ����������� ������
*/
{
	unsigned char AddDDRAM, AddCGRAM;
	Byte=0;
	AddDDRAM=0;
	AddCGRAM=0;

	AddDDRAM=0b10000000+X;
	
	if (Y>=1) {
				AddCGRAM=0b01000001;
			 }
	else 	{
				AddCGRAM=0b01000000;
			}

	while (lcdIsBusy())	;
	lcdRawSendByte(AddCGRAM, LCD_COMMAND);	//������������� ����� ������� ��� ������ ������
	 
	while (lcdIsBusy())	;
	lcdRawSendByte(AddDDRAM, LCD_COMMAND); //������������� ����� ������� � ������� ��� ������ ������

	while (lcdIsBusy())	;
	lcdRawSendByte(Byte, LCD_DATA);
}

void lcdClearXYbyte(char Byte, char X, char Y)
/*
	������������ ���� � ������������ � � Y � ����������� ������
*/
{
	char AddDDRAM, AddCGRAM;
	Byte=0;
	AddDDRAM=0;
	AddCGRAM=0;

	AddDDRAM=0b10000000+X;
	
	if (Y>=1) {
				AddCGRAM=0b01000001;
			 }
	else 	{
				AddCGRAM=0b01000000;
			}

	while (lcdIsBusy())	;
	lcdRawSendByte(AddCGRAM, LCD_COMMAND);	
	 
	while (lcdIsBusy())	;
	lcdRawSendByte(AddDDRAM, LCD_COMMAND);

	while (lcdIsBusy())	;
	lcdRawSendByte(Byte, LCD_DATA);
}
////////////////////////////////////////////////////////////////////////////////////////

void lcdSetCursor(char cursor)
/*
	Устанавливает режим курсора: 0 - выключен, 2 - включен, 3 - моргает.
	Если на момент запуска LCD был выключен (lcdSetDisplay), то он будет включен.
*/
{
	while (lcdIsBusy())	;
	
	lcdRawSendByte((0b00001100 | cursor), LCD_COMMAND);
}

void lcdSetDisplay(char state)
/*
	Включает или выключает отображение символов LCD.
	При каждом вызове выключает курсор.
*/
{
	while (lcdIsBusy())	;
	
	lcdRawSendByte((0b00001000 | state), LCD_COMMAND);
	
}


void lcdClear(void)
/*
	Очищает LCD.
*/
{
	while (lcdIsBusy()) ;
	
	lcdRawSendByte(0b00000001, LCD_COMMAND);
}

void lcdGotoXY(char str, char col)
/*
	Устанавливает курсор в заданную позицию.
*/
{
	while (lcdIsBusy())	;
	
	lcdRawSendByte((0b10000000|((0x40*str)+col)), LCD_COMMAND);
}

void lcdDisplayScroll(char pos, char dir)
/*
	Сдвигает область отображения на указанное количество символов
	вправо или влево.
*/
{
	while (pos){
		while (lcdIsBusy()) ;
	
		lcdRawSendByte((0b00011000|dir), LCD_COMMAND);
		pos--;
	}	
}

#ifdef LCD_USE_RAM_FUNCTIONS
void lcdPuts(char *str)
/*
	Выводит строку из RAM в позицию курсора.
*/
{
	while (*str){
		while (lcdIsBusy()) ;
		
		lcdRawSendByte(*str++, LCD_DATA);
	}
}
#endif

#ifdef LCD_USE_FLASH_FUNCTIONS
void lcdPutsf(char *str)
/*
	Выводит строку из flash в позицию курсора.
*/
{
	while (pgm_read_byte(str)){
		while (lcdIsBusy()) ;
		
		lcdRawSendByte(pgm_read_byte(str++), LCD_DATA);
	}
}
#endif

#ifdef LCD_USE_EEPROM_FUNCTIONS
void lcdPutse(uint8_t *str)
/*
	Выводит строку из eeprom в позицию курсора.
*/
{
	while (eeprom_read_byte(str)){
		while (lcdIsBusy()) ;
		
		lcdRawSendByte((char)(eeprom_read_byte(str++)), LCD_DATA);
	}
}
#endif


#ifdef LCD_USE_CUSTOM_CHARACTER_FUNCTIONS

#ifdef LCD_USE_RAM_FUNCTIONS
void lcdLoadCharacter(char code, char *pattern)
/*
	Загружает символ в знакогенератор.
*/
{
	while (lcdIsBusy()) ;
	
	lcdRawSendByte((code<<3)|0b01000000, LCD_COMMAND);
	
	for (char i = 0; i <= 7; i++){
		while (lcdIsBusy()) ;
				
		lcdRawSendByte(*pattern++, LCD_DATA);
	}
	while (lcdIsBusy()) ;
	lcdRawSendByte(0b10000000, LCD_COMMAND);
}
#endif

#ifdef LCD_USE_FLASH_FUNCTIONS
void lcdLoadCharacterf(char code, char *pattern)
/*
	Загружает символ из flash в знакогенератор.
*/
{
	while (lcdIsBusy()) ;
	
	lcdRawSendByte((code<<3)|0b01000000, LCD_COMMAND);
	
	for (char i = 0; i <= 7; i++){
		while (lcdIsBusy()) ;
				
		lcdRawSendByte(pgm_read_byte(pattern++), LCD_DATA);
	}
	while (lcdIsBusy()) ;
	lcdRawSendByte(0b10000000, LCD_COMMAND);
}
#endif

#ifdef LCD_USE_EEPROM_FUNCTIONS
void lcdLoadCharactere(char code, char *pattern)
/*
	Загружает символ из eeprom в знакогенератор.
*/
{
	while (lcdIsBusy()) ;
	
	lcdRawSendByte((code<<3)|0b01000000, LCD_COMMAND);
	
	for (char i = 0; i <= 7; i++){
		while (lcdIsBusy()) ;
				
		lcdRawSendByte(eeprom_read_byte(pattern++), LCD_DATA);
	}
	while (lcdIsBusy()) ;
	lcdRawSendByte(0b10000000, LCD_COMMAND);
}
#endif

#endif
