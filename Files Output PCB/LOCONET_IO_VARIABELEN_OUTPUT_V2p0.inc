

;***************************************************************************
;* LocoNet In/out module met PIC 18F4620 microcontroller                   *
;*                                                                         *
;* Datum: 06 MEI 2023                     Versie LOCONET OUTPUT V2.0        *
;*                                                                         *
;* Filenaam: LOCONET_IO_VARIABELEN_OUTPUT_V2p0.INC                         *
;*                                                                         *
;***************************************************************************


;************  POORTEN   ********************************************

	#DEFINE DIP9   PORTE,RE3    
	#DEFINE DIP10  PORTA,RA1  
	#DEFINE DIP11   PORTA,RA2  
	
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
	
	#DEFINE SV 	PORTD,RD1
	
	#DEFINE POORT1  PORTD,RD2
	#DEFINE POORT2  PORTD,RD3
	#DEFINE POORT3  PORTC,RC4
	#DEFINE POORT4  PORTC,RC5
	#DEFINE POORT5  PORTD,RD4
	#DEFINE POORT6  PORTD,RD5
	#DEFINE POORT7  PORTD,RD6
	#DEFINE POORT8  PORTD,RD7 
	#DEFINE POORT9 PORTB,RB0
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
; BANK4  0-255 S88 Data BUFFER (256 BYTES)



	cblock 0x00	;algemene variabelen

COUNTER_OUTPUT1	    ;telt 0-255ms
COUNTER_OUTPUT2	    ;telt 0-255
COUNTER_OUTPUT3	    ;telt 0-255
COUNTER_OUTPUT4	    ;telt 0-255
COUNTER_OUTPUT5	    ;telt 0-255
COUNTER_OUTPUT6	    ;telt 0-255
COUNTER_OUTPUT7	    ;telt 0-255
COUNTER_OUTPUT8	    ;telt 0-255
COUNTER_OUTPUT9	    ;telt 0-255
COUNTER_OUTPUT10	    ;telt 0-255
COUNTER_OUTPUT11	    ;telt 0-255
COUNTER_OUTPUT12	    ;telt 0-255
COUNTER_OUTPUT13	    ;telt 0-255
COUNTER_OUTPUT14	    ;telt 0-255
COUNTER_OUTPUT15	    ;telt 0-255
COUNTER_OUTPUT16	    ;telt 0-255
	
STATUS_OUTPUT1
STATUS_OUTPUT2	
STATUS_OUTPUT3	
STATUS_OUTPUT4	
STATUS_OUTPUT5	
STATUS_OUTPUT6	
STATUS_OUTPUT7	
STATUS_OUTPUT8	
STATUS_OUTPUT9	
STATUS_OUTPUT10	
STATUS_OUTPUT11	
STATUS_OUTPUT12	
STATUS_OUTPUT13	
STATUS_OUTPUT14	
STATUS_OUTPUT15	
STATUS_OUTPUT16	
	
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

;Variabelen gebruikt bij S88 opslag DATA

S88_STATUS   ; BIT0= vorige toestand S88 CLOCK



;cblock 0x70-0x7F -->  LocoNet variabele zie LOCONET_18F4X20.INC

	endc

;********************************************************************
;************ TABELLEN **********************************************
;********************************************************************

    CBLOCK 0x100
TE_VERZENDEN_DATA       ;256 BYTES
    ENDC



;	cblock 0x300
;LN_TRANSMIT_BUFFER			; LocoNet Transmit buffer zie LOCONET_18F4X20.INC
;	endc

;	cblock 0x380
;LN_RECEIVE_BUFFER			; LocoNet Receive buffer zie LOCONET_18F4X20.INC
;	endc

	CBLOCK 0x400
S88_BUFFER
	ENDC


