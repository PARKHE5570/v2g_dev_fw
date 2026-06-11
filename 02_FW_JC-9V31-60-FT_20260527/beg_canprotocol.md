# Bottom Level Protocol
It adopts the extended frame mode of the CAN bus, CAN2.0B.
The communicate BAUD rate is 125K.
CAN bus uses a linear bus terminated at each end with 120 Ω resistors.

# DATA TYPE
Bit shall be sent in the descending order from MSB.
Byte shall be sent in ascending order from byte 0 (Little-endian / Intel type)
Data type includes fixed point number and floating point number.

# FIXED POINT NUMBER
1 4 bytes length, detail format and send sequence refer to

# FLOATING POINT NUMBER
The storage format of floating point number is 4-byte and the number will be sent after it is converted into HEX-ASCII code. The number is sending according to the sequence of sign bit, code, high bit of mantissa and low bit of mantissa. The floating point number uses IEEE 32-bit standard floating point number format (standard floating point number format of C language), the length is 32-bit, and the format is as shown below:
D31|D30—D23|D22—D16|D15—D8|D7—D0|
Sign of floating pointnumber|code|high bit of mantissa|middle bit of mantissa|low bit of mantissa|

If the code is E, mantissa is M, then: floating point number=±(1+M×2-23)·2E-127. Whether the floating point number is positive or negative is dependent on the value of sign bit (S). S=1 means the floating point number is negative, and S=0 means the floating point number is positive.
For example, if the 32-bit floating point number is 43H, FAH, 00H, and 00H ( S=0, E=135, M=0x7A0000=61×217), the value of the floating point number is (1+61×217×2-23) ×2135-127=500.
If the floating point number is 40.0, the 4-byte ASCII code is 42, 20, 00, 00, and the sending sequence is 42, 20, 00, 00.
If the floating point number is 2.40, the 4-byte ASCII code is 40, 19, 99, 9A, and the sending sequence is 40, 19, 99, 9A.



# Frame Format
Frame is the basic unit of the information. The frame format is shown as the table below.
Description|Code
Start of frame|sof (1 bit)
Arbitration field|Identifier (11 bits),SRR,IDE,Identifier (18 bits),RTRControl field,reserved (1 bit),Data Len (4 bits)
Data field|Data (0~8 bytes)
CRC field|CRC (16 bits)
Ack field|Ack (2 bits)
End of frame|(7 bits)

Identifier | Data
29 bits | 8 Bytes
Identifier | Byte 0, Byte 1, Byte 2, Byte 3, Byte 4, Byte 5, Byte 6, Byte 7

# Identifier
Bits 28 ~ 26|Bits 25 ~ 22|Bits 21 ~ 16|Bits 15 ~ 8|Bits 7 ~ 0
Error code(3 bits)|Device No(4 bits)|Command No(6 bits)|Destination Address(8 bits)|Source Address(8 bits)
Error code : Message error reason.

The Error code is shown as the table below.
Error code|Description
0x00|Normal
0x01|(None)
0x02|Command invalid
0x03|Data invalid
(None)|(None)
0x07|In start processing

# Device No
Device No|Description
0x0A|Protocol between controller and single module
0x0B|Protocol between controller and module group

# Command No
READ(0x23) : Get data from module register to controller
WRITE(0x24) : Controller write data to module register

# Destination address/Source Address
It is a broadcast message if the module address in destination address is 0x3F, broadcast message don’t need answer except 0x01,0x02 command.
When the Device No. is 0x0A, the data from controller to module, the destination address is the module address.
When the Device No. is 0x0B, the data from controller to module, the destination address is the module group address.
If the command is 0x01/0x02 with the Device No. is 0x0A, if the destination address is 0x3F, the Master module will feedback with the source address 0x3F to show it is system information feedback.
If the command is 0x01/0x02 with the Device No. is 0x0B, the destination address is module group address, the Master module in this module group will feedback with the source address same to group address to show it is group system information feedback.

destination/source address|Bit 7|Bit 6|Bit 5|Bit 4|Bit 3|Bit 2|Bit 1|Bit 0|
Module : |Reserved|Reserved| (6bit) Module address: 00 ~ 0x3E  Broadcast address: 3F|
Controller : |(8bit) Controller address: 0xF0 ~ 0xF8 Default address: 0xF0|
Notes: The address of ABU(Auxiliary Bypass Unit) is 0xFA.
Supports up to 60 modules in parallel.

The module will get the automatic allocated address after power on.
The group number is determined by the dial on the panel.
If just have one group in the system, the dial should all switch to zero.
The power module address in the System/Group is distributed automaticly in the 0x07 err code Start processing and at this time it should not start the set and control logic.



# Data field description
No|Command|Write / Read|Data Name|Byte 0|Byte 1|Byte 2|Byte 3|Byte 4|Byte 5|Byte 6|Byte 7|
1|0x23|Read|System DC side Voltage（mV）|0x10|0x01|||System DC side Voltage（mV）(4byte, byte4~byte7)|
2|0x23|Read|System DC side Total Current(mA)|0x10|0x02|||System DC side Total Current (mA) (4byte, byte4~byte7)|
3|0x23|Read|Power module number|0x10|0x10|||||Power Module number (2byte, byte6~byte7)|
4|0x23|Read|Power module DC side Voltage（mV）|0x11|0x01|||Power module DC side Voltage（mV）(4byte, byte4~byte7)|
5|0x23|Read|Power module DC side current( mA)|0x11|0x02|||Power module DC side current( mA) (4byte, byte4~byte7)|
6|0x23|Read|AC AB line Voltage（mV）|0x11|0x03|||AC AB line Voltage（mV）(4byte, byte4~byte7)|
7|0x23|Read|AC BC line Voltage（mV）|0x11|0x04|||AC BC line Voltage（mV）(4byte, byte4~byte7)|
8|0x23|Read|AC CA line Voltage （mV）|0x11|0x05|||AC CA line Voltage （mV）(4byte, byte4~byte7)|
9|0x23|Read|Power module Ambient temperature（m°C）|0x11|0x06|||Power module Ambient temperature（m°C）(4byte, byte4~byte7)|
10|0x23|Read|Power module DC side port Voltage（mV）|0x11|0x07|||Power module DC side port Voltage（mV）(4byte, byte4~byte7)|
11|0x23|Read|Power module DC side current limit（mA）|0x11|0x08|||Power module DC side current limit（mA）(4byte, byte4~byte7)|
12|0x23|Read|DC DisCharge cut-off voltage（mV）|0x11|0x09|||DC DisCharge cut-off voltage（mV）(4byte, byte4~byte7)|
13|0x23|Read|Power module status|0x11|0x10|||Power module status3 (1byte, byte4)|Power module status 2 (1byte, byte5)|Power module status 1 (1byte, byte6)|Power module status 0 (1byte, byte7)|
14|0x23|Read|Inverter Status|0x11|0x11|||Inverter Status 2 (1byte, byte5)|Inverter Status 1 (1byte, byte6)|Inverter status 0 (1byte, byte7)|
15|0x23|Read|module group number|0x11|0x20|||||module group number (2byte, byte6~byte7)|
16|0x23|Read|DC software version|0x11|0x22|||||DC software version (2byte, byte6~byte7)|
17|0x23|Read|PFC software version|0x11|0x23|||||PFC software version (2byte, byte6~byte7)|
18|0x23|Read|Power module barcode information0|0x11|0x24|||Power module barcode information0 (4byte, byte4~byte7)|
19|0x23|Read|Power module barcode information1|0x11|0x25|||Power module barcode information1 (4byte, byte4~byte7)|
20|0x23|Read|Power module barcode information2|0x11|0x27|||Power module barcode information2 (4byte, byte4~byte7)|
21|0x23|Read|Power module barcode information3|0x11|0x28|||Power module barcode information3 (4byte, byte4~byte7)|
22|0x23|Read|DC max output voltage（mV）|0x11|0x30|||DC max output voltage（mV）(4byte, byte4~byte7)|
23|0x23|Read|DC min output voltage（mV）|0x11|0x31|||DC min output voltage（mV）(4byte, byte4~byte7)|
24|0x23|Read|DC max output current（mA）|0x11|0x32|||DC max output current（mA）(4byte, byte4~byte7)|
25|0x23|Read|DC rated output power（mW）|0x11|0x33|||DC rated output power（mW）(4byte, byte4~byte7)|
26|0x23|Read|Fan Speed（rpm）|0x12|0x01|||Fan Speed（rpm）(4byte, byte4~byte7)|
27|0x23|Read|Power module Water inlet temperature（m°C）|0x12|0x04|||Power module Water inlet temperature（m°C）(4byte, byte4~byte7)|
28|0x23|Read|Power module Water outlet temperature（m°C）|0x12|0x05|||Power module Water outlet temperature（m°C）(4byte, byte4~byte7)|
29|0x23|Read|AC A phase Voltage（mV）|0x21|0x01|||AC A phase Voltage（mV）(4byte, byte4~byte7)|
30|0x23|Read|AC B phase Voltage（mV）|0x21|0x02|||AC B phase Voltage（mV）(4byte, byte4~byte7)|
31|0x23|Read|AC C phase Voltage（mV）|0x21|0x03|||AC C phase Voltage（mV）(4byte, byte4~byte7)|
32|0x23|Read|AC A phase current（mA）|0x21|0x04|||AC A phase current（mA）(4byte, byte4~byte7)|
33|0x23|Read|AC B phase current（mA）|0x21|0x05|||AC B phase current（mA）(4byte, byte4~byte7)|
34|0x23|Read|AC C phase current（mA）|0x21|0x06|||AC C phase current（mA）(4byte, byte4~byte7)|
35|0x23|Read|AC frequency（mHz）|0x21|0x07|||AC frequency（mHz）(4byte, byte4~byte7)|
36|0x23|Read|Total active power（mW）|0x21|0x08|||Total active power（mW）(4byte, byte4~byte7)|
37|0x23|Read|AC A phase active power（mW）|0x21|0x09|||AC A phase active power（mW）(4byte, byte4~byte7)|
38|0x23|Read|AC B phase active power（mW）|0x21|0x0A|||AC B phase active power（mW）(4byte, byte4~byte7)|
39|0x23|Read|AC C phase active power（mW）|0x21|0x0B|||AC C phase active power（mW）(4byte, byte4~byte7)|
40|0x23|Read|Total reactive power（mVA）|0x21|0x0C|||Total reactive power（mVA）(4byte, byte4~byte7)|
41|0x23|Read|AC A phase reactive power(mVA)|0x21|0x0D|||AC A phase reactive power(mVA) (4byte, byte4~byte7)|
42|0x23|Read|AC B phase reactive power(mVA)|0x21|0x0E|||AC B phase reactive power(mVA) (4byte, byte4~byte7)|
43|0x23|Read|AC C phase reactive power(mVA)|0x21|0x0F|||AC C phase reactive power(mVA) (4byte, byte4~byte7)|
44|0x23|Read|Total apparent power（mVA）|0x21|0x10|||Total apparent power（mVA）(4byte, byte4~byte7)|
45|0x23|Read|AC A phase apparent power(mVA)|0x21|0x11|||AC A phase apparent power(mVA) (4byte, byte4~byte7)|
46|0x23|Read|AC B phase apparent power(mVA)|0x21|0x12|||AC B phase apparent power(mVA) (4byte, byte4~byte7)|
47|0x23|Read|AC C phase apparent power(mVA)|0x21|0x13|||AC C phase apparent power(mVA) (4byte, byte4~byte7)|
48|0x23|Read|System DC high voltage side Voltage（mV）|0x40|0x01|||System DC high voltage side Voltage（mV）(4byte, byte4~byte7)|
49|0x23|Read|PM DC high voltage side voltage（mV）|0x41|0x01|||PM DC high voltage side voltage（mV）(4byte, byte4~byte7)|
50|0x23|Read|PM DC high voltage side current（mA）|0x41|0x02|||PM DC high voltage side current（mA）(4byte, byte4~byte7)|
51|0x24|Write|System DC voltage（mV）|0x10|0x01|||System DC voltage（mV）(4byte, byte4~byte7)|
52|0x24|Write|System Total DC current（mA）|0x10|0x02|||System Total DC current（mA）(4byte, byte4~byte7)|
53|0x24|Write|Power module DC side voltage （mV）|0x11|0x01|||Power module DC side voltage （mV）(4byte, byte4~byte7)|
54|0x24|Write|Power module DC side current (mA)|0x11|0x02|||Power module DC side current (mA) (4byte, byte4~byte7)|
55|0x24|Write|Module group number|0x11|0x03|||||module group number (2byte, byte6~byte7)|
56|0x24|Write|module On/Off|0x11|0x10|||||module On/Off (2byte, byte6~byte7)|
57|0x24|Write|CAN bus muting mode|0x11|0x1E|||||CAN bus muting mode (2byte, byte6~byte7)|
58|0x24|Write|Green Led blink|0x11|0x20|||||Green Led blink (2byte, byte6~byte7)|
59|0x24|Write|sleep|0x11|0x21|||||sleep (2byte, byte6~byte7)|
60|0x24|Write|Walk-in enable|0x11|0x22|||||Walk-in enable (2byte, byte6~byte7)|
61|0x24|Write|Address allocation method|0x11|0x23|||||Address allocation method (2byte, byte6~byte7)|
62|0x24|Write|DC side output high or low voltage mode|0x11|0x26|||||DC side output high or low voltage mode (2byte, byte6~byte7)|
63|0x24|Write|DC DisCharge cut-off voltage（mV）|0x11|0x32|||DC DisCharge cut-off voltage(mV) (2byte, byte6~byte7)|
64|0x24|Write|Noise Reduction Mode|0x11|0x33|||||Noise Reduction Mode (2byte, byte6~byte7)|
65|0x24|Write|automatic voltage stabilization mode|0x11|0x34|||||automatic voltage stabilization mode (2byte, byte6~byte7)|
66|0x24|Write|Power Factor adjust（0.001） Set Abs in (0.8,1)|0x21|0x05|||Power Factor adjust（0.001） Set Abs in (0.8,1) (4byte, byte4~byte7)|
67|0x24|Write|OffGrid Voltage Set(220Vac/230Vac)|0x21|0x06|||OffGrid Voltage Set(220Vac/230Vac) (4byte, byte4~byte7)|
68|0x24|Write|OffGrid Frequency Set(50Hz/60HZ)|0x21|0x07|||OffGrid Frequency Set(50Hz/60HZ) (4byte, byte4~byte7)|
69|0x24|Write|Set Reactive Power value（mVar）|0x21|0x08|||Set Reactive Power value(mVar) (4byte, byte4~byte7)|
70|0x24|Write|Working mode|0x21|0x10|||||Working mode (2byte, byte6~byte7)|
71|0x24|Write|Turn off islanding detection|0x21|0x14|||||Turn off islanding detection (2byte, byte6~byte7)|
72|0x24|Write|Reactive Power Setting Type|0x21|0x17|||||Reactive Power Setting Type (2byte, byte6~byte7)|
73|0x24|Write|Energy storage mode|0x21|0x18|||||Energy storage mode (2byte, byte6~byte7)|
74|0x24|Write|Level 1 over voltage protection value（mV）（rated～2 x rated）|0x21|0x20|||Level 1 over voltage protection value（mV）（rated～2 x rated）(4byte, byte4~byte7)|
75|0x24|Write|Level 1 over voltage protection time（ms）(1ms～10000s)|0x21|0x21|||Level 1 over voltage protection time（ms）(1ms～10000s) (4byte, byte4~byte7)|
76|0x24|Write|Level 2 over voltage protection value（mV）（rated～2 x rated）|0x21|0x22|||Level 2 over voltage protection value（mV）（rated～2 x rated）(4byte, byte4~byte7)|
77|0x24|Write|Level 2 over voltage protection time（ms）(1ms～10000s)|0x21|0x23|||Level 2 over voltage protection time（ms）(1ms～10000s) (4byte, byte4~byte7)|
78|0x24|Write|Level 1 under voltage protection value（mV）（0.2 x rated～rated）|0x21|0x24|||Level 1 under voltage protection value（mV）（0.2 x rated～rated）(4byte, byte4~byte7)|
79|0x24|Write|Level 1 under voltage protection time（ms）(1ms～10000s)|0x21|0x25|||Level 1 under voltage protection time（ms）(1ms～10000s) (4byte, byte4~byte7)|
80|0x24|Write|Level 2 under voltage protection value（mV）（0.2 x rated～rated）|0x21|0x26|||Level 2 under voltage protection value（mV）（0.2 x rated～rated）(4byte, byte4~byte7)|
81|0x24|Write|Level 2 under voltage protection time（ms）(1ms～10000s)|0x21|0x27|||Level 2 under voltage protection time（ms）(1ms～10000s) (4byte, byte4~byte7)|
82|0x24|Write|Level 1 over frequence protection value (mHz) （rated～rated+5Hz）|0x21|0x28|||Level 1 over frequence protection value (mHz) （rated～rated+5Hz）(4byte, byte4~byte7)|
83|0x24|Write|Level 1 over frequence protection time（ms）(1ms～10000s)|0x21|0x29|||Level 1 over frequence protection time（ms）(1ms～10000s) (4byte, byte4~byte7)|
84|0x24|Write|Level 2 over frequence protection value (mH) （rated～rated+5Hz）|0x21|0x2A|||Level 2 over frequence protection value (mH) （rated～rated+5Hz）(4byte, byte4~byte7)|
85|0x24|Write|Level 2 over frequence protection time（ms）(1ms～10000s)|0x21|0x2B|||Level 2 over frequence protection time（ms）(1ms～10000s) (4byte, byte4~byte7)|
86|0x24|Write|Level 1 under frequence protection value (mHz) （rated-5Hz～rated）|0x21|0x2C|||Level 1 under frequence protection value (mHz) （rated-5Hz～rated） (4byte, byte4~byte7)|
87|0x24|Write|Level 1 under frequence protection time（ms）(1ms～10000s)|0x21|0x2D|||Level 1 under frequence protection time（ms）(1ms～10000s) (4byte, byte4~byte7)|
88|0x24|Write|Level 2 under frequence protection value /mHz （rated-5Hz～rated）|0x21|0x2E|||Level 2 under frequence protection value /mHz （rated-5Hz～rated）(4byte, byte4~byte7)|
89|0x24|Write|Level 2 under frequence protection time（ms）(1ms～10000s)|0x21|0x2F|||Level 2 under frequence protection time（ms）(1ms～10000s) (4byte, byte4~byte7)|
90|0x24|Write|power regulation mode (AC/DC) mode|0x31|0x11|||||power regulation mode (AC/DC) mode (2byte, byte6~byte7)|
91|0x24|Write|AC Active Power setting|0x31|0x23|||AC Active Power setting (4byte, byte4~byte7)|
92|0x24|Write|AC Active power fall gradient|0x31|0x24|||AC Active power fall gradient (4byte, byte4~byte7)|
93|0x24|Write|AC Active power rise gradient|0x31|0x25|||AC Active power rise gradient (4byte, byte4~byte7)|
94|0x24|Write|Enable low voltage ride-through|0x31|0xD0|||Enable low voltage ride-through (4byte, byte4~byte7)|
95|0x24|Write|Enable High voltage ride-through|0x31|0xD1|||Enable High voltage ride-through (4byte, byte4~byte7)|
96|0x24|Write|System DC high voltage side Voltage（mV）|0x40|0x01|||System DC high voltage side Voltage（mV）(4byte, byte4~byte7)|
97|0x24|Write|Power module DC high voltage side voltage/ mV|0x41|0x01|||Power module DC high voltage side voltage/ mV (4byte, byte4~byte7)|
98|0x24|Write|Power module DC high voltage side current/ mA|0x41|0x02|||Power module DC high voltage side current/ mA (4byte, byte4~byte7)|

Command basic principle
(1) CMD=0x23 denote reading the PM information 0x24 denote writingthe PM setting.
(2) Byte0 =0x10 denote system basic information Byte0 =0x11 denote single PM basic information Byte0 =0x21 denote single PM AC side information Byte0 =0x41 denote single bidirectional DC/DC PM basic information.
(3) blank part is reserved default value is 0.
PM: Power module
Note: If the module or system requires grid connection certification, please refer to the corresponding independent protocol text descriptions for VDE4105, UL1741SA, IEEE1547 (UL1741SB), the corresponding commands is not listed in this protocol Text.
If there is no certification requirement, please ignore it.

## Data field description
# Reading data field
Command No | Description | Data infomation (Byte 0, Byte 1, Byte 2, Byte 3, Byte 4, Byte 5, Byte 6, Byte 7) |
# ----------------
# Ctrl Tx
0x23 | Read system information | 0x10 | 0x01 | | | | | | |
0x23 | Read system information | 0x10 | 0x02 | | | | | | |
0x23 | Read system information | 0x10 | 0x10 | | | | | | |
NULL
Note
Device No. is 0x0A, the destination address is broadcast address 0x3F;
Device No. is 0x0B, the destination address is group No.
# Mdl Ans
0x23 | Answer | 0x10 | 0x01 | | | System DC Voltage (mV), 4byte |
0x23 | Answer | 0x10 | 0x02 | | | System DC total current (mA), 4byte |
Note
Device No is 0x0A,module answer the total current of the system, the source address is 0x3F;
Device No is 0x0B,module answer the total current of the group, the source address is group No.
0x23 | Answer | 0x10 | 0x10 | | | | | Mdl numbers, 2byte|
Note
Device No. is 0x0A,module answer the total system Mdl numbers;
Device No. is 0x0B,module answer the Group Mdl numbers

# Example case
Ctrl Tx 02 A3 3F F0 10 01 00 00 00 00 00 00 : read system DC voltage
Mdl Ans 02 A3 F0 3F 10 01 00 00 00 0A AE 60 : system DC voltage value is 700V
Ctrl Tx 02 A3 3F F0 10 02 00 00 00 00 00 00 : read system DC current
Mdl Ans 02 A3 F0 3F 10 02 00 00 00 00 C3 50 : system DC total current is 50A
Ctrl Tx 02 E3 02 F0 10 10 00 00 00 00 00 00 : read group 2 Mdl numbers
Mdl Ans 02 E3 F0 02 10 10 00 00 00 00 00 05 : group 2 Mdl numbers is 5
# ----------------
# ----------------
# Ctrl Tx
0x23 | Read Mdl N# infomation | 0x11 | 0x01 | | | | | | |
0x23 | Read Mdl N# infomation | 0x11 | 0x02 | | | | | | |
0x23 | Read Mdl N# infomation | 0x11 | 0x03 | | | | | | |
0x23 | Read Mdl N# infomation | 0x11 | 0x04 | | | | | | |
0x23 | Read Mdl N# infomation | 0x11 | 0x05 | | | | | | |
0x23 | Read Mdl N# infomation | 0x11 | 0x06 | | | | | | |
0x23 | Read Mdl N# infomation | 0x11 | 0x10 | | | | | | |
0x23 | Read Mdl N# infomation | 0x11 | 0x11 | | | | | | |
0x23 | Read Mdl N# infomation | 0x11 | 0x20 | | | | | | |
0x23 | Read Mdl N# infomation | 0x11 | 0x30 | | | | | | |
0x23 | Read Mdl N# infomation | 0x11 | 0x31 | | | | | | |
0x23 | Read Mdl N# infomation | 0x11 | 0x32 | | | | | | |
0x23 | Read Mdl N# infomation | 0x11 | 0x33 | | | | | | |
Blank is 0
Note N is the destination address.
# Mdl Ans
0x23 | Answer | 0x11 | 0x01 | | | The correspond Mdl information , 4byte|
0x23 | Answer | 0x11 | 0x02 | | | The correspond Mdl information , 4byte|
0x23 | Answer | 0x11 | 0x03 | | | The correspond Mdl information , 4byte|
0x23 | Answer | 0x11 | 0x04 | | | The correspond Mdl information , 4byte|
0x23 | Answer | 0x11 | 0x05 | | | The correspond Mdl information , 4byte|
0x23 | Answer | 0x11 | 0x06 | | | The correspond Mdl information , 4byte|
0x23 | Answer | 0x11 | 0x30 | | | The correspond Mdl information , 4byte|
0x23 | Answer | 0x11 | 0x31 | | | The correspond Mdl information , 4byte|
0x23 | Answer | 0x11 | 0x32 | | | The correspond Mdl information , 4byte|
0x23 | Answer | 0x11 | 0x33 | | | The correspond Mdl information , 4byte|
Note voltage unit is mV,current unit is mA, power unit is mW,temperature unit is m

0x23 | Answer | 0x11 | 0x10 | | | | Mdl Status 2 | Mdl Status 1 | Mdl Status 0 |
0x23 | Answer | 0x11 | 0x11 | | | | Inverter status 2 | Inverter status 1 | Inverter status 0 |
Blank is 0, status of the bit meaning reference the "ALARM/STATUS" section

0x23 | Answer | 0x11 | 0x20 | | | | | Mdl group number, 2byte|
0x23 | Answer | 0x11 | 0x22 | | | | | DC software version, 2byte|
0x23 | Answer | 0x11 | 0x23 | | | | | PFC software version, 2byte|
Blank is 0

0x23 | Answer | 0x11 | 0x24 | | | Power module barcode information0, 4byte|
0x23 | Answer | 0x11 | 0x25 | | | Power module barcode information1, 4byte|
0x23 | Answer | 0x11 | 0x27 | | | Power module barcode information2, 4byte|
0x23 | Answer | 0x11 | 0x28 | | | Power module barcode information3, 4byte|
Blank is 0, analysis of power module barcode information reference "Module barcode information analysis" section 
# Example case
Ctrl Tx 02 A3 00 F0 11 01 00 00 00 00 00 00 : read Mdl #0 DC side voltage
Mdl Ans 02 A3 F0 00 11 01 00 00 00 07 a1 20 : Mdl #0 DC side voltage value is 500V
If the Mdl address is 0, and in the group 2, use the Group Device No.ask (all the Mdl in this group answerthe command with the Mdl address)

Ctrl Tx 02 E3 02 F0 11 03 00 00 00 00 00 00 : read group 2 Mdl AC line ABvoltage
Mdl Ans 02 E3 F0 00 11 03 00 00 00 04 BA F0 : Group 2 Mdl (add is 0) answer VAB=310V
# -------------
# -------------
# Ctrl Tx
0x23 | Read Fan Speed（rpm）| 0x12 | 0x01 | | | | | | |
0x23 | Read Power module Water inlet temperature（m°C）| 0x12 | 0x04 | | | | | | |
0x23 | Read Power module Water outlet temperature（m°C）| 0x12 | 0x05 | | | | | | |

# Mdl Ans
0x23 | Answer | 0x12 | 0x01 | | | Fan Speed（rpm）, 4byte|
0x23 | Answer | 0x12 | 0x04 | | | Power module Water inlet temperature（m°C）, 4byte|
0x23 | Answer | 0x12 | 0x05 | | | Power module Water outlet temperature（m°C）, 4byte|
Point to Point command, Mdl answer command;if group command, all the Mdl in Group answer the current validate value.
Note This command is only supported by some modules;
The unit of fan speed is rpm, which is the number of revolutions per minute.
# Example case
none

# -------------
# -------------
# Ctrl Tx
0x23 | Read Mdl N# AC side infomation| 0x21 | 0x01 | | | | | | |
0x23 | Read Mdl N# AC side infomation| 0x21 | 0x02 | | | | | | |
0x23 | Read Mdl N# AC side infomation| 0x21 | 0x03 | | | | | | |
0x23 | Read Mdl N# AC side infomation| 0x21 | 0x04 | | | | | | |
0x23 | Read Mdl N# AC side infomation| 0x21 | 0x05 | | | | | | |
0x23 | Read Mdl N# AC side infomation| 0x21 | 0x06 | | | | | | | 
0x23 | Read Mdl N# AC side infomation| 0x21 | 0x07 | | | | | | |
0x23 | Read Mdl N# AC side infomation| 0x21 | 0x08 | | | | | | |
0x23 | Read Mdl N# AC side infomation| 0x21 | 0x09 | | | | | | |
0x23 | Read Mdl N# AC side infomation| 0x21 | 0x0A | | | | | | |
0x23 | Read Mdl N# AC side infomation| 0x21 | 0x0B | | | | | | |
0x23 | Read Mdl N# AC side infomation| 0x21 | 0x10 | | | | | | |
0x23 | Read Mdl N# AC side infomation| 0x21 | 0x11 | | | | | | |
0x23 | Read Mdl N# AC side infomation| 0x21 | 0x12 | | | | | | |
0x23 | Read Mdl N# AC side infomation| 0x21 | 0x13 | | | | | | |

# Mdl Ans
0x23 | Answer | 0x21 | 0x01 | | | AC A phase Voltage（mV）, 4byte|
0x23 | Answer | 0x21 | 0x02 | | | AC B phase Voltage（mV）, 4byte|
0x23 | Answer | 0x21 | 0x03 | | | AC C phase Voltage（mV）, 4byte|
0x23 | Answer | 0x21 | 0x04 | | | AC A phase current（mA）, 4byte|
0x23 | Answer | 0x21 | 0x05 | | | AC B phase current（mA）, 4byte|
0x23 | Answer | 0x21 | 0x06 | | | AC C phase current（mA）, 4byte|
0x23 | Answer | 0x21 | 0x07 | | | AC frequency（mHz）, 4byte|
0x23 | Answer | 0x21 | 0x08 | | | Total active power（mW）, 4byte|
0x23 | Answer | 0x21 | 0x09 | | AC A phase active power（mW）, 4byte|
0x23 | Answer | 0x21 | 0x0A | | AC B phase active power（mW）, 4byte|
0x23 | Answer | 0x21 | 0x0B | | AC C phase active power（mW）, 4byte|
0x23 | Answer | 0x21 | 0x0C | | Total reactive power（mVA）, 4byte|
0x23 | Answer | 0x21 | 0x0D | | AC A phase reactive power(mVA), 4byte|
0x23 | Answer | 0x21 | 0x0E | | AC B phase reactive power(mVA), 4byte|
0x23 | Answer | 0x21 | 0x0F | | AC C phase reactive power(mVA), 4byte|
0x23 | Answer | 0x21 | 0x10 | | Total apparent power（mVA）, 4byte|
0x23 | Answer | 0x21 | 0x11 | | AC A phase apparent power(mVA), 4byte|
0x23 | Answer | 0x21 | 0x12 | | AC B phase apparent power(mVA), 4byte|
0x23 | Answer | 0x21 | 0x13 | | AC C phase apparent power(mVA), 4byte|
Point to Point command, Mdl answer command;if group command, all the Mdl in Group answer the current validate value.
Note voltage unit is mV,current unit is mA, power unit is mW,temperature unit is m℃℃,frequence unit is mHz
# Example case
Ctrl Tx 02 A3 00 F0 21 04 00 00 00 00 00 00 : read Mdl #0 A phase current information
Mdl Ans 02 A3 F0 00 21 04 00 00 00 00 13 88 : Mdl #0 current is 5A
If the Mdl address is 0, and in the group 2, use the Group Device No. command all the Mdl in this group answer the command with the Mdl address
Ctrl Tx 02 E3 02 F0 21 08 00 00 00 00 00 00 : read group 2 Mdl total active power value
Mdl Ans 02 E3 F0 00 21 08 00 00 00 E4 E1 C0 : group 2 Mdl add is 0 answer total active power is 15kW.
# -------------
# -------------
# Setting data field
# Ctrl Tx
0x24 | Setting system all Mdl output| 0x10 | 0x01 | | | System DC voltage mV, 4byte|
0x24 | Setting system all Mdl output| 0x10 | 0x02 | | | System DC current mA, 4byte|
Note
The destination address is 0x3F, if the device No.is 0x0A.
The destination address is group No.,if the device No.is 0x0B.
Voltage unit is mV,current unit is mA, and the current value is the all Mdls output current sum.
If the group setting is changed, it must wait for 4s ,after this change then can send this order

# Mdl Ans
Note
If it is broadcast command, only master Mdl answer the command, and the source address is 3F.
If it is group command, only the master Mdl in group answer the command, and the source address is group number.
The answer data is the current setting value.

# Example case
Ctrl Tx 02 A4 3F F0 10 01 00 00 00 0A D5 70 : setting the system DC voltage 710V.
Mdl Ans 02 A4 F0 3F 10 01 00 00 00 0A D5 70 : master Mdl answer the current setting voltage is 710V
If the Mdl address is 0#, the group number is use the group device number setting , the master Mdl inthe group will answer

Ctrl Tx 02 E4 02 F0 10 02 00 00 00 00 13 88 : setting group 2 Mdl total current is 5A
Mdl Ans 02 E4 F0 02 10 02 00 00 00 00 13 88 : master Mdl in group 2 answer the current setting value
# -------------
# -------------
# Ctrl Tx
0x24 | Setting single Mdl output| 0x11 | 0x01 | | | Mdl DC voltage mV, 4byte|
0x24 | Setting single Mdl output| 0x11 | 0x02 | | | Mdl DC current mA, 4byte|
Note
If the device number is 0A, if the desitination address in the command is 3F, means broadcast command, no answer frame.
if the desitination address in the command is single Mdl address, means P2P command, the certain Mdl receive and answer.
If the device number is 0B, the desitination address in the command is group number.
Voltage unit is mV,current unit is mA.

# Mdl Ans
broadcast command has not answer frame, P2P command has the answer.
If it is Group command , all Mdls in the group answer the current validate value.

# Example case
Ctrl Tx 02 A4 3F F0 11 01 00 00 00 0B 71 B0 : setting all the Mdls voltage is 750V
Mdl Ans : broadcast with no answer
Ctrl Tx 02 A4 00 F0 11 02 00 00 00 00 13 88 : setting Mdl0# current is 5A
Mdl Ans 02 A4 F0 00 11 02 00 00 00 00 13 88 : Mdl0# answer the current setting value
if the Mdl address is 0#, the group number is use the group device number setting , all Mdls in the group will answer with the Mdl address.
Ctrl Tx 02 E4 02 F0 11 01 00 00 00 0A D5 70 : setting group 2 Mdls voltage is 710V
Mdl Ans 02 E4 F0 00 11 01 00 00 00 0A D5 70 : master Mdl (address is 0) in the group answer

# -------------
# -------------
# Ctrl Tx
0x24 | Setting #N module group number | 0x11 | 0x03 | | | | | group number, 2byte|
Note
Group number start from 1.
N is destination address of the ID.
This command is applicable to modules that support group control.

# Mdl Ans
broadcast command has no answer frame, P2P command has the answer.
If it is Group command , all Mdls in the group answer the current validate value.

# Example case
Ctrl Tx 02 A4 3F F0 11 03 00 00 00 00 00 02 : setting all Mdls as Group 2.
Mdl Ans : broadcast with no answer
Ctrl Tx 02 A4 00 F0 11 03 00 00 00 00 00 03 : setting Mdl 0# as Group 3
Mdl Ans 02 A4 F0 00 11 03 00 00 00 00 00 03 : Mdl 0# answer the current Group is 3
if the Mdl address is 0#, the group number is use the group device number setting , all Mdls in the
group will answer with the Mdl address.
Ctrl Tx 02 E4 02 F0 11 03 00 00 00 00 00 05 : setting Mdls of Group 2 as Group 5
Mdl Ans 02 E4 F0 00 11 03 00 00 00 00 00 05 : Mdl(Group is 2 and Address is 0) answer the current Group is 5
# -------------
# -------------
# Ctrl Tx
0x24 | Setting all Mdl open or close | 0x11 | 0x10 | | | | | On/off, 2byte|
0xA1:Off 0xA0:On
Note
Broadcast command, no answer
If the device number is 0x0A, the Mdl address is 0x3F for the destination address in the command
If the device number is 0x0B, the destination address is the certain group number

# Mdl Ans
broadcast command has no answer frame, P2P command has the answer.
If it is Group command , all Mdls in the group answer the current validate value.

# Example case
Ctrl Tx 02 A4 3F F0 11 10 00 00 00 00 00 A1 : setting all Mdls Off
Mdl Ans : broadcast with no answer

Ctrl Tx 02 A4 00 F0 11 10 00 00 00 00 00 A0 : setting Mdl0# On
Mdl Ans 02 A4 F0 00 11 10 00 00 00 00 00 A0 : Mdl0# answer the current command is On
if the Mdl address is 0#, the group number is use the group device number setting , all Mdls in the group will answer with the Mdl address

Ctrl Tx 02 E4 02 F0 11 10 00 00 00 00 00 A1 : setting group 2 Mdls Off
Mdl Ans 02 E4 F0 00 11 10 00 00 00 00 00 A1 : Mast Mdl in the group(address is 0) answer current command

# -------------
# -------------
# Ctrl Tx
0x24 | Shielding module DC CAN data | 0x11 | 0x1E | | | | | CAN bus muting mode, 2byte|
0xA0: Normal communication(default);
0xA1: Shield module current sharing data;
0xA2: Shield communication between modules;

0x24 | Setting Mdl N green LED blink | 0x11 | 0x20 | | | | | Green led blink, 2byte|
Note
0xA1 blink, 0xA0 normal. N is in the destinaion address in the ID

0x24 | Setting Mdl N sleep | 0x11 | 0x21 | | | | | Sleep, 2byte|
Note
0xA1:sleep , 0x A0:not sleep.

0x24 | Setting Mdl N walkin | 0x11 | 0x22 | | | | | Walkin Enable, 2byte|
Note
0xA1 enable walkin, 0xA0 forbit walkin

# Mdl Ans
broadcast command has no answer frame, P2P command has the answer.
If it is Group command , all Mdls in the group answer the current validate value.

# Example case
Example with the Mdl N# green LED blink:
Ctrl Tx 02 A4 3F F0 11 20 00 00 00 00 00 A1 : setting all Mdls green LED bling
Mdl Ans : broadcast with no answer

Ctrl Tx 02 A4 00 F0 11 20 00 00 00 00 00 A0 : setting Mdl0# not blink
Mdl Ans 02 A4 F0 00 11 20 00 00 00 00 00 A0 : Mdl0# answer not blink
if the Mdl address is 0#, the group number is use the group device number setting , all Mdls in the
group will answer with the Mdl address

Ctrl Tx 02 E4 02 F0 11 20 00 00 00 00 00 A1 : setting group Mdls blink
Mdl Ans 02 E4 F0 00 11 20 00 00 00 00 00 A1 : Mast Mdl (address is 0) answer blink

# -------------
# -------------
# Ctrl Tx
0x24 | Setting a single module address allocation method | 0x11 | 0x23 | | | | | Address allocation method, 2byte|
0xA0: Automatic address allocation mode(default);
0xA1: Dialing mode;

# Mdl Ans
broadcast command has no answer frame, P2P command has the answer.
If it is Group command , all Mdls in the group answer the current validate value.

# Example case
Ctrl Tx 02 A4 3F F0 11 23 00 00 00 00 00 A1 setting all Mdls addresses to dialing mode
Mdl Ans : broadcast with no answer

# -------------
# -------------
# Ctrl Tx
0x24 | Setting Mdl N DC High or low Voltage mode | 0x11 | 0x26 | | | | | DC side output high /low voltage mode, 2byte|
0xA0: Low voltage mode;
0xA1: High voltage mode(default);
0xA2: Module automatic control switching;
Note
(1) This command is only applicable to modules that output serial parallel mode, The serial/parallel switching points of each module may be different. Set valid in standby mode.
(2)Low voltage mode - output relay parallel mode;
High voltage mode - output relay series mode;
Automatic switching mode: The rectifier switches between series and parallel modes based on the set voltage, while the inverter switches between series and parallel modes based on the port voltage.
(3)When manually set to high voltage mode, but the actual output voltage is lower than serial/parallel switching points, the module cannot output full power, and the maximum current will be limited.
(4) When manually set to low-voltage mode, the actual output voltage cannot exceed serial/parallel switching points.If the external high voltage above serial/parallel switching points + A certain voltage is accidentally connected, the module will give an overvoltage alarm, and in extreme cases, the internal components of the module will be damaged.

# Mdl Ans
broadcast command has not answer frame, P2P command has the answer.
If it is Group command , all Mdls in the group answer the current validate value.

# Example case
Ctrl Tx 02 A4 3F F0 11 26 00 00 00 00 00 A1 : setting all Mdls High voltage mode
Mdl Ans : broadcast with no answer

Ctrl Tx 02 A4 00 F0 11 26 00 00 00 00 00 A2 : setting Mdl0# Module automatic control switching 
Mdl Ans 02 A4 F0 00 11 26 00 00 00 00 00 A2 : Mdl0# answer current value.
if the Mdl address is 0#, the group number is use the group device number setting , all Mdls in the group will answer with the Mdl address

Ctrl Tx 02 E4 02 F0 11 26 00 00 00 00 00 A1 : setting group Mdls High voltage mode.
Mdl Ans 02 E4 F0 00 11 26 00 00 00 00 00 A1 : Mdl (address is 0) answer High voltage mode.

# -------------
# -------------
# Ctrl Tx
0x24 | Setting Mdl N DC DisCharge cut-off voltage | 0x11 | 0x32 | | | DC DisCharge cut-off voltage (mV), 4byte|
Note
Voltage unit is mV,just for inverter Dc Discharge cut-off voltage

# Mdl Ans
broadcast command has not answer frame, P2P command has the answer.
If it is Group command , all Mdls in the group answer the current validate value.

# Example case
Ctrl Tx 02 A4 00 F0 11 32 00 00 00 04 93 E0 : setting Mdl0# Dc Discharge cut-off voltage is 300V
Mdl Ans 02 A4 F0 00 11 32 00 00 00 04 93 E0 : Mdl0# answer the current setting value
if the Mdl address is 0#, the group number is use the group device number setting , all Mdls in the
group will answer with the Mdl address.

Ctrl Tx 02 E4 02 F0 11 32 00 00 00 0A D5 70 : setting group 2 Mdls Dc Discharge cut-off voltage is 710V
Mdl Ans 02 E4 F0 00 11 32 00 00 00 0A D5 70 : Mdl (address is 0) in the group answer.

# -------------
# -------------
# Ctrl Tx
0x24 | Setting Noise Reduction Mode | 0x11 | 0x33 | | | | | Noise Reduction Mode, 2byte|
0xA0: Power mode(default);
0xA1: Noise reduction mode;
0xA2: Silent mode;

# Mdl ans
None

# Example case
None

# -------------
# -------------
# Ctrl Tx
0x24 | Setting Mdl N automatic voltage stabilization mode | 0x11 | 0x34 | | | | | automatic voltage stabilization mode, 2byte|
A0: disable port automatic voltage stabilization mode (default), following the command number 0x2110 mode setting
A1: port Automatic voltage stabilization mode
Note
The port automatic voltage stabilization mode will automatically switch between rectifier and grid inverter modes based on changes in port voltage. Set valid in standby mode.

# Mdl ans
broadcast command has not answer frame, P2P command has the answer.
If it is Group command , all Mdls in the group answer the current validate value.

# Example case
Ctrl Tx 02 A4 3F F0 11 34 00 00 00 00 00 A1 : setting all Mdls automatic voltage stabilization mode
Mdl Ans : broadcast command with no answer
Ctrl Tx 02 A4 00 F0 11 34 00 00 00 00 00 A0 : setting Mdl0# disable automatic voltage stabilization
Mdl Ans 02 A4 F0 00 11 34 00 00 00 00 00 A0 : Mdl0# answer current value

# -------------
# -------------
# Ctrl Tx
0x24 | Setting Mdl N OffGrid Voltage | 0x21 | 0x06 | | | OffGrid AC Voltage Set (mV), 4byte|
Unit: mV;
Note
Currently, only offGrid voltages 220Vac and 230Vac are supported (line voltages 380Vac and 400Vac)
The voltage and frequency settings for off grid should also refer to the precautions for frequency settings.
The default is 220Vac. If you only use 220Vac, you do not need to set this command.

0x24 | Setting Mdl N OffGrid frequency | 0x21 | 0x07 | | | OffGrid AC frequency Set (mHz), 4byte|
Unit: mHz;
Note
It needs to be set when the module is shut down, and it needs to be powered on again. Only 50Hz and 60Hz can be set.
The system parallel module needs to ensure that all modules are set to the same off grid voltage and frequency during shutdown, and then re powered on after modification.
The default output format of the module is 220Vac/50Hz
The supported format is 220Vac/50Hz; 220Vac/60Hz; 230Vac/50Hz; 230Vac/60Hz.

# Mdl ans
broadcast command has not answer frame, P2P command has the answer.
If it is Group command , all Mdls in the group answer the current validate value.

# Example case
Ctrl Tx: 02 A4 00 F0 21 06 00 00 00 03 5B 60 : setting Mdl0# offgrid AC voltage 220Vac.
Mdl Ans: 02 A4 F0 00 21 06 00 00 00 03 5B 60 : Mdl0# answer current value.
Ctrl Tx: 02 A4 00 F0 21 06 00 00 00 03 82 70 : setting Mdl0#offgrid AC voltage 230Vac.
Mdl Ans: 02 A4 F0 00 21 06 00 00 00 03 82 70 : Mdl0# answer current value.
Ctrl Tx: 02 A4 00 F0 21 07 00 00 00 00 C3 50 : setting Mdl0# offgrid AC frequency 50Hz.
Mdl Ans: 02 A4 F0 00 21 07 00 00 00 00 C3 50 : Mdl0# answer current value.
Ctrl Tx: 02 A4 00 F0 21 07 00 00 00 00 EA 60 : setting Mdl0# offgrid AC frequency 60Hz.
Mdl Ans: 02 A4 F0 00 21 07 00 00 00 00 EA 60 : Mdl0# answer current value.

# -------------
# -------------
# Ctrl Tx
0x24 | Setting Mdl N working mode | 0x21 | 0x10 | | | | | Working mode, 2byte|
Note
0xA0-rectification default
0xA1-grid connected inverter
0xA2-off grid inverter

0x24 | Setting Mdl N to Turn off islanding detection | 0x21 | 0x14 | | | | | Turn off islanding detection, 2byte|
Note
0xA0- Turn on islanding detection
0xA1- Turn off islanding detection (default)

# Mdl ans
broadcast command has no answer frame, P2P command has the answer.
If it is Group command , all Mdls in the group answer the current validate value.

# Example case
Ctrl Tx 02 A4 3F F0 21 10 00 00 00 00 00 A1 : setting all Mdls is grid connected inverter working mode
Mdl Ans : broadcast command with no answer

Ctrl Tx 02 A4 00 F0 21 10 00 00 00 00 00 A0 : setting Mdl0# is rectifier working mode
Mdl Ans 02 A4 F0 00 21 10 00 00 00 00 00 A0 : Mdl0# answer current value

# -------------
# -------------
# Ctrl Tx
0x24 | Setting the AC side protection value and time | 0x21 | 0x20 ~ 31 | | | All the setting value, 4byte|
Note protection value unit is mV protection time is mS.

# Mdl ans
broadcast command has no answer frame, P2P command has the answer.
If it is Group command , all Mdls in the group answer the current validate value.

# Example case
Ctrl Tx 02 A4 3F F0 21 20 00 00 00 03 D0 90 : setting all Mdls level 1 over voltage protection point is 250V
Mdl Ans : broadcast with no answer
Ctrl Tx 02 A4 00 F0 21 21 00 00 00 00 07 D0 : setting Mdl0# level 1 over voltage protection time is 2S
Mdl Ans 02 A4 F0 00 21 21 00 00 00 00 07 D0 : Mdl0# answer the current value

# -------------
# -------------
# Ctrl Tx
0x24 | Set AC side Reactive Power value | 0x21 | 0x08 | | | Set AC side Reactive Power value (mVar), 4byte|
Note : the unit is mVar.

# Mdl ans
broadcast command has no answer frame, P2P command has the answer.
If it is Group command , all Mdls in the group answer the current validate value.

# Example case
Ctrl Tx 02 A4 00 F0 21 08 00 00 00 0F 42 40 : setting Mdl0# Reactive Power output 1000Var
Mdl Ans 02 A4 F0 00 21 08 00 00 00 0F 42 40 : Mdl0# answer the current value

# -------------
# -------------
# Ctrl Tx
0x24 | Set AC side Reactive Power Set Type | 0x21 | 0x17 | | | | | Reactive Power setting Type, 2byte|
Note
The bidirectional ACDC module provides different ways to set the reactive Power mode according to different customer usage habits, if you want to set the Ac reactive Power, you must first set this command to select the setting type mode, and then use the corresponding small class command to set.
This protocol can be ignored if there is no reactive power setup requirement.

0xA0: Reactive power output is not set (default).
0xA1: By setting Reactive power with the PF setup, the corresponding small class command is 0x21 05
0xA2: By setting Reactive power with reactive power,The corresponding small class command is 0x21 08
0xA4: Automatically adjust reactive power mode through cos (P) - VDE4105 requirements
0xA5:Automatically adjust reactive power mode through Q (U) - VDE4105/UL1741SA/IEEE1547 Requirements
0xA6: Automatically adjust reactive power mode through Q (P) - IEEE1547 requirements

# Mdl ans
broadcast command has no answer frame, P2P command has the answer.
If it is Group command , all Mdls in the group answer the current validate value.

# Example case
Ctrl Tx 02 A4 3F F0 21 17 00 00 00 00 00 A0 : setting all Mdls : don’t set he reactive power output
Mdl Ans : broadcast with no answer
Ctrl Tx 02 A4 00 F0 21 17 00 00 00 00 00 A2 : setting Mdl0# : By setting the command with reactive Power ((0x21 08)) to set
Mdl Ans 02 A4 F0 00 21 21 00 00 00 00 00 A2 : Mdl0# answer the current value
Ctrl Tx 02 A4 00 F0 21 17 00 00 00 00 00 A1 : setting Mdl0# : By setting the command with the PF setup (0x21 05) to set
Mdl Ans 02 A4 F0 00 21 17 00 00 00 00 00 A1 : Mdl0# answer the current value

# -------------
# -------------
# Ctrl Tx
0x24 | Setting Mdl N Energy storage mode | 0x21 | 0x18 | | | | | Energy storage mode, 2byte|
0xA0-disable Energy storage mode (default).
0xA1-Enable Energy storage mode

0x24 | Setting Mdl N Power regulation mode | 0x31 | 0x11 | | | | | Power regulation mode (AC/DC regulation), 2byte|
0xA0-DC regultion mode(default)
0xA1-AC regultion mode
Note
The setting of this function is only supported in energy storage mode. 
The DC power regulation mode does not control the active power on the AC side, controls the voltage, current, and power on the battery side. 
The AC power scheduling mode mainly controls the active and reactive power on the AC side.

# Mdl ans
None
# Example case
None

# -------------
# -------------
# Ctrl Tx
0x24 | Setting Mdl N low voltage ride- through | 0x31 | 0xD0 | | | | | low voltage ride- through (LVRT), 2byte|
0xA0-disable low voltage ride through(default);
0xA1-Enable low voltage ride through

0x24 | Setting Mdl N High voltage ride- through | 0x31 | 0xD1 | | | | | High voltage ride- through (HVRT), 2byte|
0xA0-disable High voltage ride through(default);
0xA1-Enable High voltage ride through

# Mdl ans
broadcast command has no answer frame, P2P command has the answer.
If it is Group command , all Mdls in the group answer the current validate value.

# Example case
Ctrl Tx: 02 A4 3F F0 21 18 00 00 00 00 00 A1 : Set all Mdls Energy storage mode
Mdl Ans: broadcast with no answer
Ctrl Tx: 02 A4 00 F0 31 11 00 00 00 00 00 A1 : set mdl 0# AC regulation mode.
Mdl Ans: 02 A4 F0 00 31 11 00 00 00 00 00 A1 : mdl 0# answer the current value
Ctrl Tx: 02 A4 00 F0 31 11 00 00 00 00 00 A0 : set mdl 0# DC regulation mode.
Mdl Ans: 02 A4 F0 00 31 11 00 00 00 00 00 A1 : mdl 0# answer the current value
Ctrl Tx: 02 A4 00 F0 31 D0 00 00 00 00 00 A0 : set mdl 0# disable LVRT
Mdl Ans: 02 A4 F0 00 31 D0 00 00 00 00 00 A0 : mdl 0# answer the current value
Ctrl Tx: 02 A4 00 F0 31 D1 00 00 00 00 00 A1 : set mdl 0# enable HVRT
Mdl Ans: 02 A4 F0 00 31 D1 00 00 00 00 00 A1 : mdl 0#answer the current value

# -------------
# -------------
# Ctrl Tx
0x24 | Setting Mdl N AC Active Power | 0x31 | 0x23 | | | AC Active Power setting(mW), 4byte|
Unit : mW
Note
This AC side active power setting function only supports setting in energy storage mode and AC power regultion mode;
This function is only applicable to grid connected inverter and rectifier modes.

0x24 | Setting Mdl N Active Power Fall gradient | 0x31 | 0x24 | | | AC Active power fall gradient(%Prate/min), 4byte|
Unit :%Prate/min

0x24 | Setting Mdl N Active Power rise gradient | 0x31 | 0x25 | | | AC Active power rise gradient(%Prate/min), 4byte|
Unit :%Prate/min

# Mdl ans
broadcast command has no answer frame, P2P command has the answer.
If it is Group command , all Mdls in the group answer the current validate value.

# Example case
Ctrl Tx: 02 A4 00 F0 31 23 00 00 01 4F B1 80 : set mdl 0# AC Active Power 22000W
(22000 * 1000mW =22000 * 1000 Convert decimal to hexadecimal 0x014F B180)
Mdl Ans: 02 A4 F0 00 31 23 00 00 01 4F B1 80 : mdl 0#answer the current value
Ctrl Tx: 02 A4 00 F0 31 23 00 00 00 4C 4B 40 : set mdl 0# AC Active Power 5000W
(5000 * 1000mW =5000 * 1000 Convert decimal to hexadecimal 0x4C 4B40)
Mdl Ans: 02 A4 F0 00 31 23 00 00 00 4C 4B 40 : mdl 0#answer the current value

Ctrl Tx: 02 A4 00 F0 31 24 00 00 00 00 0B B8 : set mdl0# AC Active power fall gradient
3000%Prate/min, is 50%Prate/s, 100%Prate reduce to min power is 2s.
Mdl Ans: 02 A4 F0 00 31 24 00 00 00 00 0B B8 : mdl 0#answer the current value
Ctrl Tx: 02 A4 00 F0 31 24 00 00 00 00 00 28 : et mdl0# AC Active power fall gradient 40%Prate/min
100%Prate reduce to min power is 2.5min.(40 Convert decimal to hexadecimal 0x28)
Mdl Ans: 02 A4 F0 00 31 24 00 00 00 00 00 28 : mdl 0#answer the current value

Ctrl Tx: 02 A4 00 F0 31 25 00 00 00 00 0B B8 : set mdl0# AC Active power rise gradient
3000%Prate/min, is 50%Prate/s, 100%Prate Start time is 2s.
Mdl Ans: 02 A4 F0 00 31 25 00 00 00 00 0B B8 : mdl 0#answer the current value
Ctrl Tx: 02 A4 00 F0 31 25 00 00 00 00 00 28 : set mdl0# AC Active power rise gradient
40%Prate/min, 100%Prate Start time is 2.5min. (40 Convert decimal to hexadecimal 0x28)
Mdl Ans: 02 A4 F0 00 31 25 00 00 00 00 00 28 : mdl 0#answer the current value

# -------------
# -------------
NOTES
for the certain group command, the device number is 0x0B, the destination address is group number.
If the destination address is 3F, means to all groups, for the emergency situation to close all Mdls. 
The read command only support system data read, and the setting command only support system command and On/off command.
NOTES if the frame identifier data is 0x0757F8XX, means the data is the Mdls inner datatransfer, need ignored by the upper controller.

# ALARM/STATUS
Command | Write / Read | Byte 0 | Byte 1 | Byte 2 | Byte 3 | Byte 4 | Byte 5 | Byte 6 | Byte 7 |
0x23 | Read | 0x11 | 0x10 | | | Module status 3 | Module status 2 | Module status 1 | Module status 0 |
0x23 | Read | 0x11 | 0x11 | | | | Inverter status 2 | Inverter status 1 | Inverter status 0 |

# ALARM / STATUS DATA Detail (Module status)
0x23 | 0x11 | 0x 10 | 0x00 | 0x00 | Module status 3 | Module status 2 | Module status 1 | Module status 0 |
# Module status 3
Bit 7 : none
Bit 6 : none
Bit 5 : none
Bit 4 : none
Bit 3 : none
Bit 2 : none
Bit 1,0 : (2bit) 
0(0b00): Not supporting series parallel mode
1(0b01): Module automatic control
2(0b10): Parallel mode (Low voltage mode)
3(0b11): Series mode (High voltage mode)

# Module status 2
Bit 7 : set(1) : Mdl PFC side off
Bit 6 : set(1) : input over voltage protection
Bit 5 : set(1) : input low voltage protection
Bit 4 : set(1) : 3 phase input unbalance alarm
Bit 3 : set(1) : 3 phase input phase lost alarm
Bit 2 : set(1) : Mdl load sharing alarm
Bit 1 : set(1) : Mdl ID repeat alarm
Bit 0 : set(1) : Mdl power limit status

# Module status 1
Bit 7 : set(1) : Mdl communication interrupt alarm
Bit 6 : set(1) : WALK-IN enable
Bit 5 : set(1) : ouput over voltage alarm
Bit 4 : set(1) : over temperature alarm
Bit 3 : set(1) : Fan fault alarm
Bit 2 : set(1) : Mdl protection alarm
Bit 1 : set(1) : Mdl fault alarm
Bit 0 : set(1) : Mdl DC side off status

# Module status 0
Bit 7 : none
Bit 6 : none
Bit 5 : set(1) : Mdl discharge abnormal
Bit 4 : set(1) : Mdl sleeping
Bit 3 : none
Bit 2 : none
Bit 1 : none
Bit 0 : set(1) : output short current

# -----------
# ALARM / STATUS DATA Detail (Inverter status)
0x23 | 0x11 | 0x 11 | 0x00 | 0x00 | 0x00 | Module status 2 | Module status 1 | Module status 0 |

# Inverter status 2
Bit 7 : none
Bit 6 : none
Bit 5 : none
Bit 4 : set(1) : Estop
Bit 3 : set(1) : Offgrid mode with grid
Bit 2 : none
Bit 1 : set(1) : offgrid AC over load
Bit 0 : set(1) : offgrid inverter, reset(0) : grid inverter

# Inverter status 1 (reserved) 
Bit 7 : none
Bit 6 : none
Bit 5 : none
Bit 4 : none
Bit 3 : none
Bit 2 : none
Bit 1 : none
Bit 0 : none

# Inverter status 0
Bit 7 : none
Bit 6 : set(1) : island
Bit 5 : set(1) : PLL Fault
Bit 4 : set(1) : AC Fail
Bit 3 : none
Bit 2 : none
Bit 1 : set(1) : UV Vo in Grid inverter
Bit 0 : set(1) : invert mode, reset(0) : rectifier mode

# ------------
# ALARM / STATUS DATA Detail (Working Mode Status)
"Inverter status 2.Bit 0 == 0 (grid inverter)" && "Inverter status 0.Bit 0 == 0 (rectifier mode)"    : Working mode = rectifier mode
"Inverter status 2.Bit 0 == 0 (grid inverter)" && "Inverter status 0.Bit 0 == 1 (invert mode)"       : Working mode = invert mode
"Inverter status 2.Bit 0 == 1 (offgrid inverter)" && "Inverter status 0.Bit 0 == 1 (invert mode)"    : Working mode = offgrid inverter mode
"Inverter status 2.Bit 0 == 1 (offgrid inverter)" && "Inverter status 0.Bit 0 == 0 (rectifier mode)" : Working mode = None


# ------------
# Mdl on/off control
After the upper controller send the Mdl turn on command need keep the communication, like to send the setting command or read the system data command in series, if the Mdl cannot get the command from the upper controller more than 10s, the Mdl will have the communication fault alarm and trun off auotmaticly.
If the Mdl is on before the communication interrupt, when the communication recovery again, the Mdl will trun on automaticly and output the default setting output voltage and current. But if the Mdl is offbefore the communication interrupt, when the communication recovery again, it will keep off till the new trun on command sent by the upper controller.
Communication interval time: the controller order interval time should between 50~200ms.

# Mdl working mode setting
Bidirectional power module can working in several mode, like rectification or grid connection inverter mode, this mode will storage into the EEPROM before trun on the Mdl, the upper controller should do the mode setting firstly to keep the right current flow direction.

# Mdl walk in
Walk in function can make the output current start from zero to rated in the appoint time to avoid the impact to the input source. 
The default time is 5S, and enable. 
The upper controller also can instead this function by send the current limit command step by step.

The standard Mdl turn on time sequence : Power on --> setting the working mode --> setting the Mdl output voltage and current --> switch on the system output contactor --> Mdl open.
The standard Mdl trun off sequence : Mdl close --> switch off the system output contactor.

# Mdl Output voltage and current setting
Different Mdl have different voltage and current setting scale, if the setting value exceed the Mdl accept scale, the command will not implement. 
And the value will still use the before valid setting value(if not setting before, will use the default value).
If the system has the Mdl protection and fault alarm, can by the command 02 A3 xx F0 11 10 00 00 00 00 00 00 and 02 A3 xx F0 11 11 00 00 00 00 00 00 (xx is the Mdl address) to get the Mdl status, this Mdl will not implement the open command and will not been calculated as the part of the system total current average setting.
It can control the Mdl’s voltage and current by the command of system total current setting and voltage setting. To fit different control way also can use the single Mdl control command to control the system total Mdls or Groups inner all Mdls or single Mdl’s voltage and current. the upper controller needs to select the right command to setting the ouput current.

# Mdl Address and Group Number
There are two ways to obtain the address of a module:
# 1. Automatic allocation method (default):
After the module is powered on, it will automatically assign an address, which is assigned according to the internal factory serial number of the module. Therefore, as long as the module is not replaced, inserted with a new module, or unplugged, the address of the module will be the same every time it is powered on. The group number of the module is set through the dialing on the panel. When leaving the factory, all dialing are set to 0, so the grouping of all modules defaults to group 0. For modules without dialing , the module group number can be set using the command "02 A4 xx F0 11 03 00 00 00 yy yy" (where xx is the module address and yy yy is group number), but the group number is not saved to EEPROM, so it needs to be reset every time it is powered on.
Due to the automatic allocation of module addresses, group addresses can be manually set. To query the module addresses within a group, the following method can be used: after the module address allocation is completed (about 10 seconds later), monitor and issue the command "02 A3 xx F0 11 20 00 00 00 00 00" to poll the module status (where xx is the module address), and query module group number. After determining the module address within the group, if the module and dialing settings have not been replaced, there is no need to perform group number address matching during subsequent power on.

# 2. Dialing method (valid only for modules that support dialing function)
The address of the module is determined by dialing. 
To replace or insert a new module, it is necessary to operate the dialing and set it to the desired address. When leaving the factory, the dialing switches are all set to 0, so the address defaults to 0 in dialing mode. The module group number is set through "02 A4 xx F0 11 03 00 00 00 00 yy yy" (where xx is the module address and yy yy is the set group number), but the group number is not saved to the EEPROM, so it needs to be reset every time it is powered on. When using the dialing method, it is necessary to set and confirm the group number of the module.
To query the module addresses within a group, the following method can be used: after the module address allocation is completed (about 10 seconds later), monitor and issue the command "02 A3 xx F0 11 20 00 00 00 00 00" to poll the module status (where xx is the module address), and query module group number. After determining the module address within the group, if the module and dialing settings have not been replaced, there is no need to perform group number address matching during subsequent power on.
When there is a module address conflict on the system, the module with address conflict lights up red and reports an address conflict fault. After removing the conflicting address, normal operation can be restored.

# Mdl sleep function
Sleeping function is used to improve the Mdl power transfer efficiency when the low load.
the sleep command can be enable or disable ,The sleep Mdl is in the stand by status, and can not open by the turn on command. And also not join the total current calculate. So need to do the sleep setting before the current limit command send.
For example, the system has 6 Mdls, address is from 0 to 5, and the load requirement is 40A If the upper controller want to let address 0 and 1 Mdls go into sleep mode to improve the efficience, need to send the command ”02 A4 00 F0 11 21 00 00 00 00 00 A1” and 02 A4 01 F0 11 21 00 00 00 00 00 A1” ahead to let the Mdl0 and Mdl1 to sleep, and then by the command of system total current to send 40A. and then send the system Mdls turn on command. This will let the Mdl 0 and 1 close, and Mdle 2~5 open each Modle will output 10A, total 40A.

# CAN communication hardware wiring
To avoid the disturb, system CAN wiring should far away from the power cable, and use the twisted pair cable with the shielding layer. And use the 120ΩΩmatched resistance on the cable two ends.
The instructions interval of sending to module from monitoring is recommended to be 50 to 200ms , greater than 20ms.

# Module barcode information analysis
Issue the read command "02 A3 xx F0 11 2y 00 00 00 00 00 00" to read the barcode of the xx module Information (xx is the module address), The values of y are 4, 5, 7, and 8, and there are a total of 4 protocols used to obtain the barcode information of the module.
The meaning is:
|Byte 0|Byte 1|Byte 2|Byte 3|Byte 4|Byte 5|Byte 6|Byte 7|
|0x11|0x24|0x00|0x00|Reserved|Reserved|Factory number + version(letter)|version (number)|
|0x11|0x25|0x00|0x00|(Year - 2000)|Month|serial number,2byte|
|0x11|0x27|0x00|0x00|Type (letter)ASCII|Type (number 1)ASCII|Type (number 2)ASCII|Type (number 3)ASCII|
|0x11|0x28|0x00|0x00|Type (number 4)ASCII|Type (number 5)ASCII|Type (number 6)ASCII|Type (number 7)ASCII|

# Explain:
(1) The range of version numbers (letters) is 0-15, where 0 corresponds to the letter A, and so on;
(2) Protocol 0x23_0x1127 and 0x23_0x1128 are parsed using ASCII code;
For example:
According to the response from module 0 #, the barcode information is analyzed as follows:

Response to the first agreement: “02 A3 F0 00      11 24 00 00 00 00 E0 02”
Analysis value : 
    (E (At byte7))Factory number : 14
    (0 (At byte7))Version (letter) : A
    (02(At byte8))Version (number) : 0 2

Response to the Second agreement: “02 A3 F0 00      11 25 00 00 18 31 08 58”
Analysis value : 
    (18 (At byte5)) Year - 2000 : 24 (that is 2024)
    (31 (At byte6)) Month : (3)March, 1(Section Divider)
    (08 58 (At byte7,8)) serial number : 02136th one

Response to the Third agreement:“02 A3 F0 00      11 27 00 00 4E 32 30 30”
Analysis value : 
    (4E (At byte5)) Type (letter)ASCII : 'N'
    (32 (At byte6)) Type (number 1)ASCII : '2'
    (30 (At byte7)) Type (number 2)ASCII : '0'
    (30 (At byte8)) Type (number 3)ASCII : '0'

Response to the Fourth agreement:“02 A3 F0 00      11 28 00 00 31 30 30 32”
Analysis value : 
    (31 (At byte5)) Type (number 4)ASCII : '1'
    (30 (At byte6)) Type (number 5)ASCII : '0'
    (30 (At byte7)) Type (number 6)ASCII : '0'
    (32 (At byte8)) Type (number 7)ASCII : '2'

Combine the information analyzed above into a barcode:
Barcode : 14A022403102136N2001002
            Factory : 14
            Version : A02
            Date : 2024 03
            Serial No : 02136
            Type : N2001002

# -------------
# -------------
# End of File
