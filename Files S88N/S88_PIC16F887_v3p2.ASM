;**************************************************************************
;* S88 terugmelder met PIC 16F887 microcontroller met 32 ingangen         *
;*                                                                        *
;* Geschreven door Geert Giebens voor PIC 16F887 microcontroller          *
;*                                                                        *
;* Datum: 15 maart 2012                 Versie S88_PIC16F887_V3.2         *
;*                                                                        *
;* Filenaam: S88_PIC16F887_v3p2.asm                                       *
;*								          *
;* Aanpassingen versie 3.2:					          *
;*  INTERRUPT routine aangepast: copy DATA IN bijgevoegd 		  *
;*                        					          *
;*                         _____________________		          *
;*                LATCH   | 1  RE3 *  *  RB7 40 | S88 ingang 32           *
;*             DATA OUT   | 2  RA0  **   RB6 39 | S88 ingang 31           *
;*        S88 ingang  1   | 3  RA1       RB5 38 | S88 ingang 30           *
;*        S88 ingang  2   | 4  RA2       RB4 37 | S88 ingang 29           *
;*        S88 ingang  3   | 5  RA3   I   RB3 36 | S88 ingang 28           *
;*        S88 ingang  4   | 6  RA4   C   RB2 35 | S88 ingang 27           *
;*        S88 ingang  5   | 7  RA5       RB1 34 | S88 ingang 26           *
;*        S88 ingang  6   | 8  RE0   P   RB0 33 | CLOCK                   *
;*        S88 ingang  7   | 9  RE1   I       32 | +5V		 	  *
;*        S88 ingang  8   | 10 RE2   C       31 | Massa	 	          *
;*	    	      +5V | 11           RD7 30 | S88 ingang 25           *
;*	    	    Massa | 12       1   RD6 29 | DATA IN                 *
;*        S88 ingang  9   | 13 RA7   6   RD5 28 | S88 ingang 24           *
;*        S88 ingang 10   | 14 RA6   F   RD4 27 | S88 ingang 23           *
;*        S88 ingang 11   | 15 RC0   8   RC7 26 | S88 ingang 22           *
;*        S88 ingang 12   | 16 RC1   8   RC6 25 | S88 ingang 21           *
;*        S88 ingang 13   | 17 RC2   7   RC5 24 | S88 ingang 20           *
;*        S88 ingang 14   | 18 RC3       RC4 23 | S88 ingang 19           *
;*        S88 ingang 15   | 19 RD0       RD3 22 | S88 ingang 18           *
;*        S88 ingang 16   | 20 RD1       RD2 21 | S88 ingang 17           *
;*                         _____________________			  *
;*								          *
;*								          *								    
;**************************************************************************



;********************************************************************
;************ µCONTROLLER AFHANKELIJKE INSTELLINGEN *****************
;********************************************************************

	list		p=16f887	; list directive to define processor
	#include	<p16f887.inc>	; processor specific variable definitions

	__CONFIG    _CONFIG1, _LVP_OFF & _FCMEN_ON & _IESO_OFF & _BOR_OFF & _CPD_OFF & _CP_OFF & _MCLRE_OFF & _PWRTE_ON & _WDT_OFF & _INTRC_OSC_NOCLKOUT
	__CONFIG    _CONFIG2, _WRT_OFF & _BOR21V

	errorlevel -302, -306


;********************************************************************
;************ CONSTANTEN ********************************************
;********************************************************************

C_LAAG 		EQU 0x40			; drempelwaarde waarbij FF geset  wordt
C_HOOG 		EQU 0xc0			; drempelwaarde waarbij FF gereset wordt

		#define S88DATAOUT PORTA,0	; S88 protocol Data OUT
		#define S88DATAIN  PORTD,6	; S88 protocol Data IN
		#define S88LATCH PORTE,3	; S88 protocol LATCH
		#define S88CLOCK PORTB,0	; S88 protocol CLOCK

;*******************************************************************
;************ VARIABELEN ********************************************
;********************************************************************

		cblock 0x20

STATUS_TEMP				;tijdelijke opslag tijdens Interruptroutine
W_TEMP
PCLATH_TEMP			

DATAINBIT				; BIT 0 geeft de toestand van DATA IN weer zoals ingelezen iets na CLOCK puls 	
FFB1					; geeft de toestand van ingang 1-8 weer indien ingang laag dan is bit hoog
FFB2					; geeft de toestand van ingang 9-16 weer indien ingang laag dan is bit hoog
FFB3					; geeft de toestand van ingang 17-24 weer indien ingang laag dan is bit hoog
FFB4					; geeft de toestand van ingang 25-32 weer indien ingang laag dan is bit hoog
SCHUIFB1				; schuifregister 1 voor het doorschuiven data via S88 interface (interuptgestuurd)
SCHUIFB2				; schuifregister 2 voor het doorschuiven data via S88 interface (interuptgestuurd)
SCHUIFB3				; schuifregister 3 voor het doorschuiven data via S88 interface (interuptgestuurd)
SCHUIFB4				; schuifregister 4 voor het doorschuiven data via S88 interface (interuptgestuurd)

TELLERS 				; teller array[33]: (0-255)  eerste teller is hulpteller init
		endc


;********************************************************************
;*******************  MAKRO  BITTEST  *******************************
;********************************************************************

BITTEST	MACRO POORT,POORTBIT,MACROTEL,MACROFF,BITNR

	LOCAL SUB_SPR2, SUB_SPR1,SUB_SPR3,SUB_END
	

		BTFSS POORT,POORTBIT
		GOTO SUB_SPR1

		MOVF MACROTEL,w
		BTFSC STATUS,Z
		GOTO SUB_END			; indien ingang hoog (+5v) en teller niet 0 dan teller verlagen	
		DECF MACROTEL,f			
		; MOVF MACROTEL,W
		XORLW C_LAAG
		BTFSS STATUS,Z			; indien teller=64 dan RESET FF 
		GOTO SUB_END
		BCF MACROFF,BITNR
		GOTO SUB_END

SUB_SPR1
		INCFSZ MACROTEL,f		; indien ingang laag (massa)dan teller verhogen
		GOTO SUB_SPR2
		DECF MACROTEL,f			; 0xFF blijft de hoogste mogelijke waarde
		GOTO SUB_END
SUB_SPR2	
		MOVF MACROTEL,W
		XORLW C_HOOG
		BTFSS STATUS,Z			; indien teller=192 dan SET FF 
		GOTO SUB_END
		BSF MACROFF,BITNR

SUB_END	
		ENDM


;********************************************************************
;*******  START ASSEMBLER CODE **************************************
;********************************************************************

RESET		ORG     0x0000    	; processor reset vector is altijd adres 0000

		clrf 	PCLATH
		pagesel MAIN	
  		GOTO    MAIN        	; ga naar begin van program


;********************************************************************
;******************* INTERRUPT SERVICE ROUTINE  *********************
;********************************************************************
		
					;interupt routine start bij positieve overgang clock ingang 

	     	ORG    0x0004
       
ISR     	
		MOVWF W_TEMP 		;Copy W to TEMP register
		SWAPF STATUS,W 		;Swap status to be saved into W
		CLRF STATUS 		;bank 0, regardless of current bank, Clears IRP,RP1,RP0
		MOVWF STATUS_TEMP 	;Save status to bank zero STATUS_TEMP register
		MOVF PCLATH,W 		;Only required if using pages 1, 2 and/or 3
		MOVWF PCLATH_TEMP 	;Save PCLATH into W
		CLRF PCLATH 		;Page zero, regardless of current page


		BTFSC S88LATCH		; is LATCH ingang laag dan SCHUIF routine 
		goto spr2
		BCF STATUS,C
		BTFSC DATAINBIT,0
		BSF STATUS,C
		RRF SCHUIFB4,F
		RRF SCHUIFB3,F
		RRF SCHUIFB2,F
		RRF SCHUIFB1,F
		goto spr3		
spr2					;indien hoog dan LATCH ROUTINE 
		MOVF FFB1,W 		;Laad inhoud FF's naar schuifregister
		MOVWF SCHUIFB1
		MOVF FFB2,W
		MOVWF SCHUIFB2
		MOVF FFB3,W
		MOVWF SCHUIFB3
		MOVF FFB4,W
		MOVWF SCHUIFB4

spr3		BTFSS SCHUIFB1,0	;zet S88 DATA UIT op uitgang
		goto spr4
		BSF S88DATAOUT
		goto spr5
spr4		BCF S88DATAOUT
		
spr5		goto $+1		;wacht even 
		goto $+1
		goto $+1
		goto $+1
		goto $+1
		goto $+1
		goto $+1
		goto $+1
		BCF DATAINBIT,0		;lees S88 DATA IN die dan bij volgende schuifroutine kan gebruikt worden
		BTFSC S88DATAIN
		BSF DATAINBIT,0


isr_end 					; restore context then return
		BCF INTCON,INTF			;interupt vrijgeven
       		MOVF PCLATH_TEMP, W 		;Restore PCLATH
		MOVWF PCLATH			;Move W into PCLATH
		SWAPF STATUS_TEMP,W 		;Swap STATUS_TEMP register into W
								;(sets bank to original state)
		MOVWF STATUS 			;Move W into STATUS register
		SWAPF W_TEMP,F 			;Swap W_TEMP
		SWAPF W_TEMP,W 			;Swap W_TEMP into W
	
        	retfie


;********************************************************************
;********  INITIATIE  SUBROUTINE  ***********************************
;********************************************************************
	
INIT	
		;****************************************************
		;*******  INIT REGISTERS EN VARIABELEN **************
		;****************************************************

		banksel OSCCON
		BSF OSCCON,4		; van 4 naar 8MHz interne Oscilator
		BSF OSCCON,SCS

		banksel STATUS
		BCF STATUS,IRP		;voor indirect adresseren in bank 00

		movlw LOW TELLERS+1	;0 als startwaarde tellers[1-32], eerste tellers[0] is hulpteller
		movwf FSR
		movlw .32
		movwf TELLERS
I_spr1		clrf INDF
		incf FSR,f 
		decfsz TELLERS,f
		goto I_spr1

		CLRF FFB1
		CLRF FFB2
		CLRF FFB3
		CLRF FFB4
		CLRF SCHUIFB1
		CLRF SCHUIFB2
		CLRF SCHUIFB3
		CLRF SCHUIFB4
	

		;****************************************************
		;*******  INIT I/O POORTEN  *************************
		;****************************************************
		
		banksel ANSEL
		CLRF ANSEL		;Geen analoge ingangen
		CLRF ANSELH
			
		banksel TRISA		;0 is uitgang, 1 is ingang
		movlw b'11111110'
		movwf TRISA		
		movlw b'11111111'		
		movwf TRISB		
		movlw b'11111111'		
		movwf TRISC	
		movlw b'11111111'		
		movwf TRISD	
		movlw b'00001111'
		movwf TRISE		


		;****************************************************
		;*******  INIT INTERRUPT ****************************
		;****************************************************
		
		BSF OPTION_REG,INTEDG		;eerstvolgende interrupt bij positieve flank S88 CLOCK	
		banksel INTCON
		movlw b'10010000'		; enabele globale en RB0 INT 
		movwf INTCON

		RETURN


;********************************************************************
;******************* MAIN PROGRAM  **********************************
;********************************************************************
  
MAIN		CALL INIT		

START_LOOP	BITTEST PORTA,1,TELLERS+.1,FFB1,0	;******************* Lees ingang 1
		BITTEST PORTA,2,TELLERS+.2,FFB1,1	;******************* Lees ingang 2
		BITTEST PORTA,3,TELLERS+.3,FFB1,2	;******************* Lees ingang 3
		BITTEST PORTA,4,TELLERS+.4,FFB1,3	;******************* Lees ingang 4
		BITTEST PORTA,5,TELLERS+.5,FFB1,4	;******************* Lees ingang 5
		BITTEST PORTE,0,TELLERS+.6,FFB1,5	;******************* Lees ingang 6
		BITTEST PORTE,1,TELLERS+.7,FFB1,6	;******************* Lees ingang 7
		BITTEST PORTE,2,TELLERS+.8,FFB1,7	;******************* Lees ingang 8

		BITTEST PORTA,7,TELLERS+.9,FFB2,0	;******************* Lees ingang 9
		BITTEST PORTA,6,TELLERS+.10,FFB2,1	;******************* Lees ingang 10
		BITTEST PORTC,0,TELLERS+.11,FFB2,2	;******************* Lees ingang 11
		BITTEST PORTC,1,TELLERS+.12,FFB2,3	;******************* Lees ingang 12
		BITTEST PORTC,2,TELLERS+.13,FFB2,4	;******************* Lees ingang 13
		BITTEST PORTC,3,TELLERS+.14,FFB2,5	;******************* Lees ingang 14
		BITTEST PORTD,0,TELLERS+.15,FFB2,6	;******************* Lees ingang 15
		BITTEST PORTD,1,TELLERS+.16,FFB2,7	;******************* Lees ingang 16

		BITTEST PORTD,2,TELLERS+.17,FFB3,0	;******************* Lees ingang 17
		BITTEST PORTD,3,TELLERS+.18,FFB3,1	;******************* Lees ingang 18
		BITTEST PORTC,4,TELLERS+.19,FFB3,2	;******************* Lees ingang 19
		BITTEST PORTC,5,TELLERS+.20,FFB3,3	;******************* Lees ingang 20
		BITTEST PORTC,6,TELLERS+.21,FFB3,4	;******************* Lees ingang 21
		BITTEST PORTC,7,TELLERS+.22,FFB3,5	;******************* Lees ingang 22
		BITTEST PORTD,4,TELLERS+.23,FFB3,6	;******************* Lees ingang 23
		BITTEST PORTD,5,TELLERS+.24,FFB3,7	;******************* Lees ingang 24

		BITTEST PORTD,7,TELLERS+.25,FFB4,0	;******************* Lees ingang 25
		BITTEST PORTB,1,TELLERS+.26,FFB4,1	;******************* Lees ingang 26
		BITTEST PORTB,2,TELLERS+.27,FFB4,2	;******************* Lees ingang 27
		BITTEST PORTB,3,TELLERS+.28,FFB4,3	;******************* Lees ingang 28
		BITTEST PORTB,4,TELLERS+.29,FFB4,4	;******************* Lees ingang 29
		BITTEST PORTB,5,TELLERS+.30,FFB4,5	;******************* Lees ingang 30
		BITTEST PORTB,6,TELLERS+.31,FFB4,6	;******************* Lees ingang 31
		BITTEST PORTB,7,TELLERS+.32,FFB4,7	;******************* Lees ingang 32

		GOTO START_LOOP

		END






