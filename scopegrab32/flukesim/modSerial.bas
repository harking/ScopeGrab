
' COMopenPort() - default: 9600,N,8,1, no handshake e.g. no xon/xoff
'   COMemptyRxBuffer()
'   COMsendResponse(BYVAL Resp$, BYVAL asciiMode AS BYTE)
'   COMgetRequest(tTimeout AS INTEGER, BYREF gotFullReq AS INTEGER) AS STRING - timeout in 100ms units
'   COMchangeBaud(newBaud AS INTEGER)
' COMclosePort()

TYPE CommTimeouts
   ReadIntervalTimeout AS DWORD
   ReadTotalTimeoutMultiplier AS DWORD
   ReadTotalTimeoutConstant AS DWORD
   WriteTotalTimeoutMultiplier AS DWORD
   WriteTotalTimeoutConstant AS DWORD
END TYPE
GLOBAL myCommTimeouts AS CommTimeouts

'------------------------------------------------------------------------------
' COMopenPort
' Returns: handle to COM port, or NULL
'------------------------------------------------------------------------------
FUNCTION COMopenPort() AS LONG
    LOCAL hComHdl AS DWORD

    hCOM& = FREEFILE
    FUNCTION = hCOM&
    ERRCLEAR

    COMM OPEN HostPort$ AS #hCOM

    IF ERRCLEAR THEN
        FUNCTION = %FALSE
        EXIT FUNCTION
    END IF

    COMM SET #hCOM, BAUD     = 1200
    COMM SET #hCOM, BYTE     = 8               ' 8 bits per byte
    COMM SET #hCOM, CTSFLOW  = %FALSE
    COMM SET #hCOM, DSRFLOW  = %FALSE
    COMM SET #hCOM, DSRSENS  = %FALSE
    COMM SET #hCOM, DTRFLOW  = %FALSE
    COMM SET #hCOM, DTRLINE  = %FALSE
    COMM SET #hCOM, NULL     = %FALSE      ' don't discard NULL's
    COMM SET #hCOM, PARITY   = %FALSE    ' no parity
    COMM SET #hCOM, RTSFLOW  = 0             ' RTS Flow control
    COMM SET #hCOM, RXBUFFER = 32768    ' rx buffer size
    COMM SET #hCOM, STOP     = 0                ' 1 stop bit
    COMM SET #hCOM, TXBUFFER = 32768    ' tx buffer size
    COMM SET #hCOM, XINPFLOW = %FALSE
    COMM SET #hCOM, XOUTFLOW = %FALSE

    hComHdl = FILEATTR(hCOM,2) ' get OS file handle
    myCommTimeouts.ReadIntervalTimeout = 250
    myCommTimeouts.ReadTotalTimeoutMultiplier = 250
    myCommTimeouts.ReadTotalTimeoutConstant = 2000
    myCommTimeouts.WriteTotalTimeoutMultiplier = 200
    myCommTimeouts.WriteTotalTimeoutConstant = 2000
    SetCommTimeouts(hComHdl, BYVAL VARPTR(myCommTimeouts.ReadIntervalTimeout))

END FUNCTION

'------------------------------------------------------------------------------
' COMchangeBaud
' Change the ports baud rate and clear the rx buffers
'------------------------------------------------------------------------------
SUB COMchangeBaud(newBaud AS INTEGER)
    LOCAL hComHdl AS DWORD
    COMM SET #hCOM, BAUD = newBaud
    hComHdl = FILEATTR(hCOM,2) ' get OS file handle
    myCommTimeouts.ReadIntervalTimeout = 250
    myCommTimeouts.ReadTotalTimeoutMultiplier = 250
    myCommTimeouts.ReadTotalTimeoutConstant = 2000
    myCommTimeouts.WriteTotalTimeoutMultiplier = 200
    myCommTimeouts.WriteTotalTimeoutConstant = 2000
    SetCommTimeouts(hComHdl, BYVAL VARPTR(myCommTimeouts.ReadIntervalTimeout))
    CALL COMemptyRxBuffer()
END SUB

'------------------------------------------------------------------------------
' COMclosePort
'------------------------------------------------------------------------------
SUB COMclosePort()
    COMM CLOSE #hCOM
END SUB

'------------------------------------------------------------------------------
' COMemptyRxBuffer
' If there are characters in the Rx buffer, it is emptied
'------------------------------------------------------------------------------
SUB COMemptyRxBuffer()
    LOCAL txtS AS STRING

    DO WHILE COMM(#hCOM, RXQUE)
        COMM RECV #hCOM, COMM(#hCOM, RXQUE), txtS$
        SLEEP 500
    LOOP

END SUB

'------------------------------------------------------------------------------
' COMsendResponse
' Place string into tx buffer, append CR LF if in ASCII mode
'------------------------------------------------------------------------------
SUB COMsendResponse(BYVAL Resp$, BYVAL asciiMode AS BYTE)
    COMM SEND #hCOM, Resp$
    IF asciiMode<>0 THEN
        COMM SEND #hCOM, CHR$(&H0D)
        COMM SEND #hCOM, CHR$(&H0A)
    END IF
END SUB

'------------------------------------------------------------------------------
' COMgetRequest
' Receive one line of ASCII data, or wait until timeout
' Returns: received ASCII string, sets gotFullReq to TRUE if full line
'------------------------------------------------------------------------------
FUNCTION COMgetRequest(tTimeout AS INTEGER, BYREF gotFullReq AS INTEGER) AS STRING
    DIM Req$, i%, tmp$
    Req$ = ""
    i% = 0
    gotFullReq = 0
    'do While Req$="" and i%<MaxRetries
    '    comm line input #hCOM, Req$ ' does not include CrLf's...
    '    i% = i% + 1
    'loop
    DO WHILE (gotFullReq=0) AND (i%<tTimeout)
        IF COMM(#hCOM, RXQUE)>0 THEN
            i% = 0' reset timeout
            COMM RECV #hCOM, 1, tmp$
            Req$ = Req$ + tmp$
            IF RIGHT$(Req$,2)=CHR$(13,10) OR RIGHT$(Req$,1)=CHR$(13) THEN
                gotFullReq = 1: EXIT LOOP
            END IF
        ELSE ' no chars there yet, wait
            SLEEP 100
            i% = i% + 1
        END IF
    LOOP
    COMgetRequest = Req$
END FUNCTION
