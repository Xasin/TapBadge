EESchema Schematic File Version 4
LIBS:TapBadge-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 2
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Battery_Management:MCP73831-2-OT U?
U 1 1 5B940A65
P 3675 4175
AR Path="/5B940A65" Ref="U?"  Part="1" 
AR Path="/5B940565/5B940A65" Ref="U?"  Part="1" 
F 0 "U?" H 3675 4653 50  0000 C CNN
F 1 "MCP73831-2-OT" H 3675 4562 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23-5" H 3725 3925 50  0001 L CIN
F 3 "https://www.mouser.de/ProductDetail/Microchip-Technology/MCP73831T-2ATI-OT?qs=sGAEpiMZZMtLck3p7ZBovWxZVgPsNTST" H 3525 4125 50  0001 C CNN
F 4 "https://lcsc.com/product-detail/PMIC-Battery-Management_MICROCHIP_MCP73831T-2ATI-OT_MCP73831T-2ATI-OT_C14879.html" H 3675 4175 50  0001 C CNN "URL"
	1    3675 4175
	1    0    0    -1  
$EndComp
$Comp
L Battery_Management:BQ297xy U?
U 1 1 5B940A6D
P 5275 5025
AR Path="/5B940A6D" Ref="U?"  Part="1" 
AR Path="/5B940565/5B940A6D" Ref="U?"  Part="1" 
F 0 "U?" V 5229 5254 50  0000 L CNN
F 1 "BQ297xy" V 5320 5254 50  0000 L CNN
F 2 "Package_SON:WSON-6_1.5x1.5mm_P0.5mm" H 5275 5375 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/bq2970.pdf" H 5025 5225 50  0001 C CNN
F 4 "https://lcsc.com/product-detail/PMIC-Supervisors_TI_BQ29700DSER_BQ29700DSER_C183096.html" H 5275 5025 50  0001 C CNN "URL"
	1    5275 5025
	0    1    1    0   
$EndComp
$Comp
L Regulator_Linear:SPX3819M5-L-3-3 U?
U 1 1 5B940A75
P 6350 4175
AR Path="/5B940A75" Ref="U?"  Part="1" 
AR Path="/5B940565/5B940A75" Ref="U?"  Part="1" 
F 0 "U?" H 6350 4517 50  0000 C CNN
F 1 "SPX3819M5-L-3-3" H 6350 4426 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23-5" H 6350 4500 50  0001 C CNN
F 3 "https://www.exar.com/content/document.ashx?id=22106&languageid=1033&type=Datasheet&partnumber=SPX3819&filename=SPX3819.pdf&part=SPX3819" H 6350 4175 50  0001 C CNN
F 4 "https://lcsc.com/product-detail/Low-Dropout-Regulators-LDO_EXAR_SPX3819M5-L-3-3-TR_SPX3819M5-L-3-3-TR_C9055.html" H 6350 4175 50  0001 C CNN "URL"
	1    6350 4175
	1    0    0    -1  
$EndComp
$Comp
L Device:Battery_Cell BT?
U 1 1 5B940DBB
P 4775 4775
F 0 "BT?" V 4520 4825 50  0000 C CNN
F 1 "Battery_Cell" V 4611 4825 50  0000 C CNN
F 2 "" V 4775 4835 50  0001 C CNN
F 3 "~" V 4775 4835 50  0001 C CNN
	1    4775 4775
	1    0    0    -1  
$EndComp
$Comp
L FS8205:FS8205 TERM?
U 1 1 5B9490E1
P 4875 5875
F 0 "TERM?" V 5025 6325 50  0000 C CNN
F 1 "FS8205" V 5116 6325 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23-6_Handsoldering" H 4875 5875 50  0001 C CNN
F 3 "" H 4875 5875 50  0001 C CNN
F 4 "https://lcsc.com/product-detail/MOSFET_FS8205_C32254.html" V 4875 5875 50  0001 C CNN "URL"
	1    4875 5875
	0    1    1    0   
$EndComp
Wire Wire Line
	4775 5875 4975 5875
Wire Wire Line
	5175 5575 5175 5425
Wire Wire Line
	5375 5425 5375 5475
Wire Wire Line
	5375 5475 5475 5475
Wire Wire Line
	5475 5475 5475 5575
Wire Wire Line
	4775 4875 4775 5025
Wire Wire Line
	4775 5025 4975 5025
Connection ~ 4775 5025
Wire Wire Line
	4775 5025 4775 5875
Wire Wire Line
	5675 5875 5800 5875
$Comp
L power:GND #PWR?
U 1 1 5B94A36B
P 5800 5875
F 0 "#PWR?" H 5800 5625 50  0001 C CNN
F 1 "GND" H 5805 5702 50  0000 C CNN
F 2 "" H 5800 5875 50  0001 C CNN
F 3 "" H 5800 5875 50  0001 C CNN
	1    5800 5875
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5B94A5CF
P 5175 4375
F 0 "#PWR?" H 5175 4125 50  0001 C CNN
F 1 "GND" H 5180 4202 50  0000 C CNN
F 2 "" H 5175 4375 50  0001 C CNN
F 3 "" H 5175 4375 50  0001 C CNN
	1    5175 4375
	-1   0    0    1   
$EndComp
$Comp
L Device:R_Small R?
U 1 1 5B94A63B
P 5175 4500
F 0 "R?" H 5116 4454 50  0000 R CNN
F 1 "2.2k" H 5116 4545 50  0000 R CNN
F 2 "" H 5175 4500 50  0001 C CNN
F 3 "~" H 5175 4500 50  0001 C CNN
	1    5175 4500
	-1   0    0    1   
$EndComp
Wire Wire Line
	5175 4400 5175 4375
Wire Wire Line
	5175 4600 5175 4625
Text Label 4775 5625 1    50   ~ 0
BATT_RAW
Wire Wire Line
	5375 4625 5525 4625
$Comp
L Device:C_Small C?
U 1 1 5B964978
P 5725 4625
F 0 "C?" V 5496 4625 50  0000 C CNN
F 1 "100nF" V 5587 4625 50  0000 C CNN
F 2 "" H 5725 4625 50  0001 C CNN
F 3 "~" H 5725 4625 50  0001 C CNN
	1    5725 4625
	0    1    1    0   
$EndComp
Wire Wire Line
	5625 4625 5525 4625
Connection ~ 5525 4625
Wire Wire Line
	5825 4625 6050 4625
Text Label 6050 4625 3    50   ~ 0
BATT_RAW
Text HLabel 2450 3875 0    50   Input ~ 0
VCharge
Wire Wire Line
	2450 3875 3675 3875
$Comp
L Device:R_Small R?
U 1 1 5B965B63
P 3275 4500
F 0 "R?" H 3216 4454 50  0000 R CNN
F 1 "2.2k" H 3216 4545 50  0000 R CNN
F 2 "" H 3275 4500 50  0001 C CNN
F 3 "~" H 3275 4500 50  0001 C CNN
	1    3275 4500
	-1   0    0    1   
$EndComp
Wire Wire Line
	3275 4275 3275 4400
Wire Wire Line
	3675 4475 3675 4625
Wire Wire Line
	3675 4625 3275 4625
Wire Wire Line
	3275 4625 3275 4600
$Comp
L power:GND #PWR?
U 1 1 5B965F92
P 3675 4625
F 0 "#PWR?" H 3675 4375 50  0001 C CNN
F 1 "GND" H 3680 4452 50  0000 C CNN
F 2 "" H 3675 4625 50  0001 C CNN
F 3 "" H 3675 4625 50  0001 C CNN
	1    3675 4625
	1    0    0    -1  
$EndComp
Connection ~ 3675 4625
Wire Wire Line
	4075 4275 4225 4275
Text HLabel 4225 4375 3    50   Output ~ 0
STAT
Wire Wire Line
	4225 4275 4225 4375
Wire Wire Line
	4775 4075 5175 4075
Wire Wire Line
	4775 4075 4775 4575
Wire Wire Line
	5525 4075 5525 4625
Wire Wire Line
	4075 4075 4775 4075
Connection ~ 4775 4075
Wire Wire Line
	5525 4075 5975 4075
Connection ~ 5525 4075
$Comp
L power:+BATT #PWR?
U 1 1 5B969CE8
P 5175 4075
F 0 "#PWR?" H 5175 3925 50  0001 C CNN
F 1 "+BATT" H 5190 4248 50  0000 C CNN
F 2 "" H 5175 4075 50  0001 C CNN
F 3 "" H 5175 4075 50  0001 C CNN
	1    5175 4075
	1    0    0    -1  
$EndComp
Connection ~ 5175 4075
Wire Wire Line
	5175 4075 5525 4075
Wire Wire Line
	6050 4175 5975 4175
Wire Wire Line
	5975 4175 5975 4075
Connection ~ 5975 4075
Wire Wire Line
	5975 4075 6050 4075
$Comp
L power:GND #PWR?
U 1 1 5B96B5BA
P 6350 4500
F 0 "#PWR?" H 6350 4250 50  0001 C CNN
F 1 "GND" H 6355 4327 50  0000 C CNN
F 2 "" H 6350 4500 50  0001 C CNN
F 3 "" H 6350 4500 50  0001 C CNN
	1    6350 4500
	1    0    0    -1  
$EndComp
Wire Wire Line
	6350 4475 6850 4475
Wire Wire Line
	6850 4475 6850 4400
Wire Wire Line
	6350 4475 6350 4500
Connection ~ 6350 4475
Wire Wire Line
	6850 4200 6850 4075
Wire Wire Line
	6850 4075 6650 4075
Wire Wire Line
	6850 4075 6975 4075
Wire Wire Line
	6975 4075 6975 3950
Connection ~ 6850 4075
$Comp
L power:+3.3V #PWR?
U 1 1 5B97002B
P 6975 3950
F 0 "#PWR?" H 6975 3800 50  0001 C CNN
F 1 "+3.3V" H 6990 4123 50  0000 C CNN
F 2 "" H 6975 3950 50  0001 C CNN
F 3 "" H 6975 3950 50  0001 C CNN
	1    6975 3950
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C?
U 1 1 5B974FC7
P 6850 4300
F 0 "C?" H 6942 4346 50  0000 L CNN
F 1 "10uF" H 6942 4255 50  0000 L CNN
F 2 "" H 6850 4300 50  0001 C CNN
F 3 "~" H 6850 4300 50  0001 C CNN
	1    6850 4300
	1    0    0    -1  
$EndComp
$EndSCHEMATC
