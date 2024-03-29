
;***************************************************************************
;* LocoNet In/out module met PIC 18F4620 microcontroller                   *
;*                                                                         *
;* Geschreven door Geert Giebens voor PIC 18F4620 microcontroller          *
;*                                                                         *
;* Datum: 06 MEI 2023                     Versie LOCONET IO V1.0           *
;*                                                                         *
;* Filenaam: LOCONET_DECODEER_OUTPUT_V1p0.INC                               *
;*                                                                         *
;***************************************************************************

#DEFINE OPC_SW_REQ 0xB0
#DEFINE OPC_INPUT_REP 0xB2

#DEFINE TORTOISE 1	       ;(0=TORTOISE 4,25s uitgang)
    IF TORTOISE ==1 
	    #DEFINE TIJD_SPOEL_UIT .15 ;(+-250ms)
    ELSE 
	    #DEFINE TIJD_SPOEL_UIT .255 ;(+-4,25s)
    ENDIF	
	
;LN_BYTE_RECEIVE1=       Opcode OPC_SW_REQ (B0)?
;LN_BYTE_RECEIVE2=	     SW1 = b'0,A6,A5,A4,A3,A2,A1,A0'
;LN_BYTE_RECEIVE3=       SW2 = b'0,0,DIR,ON,A10,A9,A8,A7'
	
;SW1 = '0,A6,A5,A4,A3,A2,A1,A0'
	
;DIP10 ='OFF'	
;      '           A3,A2,A1,A0' = (UITGANG_NR_M-1)

;DIP10 ='ON'
;                  A3         ' = DIP11
;      '              A2,A1,A0' = SHIFT RIGHT (UITGANG_NR_M-1)
	
;      '        A4            ' = INV ADDRESS1
;      '     A5               ' = INV ADDRESS2
;      '  A6                  ' = INV ADDRESS3
	
;SW2 = '0,0,DIR,ON,A10,A9,A8,A7'
;      '                     A7' = INV ADDRESS4
;      '                  A8   ' = INV ADDRESS5
;      '               A9      ' = INV ADDRESS6
;      '           A10         ' = INV ADDRESS7
	
;DIP10 ='OFF' POORT_OUT =DIR
	


	
MACRO_UITGANG	MACRO UITGANG_NR_M, COUNTER_OUTPUT_M, POORT_OUT_M, POORT_OUT_BIT_M
	
	
	LOCAL M_END, M_SPR1, M_SPR2, M_SPR3, M_SPR4, M_SPR9, M_SPR10
	
		
	BTFSS LN_BYTE_RECEIVE3,4	;enkel actie bij 'ON'
	GOTO M_END	

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
	
;	BCF LED3	    ;data op LocoNet voor dit device dan laat LED3 0,2s oplichten
;	MOVLW 12
;	MOVWF TEL_LED3

	BTFSS DIP10	;indien 16 adressen 
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
	MOVLW TIJD_SPOEL_UIT
	MOVWF COUNTER_OUTPUT_M
	
	BCF LED5
	MOVLW 12
	MOVWF TEL_LED5
	
	MOVLW UITGANG_NR_M-1	;opslaan in EEPROM
	MOVWF EEADR
	BSF EEDATA,0
	CALL COPY_NAAR_EEPROM
	BRA M_END
M_SPR3	
	BCF POORT_OUT_M,POORT_OUT_BIT_M
	
	BCF LED4	
	MOVLW 12
	MOVWF TEL_LED4
	
	MOVLW UITGANG_NR_M-1	;opslaan in EEPROM
	MOVWF EEADR
	BCF EEDATA,0
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
	
	BCF STATUS,C		    ;((UITGANG_NR_M-1) /2) = (A1,A2,A3) 
	MOVLW UITGANG_NR_M-1
	RRCF WREG,W
	XORWF LN_BYTE_RECEIVE2,W
	ANDLW b'00000111'
	BTFSS STATUS,Z
	BRA M_END
	
	BCF LED3	    ;data op LocoNet voor dit device dan laat LED3 0,2s oplichten
	MOVLW 12
	MOVWF TEL_LED3
	
	MOVLW UITGANG_NR_M-1	    
	BTFSC WREG,0
	BRA M_SPR2
	
	BTFSS LN_BYTE_RECEIVE3,5
	BRA M_SPR9
	BSF POORT_OUT_M,POORT_OUT_BIT_M
	MOVLW TIJD_SPOEL_UIT
	MOVWF COUNTER_OUTPUT_M
	
	BCF LED5	    
	MOVLW 12
	MOVWF TEL_LED5
	
	MOVLW UITGANG_NR_M-1	;opslaan in EEPROM
	MOVWF EEADR
	BSF EEDATA,0
	CALL COPY_NAAR_EEPROM	
	BRA M_END
M_SPR9	
	BCF POORT_OUT_M,POORT_OUT_BIT_M
	
	MOVLW UITGANG_NR_M-1	;opslaan in EEPROM
	MOVWF EEADR
	BCF EEDATA,0
	CALL COPY_NAAR_EEPROM	
	BRA M_END
	
M_SPR2	
	
	BTFSC LN_BYTE_RECEIVE3,5
	BRA M_SPR10
	BSF POORT_OUT_M,POORT_OUT_BIT_M
	MOVLW TIJD_SPOEL_UIT
	MOVWF COUNTER_OUTPUT_M
	
	BCF LED4	
	MOVLW 12
	MOVWF TEL_LED4
	
	MOVLW UITGANG_NR_M-1	;opslaan in EEPROM
	MOVWF EEADR
	BSF EEDATA,0
	CALL COPY_NAAR_EEPROM
	BRA M_END
	
M_SPR10	
	BCF POORT_OUT_M,POORT_OUT_BIT_M
	
	MOVLW UITGANG_NR_M-1	;opslaan in EEPROM
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
	GOTO NEXT_OPC_INPUT_REP	
	
	BTFSS SV	;als SV = '1' dan geef data weer voor OPC_SW_REQ
	RETURN
	
	MACRO_UITGANG 1, COUNTER_OUTPUT1, POORT1
	MACRO_UITGANG 2, COUNTER_OUTPUT2, POORT2
	MACRO_UITGANG 3, COUNTER_OUTPUT3, POORT3
	MACRO_UITGANG 4, COUNTER_OUTPUT4, POORT4
	MACRO_UITGANG 5, COUNTER_OUTPUT5, POORT5
	MACRO_UITGANG 6, COUNTER_OUTPUT6, POORT6
	MACRO_UITGANG 7, COUNTER_OUTPUT7, POORT7
	MACRO_UITGANG 8, COUNTER_OUTPUT8, POORT8
	MACRO_UITGANG 9, COUNTER_OUTPUT9, POORT9
	MACRO_UITGANG 10, COUNTER_OUTPUT10, POORT10
	MACRO_UITGANG 11, COUNTER_OUTPUT11, POORT11
	MACRO_UITGANG 12, COUNTER_OUTPUT12, POORT12
	MACRO_UITGANG 13, COUNTER_OUTPUT13, POORT13
	MACRO_UITGANG 14, COUNTER_OUTPUT14, POORT14
	MACRO_UITGANG 15, COUNTER_OUTPUT15, POORT15
	MACRO_UITGANG 16, COUNTER_OUTPUT16, POORT16

	RETURN
	
	
NEXT_OPC_INPUT_REP

;LN_BYTE_RECEIVE1=       Opcode OPC_INPUT_REP(B2)?
;LN_BYTE_RECEIVE2=	 IN1 = b'0,A6,A5,A4,A3,A2,A1,A0'
;LN_BYTE_RECEIVE3=       IN2 = b'0,X,I,L,A10,A9,A8,A7'


    MOVLW OPC_INPUT_REP
	CPFSEQ LN_BYTE_RECEIVE1
	RETURN
	
	LFSR FSR1, S88_BUFFER	
	
	MOVF LN_BYTE_RECEIVE2,W
	BCF STATUS,C
	RRCF WREG,W
	BCF STATUS,C
	RRCF WREG,W 
	MOVWF FSR1L
	BTFSC LN_BYTE_RECEIVE3,0
	BSF FSR1L,5
	BTFSC LN_BYTE_RECEIVE3,1
	BSF FSR1L,6
	BTFSC LN_BYTE_RECEIVE3,2
	BSF FSR1L,7	; A9,A8,A7,A6,A5,A4,A3,A2 --> FSR1L
	
	BTFSC LN_BYTE_RECEIVE2,1
	BRA SPR_NOIP1
	BTFSC LN_BYTE_RECEIVE2,0
	BRA SPR_NOIP2
	BTFSC LN_BYTE_RECEIVE3,5
	BRA SPR_NOIP3
	
	BTFSC LN_BYTE_RECEIVE3,4
	BRA SPR_NOIP4	
	BCF INDF1,0 ;bit'00000001' --> 0 
	RETURN	
SPR_NOIP4
	BSF INDF1,0 ;bit'00000001' --> 1
	RETURN
	
SPR_NOIP3	
	BTFSC LN_BYTE_RECEIVE3,4
	BRA SPR_NOIP5	
	BCF INDF1,1 ;bit'00000010' --> 0 
	RETURN	
SPR_NOIP5
	BSF INDF1,1 ;bit'00000010' --> 1
	RETURN

SPR_NOIP2	
	BTFSC LN_BYTE_RECEIVE3,5
	BRA SPR_NOIP6
	
	BTFSC LN_BYTE_RECEIVE3,4
	BRA SPR_NOIP7	
	BCF INDF1,2 ;bit'00000100' --> 0 
	RETURN	
SPR_NOIP7
	BSF INDF1,2 ;bit'00000100' --> 1
	RETURN
	
SPR_NOIP6	
	BTFSC LN_BYTE_RECEIVE3,4
	BRA SPR_NOIP8	
	BCF INDF1,3 ;bit'00001000' --> 0 
	RETURN	
SPR_NOIP8
	BSF INDF1,3 ;bit'00001000' --> 1
	RETURN
	
SPR_NOIP1	
	BTFSC LN_BYTE_RECEIVE2,0
	BRA SPR_NOIP9
	BTFSC LN_BYTE_RECEIVE3,5
	BRA SPR_NOIP10
	
	BTFSC LN_BYTE_RECEIVE3,4
	BRA SPR_NOIP11	
	BCF INDF1,4 ;bit'00010000' --> 0 
	RETURN	
SPR_NOIP11
	BSF INDF1,4 ;bit'00010000' --> 1
	RETURN
	
SPR_NOIP10	
	BTFSC LN_BYTE_RECEIVE3,4
	BRA SPR_NOIP12	
	BCF INDF1,5 ;bit'00100000' --> 0 
	RETURN	
SPR_NOIP12
	BSF INDF1,5 ;bit'00100000' --> 1
	RETURN

SPR_NOIP9	
	BTFSC LN_BYTE_RECEIVE3,5
	BRA SPR_NOIP13
	
	BTFSC LN_BYTE_RECEIVE3,4
	BRA SPR_NOIP14	
	BCF INDF1,6 ;bit'00100000' --> 0 
	RETURN	
SPR_NOIP14
	BSF INDF1,6 ;bit'01000000' --> 1
	RETURN
	
SPR_NOIP13	
	BTFSC LN_BYTE_RECEIVE3,4
	BRA SPR_NOIP15	
	BCF INDF1,7 ;bit'10000000' --> 0 
	RETURN	
SPR_NOIP15
	BSF INDF1,7 ;bit'10000000' --> 1
	RETURN
	
	