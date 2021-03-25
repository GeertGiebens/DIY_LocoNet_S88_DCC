

;***************************************************************************
;* LocoNet In/out module met PIC 18F4620 microcontroller                   *
;*                                                                         *
;* Datum: 01 FEB 2021                     Versie LOCONET INPUT V1.0        *
;*                                                                         *
;* Filenaam: LOCONET_IO_VARIABELEN_INPUT_V1p0.INC                          *
;*                                                                         *
;***************************************************************************


;************  POORTEN   ********************************************

	#DEFINE DIP9   PORTE,RE3  
	#DEFINE DIP10  PORTA,RA1  
	#DEFINE DIP11   PORTA,RA2  
	
	;       LED1 =+5V
	#DEFINE LED2     PORTA,RA5
	#DEFINE LED3     PORTE,RE0
	#DEFINE LED4     PORTE,RE1
	#DEFINE LED5     PORTE,RE2

	#DEFINE ADDRESS1 PORTA,RA7
	#DEFINE ADDRESS2 PORTA,RA6
	#DEFINE ADDRESS3 PORTC,RC0
	#DEFINE ADDRESS4 PORTC,RC1
	#DEFINE ADDRESS5 PORTC,RC2
	#DEFINE ADDRESS6 PORTC,RC3
	#DEFINE ADDRESS7 PORTD,RD0
	#DEFINE ADDRESS8 PORTD,RD1

	#DEFINE POORT1  PORTD,RD2
	#DEFINE POORT2  PORTD,RD3
	#DEFINE POORT3  PORTC,RC4
	#DEFINE POORT4  PORTC,RC5
	#DEFINE POORT5  PORTD,RD4
	#DEFINE POORT6  PORTD,RD5
	#DEFINE POORT7  PORTD,RD6
	#DEFINE POORT8  PORTD,RD7

	#DEFINE POORT9  PORTB,RB0
	#DEFINE POORT10 PORTB,RB1
	#DEFINE POORT11 PORTB,RB2
	#DEFINE POORT12 PORTB,RB3
	#DEFINE POORT13 PORTB,RB4
	#DEFINE POORT14 PORTB,RB5
	#DEFINE POORT15 PORTB,RB6
	#DEFINE POORT16 PORTB,RB7


;************ VARIABELEN ********************************************

;RAM geheugen indeling:

; BANK0  00-h6F  algemene variabelen
; BANK0  h70-h7F LocoNet variabelen
; BANK1: 0-255   TE VERZENDEN DATA LocoNet(RING BUFFER 256 BYTES)
; BANK3  128-255 RECEIVE BUFFER LocoNet(128 BYTES)
; BANK3  000-127 TRANSMIT BUFFER LocoNet(128 BYTES)



	cblock 0x00	;algemene variabelen

COUNTER_INPUT1	    ;telt 0-255
COUNTER_INPUT2	    ;telt 0-255
COUNTER_INPUT3	    ;telt 0-255
COUNTER_INPUT4	    ;telt 0-255
COUNTER_INPUT5	    ;telt 0-255
COUNTER_INPUT6	    ;telt 0-255
COUNTER_INPUT7	    ;telt 0-255
COUNTER_INPUT8	    ;telt 0-255
COUNTER_INPUT9	    ;telt 0-255
COUNTER_INPUT10	    ;telt 0-255
COUNTER_INPUT11	    ;telt 0-255
COUNTER_INPUT12	    ;telt 0-255
COUNTER_INPUT13	    ;telt 0-255
COUNTER_INPUT14	    ;telt 0-255
COUNTER_INPUT15	    ;telt 0-255
COUNTER_INPUT16	    ;telt 0-255
	
STATUS_INPUT1
STATUS_INPUT2	
STATUS_INPUT3	
STATUS_INPUT4	
STATUS_INPUT5	
STATUS_INPUT6	
STATUS_INPUT7	
STATUS_INPUT8	
STATUS_INPUT9	
STATUS_INPUT10	
STATUS_INPUT11	
STATUS_INPUT12	
STATUS_INPUT13	
STATUS_INPUT14	
STATUS_INPUT15	
STATUS_INPUT16	
	
TEL_LED2
TEL_LED3
TEL_LED4
TEL_LED5
	
TE_VERZENDEN_BYTE1  ;gebruikt om data op te slaan in te verzenden data buffer
TE_VERZENDEN_BYTE2
TE_VERZENDEN_BYTE3
TE_VERZENDEN_BYTE4
TE_VERZENDEN_BYTE5
TE_VERZENDEN_BYTE6
TE_VERZENDEN_BYTE7
TE_VERZENDEN_BYTE8
TE_VERZENDEN_BYTE9
TE_VERZENDEN_BYTE10
TE_VERZENDEN_BYTE11
TE_VERZENDEN_BYTE12
TE_VERZENDEN_BYTE13
TE_VERZENDEN_BYTE14
TE_VERZENDEN_BYTE15
TE_VERZENDEN_EXORBYTE
AANTAL_BYTES        ;aantal bytes te verzenden (is eerste byte in tabel daarna te verzenden bytes
POSITIE_TELLER_IN   ;waar staat pointer bij data in 0-->255    
POSITIE_TELLER_OUT   ;waar staat pointer bij data out
VERSCHIL_TELLER     ;verschil tussen POSITIE_TELLER_OUT en POSITIE_TELLER_IN mag niet meer worden dan 255
HULPTEL


;Variabelen gebruikt bij decodeer Loconet opdracht:

LN_BYTE_RECEIVE1
LN_BYTE_RECEIVE2
LN_BYTE_RECEIVE3
LN_BYTE_RECEIVE4
LN_BYTE_RECEIVE5
LN_BYTE_RECEIVE6
LN_BYTE_RECEIVE7
LN_BYTE_RECEIVE8
LN_BYTE_RECEIVE9
LN_BYTE_RECEIVE10
LN_BYTE_RECEIVE11
LN_BYTE_RECEIVE12
LN_BYTE_RECEIVE13
LN_BYTE_RECEIVE14
LN_BYTE_RECEIVE15



;cblock 0x70-0x7F -->  LocoNet variabele zie LOCONET_18F4X20_V1p2.INC

	endc

;********************************************************************
;************ TABELLEN **********************************************
;********************************************************************

    CBLOCK 0x100
TE_VERZENDEN_DATA       ;256 BYTES
    ENDC



;	cblock 0x300
;LN_TRANSMIT_BUFFER			; LocoNet Transmit buffer zie LOCONET_18F4X20_INPUT_V1p0.INC
;	endc

;	cblock 0x380
;LN_RECEIVE_BUFFER			; LocoNet Receive buffer zie LOCONET_18F4X20_INPUT_V1p0.INC
;	endc


