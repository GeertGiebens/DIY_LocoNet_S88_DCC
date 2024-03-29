
;***************************************************************************
;* LocoNet In/out module met PIC 18F4620 microcontroller                   *
;*                                                                         *
;* Geschreven door Geert Giebens voor PIC 18F4620 microcontroller          *
;*                                                                         *
;* Datum: 25 DEC 2021                     Versie LOCONET IO V1.0           *
;*                                                                         *
;* Filenaam: LOCONET_DECODEER_KEYBOARD_V1p0.INC                               *
;*                                                                         *
;***************************************************************************


MACRO_UITGANG	MACRO UITGANG_NR_M, POORT_OUT_M, POORT_OUT_BIT_M, STATUS_INPUT_M
	
	
	LOCAL M_END, M_SPR1, M_SPR2, M_SPR3, M_SPR4, M_SPR9, M_SPR10
	
	BTFSS LN_BYTE_RECEIVE3,4
	BRA M_END

	BTFSS LN_BYTE_RECEIVE2,4
	BRA $+8
	BTFSC ADDRESS1
	BRA M_END
	BRA $+6
	BTFSS ADDRESS1
	BRA M_END
	
	BTFSS LN_BYTE_RECEIVE2,5
	BRA $+8
	BTFSC ADDRESS2
	BRA M_END
	BRA $+6
	BTFSS ADDRESS2
	BRA M_END	
	
	BTFSS LN_BYTE_RECEIVE2,6
	BRA $+8
	BTFSC ADDRESS3
	BRA M_END
	BRA $+6
	BTFSS ADDRESS3
	BRA M_END
	
	BTFSS LN_BYTE_RECEIVE3,0
	BRA $+8
	BTFSC ADDRESS4
	BRA M_END
	BRA $+6
	BTFSS ADDRESS4
	BRA M_END
	
	BTFSS LN_BYTE_RECEIVE3,1
	BRA $+8
	BTFSC ADDRESS5
	BRA M_END
	BRA $+6
	BTFSS ADDRESS5
	BRA M_END	
	
	BTFSS LN_BYTE_RECEIVE3,2
	BRA $+8
	BTFSC ADDRESS6
	BRA M_END
	BRA $+6
	BTFSS ADDRESS6
	BRA M_END	
	
	BTFSS LN_BYTE_RECEIVE3,3
	BRA $+8
	BTFSC ADDRESS7
	BRA M_END
	BRA $+6
	BTFSS ADDRESS7
	BRA M_END

	BTFSS DIP9	;indien 16 adressen 
	BRA M_SPR1
	
;16 adressen:
	MOVF LN_BYTE_RECEIVE2,W
	ANDLW b'00001111'
	XORLW UITGANG_NR_M-1
	BTFSS STATUS,Z
	BRA M_END
	
	BCF LED3	    ;data op LocoNet voor dit device dan laat LED3 0,2s oplichten
	MOVLW 12
	MOVWF TEL_LED3
	
	BTFSS LN_BYTE_RECEIVE3,5	;is DIR '1' dan set uitgang anders reset uitgang
	BRA M_SPR3
	BSF POORT_OUT_M,POORT_OUT_BIT_M
	
	BCF LED5
	MOVLW 12
	MOVWF TEL_LED5
	
	MOVLW UITGANG_NR_M+15	;opslaan in EEPROM
	MOVWF EEADR
	BSF EEDATA,0
	CALL COPY_NAAR_EEPROM
	
	BSF STATUS_INPUT_M,1
	MOVLW UITGANG_NR_M-1	;opslaan in EEPROM
	MOVWF EEADR
	MOVF STATUS_INPUT_M,W
	MOVWF EEDATA	
	CALL COPY_NAAR_EEPROM	
	BRA M_END
	
	
M_SPR3	
	BCF POORT_OUT_M,POORT_OUT_BIT_M
	
	BCF LED4	
	MOVLW 12
	MOVWF TEL_LED4
	
	MOVLW UITGANG_NR_M+15	;opslaan in EEPROM
	MOVWF EEADR
	BCF EEDATA,0
	CALL COPY_NAAR_EEPROM
	
	BCF STATUS_INPUT_M,1
	MOVLW UITGANG_NR_M-1	;opslaan in EEPROM
	MOVWF EEADR
	MOVF STATUS_INPUT_M,W
	MOVWF EEDATA	
	CALL COPY_NAAR_EEPROM	
	BRA M_END
		
M_SPR1
; 8 adressen:
	BTFSS LN_BYTE_RECEIVE2,3    ;reeks 1-8 of 9-16?
	BRA $+8
	BTFSC DIP11
	BRA M_END
	BRA $+6
	BTFSS DIP11
	BRA M_END
	
	BCF STATUS,C		    ;((UITGANG_NR_M-1) /2) = (A0,A1,A2) 
	MOVLW UITGANG_NR_M-1
	RRCF WREG,W
	XORWF LN_BYTE_RECEIVE2,W
	ANDLW b'00000111'
	BTFSS STATUS,Z
	BRA M_END
	
	MOVLW UITGANG_NR_M-1	    
	BTFSC WREG,0
	BRA M_SPR2
	
	BTFSS LN_BYTE_RECEIVE3,5
	BRA M_SPR9
	BSF POORT_OUT_M,POORT_OUT_BIT_M
	
	BCF LED5	    
	MOVLW 12
	MOVWF TEL_LED5
	
	MOVLW UITGANG_NR_M+15	;opslaan in EEPROM
	MOVWF EEADR
	BSF EEDATA,0
	CALL COPY_NAAR_EEPROM	
	BRA M_END
M_SPR9	
	BCF POORT_OUT_M,POORT_OUT_BIT_M
	
	MOVLW UITGANG_NR_M+15	;opslaan in EEPROM
	MOVWF EEADR
	BCF EEDATA,0
	CALL COPY_NAAR_EEPROM	
	BRA M_END
	
M_SPR2	
	
	BTFSC LN_BYTE_RECEIVE3,5
	BRA M_SPR10
	BSF POORT_OUT_M,POORT_OUT_BIT_M
	
	BCF LED4	
	MOVLW 12
	MOVWF TEL_LED4
	
	MOVLW UITGANG_NR_M+15	;opslaan in EEPROM
	MOVWF EEADR
	BSF EEDATA,0
	CALL COPY_NAAR_EEPROM
	BRA M_END
	
M_SPR10	
	BCF POORT_OUT_M,POORT_OUT_BIT_M
	
	MOVLW UITGANG_NR_M+15	;opslaan in EEPROM
	MOVWF EEADR
	BCF EEDATA,0
	CALL COPY_NAAR_EEPROM
	
M_END	NOP	
	ENDM

	
DECODEER_LOCONET_DATA

	BTFSS LN_NIEUWDATA		;Is er nieuwe LocoNet Data? Zo ja decodeer.
	RETURN
	
	MOVLB HIGH LN_RECEIVE_BUFFER
	MOVF LOW LN_RECEIVE_BUFFER,W,BANKED
	MOVWF LN_BYTE_RECEIVE1
	MOVF (LOW LN_RECEIVE_BUFFER)+1,W,BANKED
	MOVWF LN_BYTE_RECEIVE2
	MOVF (LOW LN_RECEIVE_BUFFER)+2,W,BANKED
	MOVWF LN_BYTE_RECEIVE3

	
        BCF LN_NIEUWDATA
	BCF LED2	    ;als data op LocoNet dan laat LED2 0,1s oplichten
	MOVLW 6
	MOVWF TEL_LED2
	
	MOVLW OPC_SW_REQ		        	;is opcode OPC_SW_REQ (B0)
	CPFSEQ LN_BYTE_RECEIVE1
	GOTO GOTO_NEXT1
	
;	BTFSS DIP10	;FUNCTIE GPON GPOFF
;	GOTO GOTO_NEXT0
	MACRO_UITGANG 1,  LED1_8, 0, STATUS_INPUT1
	MACRO_UITGANG 2,  LED1_8, 1, STATUS_INPUT2
;GOTO_NEXT0	
	MACRO_UITGANG 3,  LED1_8, 2, STATUS_INPUT3
	MACRO_UITGANG 4,  LED1_8, 3, STATUS_INPUT4
	MACRO_UITGANG 5,  LED1_8, 4, STATUS_INPUT5
	MACRO_UITGANG 6,  LED1_8, 5, STATUS_INPUT6
	MACRO_UITGANG 7,  LED1_8, 6, STATUS_INPUT7
	MACRO_UITGANG 8,  LED1_8, 7, STATUS_INPUT8
	MACRO_UITGANG 9,  LED9_16,0, STATUS_INPUT9
	MACRO_UITGANG 10, LED9_16,1, STATUS_INPUT10
	MACRO_UITGANG 11, LED9_16,2, STATUS_INPUT11
	MACRO_UITGANG 12, LED9_16,3, STATUS_INPUT12
	MACRO_UITGANG 13, LED9_16,4, STATUS_INPUT13
	MACRO_UITGANG 14, LED9_16,5, STATUS_INPUT14
	MACRO_UITGANG 15, LED9_16,6, STATUS_INPUT15
	MACRO_UITGANG 16, LED9_16,7, STATUS_INPUT16
	
	RETURN
	
GOTO_NEXT1	
;	BTFSC DIP10	;FUNCTIE GPON GPOFF
;	RETURN
	
;	MOVLW OPC_GPON
;	CPFSEQ LN_BYTE_RECEIVE1
;	GOTO GOTO_NEXT2
;	BCF LED1_8,1
;	BSF LED1_8,0
;
;GOTO_NEXT2	
;	MOVLW OPC_GPOFF
;	CPFSEQ LN_BYTE_RECEIVE1
;	RETURN
;	BSF LED1_8,1
;	BCF LED1_8,0
	RETURN
	
	
	
	
	
	