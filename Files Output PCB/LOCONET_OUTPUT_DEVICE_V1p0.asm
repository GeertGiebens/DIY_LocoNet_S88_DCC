
;***************************************************************************
;* LocoNet Output module met PIC 18F4620 microcontroller                   *
;*                                                                         *
;* Geschreven door Geert Giebens                                           *
;*                                                                         *
;* Datum: 17 MRT 2021             Versie LOCONET OUTPUT DEVICE V1.0        *
;*                                                                         *
;* Filenaam: LOCONET_OUTPUT_DEVICE_V1p0.asm                                          *
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
;*									   *
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
;*                LED 3   | 8  RE0   P   RB0 33 |      DCC In (PCB V2)     *
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
;*     poort 9 (PCB V2)   | 20 RD1       RD2 21 |      poort 1             *
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

    #DEFINE OLD_VERSION_PCB 0  ;(0=OLD VERSION) PCB V6.0
    #DEFINE TORTOISE 1	       ;(0=TORTOISE 4,25s uitgang

    #include LOCONET_IO_VARIABELEN_OUTPUT_V1p0.inc


	
;******************************************************************************
;*******  MAIN PROGRAM  *******************************************************
;******************************************************************************

    ORG 00000h


 	GOTO MAIN					
 
    ORG 00018h
	GOTO Low_Priority_Interrupt		;LOCONET_18F4X20_V6p0.INC

	
;********************************************************************************


    #include LOCONET_18F4X20_INPUT_V1p0.INC

    #include LOCONET_IO_PROCEDURES_OUTPUT_V1p0.inc

    #include LOCONET_DECODEER_OUTPUT_V1p0.inc



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

        RETURN


		
;**********************************************************

;INIT POORTEN en Tabel Variabelen

INIT_POORTEN_AND_VARIABELEN 
	MOVLW b'11001111'
	MOVWF TRISA 
	IF OLD_VERSION_PCB == 0
	    MOVLW b'00000000'
	ELSE
	    MOVLW b'00000001'
	ENDIF    
	MOVWF TRISB
	MOVLW b'11001111'
	MOVWF TRISC
	MOVLW b'00000001'
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
	
	MOVLW 1
	MOVWF COUNTER_OUTPUT1
	MOVWF COUNTER_OUTPUT2
	MOVWF COUNTER_OUTPUT3
	MOVWF COUNTER_OUTPUT4
	MOVWF COUNTER_OUTPUT5
	MOVWF COUNTER_OUTPUT6
	MOVWF COUNTER_OUTPUT7
	MOVWF COUNTER_OUTPUT8
	MOVWF COUNTER_OUTPUT9
	MOVWF COUNTER_OUTPUT10
	MOVWF COUNTER_OUTPUT11
	MOVWF COUNTER_OUTPUT12
	MOVWF COUNTER_OUTPUT13
	MOVWF COUNTER_OUTPUT14
	MOVWF COUNTER_OUTPUT15
	MOVWF COUNTER_OUTPUT16
	
	
;INIT EEPROM AND set output zoals EEPROM

        BCF EECON1,EEPGD
        BCF EECON1,CFGS
        BSF EECON1,WREN

        CLRF EEADRH
        CLRF EEADR    

	BCF POORT1
	BCF POORT2
	BCF POORT3
	BCF POORT4
	BCF POORT5
	BCF POORT6
	BCF POORT7
	BCF POORT8
	BCF POORT9
	BCF POORT10
	BCF POORT11
	BCF POORT12
	BCF POORT13
	BCF POORT14
	BCF POORT15
	BCF POORT16
	

	BTFSS DIP9
	RETURN	

	
	
	BSF EECON1, RD            ; EEPROM Read
        MOVF EEDATA, W            ; W = EEDATA
        BTFSC WREG,0
	BSF POORT1
        INCF EEADR,F 	
	
	BSF EECON1, RD            ; EEPROM Read
        MOVF EEDATA, W            ; W = EEDATA
        BTFSC WREG,0
	BSF POORT2
        INCF EEADR,F 
	
	BSF EECON1, RD            ; EEPROM Read
        MOVF EEDATA, W            ; W = EEDATA
        BTFSC WREG,0
	BSF POORT3
        INCF EEADR,F 
	
	BCF POORT4
	BSF EECON1, RD            ; EEPROM Read
        MOVF EEDATA, W            ; W = EEDATA
        BTFSC WREG,0
	BSF POORT4
        INCF EEADR,F 
	
	BSF EECON1, RD            ; EEPROM Read
        MOVF EEDATA, W            ; W = EEDATA
        BTFSC WREG,0
	BSF POORT5
        INCF EEADR,F 
	
	BSF EECON1, RD            ; EEPROM Read
        MOVF EEDATA, W            ; W = EEDATA
        BTFSC WREG,0
	BSF POORT6
        INCF EEADR,F 
	
	BSF EECON1, RD            ; EEPROM Read
        MOVF EEDATA, W            ; W = EEDATA
        BTFSC WREG,0
	BSF POORT7
        INCF EEADR,F 
	
	BSF EECON1, RD            ; EEPROM Read
        MOVF EEDATA, W            ; W = EEDATA
        BTFSC WREG,0
	BSF POORT8
        INCF EEADR,F 
	
	BSF EECON1, RD            ; EEPROM Read
        MOVF EEDATA, W            ; W = EEDATA
        BTFSC WREG,0
	BSF POORT9
        INCF EEADR,F 
	
	BSF EECON1, RD            ; EEPROM Read
        MOVF EEDATA, W            ; W = EEDATA
        BTFSC WREG,0
	BSF POORT10
        INCF EEADR,F 
	
	BSF EECON1, RD            ; EEPROM Read
        MOVF EEDATA, W            ; W = EEDATA
        BTFSC WREG,0
	BSF POORT11
        INCF EEADR,F 
	
	BSF EECON1, RD            ; EEPROM Read
        MOVF EEDATA, W            ; W = EEDATA
        BTFSC WREG,0
	BSF POORT12
        INCF EEADR,F 
	
	BSF EECON1, RD            ; EEPROM Read
        MOVF EEDATA, W            ; W = EEDATA
        BTFSC WREG,0
	BSF POORT13
        INCF EEADR,F 
	
	BSF EECON1, RD            ; EEPROM Read
        MOVF EEDATA, W            ; W = EEDATA
        BTFSC WREG,0
	BSF POORT14
        INCF EEADR,F 
	
	BSF EECON1, RD            ; EEPROM Read
        MOVF EEDATA, W            ; W = EEDATA
        BTFSC WREG,0
	BSF POORT15
        INCF EEADR,F 
	
	BSF EECON1, RD            ; EEPROM Read
        MOVF EEDATA, W            ; W = EEDATA
        BTFSC WREG,0
	BSF POORT16
        INCF EEADR,F 
	

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

VERLAAG_UITGANGTELLERS
	
	;Als DIP9 = '0' dan reset uitgang na 250ms
	
	BTFSC DIP9
	RETURN
	
	DECFSZ COUNTER_OUTPUT1,F
	BRA $+6
	BCF POORT1
	INCF COUNTER_OUTPUT1,F
	
	DECFSZ COUNTER_OUTPUT2,F
	BRA $+6
	BCF POORT2
	INCF COUNTER_OUTPUT2,F
		
	DECFSZ COUNTER_OUTPUT3,F
	BRA $+6
	BCF POORT3
	INCF COUNTER_OUTPUT3,F
			
	DECFSZ COUNTER_OUTPUT4,F
	BRA $+6
	BCF POORT4
	INCF COUNTER_OUTPUT4,F
		
	DECFSZ COUNTER_OUTPUT5,F
	BRA $+6
	BCF POORT5
	INCF COUNTER_OUTPUT5,F
		
	DECFSZ COUNTER_OUTPUT6,F
	BRA $+6
	BCF POORT6
	INCF COUNTER_OUTPUT6,F
		
	DECFSZ COUNTER_OUTPUT7,F
	BRA $+6
	BCF POORT7
	INCF COUNTER_OUTPUT7,F
		
	DECFSZ COUNTER_OUTPUT8,F
	BRA $+6
	BCF POORT8
	INCF COUNTER_OUTPUT8,F
		
	DECFSZ COUNTER_OUTPUT9,F
	BRA $+6
	BCF POORT9
	INCF COUNTER_OUTPUT9,F
		
	DECFSZ COUNTER_OUTPUT10,F
	BRA $+6
	BCF POORT10
	INCF COUNTER_OUTPUT10,F
		
	DECFSZ COUNTER_OUTPUT11,F
	BRA $+6
	BCF POORT11
	INCF COUNTER_OUTPUT11,F
		
	DECFSZ COUNTER_OUTPUT12,F
	BRA $+6
	BCF POORT12
	INCF COUNTER_OUTPUT12,F
		
	DECFSZ COUNTER_OUTPUT13,F
	BRA $+6
	BCF POORT13
	INCF COUNTER_OUTPUT13,F
		
	DECFSZ COUNTER_OUTPUT14,F
	BRA $+6
	BCF POORT14
	INCF COUNTER_OUTPUT14,F
		
	DECFSZ COUNTER_OUTPUT15,F
	BRA $+6
	BCF POORT15
	INCF COUNTER_OUTPUT15,F
		
	DECFSZ COUNTER_OUTPUT16,F
	BRA $+6
	BCF POORT16
	INCF COUNTER_OUTPUT16,F
	
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
	CALL VERLAAG_UITGANGTELLERS
  
VOLGENDE
        CALL TEST_NIEUWE_TE_VERZENDEN_DATA  ;LOCONET_IO_PROCEDURES_OUTPUT_V1p0.INC
        CALL DECODEER_LOCONET_DATA          ;LOCONET_DECODEER_OUTPUT_16p0.INC

        GOTO LUS

        END 
	