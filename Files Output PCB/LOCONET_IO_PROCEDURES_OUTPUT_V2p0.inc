
;***************************************************************************
;* LocoNet In/out module met PIC 18F4620  microcontroller                  *
;*                                                                         *
;*                                                                         *
;* Geschreven door Geert Giebens voor PIC 18F4620  microcontroller         *
;*                                                                         *
;* Datum: 04 MRT 2021                      Versie LOCONET OUTPUT V1.0      *
;*                                                                         *
;* Filenaam: LOCONET_IO_PROCEDURES_OUTPUT_V1p0.INC                         *
;*                                                                         *
;***************************************************************************



;***************************************************
;*******  Plaats data in te verzenden buffer   *****
;***************************************************

;TE_VERZENDEN_BYTE1  ;gebruikt om data op te slaan in te verzenden data buffer
;...
;TE_VERZENDEN_BYTE15
;TE_VERZENDEN_EXORBYTE
;AANTAL_BYTES        ;aantal bytes te verzenden (is eerste byte in tabel daarna te verzenden bytes
;POSITIE_TELLER_IN   ;waar staat pointer bij data in 0-->255    
;POSITIE_TELLER_OUT   ;waar staat pointer bij data out
;VERSCHIL_TELLER

TE_VERZENDEN_BYTES_IN_BUFFER


    MOVF VERSCHIL_TELLER,W              ;als er een kans bestaat dat er meer dan 255 BYTES nog in te verzenden buffer staan, dan data niet in buffer
    ADDWF AANTAL_BYTES,W
    BNC SPR001

    RETURN  
    
    BCF LED3	    ;als data naar LocoNet dan laat LED3 0,2s oplichten
    MOVLW 12
    MOVWF TEL_LED3

SPR001

    CLRF TE_VERZENDEN_EXORBYTE
    LFSR 0,TE_VERZENDEN_DATA
    LFSR 1,TE_VERZENDEN_BYTE1
    MOVF POSITIE_TELLER_IN,W
    MOVWF FSR0L
    INCF POSITIE_TELLER_IN,F
    MOVF AANTAL_BYTES,W
    MOVWF INDF0
    MOVWF HULPTEL
    INCF VERSCHIL_TELLER,F
    DECF HULPTEL,F  

LOOP_TVBIB
    INCF FSR0L,F
    INCF POSITIE_TELLER_IN,F
    INCF VERSCHIL_TELLER,F  
    MOVF INDF1,W
    MOVWF INDF0
    XORWF TE_VERZENDEN_EXORBYTE,F   
    INCF FSR1L,F
    DECFSZ HULPTEL,F
    BRA LOOP_TVBIB

    INCF FSR0L,F
    INCF POSITIE_TELLER_IN,F
    INCF VERSCHIL_TELLER,F  
    MOVF TE_VERZENDEN_EXORBYTE,W
    COMF WREG,W
    MOVWF INDF0    

    RETURN

	
;*********************************************************
;*******  Test of er nieuwe data te verzenden valt   *****
;*********************************************************

TEST_NIEUWE_TE_VERZENDEN_DATA


        BTFSC LN_TRANSMITDATA          ; Heeft Loconet INCLUDE program de Vorige LocoNet data verzonden?
        RETURN

        MOVF POSITIE_TELLER_IN,W
        CPFSEQ  POSITIE_TELLER_OUT
        BRA TNTSPR1
        RETURN
TNTSPR1
        LFSR 0, LN_TRANSMIT_BUFFER
        LFSR 2, TE_VERZENDEN_DATA
        MOVF POSITIE_TELLER_OUT,W
        ADDWF FSR2L,F

        MOVF INDF2,W
        MOVWF HULPTEL
        MOVWF LN_TRANSMIT_TOTBYTES
        INCF  POSITIE_TELLER_OUT,F
        DECF VERSCHIL_TELLER,F

LOOP_TNTVD
        INCF FSR2L,F
        MOVF INDF2,W
        MOVWF INDF0
        INCF FSR0L,F
        INCF  POSITIE_TELLER_OUT,F
        DECF VERSCHIL_TELLER,F
        DECFSZ HULPTEL,F
        BRA LOOP_TNTVD
        BSF LN_TRANSMITDATA

        RETURN
	
;********************************************************************************
;*******  LAAD DATA IN EEPROM  **************************************************
;******************************************************************************

;EEADRH, EEADRL en EE_DATA moeten al op voorhand ingevuld worden

COPY_NAAR_EEPROM

        BCF INTCON,GIE
        MOVLW 55h
        MOVWF EECON2
        MOVLW 0AAh
        MOVWF EECON2
        BSF EECON1,WR
        BSF INTCON,GIE
	BTFSC EECON1, WR
	BRA $-2

        RETURN	