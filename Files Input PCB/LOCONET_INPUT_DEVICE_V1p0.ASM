
;***************************************************************************
;* LocoNet Output module met PIC 18F4620 microcontroller                   *
;*                                                                         *
;* Geschreven door Geert Giebens                                           *
;*                                                                         *
;* Datum: 19 MRT 2021             Versie LOCONET INPUT  DEVICE V1.0        *
;*                                                                         *
;* Filenaam: LOCONET_INPUT_DEVICE_V1.ASM                                   *
;*     	                                                                   *	   
;* DISCLAIMER: LocoNet is een Copyrighted product van Digitrax Inc.        *
;*             De software en hardware mag enkel gebruikt worden    	   *
;*             voor persoonlijk gebruik en op risico van de                *
;*             gebruiker zelf. De auteur kan geen garantie                 *
;*             bieden op de correcte werking van deze software.     	   *
;*     	                                                                   *
;*     	                                                                   *
;* Aanpassingen versie 1.0:                                                *
;*  -04/03/21: Algemene aanpassingen van Loconet IO naar Loconet OUTPUT    *
;*     	                                                                   *
;*     	                                                                   *  
;*                         _____________________                           *
;*           (Vpp) DIP9   | 1  RE3 *  *  RB7 40 |      poort 16 (PGD)      *
;*      Comperator Vin+   | 2  RA0  **   RB6 39 |      poort 15 (PGC)      *
;*                DIP10   | 3  RA1       RB5 38 |      poort 14            *
;*                DIP11   | 4  RA2       RB4 37 |      poort 13            *
;*      Comperator Vin-   | 5  RA3   I   RB3 36 |      poort 12            *
;*      Comperator  Out   | 6  RA4   C   RB2 35 |      poort 11            *
;*                LED 2   | 7  RA5       RB1 34 |      poort 10            *
;*                LED 3   | 8  RE0   P   RB0 33 |      poort 9             *
;*                LED 4   | 9  RE1   I       32 |     +5V	           *
;*                LED 5   | 10 RE2   C       31 |     Massa	           *
;*                   +5V  | 11           RD7 30 |      poort 8             *
;*                 Massa  | 12       1   RD6 29 |      poort 7             *
;*        DIP Address 1   | 13 RA7   8   RD5 28 |      poort 6             *
;*        DIP Address 2   | 14 RA6   F   RD4 27 |      poort 5             *
;*        DIP Address 3   | 15 RC0   4   RC7 26 |     LN_RECEIVER          *
;*        DIP Address 4   | 16 RC1   6   RC6 25 |     LN_TRANSMITTER       *
;*        DIP Address 5   | 17 RC2   2   RC5 24 |      poort 4             *
;*        DIP Address 6   | 18 RC3   0   RC4 23 |      poort 3             *
;*        DIP Address 7   | 19 RD0       RD3 22 |      poort 2             *
;*        DIP Address 8    | 20 RD1       RD2 21 |      poort 1            *
;*                         _____________________                           *
;*                                                                         *								    
;***************************************************************************

	   LIST P=18F4620 
	   #include <P18F4620.INC> 

     CONFIG OSC =  INTIO67
     CONFIG PWRT = ON, BOREN = OFF
     CONFIG WDT = OFF
     CONFIG MCLRE = OFF,  LPT1OSC = OFF, PBADEN = OFF
     CONFIG STVREN = OFF, LVP = OFF, XINST = OFF, DEBUG = OFF
     CONFIG CP0 = OFF, CP1 = OFF, CP2 = OFF, CP3 = OFF
     CONFIG CPB = OFF, CPD = OFF

    RADIX DEC

    #DEFINE DEBUGGER 0 ;1= debug

    #include LOCONET_IO_VARIABELEN_INPUT_V1p0.INC


	
;******************************************************************************
;*******  MAIN PROGRAM  *******************************************************
;******************************************************************************

    ORG 00000h


 	GOTO MAIN						;LOCONET_IO_V6p0.ASM
 
    ORG 00018h
	GOTO Low_Priority_Interrupt		;LOCONET_18F4X20_V6p0.INC

	
;********************************************************************************


    #include LOCONET_18F4X20_INPUT_V1p0.INC

    #include LOCONET_IO_PROCEDURES_INPUT_V1p0.INC

    #include LOCONET_DECODEER_INPUT_V1p0.INC

    #include LOCONET_IO_INGANG_INPUT_V1p0.INC



;**********************************************************
;*******  INIT MAIN PROGRAM  ******************************
;**********************************************************

INIT_MAIN

;INIT_OSCILATOR

        BSF OSCCON,IRCF0        ;zet oscilator op 32MHz (in deze volgorde bit's zetten!!)
        BSF OSCCON,IRCF1  
        BSF OSCTUNE,PLLEN  	
		
;INIT Timer 3 op 60Hz --> interrupt TMR3IF

        CLRF TMR3L  
    	CLRF TMR3H   
    	MOVLW b'00010001'
    	MOVWF T3CON  

;INIT Timer 0 om pulsen op te wekken van 1ms tot 255ms

        MOVLW b'00000100'   ;prescaler 1/32
        MOVWF T0CON  

        CLRF POSITIE_TELLER_IN     
        CLRF POSITIE_TELLER_OUT
        CLRF VERSCHIL_TELLER

        MOVLW 0Fh                   ;geen analoge ingangen
        MOVWF ADCON1
	
;INIT EEPROM AND load from EEPROM

        BCF EECON1,EEPGD
        BCF EECON1,CFGS
        BSF EECON1,WREN

        CLRF EEADRH
        CLRF EEADR    

	BSF EECON1, RD            ; EEPROM Read
        MOVF EEDATA, W            ; W = EEDATA
        MOVWF STATUS_INPUT1
        INCF EEADR,F 
	BSF EECON1, RD            ; EEPROM Read
        MOVF EEDATA, W            ; W = EEDATA
        MOVWF STATUS_INPUT2
        INCF EEADR,F 
	BSF EECON1, RD            ; EEPROM Read
        MOVF EEDATA, W            ; W = EEDATA
        MOVWF STATUS_INPUT3
        INCF EEADR,F 
	BSF EECON1, RD            ; EEPROM Read
        MOVF EEDATA, W            ; W = EEDATA
        MOVWF STATUS_INPUT4
        INCF EEADR,F 
	BSF EECON1, RD            ; EEPROM Read
        MOVF EEDATA, W            ; W = EEDATA
        MOVWF STATUS_INPUT5
        INCF EEADR,F 
	BSF EECON1, RD            ; EEPROM Read
        MOVF EEDATA, W            ; W = EEDATA
        MOVWF STATUS_INPUT6
        INCF EEADR,F 
	BSF EECON1, RD            ; EEPROM Read
        MOVF EEDATA, W            ; W = EEDATA
        MOVWF STATUS_INPUT7
        INCF EEADR,F 
	BSF EECON1, RD            ; EEPROM Read
        MOVF EEDATA, W            ; W = EEDATA
        MOVWF STATUS_INPUT8
        INCF EEADR,F 
	BSF EECON1, RD            ; EEPROM Read
        MOVF EEDATA, W            ; W = EEDATA
        MOVWF STATUS_INPUT9
        INCF EEADR,F 
	BSF EECON1, RD            ; EEPROM Read
        MOVF EEDATA, W            ; W = EEDATA
        MOVWF STATUS_INPUT10
        INCF EEADR,F 
	BSF EECON1, RD            ; EEPROM Read
        MOVF EEDATA, W            ; W = EEDATA
        MOVWF STATUS_INPUT11
        INCF EEADR,F 
	BSF EECON1, RD            ; EEPROM Read
        MOVF EEDATA, W            ; W = EEDATA
        MOVWF STATUS_INPUT12
        INCF EEADR,F 
	BSF EECON1, RD            ; EEPROM Read
        MOVF EEDATA, W            ; W = EEDATA
        MOVWF STATUS_INPUT13
        INCF EEADR,F 
	BSF EECON1, RD            ; EEPROM Read
        MOVF EEDATA, W            ; W = EEDATA
        MOVWF STATUS_INPUT14
        INCF EEADR,F 
	BSF EECON1, RD            ; EEPROM Read
        MOVF EEDATA, W            ; W = EEDATA
        MOVWF STATUS_INPUT15
        INCF EEADR,F 
	BSF EECON1, RD            ; EEPROM Read
        MOVF EEDATA, W            ; W = EEDATA
        MOVWF STATUS_INPUT16
 

	
        RETURN


		
;**********************************************************

;INIT POORTEN en Tabel Variabelen

INIT_POORTEN_AND_VARIABELEN 
	MOVLW b'11001111'
	MOVWF TRISA 	
	MOVLW b'11111111'
	MOVWF TRISB
	MOVLW b'10111111'
	MOVWF TRISC
	MOVLW b'11111111'
	MOVWF TRISD
	MOVLW b'11101000'
	MOVWF TRISE
	
	BSF LED2
	BSF LED3
	BSF LED4
	BSF LED5
	MOVLW 1
	MOVWF TEL_LED2
	MOVWF TEL_LED3
	MOVWF TEL_LED4
	MOVWF TEL_LED5
	
	CLRF COUNTER_INPUT1
	CLRF COUNTER_INPUT2
	CLRF COUNTER_INPUT3
	CLRF COUNTER_INPUT4
	CLRF COUNTER_INPUT5
	CLRF COUNTER_INPUT6
	CLRF COUNTER_INPUT7
	CLRF COUNTER_INPUT8
	CLRF COUNTER_INPUT9
	CLRF COUNTER_INPUT10
	CLRF COUNTER_INPUT11
	CLRF COUNTER_INPUT12
	CLRF COUNTER_INPUT13
	CLRF COUNTER_INPUT14
	CLRF COUNTER_INPUT15
	CLRF COUNTER_INPUT16
	

        RETURN
	
;**********************************************************
	
VERLAAG_INDICATIETELLERS_LEDS
	
	DECFSZ TEL_LED2,F
	BRA $+6
	BSF LED2
	INCF TEL_LED2,F
	DECFSZ TEL_LED3,F
	BRA $+6
	BSF LED3
	INCF TEL_LED3,F
	DECFSZ TEL_LED4,F
	BRA $+6
	BSF LED4
	INCF TEL_LED4,F
	DECFSZ TEL_LED5,F
	BRA $+6
	BSF LED5
	INCF TEL_LED5,F
	RETURN
		
;**********************************************************
;*******  MAIN   ******************************************
;**********************************************************

MAIN
        CALL INIT_MAIN 
        CALL INIT_POORTEN_AND_VARIABELEN 
        CALL INIT_LOCONET                   ;LOCONET_18F4X20_INPUT_V1p0.INC   

LUS
        BTFSS PIR2,TMR3IF  	                ;Timer3 overflow?
        BRA VOLGENDE		
        BCF PIR2,TMR3IF                 	;YES:  (wordt elke 16,6ms uitgevoerd 60Hz)
	CALL VERLAAG_INDICATIETELLERS_LEDS
  
VOLGENDE
        CALL TEST_INGANG                    ;LOCONET_IO_INGANG_INPUT_V1p0.INC
        CALL TEST_NIEUWE_TE_VERZENDEN_DATA  ;LOCONET_IO_PROCEDURES_INPUT_V1p0.INC
        CALL DECODEER_LOCONET_DATA          ;LOCONET_DECODEER_INPUT_16p0.INC

        GOTO LUS

        END 
	