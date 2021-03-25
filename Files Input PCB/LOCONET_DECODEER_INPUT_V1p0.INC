
;***************************************************************************
;* LocoNet In/out module met PIC 18F4620 microcontroller                   *
;*                                                                         *
;* Geschreven door Geert Giebens voor PIC 18F4620 microcontroller          *
;*                                                                         *
;* Datum: 01 feb 2021                     Versie LOCONET IO V1.0           *
;*                                                                         *
;* Filenaam: LOCONET_DECODEER_INPUT_V1p0.INC                               *
;*                                                                         *
;***************************************************************************


DECODEER_LOCONET_DATA

	BTFSS LN_NIEUWDATA		;Is er nieuwe LocoNet Data? Zo ja decodeer.
	RETURN
	
        BCF LN_NIEUWDATA
	BCF LED2	    ;als data op LocoNet dan laat LED2 0,1s oplichten
	MOVLW 6
	MOVWF TEL_LED2
	RETURN
			