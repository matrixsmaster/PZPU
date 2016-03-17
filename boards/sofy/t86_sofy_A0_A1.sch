EESchema Schematic File Version 2
LIBS:t86_sofy-rescue
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:special
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:l7805cv
LIBS:WEH00
LIBS:t86_sofy-cache
EELAYER 27 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "T86 Sofy Half Board"
Date "15 mar 2016"
Rev "A0"
Comp "Soloviov D.V."
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L ATMEGA328P-P IC1
U 1 1 56E603EA
P 2900 4200
F 0 "IC1" H 2150 5450 40  0000 L BNN
F 1 "ATMEGA328P-P" H 3300 2800 40  0000 L BNN
F 2 "DIL28" H 2900 4200 30  0000 C CIN
F 3 "" H 2900 4200 60  0000 C CNN
	1    2900 4200
	1    0    0    -1  
$EndComp
$Comp
L CONN_7 P2
U 1 1 56E603EB
P 4550 1300
F 0 "P2" V 4520 1300 60  0000 C CNN
F 1 "SDCard" V 4620 1300 60  0000 C CNN
F 2 "" H 4550 1300 60  0000 C CNN
F 3 "" H 4550 1300 60  0000 C CNN
	1    4550 1300
	0    -1   -1   0   
$EndComp
$Comp
L CRYSTAL X1
U 1 1 56E603EC
P 5450 3350
F 0 "X1" H 5450 3500 60  0000 C CNN
F 1 "25M" H 5450 3200 60  0000 C CNN
F 2 "~" H 5450 3350 60  0000 C CNN
F 3 "~" H 5450 3350 60  0000 C CNN
	1    5450 3350
	-1   0    0    1   
$EndComp
$Comp
L R R2
U 1 1 56E603ED
P 4250 2350
F 0 "R2" V 4330 2350 40  0000 C CNN
F 1 "2k" V 4257 2351 40  0000 C CNN
F 2 "~" V 4180 2350 30  0000 C CNN
F 3 "~" H 4250 2350 30  0000 C CNN
	1    4250 2350
	-1   0    0    1   
$EndComp
$Comp
L R R3
U 1 1 56E603EE
P 4400 2350
F 0 "R3" V 4480 2350 40  0000 C CNN
F 1 "2k" V 4407 2351 40  0000 C CNN
F 2 "~" V 4330 2350 30  0000 C CNN
F 3 "~" H 4400 2350 30  0000 C CNN
	1    4400 2350
	-1   0    0    1   
$EndComp
$Comp
L R R4
U 1 1 56E603EF
P 4600 2350
F 0 "R4" V 4680 2350 40  0000 C CNN
F 1 "2k" V 4607 2351 40  0000 C CNN
F 2 "~" V 4530 2350 30  0000 C CNN
F 3 "~" H 4600 2350 30  0000 C CNN
	1    4600 2350
	-1   0    0    1   
$EndComp
$Comp
L LED D1
U 1 1 56E603F0
P 3600 1950
F 0 "D1" H 3600 2050 50  0000 C CNN
F 1 "LED" H 3600 1850 50  0000 C CNN
F 2 "~" H 3600 1950 60  0000 C CNN
F 3 "~" H 3600 1950 60  0000 C CNN
	1    3600 1950
	-1   0    0    1   
$EndComp
$Comp
L R R1
U 1 1 56E603F1
P 4100 2350
F 0 "R1" V 4180 2350 40  0000 C CNN
F 1 "510" V 4107 2351 40  0000 C CNN
F 2 "~" V 4030 2350 30  0000 C CNN
F 3 "~" H 4100 2350 30  0000 C CNN
	1    4100 2350
	-1   0    0    1   
$EndComp
$Comp
L R R8
U 1 1 56E603F2
P 6000 4550
F 0 "R8" V 6080 4550 40  0000 C CNN
F 1 "1k" V 6007 4551 40  0000 C CNN
F 2 "~" V 5930 4550 30  0000 C CNN
F 3 "~" H 6000 4550 30  0000 C CNN
	1    6000 4550
	0    1    1    0   
$EndComp
$Comp
L SW_PUSH_SMALL SW1
U 1 1 56E603F3
P 5950 4850
F 0 "SW1" H 6100 4960 30  0000 C CNN
F 1 "rst" H 5950 4771 30  0000 C CNN
F 2 "~" H 5950 4850 60  0000 C CNN
F 3 "~" H 5950 4850 60  0000 C CNN
	1    5950 4850
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR01
U 1 1 56E603F5
P 3400 2200
F 0 "#PWR01" H 3400 2200 30  0001 C CNN
F 1 "GND" H 3400 2130 30  0001 C CNN
F 2 "" H 3400 2200 60  0000 C CNN
F 3 "" H 3400 2200 60  0000 C CNN
	1    3400 2200
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR02
U 1 1 56E603F6
P 6650 5100
F 0 "#PWR02" H 6650 5100 30  0001 C CNN
F 1 "GND" H 6650 5030 30  0001 C CNN
F 2 "" H 6650 5100 60  0000 C CNN
F 3 "" H 6650 5100 60  0000 C CNN
	1    6650 5100
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR03
U 1 1 56E603F7
P 6400 3800
F 0 "#PWR03" H 6400 3900 30  0001 C CNN
F 1 "VCC" H 6400 3900 30  0000 C CNN
F 2 "" H 6400 3800 60  0000 C CNN
F 3 "" H 6400 3800 60  0000 C CNN
	1    6400 3800
	1    0    0    -1  
$EndComp
$Comp
L CONN_3X2 P3
U 1 1 56E603F8
P 5450 2850
F 0 "P3" H 5450 3100 50  0000 C CNN
F 1 "ICSP" V 5450 2900 40  0000 C CNN
F 2 "" H 5450 2850 60  0000 C CNN
F 3 "" H 5450 2850 60  0000 C CNN
	1    5450 2850
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR04
U 1 1 56E603F9
P 5850 3000
F 0 "#PWR04" H 5850 3000 30  0001 C CNN
F 1 "GND" H 5850 2930 30  0001 C CNN
F 2 "" H 5850 3000 60  0000 C CNN
F 3 "" H 5850 3000 60  0000 C CNN
	1    5850 3000
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR05
U 1 1 56E603FA
P 5850 2500
F 0 "#PWR05" H 5850 2600 30  0001 C CNN
F 1 "VCC" H 5850 2600 30  0000 C CNN
F 2 "" H 5850 2500 60  0000 C CNN
F 3 "" H 5850 2500 60  0000 C CNN
	1    5850 2500
	1    0    0    -1  
$EndComp
$Comp
L CONN_1 P1
U 1 1 56E603FB
P 3900 2650
F 0 "P1" H 3980 2650 40  0000 L CNN
F 1 "CONN_1" H 3900 2705 30  0001 C CNN
F 2 "" H 3900 2650 60  0000 C CNN
F 3 "" H 3900 2650 60  0000 C CNN
	1    3900 2650
	0    -1   -1   0   
$EndComp
$Comp
L VCC #PWR06
U 1 1 56E603FC
P 1850 2900
F 0 "#PWR06" H 1850 3000 30  0001 C CNN
F 1 "VCC" H 1850 3000 30  0000 C CNN
F 2 "" H 1850 2900 60  0000 C CNN
F 3 "" H 1850 2900 60  0000 C CNN
	1    1850 2900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR07
U 1 1 56E603FD
P 1850 5550
F 0 "#PWR07" H 1850 5550 30  0001 C CNN
F 1 "GND" H 1850 5480 30  0001 C CNN
F 2 "" H 1850 5550 60  0000 C CNN
F 3 "" H 1850 5550 60  0000 C CNN
	1    1850 5550
	1    0    0    -1  
$EndComp
$Comp
L C C1
U 1 1 56E603FE
P 1850 4500
F 0 "C1" H 1850 4600 40  0000 L CNN
F 1 "0.1u" H 1856 4415 40  0000 L CNN
F 2 "~" H 1888 4350 30  0000 C CNN
F 3 "~" H 1850 4500 60  0000 C CNN
	1    1850 4500
	1    0    0    -1  
$EndComp
$Comp
L C C3
U 1 1 56E60436
P 5900 3500
F 0 "C3" H 5900 3600 40  0000 L CNN
F 1 "20p" H 5906 3415 40  0000 L CNN
F 2 "~" H 5938 3350 30  0000 C CNN
F 3 "~" H 5900 3500 60  0000 C CNN
	1    5900 3500
	1    0    0    -1  
$EndComp
$Comp
L C C2
U 1 1 56E60437
P 5450 3700
F 0 "C2" H 5450 3800 40  0000 L CNN
F 1 "20p" H 5456 3615 40  0000 L CNN
F 2 "~" H 5488 3550 30  0000 C CNN
F 3 "~" H 5450 3700 60  0000 C CNN
	1    5450 3700
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR08
U 1 1 56E60438
P 5900 3850
F 0 "#PWR08" H 5900 3850 30  0001 C CNN
F 1 "GND" H 5900 3780 30  0001 C CNN
F 2 "" H 5900 3850 60  0000 C CNN
F 3 "" H 5900 3850 60  0000 C CNN
	1    5900 3850
	1    0    0    -1  
$EndComp
$Comp
L CONN_3 K1
U 1 1 56E69A84
P 5450 4800
F 0 "K1" V 5400 4800 50  0000 C CNN
F 1 "UART" V 5500 4800 40  0000 C CNN
F 2 "" H 5450 4800 60  0000 C CNN
F 3 "" H 5450 4800 60  0000 C CNN
	1    5450 4800
	1    0    0    -1  
$EndComp
$Comp
L R R5
U 1 1 56E69D08
P 5650 1850
F 0 "R5" V 5730 1850 40  0000 C CNN
F 1 "4.22k" V 5657 1851 40  0000 C CNN
F 2 "~" V 5580 1850 30  0000 C CNN
F 3 "~" H 5650 1850 30  0000 C CNN
	1    5650 1850
	0    -1   -1   0   
$EndComp
$Comp
L R R6
U 1 1 56E69D76
P 5650 2000
F 0 "R6" V 5730 2000 40  0000 C CNN
F 1 "4.22k" V 5657 2001 40  0000 C CNN
F 2 "~" V 5580 2000 30  0000 C CNN
F 3 "~" H 5650 2000 30  0000 C CNN
	1    5650 2000
	0    -1   -1   0   
$EndComp
$Comp
L R R7
U 1 1 56E69DD7
P 5650 2150
F 0 "R7" V 5730 2150 40  0000 C CNN
F 1 "4.22k" V 5657 2151 40  0000 C CNN
F 2 "~" V 5580 2150 30  0000 C CNN
F 3 "~" H 5650 2150 30  0000 C CNN
	1    5650 2150
	0    -1   -1   0   
$EndComp
$Comp
L +3.3V #PWR09
U 1 1 56E69F65
P 5100 1150
F 0 "#PWR09" H 5100 1110 30  0001 C CNN
F 1 "+3.3V" H 5100 1260 30  0000 C CNN
F 2 "" H 5100 1150 60  0000 C CNN
F 3 "" H 5100 1150 60  0000 C CNN
	1    5100 1150
	1    0    0    -1  
$EndComp
NoConn ~ 3900 4250
NoConn ~ 3900 4350
NoConn ~ 3900 4450
NoConn ~ 5100 5550
NoConn ~ 5100 5950
NoConn ~ 5100 6050
NoConn ~ 5100 6150
NoConn ~ 5100 6250
NoConn ~ 3900 5300
NoConn ~ 3900 5400
$Comp
L VCC #PWR010
U 1 1 56E6A2F7
P 4800 5300
F 0 "#PWR010" H 4800 5400 30  0001 C CNN
F 1 "VCC" H 4800 5400 30  0000 C CNN
F 2 "" H 4800 5300 60  0000 C CNN
F 3 "" H 4800 5300 60  0000 C CNN
	1    4800 5300
	1    0    0    -1  
$EndComp
Wire Wire Line
	4850 3500 3900 3500
Wire Wire Line
	4850 1650 4850 3500
Wire Wire Line
	4750 1800 4750 1650
Wire Wire Line
	4450 1800 4750 1800
Wire Wire Line
	4450 1650 4450 1800
Wire Wire Line
	4350 1650 4350 1900
Wire Wire Line
	4350 1900 4400 1900
Wire Wire Line
	4400 1900 4400 2100
Wire Wire Line
	4250 1650 4250 2100
Wire Wire Line
	4550 1650 4550 1900
Wire Wire Line
	4550 1900 5100 1900
Wire Wire Line
	5100 1900 5100 1150
Wire Wire Line
	4650 1650 4650 1950
Wire Wire Line
	4650 1950 4600 1950
Wire Wire Line
	4600 1950 4600 2100
Wire Wire Line
	3400 1700 3400 2200
Wire Wire Line
	3800 1950 4100 1950
Wire Wire Line
	4450 1700 3400 1700
Connection ~ 3400 1950
Connection ~ 4450 1700
Wire Wire Line
	4600 2600 4600 3600
Wire Wire Line
	4600 3600 3900 3600
Wire Wire Line
	4400 3400 3900 3400
Wire Wire Line
	4400 2600 4400 3400
Wire Wire Line
	4100 1950 4100 2100
Wire Wire Line
	3900 3200 4100 3200
Wire Wire Line
	4100 3200 4100 2600
Wire Wire Line
	3900 3300 4250 3300
Wire Wire Line
	4250 3300 4250 2600
Wire Wire Line
	3900 3700 5250 3700
Wire Wire Line
	5750 3800 3900 3800
Wire Wire Line
	3900 4550 5750 4550
Wire Wire Line
	5750 4550 5750 4750
Wire Wire Line
	5750 4750 5850 4750
Wire Wire Line
	6250 4550 6400 4550
Wire Wire Line
	5050 2700 4850 2700
Connection ~ 4850 2700
Wire Wire Line
	5050 2800 4600 2800
Connection ~ 4600 2800
Wire Wire Line
	5050 2900 4700 2900
Connection ~ 4700 4550
Wire Wire Line
	5150 3350 5150 3700
Wire Wire Line
	5750 3300 5750 3800
Wire Wire Line
	4400 3100 6050 3100
Wire Wire Line
	6050 3100 6050 2800
Wire Wire Line
	6050 2800 5850 2800
Connection ~ 4400 3100
Wire Wire Line
	5850 2900 5850 3000
Wire Wire Line
	5850 2500 5850 2700
Wire Wire Line
	3900 2800 3900 3100
Wire Wire Line
	1850 2900 1850 4300
Wire Wire Line
	1850 3700 2000 3700
Wire Wire Line
	2000 3400 1850 3400
Connection ~ 1850 3400
Wire Wire Line
	2000 3100 1850 3100
Connection ~ 1850 3100
Wire Wire Line
	2000 5300 1850 5300
Wire Wire Line
	1850 4700 1850 5550
Wire Wire Line
	1850 5400 2000 5400
Connection ~ 1850 5400
Connection ~ 1850 3700
Connection ~ 1850 5300
Wire Wire Line
	6400 4550 6400 3800
Connection ~ 5150 3700
Wire Wire Line
	5650 3700 5900 3700
Wire Wire Line
	5750 3300 5900 3300
Connection ~ 5750 3350
Wire Wire Line
	5900 3700 5900 3850
Wire Wire Line
	4700 2900 4700 4550
Wire Wire Line
	5100 4700 3900 4700
Wire Wire Line
	3900 4800 5100 4800
Wire Wire Line
	5100 4900 5100 5350
Wire Wire Line
	3900 3950 4650 3950
Wire Wire Line
	3900 4050 4600 4050
Wire Wire Line
	3900 4150 4550 4150
Wire Wire Line
	5400 1850 4250 1850
Connection ~ 4250 1850
Wire Wire Line
	5400 2000 4400 2000
Connection ~ 4400 2000
Wire Wire Line
	4600 2050 5400 2050
Wire Wire Line
	5400 2050 5400 2150
Connection ~ 4600 2050
Wire Wire Line
	5900 1850 6200 1850
Wire Wire Line
	6200 1850 6200 2900
Wire Wire Line
	6200 2900 5850 2900
Wire Wire Line
	5900 2000 6200 2000
Connection ~ 6200 2000
Wire Wire Line
	5900 2150 6200 2150
Connection ~ 6200 2150
Wire Wire Line
	4650 3950 4650 5650
Wire Wire Line
	4650 5650 5100 5650
Wire Wire Line
	4600 4050 4600 5750
Wire Wire Line
	4600 5750 5100 5750
Wire Wire Line
	4550 4150 4550 5850
Wire Wire Line
	4550 5850 5100 5850
Wire Wire Line
	3900 4900 4500 4900
Wire Wire Line
	4500 4900 4500 6350
Wire Wire Line
	4500 6350 5100 6350
Wire Wire Line
	3900 5000 4450 5000
Wire Wire Line
	4450 5000 4450 6450
Wire Wire Line
	4450 6450 5100 6450
Wire Wire Line
	3900 5100 4400 5100
Wire Wire Line
	4400 5100 4400 6550
Wire Wire Line
	4400 6550 5100 6550
Wire Wire Line
	3900 5200 4350 5200
Wire Wire Line
	4350 5200 4350 6650
Wire Wire Line
	4350 6650 5100 6650
Wire Wire Line
	5100 5450 4800 5450
Wire Wire Line
	4800 5450 4800 5300
$Comp
L L7805CV DA1
U 1 1 56E6A647
P 8600 3800
F 0 "DA1" H 8750 3600 60  0000 C CNN
F 1 "L2937-3.3" H 8600 4000 60  0000 C CNN
F 2 "~" H 8600 3750 60  0000 C CNN
F 3 "~" H 8600 3750 60  0000 C CNN
	1    8600 3800
	1    0    0    -1  
$EndComp
$Comp
L CP1 C4
U 1 1 56E6A65B
P 9400 4000
F 0 "C4" H 9450 4100 50  0000 L CNN
F 1 "47u x 6.3V" H 9450 3900 50  0000 L CNN
F 2 "~" H 9400 4000 60  0000 C CNN
F 3 "~" H 9400 4000 60  0000 C CNN
	1    9400 4000
	1    0    0    -1  
$EndComp
Wire Wire Line
	9150 3800 9850 3800
Wire Wire Line
	9400 4200 8900 4200
Wire Wire Line
	8900 4200 8900 4250
Wire Wire Line
	8900 4250 8600 4250
$Comp
L VCC #PWR011
U 1 1 56E6A750
P 7700 3700
F 0 "#PWR011" H 7700 3800 30  0001 C CNN
F 1 "VCC" H 7700 3800 30  0000 C CNN
F 2 "" H 7700 3700 60  0000 C CNN
F 3 "" H 7700 3700 60  0000 C CNN
	1    7700 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	7700 3700 7700 3800
Wire Wire Line
	7700 3800 8050 3800
Wire Wire Line
	9850 3800 9850 3700
Connection ~ 9400 3800
$Comp
L +3.3V #PWR012
U 1 1 56E6A847
P 9850 3700
F 0 "#PWR012" H 9850 3660 30  0001 C CNN
F 1 "+3.3V" H 9850 3810 30  0000 C CNN
F 2 "" H 9850 3700 60  0000 C CNN
F 3 "" H 9850 3700 60  0000 C CNN
	1    9850 3700
	1    0    0    -1  
$EndComp
$Comp
L LED D2
U 1 1 56E6A966
P 8600 4950
F 0 "D2" H 8600 5050 50  0000 C CNN
F 1 "LED" H 8600 4850 50  0000 C CNN
F 2 "~" H 8600 4950 60  0000 C CNN
F 3 "~" H 8600 4950 60  0000 C CNN
	1    8600 4950
	1    0    0    -1  
$EndComp
$Comp
L R R9
U 1 1 56E6A96C
P 9200 4950
F 0 "R9" V 9280 4950 40  0000 C CNN
F 1 "510" V 9207 4951 40  0000 C CNN
F 2 "~" V 9130 4950 30  0000 C CNN
F 3 "~" H 9200 4950 30  0000 C CNN
	1    9200 4950
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR013
U 1 1 56E6A97C
P 8600 4450
F 0 "#PWR013" H 8600 4450 30  0001 C CNN
F 1 "GND" H 8600 4380 30  0001 C CNN
F 2 "" H 8600 4450 60  0000 C CNN
F 3 "" H 8600 4450 60  0000 C CNN
	1    8600 4450
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR014
U 1 1 56E6A982
P 9650 5150
F 0 "#PWR014" H 9650 5150 30  0001 C CNN
F 1 "GND" H 9650 5080 30  0001 C CNN
F 2 "" H 9650 5150 60  0000 C CNN
F 3 "" H 9650 5150 60  0000 C CNN
	1    9650 5150
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR015
U 1 1 56E6A988
P 8050 4850
F 0 "#PWR015" H 8050 4810 30  0001 C CNN
F 1 "+3.3V" H 8050 4960 30  0000 C CNN
F 2 "" H 8050 4850 60  0000 C CNN
F 3 "" H 8050 4850 60  0000 C CNN
	1    8050 4850
	1    0    0    -1  
$EndComp
Wire Wire Line
	8600 4250 8600 4450
Wire Wire Line
	8050 4850 8050 4950
Wire Wire Line
	8050 4950 8400 4950
Wire Wire Line
	8800 4950 8950 4950
Wire Wire Line
	9450 4950 9650 4950
Wire Wire Line
	9650 4950 9650 5150
$Comp
L CONN_2 P4
U 1 1 56E6AB98
P 8800 6250
F 0 "P4" V 8750 6250 40  0000 C CNN
F 1 "PWR" V 8850 6250 40  0000 C CNN
F 2 "" H 8800 6250 60  0000 C CNN
F 3 "" H 8800 6250 60  0000 C CNN
	1    8800 6250
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR016
U 1 1 56E6ABA5
P 8450 5950
F 0 "#PWR016" H 8450 6050 30  0001 C CNN
F 1 "VCC" H 8450 6050 30  0000 C CNN
F 2 "" H 8450 5950 60  0000 C CNN
F 3 "" H 8450 5950 60  0000 C CNN
	1    8450 5950
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR017
U 1 1 56E6ABAB
P 8450 6550
F 0 "#PWR017" H 8450 6550 30  0001 C CNN
F 1 "GND" H 8450 6480 30  0001 C CNN
F 2 "" H 8450 6550 60  0000 C CNN
F 3 "" H 8450 6550 60  0000 C CNN
	1    8450 6550
	1    0    0    -1  
$EndComp
Wire Wire Line
	8450 5950 8450 6150
Wire Wire Line
	8450 6350 8450 6550
$Comp
L WEH001202ARPP5N00000 U1
U 1 1 56E6A4FF
P 5850 6050
F 0 "U1" H 5850 5950 50  0000 C CNN
F 1 "WEH001202" H 5850 6150 50  0000 C CNN
F 2 "MODULE" H 5850 6050 50  0001 C CNN
F 3 "DOCUMENTATION" H 5850 6050 50  0001 C CNN
	1    5850 6050
	1    0    0    -1  
$EndComp
Wire Wire Line
	5100 5050 6650 5050
Wire Wire Line
	6650 5050 6650 5100
Connection ~ 5100 5050
Wire Wire Line
	6050 4950 6050 5050
Connection ~ 6050 5050
NoConn ~ 5100 6750
$EndSCHEMATC
