

;***************************************************************************
;* Include file LocoNet Communicatie voor PIC 18F4620/18F4520              *
;*                                                                         *
;* Geschreven door Geert Giebens voor PIC 18F4620/18F4520 microcontrollers *
;*                                                                         *
;* Datum: 29 JAN 2021                                                      *
;*                                                                         *
;* Filenaam: LOCONET_18F4X20_INPUT_V1p0.INC                                      *
;*     	                                                                   *
;* DISCLAIMER: LocoNet is een Copyrighted product van Digitrax Inc.        *
;*             De software en hardware mag enkel gebruikt worden    	   *
;*             voor persoonlijk gebruik en op risico van de                *
;*             gebruiker zelf. De auteur kan geen garantie                 *
;*             bieden op de correcte werking van deze software.     	   *
;*                                                                         * 
;***************************************************************************

; Aangepast voor LOCONET INOUT DEVICE v1.0: tellers eruit gehaald


;Geheugengebruik include file:
;
; RAM geheugen:
;  BANK0: 0x70  ... 0x7F  LocoNet variabelen (16 BYTES)
;  BANK3: 0x300 ... 0x37F  TRANSMIT BUFFER (128 BYTES)
;  BANK3: 0x380 ... 0x3FF  RECEIVE BUFFER (128 BYTES)

    RADIX DEC

;********************************************************************
;************ CONSTANTEN ********************************************
;********************************************************************

		#define LN_NIEUWDATA		LN_FLAG,0	; =1 als er nieuw data klaar staat in RECEIVE buffer
		#define LN_NBYTES 		    LN_FLAG,1	; =1 Aantal in te lezen BYTES nog te bepalen uit tweede in te lezen BYTE
		#define LN_ZENDEN 	    	LN_FLAG,4   ; =1 TRANSMITTER staat klaar om eerste byte te verzenden
		#define LN_TIM1_FUNCTIE		LN_FLAG,5	; =0 CARRIER + MASTER + PRIORITY DETECT  =1 LINEBREAK DETECT
        #define LN_TIM1_FUNCTIE2    LN_FLAG,2   ; =1 Extra 60µs (zie !1)
		#define LN_TRANSMITDATA 	LN_FLAG,6	; =1 er staat data in transmitbuffer
		#define LN_CHECKSUM_ERR		LN_FLAG,7	; =1 dan is er een CHECKSUM error opgetreden bij data ontvangst
        #define FIRST_DATA_SEND     LN_FLAG,3   ; =1 na eerste ontvangen BYTE (om probleem RCIDL='0' te omzeilen na RESET)
			

;*******************************************************************
;************ VARIABELEN ********************************************
;********************************************************************

		cblock 0x70					
LN_RECEIVE_EXORBYTE			; op elke ingelezen byte wordt al een EXOR functie op uitgeoefend voor foutdetectie
LN_RECEIVE_TELLER			; telt het aantal ingelezen BYTES
LN_RECEIVE_BYTE				; ontvangen byte
LN_TRANSMIT_TELLER			; telt het aantal verzonden bytes
LN_TRANSMIT_BYTE			; verzonden byte
LN_TRANSMIT_TOTBYTES	    ; totaal te verzenden bytes
LN_RECEIVE_TOTBYTES			; totaal in te lezen bytes bepaald uit opcode (BYTE 1 en eventueel BYTE 2)

COPY_FSR1L
COPY_FSR1H
LN_COPY_WREG
LN_COPY_STATUS
LN_COPY_BSR	
LN_FLAG
LN_FLAGS_2	
		endc

		cblock 0x300
LN_TRANSMIT_BUFFER			; LocoNet Transmit buffer
		endc
		
		cblock 0x380
LN_RECEIVE_BUFFER			; LocoNet Receive buffer
		endc

		
;********************************************************************
;******************* INTERRUPT SERVICE ROUTINE  *********************
;********************************************************************

Low_Priority_Interrupt

            MOVWF LN_COPY_WREG
            MOVFF STATUS,LN_COPY_STATUS
            MOVFF BSR, LN_COPY_BSR
			BTFSS PIR1,TMR1IF            
			BRA ISR_SPR50
			BCF PIR1,TMR1IF

;TIMER1 INTERRUPT = LINEBREAK
;	    dan stop zenden (indien deze aan zenden was)
;           en start CARRIER + MASTER + PRIORITY DETECT

			BTFSS LN_TIM1_FUNCTIE
			BRA ISR_SPR20
			BCF PORTC,6		;herstart ontvanger
			BSF RCSTA,SPEN	

SCMPD ;START_CARRIER_MASTER_PRIORITY_DELAY 

			MOVF TMR3H,W    	;berekening: basistijd = CDMD=1596µs + PRIORITY DELAY = RANDOM tussen (1µs en 1024µs)
			ANDLW b'00000011'	; RANDOM Generator afkomstig van 10 laagste bits TMR3
			ADDLW 6 ; 5 waarde daarvoor
			COMF WREG,W
			MOVWF TMR1H
			MOVF TMR3L,W
            MOVWF TMR1L
			BCF LN_TIM1_FUNCTIE
            BCF LN_TIM1_FUNCTIE2 ;(zie !1)
			BRA LN_END
     

;TIMER1 INTERRUPT = CARRIER + MASTER + PRIORITY DETECT
;	  Staat er data in TRANSMIT BUFFER?
;		NEE: Herstart TIMER1 met tijd= CARRIER + MASTER + PRIORITY
;		JA: Laad eerste Byte in TRANSMIT Register
;			als LocoNet nog vrij is start met zenden

ISR_SPR20
			BTFSS LN_TRANSMITDATA
			BRA SCMPD
       
			MOVFF FSR1L,COPY_FSR1L
			MOVFF FSR1H,COPY_FSR1H
			LFSR 1, LN_TRANSMIT_BUFFER
      
			MOVF INDF1,W
			MOVWF LN_TRANSMIT_BYTE

            BTFSS FIRST_DATA_SEND   ;24/11/2018 toegevoegd omdat na opstart µC RCIDL laag blijft tot eerste ontvangen byte 
            BRA ISR_SPR201

			BTFSS BAUDCON,RCIDL		;11/12/2015 toegevoegd om na te gaan of receiver data aan het lezen is.
			BRA ISR_SPR33
            BTFSS PORTC,7               ;(zie !1)
            BRA ISR_SPR33               ;(zie !1)
ISR_SPR201
            BTFSC LN_TIM1_FUNCTIE2      ;(zie !1)
            BRA ISR_SPR21               ;(zie !1)
            CLRF SPBRGH                 ;(zie !1)
        	MOVLW 0x77                  ;(zie !1)
        	MOVWF SPBRG                 ;(zie !1)
            MOVLW .202                  ;(zie !1)
            MOVWF TMR1L                 ;(zie !1)
            SETF TMR1H                  ;(zie !1)
            BSF LN_TIM1_FUNCTIE2        ;(zie !1)
			MOVFF COPY_FSR1L,FSR1L      ;(zie !1)	
			MOVFF COPY_FSR1H,FSR1H      ;(zie !1)	
			BRA LN_END                  ;(zie !1)

ISR_SPR21                               ;(zie !1) 			
			MOVF INDF1,W
			MOVWF TXREG

            BSF FIRST_DATA_SEND
        
			BSF LN_ZENDEN
			CLRF LN_TRANSMIT_TELLER
			MOVFF COPY_FSR1L,FSR1L	
			MOVFF COPY_FSR1H,FSR1H		
            BRA LN_END

ISR_SPR33
			MOVFF COPY_FSR1L,FSR1L	
			MOVFF COPY_FSR1H,FSR1H		
			BRA SCMPD

;********************************************************************
	
;RECEIVER INTERRUPT:

;Is er een FRAMING ERROR? (kan ook afkomstig zijn van een externe LINEBREAK)
;	JA:	zet totaal in te lezen bytes op max 
;		stop verzenden Bytes (indien aan het verzenden)
;               start Linebreak delay

ISR_SPR50
			BTFSS RCSTA,FERR
			BRA ISR_SPR52
            NOP
LINEBREAK
			MOVF RCREG,W			;maak ontvangstbuffer leeg	
			MOVF RCREG,W		
			SETF LN_RECEIVE_TOTBYTES			   		
			BCF RCSTA,SPEN			;stop ontvanger
			BSF PORTC,6
			BCF LN_ZENDEN	
			BCF LN_NIEUWDATA

			MOVLW 252			;laad timer1 met 900µs = LINEBREAK     
			MOVWF TMR1H
			MOVLW 123
			MOVWF TMR1L
			BSF LN_TIM1_FUNCTIE
			BRA LN_END


;********************************************************************

;Is er is een BYTE ontvangen in RECEIVER?
;	JA: Plaats Byte in RECEIVE BUFFER
;           HERSTART TIMER1 met CARRIER MASTER PRIORITY DETECT		
;       Is de ontvangen Byte een opcode?
;			JA:	Reset RECEIVE_TELLER
;				En bepaal uit OPCODE hoeveel Bytes er ingelezen moeten worden
;					opcode = '100XXXXX' --> 2 Bytes	
;					opcode = '101XXXXX' --> 4 Bytes
;					opcode = '110XXXXX' --> 6 Bytes
;					opcode = '111XXXXX' --> N Bytes  waarbij ingelezen Byte2 = N bytes
;			NEE: is dit de laatste in te lezen Byte?
;				JA: IS NOT(Byte1 EXOR BYTE2 EXOR ... ByteN) = 0   
;				    dan LN_NIEUWDATA --> 1
;				    anders LN_CHECKSUM_ERR --> 1

ISR_SPR52  
			MOVF RCREG,W,A
			MOVWF LN_RECEIVE_BYTE

			BTFSC LN_RECEIVE_BYTE,7			
			CLRF LN_RECEIVE_TELLER
			
			MOVFF FSR1L,COPY_FSR1L
			MOVFF FSR1H,COPY_FSR1H
			MOVLW HIGH LN_RECEIVE_BUFFER
			MOVWF FSR1H,A
            MOVLW LOW LN_RECEIVE_BUFFER
			ADDWF LN_RECEIVE_TELLER,W
			MOVWF FSR1L,A
			MOVF LN_RECEIVE_BYTE,W
			MOVWF INDF1,A
            	
			MOVFF COPY_FSR1L,FSR1L	
			MOVFF COPY_FSR1H,FSR1H			

			BTFSS LN_RECEIVE_BYTE,7		;is byte opcode?
			BRA ISR_SPR70
						
			MOVF LN_RECEIVE_BYTE,W		;  JA 
			MOVWF LN_RECEIVE_EXORBYTE
			BTFSC LN_RECEIVE_BYTE,6		
			BRA ISR_SPR53
			BTFSC LN_RECEIVE_BYTE,5
			BRA ISR_SPR54 
			MOVLW 2					
			BRA ISR_SPR65
ISR_SPR54
			MOVLW 4		
			BRA ISR_SPR65
ISR_SPR53
			BTFSC LN_RECEIVE_BYTE,5
			BRA ISR_SPR55
			MOVLW 6		
			BRA ISR_SPR65
ISR_SPR55
			BSF LN_NBYTES			
			MOVLW 255		
ISR_SPR65
			MOVWF LN_RECEIVE_TOTBYTES
			INCF LN_RECEIVE_TELLER,F
			BRA ISR_SPR90					
ISR_SPR70	
			MOVF LN_RECEIVE_BYTE,W  		;  NEE
			XORWF LN_RECEIVE_EXORBYTE,F	
			BTFSC LN_NBYTES,A		
			MOVWF LN_RECEIVE_TOTBYTES
			BCF LN_NBYTES,A

			INCF LN_RECEIVE_TELLER,F
			MOVF LN_RECEIVE_TELLER,W	
            CPFSEQ LN_RECEIVE_TOTBYTES
			BRA ISR_SPR90
			MOVLW 1
			MOVWF LN_RECEIVE_TELLER				
			COMF LN_RECEIVE_EXORBYTE,F	
			BTFSS STATUS,Z
            BRA ISR_SPR71
			BSF LN_NIEUWDATA

	    	MOVF RCREG,W
			BRA ISR_SPR90

ISR_SPR71
			BSF LN_CHECKSUM_ERR
            BRA LN_END

;********************************************************************

;	Is er een Byte ontvangen, en µC is ook data aan het zenden?
;		JA: is verzonden Byte gelijk aan ontvangen byte
;			JA: Is er nog een Byte te verzenden?
;				JA: verzend volgende byte
;				NEE: stop zenden LN_TRANSMITDATA --> 0
;			NEE: forceer LINEBREAK op TRANSMITTER uitgang
;			     HERSTART TIMER1 met LINEBREAK		

ISR_SPR90		

			BTFSS LN_ZENDEN            			
			BRA SCMPD                        	;reset START_CARRIER_MASTER_PRIORITY_DELAY

			MOVF LN_RECEIVE_BYTE,W	    		;is er een COLLISION?
			XORWF LN_TRANSMIT_BYTE,W			;  JA: dan LINEBREAK
			BTFSS STATUS,Z,A
			BRA LINEBREAK

			MOVFF FSR1L,COPY_FSR1L
			MOVFF FSR1H,COPY_FSR1H		
			INCF LN_TRANSMIT_TELLER,F
           	MOVLW HIGH LN_TRANSMIT_BUFFER		
			MOVWF FSR1H,A
			MOVLW LOW LN_TRANSMIT_BUFFER
			ADDWF LN_TRANSMIT_TELLER,W     
			MOVWF FSR1L  
			MOVF INDF1,W
			MOVWF LN_TRANSMIT_BYTE

			MOVF LN_TRANSMIT_TELLER,W
            CPFSEQ LN_TRANSMIT_TOTBYTES
			BRA ISR_SPR95
			BCF LN_TRANSMITDATA
			BCF LN_ZENDEN
			MOVFF COPY_FSR1L,FSR1L	
			MOVFF COPY_FSR1H,FSR1H
            BRA SCMPD 


ISR_SPR95
			MOVF INDF1,W
			MOVWF TXREG
			BSF LN_ZENDEN

ISR_SPR97
			MOVFF COPY_FSR1L,FSR1L	
			MOVFF COPY_FSR1H,FSR1H			


LN_END 

        MOVFF LN_COPY_BSR,BSR
        MOVF LN_COPY_WREG,W
        MOVFF LN_COPY_STATUS,STATUS
        BSF INTCON,6            ;waarom weet ik niet maar deze bit moet na afloop LOW priody terug op 1 gezet worden???
        RETFIE


;********************************************************************
;********  INITIATIE  LOCONET             ***************************
;********************************************************************
		
		;****************************************************
		;*******  INIT REGISTERS EN VARIABELEN **************
		;****************************************************

INIT_LOCONET

		CLRF LN_FLAG
		CLRF LN_FLAGS_2
		MOVLW 1
		MOVWF LN_RECEIVE_TELLER

		BCF TRISC,6		;PORTC 6 = LN_TRANSMITTER OUT
		BSF TRISC,7		;PORTC 7 = LN_RECEIVER IN


		;****************************************************
		;*******  INIT LOCONET COMMUNICATIE *****************
		;****************************************************

        MOVLW b'00000001'       ;Comparator1 init
        MOVWF CMCON
        BCF TRISA,4

						; set transmitter en Receiver Baud Rate op 16,66 kHz
		BSF BAUDCON,BRG16
		BSF BAUDCON,TXCKP		; inverteer data out????
		BCF TXSTA,SYNC
		BCF TXSTA,BRGH
		CLRF SPBRGH
		MOVLW 0x77
		MOVWF SPBRG
		BSF RCSTA,SPEN
		BSF RCSTA,CREN
		BSF TXSTA,TXEN
		MOVF RCREG,W			;  om zeker te zijn dat er niets in de ontvangstbuffers zit
		MOVF RCREG,W
		MOVF RCREG,W

        BCF RCSTA,OERR
        BCF RCSTA,FERR


		CLRF TMR1H
		CLRF TMR1L


		;****************************************************
		;*******  INIT INTERRUPT en TIMER 1 *****************
		;****************************************************

		MOVLW b'00110001'		; start timer 1 op 1MHz klokfrequentie
		MOVWF T1CON    
		BSF PIE1,RCIE                   ; interrupt bij elk ingelezen byte
        BSF PIE1,TMR1IE

        BCF IPR1,TMR1IP             ;LOW INT
        BCF IPR1,RCIP
        BSF RCON,IPEN
        BSF INTCON,GIEL            ;enable low priody interrupts
	BSF INTCON,GIEH

		RETURN