
'==============================================================================
'
' FlukeSim - a very trivial simulator for Fluke Scopemeter 97
'
' Note: the GUI /will/ hang, that's normal. Shutdown via Ctrl-Alt-Del and
'       kill the flukesim.exe task there... (didn't bother with working GUI ;-)
'
' Compile with PowerBasic 7.0x
'
' Looks for D:\VStudioSource\scopegrab\flukesim\epsonesc.bin
'
'==============================================================================

$ROOTPATH = "D:\cvsSource\scopegrab32\flukesim\" ' with \ at the end, location on .bin files

#DEBUG ERROR ON
#COMPILE EXE
#REGISTER NONE
#DIM ALL
#OPTION VERSION4
%NOMMIDS = 1   ' no Multimedia ID definitions
%NONEWWAVE = 1 ' no new waveform types except WAVEFORMATEX are defined
%NONEWRIFF = 1 ' no new RIFF formats are defined
%NOJPEGDIB = 1 ' no JPEG DIB definitions
%NONEWIC = 1   ' no new Image Compressor types are defined
%NOBITMAP = 1  ' no extended bitmap info header definition

'==============================================================================
' CONST and GLOBAL includes
'==============================================================================

GLOBAL hCOM AS LONG       ' handle for the serial port
GLOBAL HostPort AS STRING ' com port selection, e.g. "COM1"

#INCLUDE "WIN32API.INC"
#INCLUDE "modSerial.bas"  ' serial port funcs


'==============================================================================
' WndProc - main window, not much to do here...
'==============================================================================

FUNCTION WndProc (BYVAL hWnd AS DWORD, BYVAL wMsg AS DWORD, _
                  BYVAL wParam AS DWORD, BYVAL lParam AS LONG) EXPORT AS LONG
    FUNCTION = DefWindowProc(hWnd, wMsg, wParam, lParam)
END FUNCTION


'==============================================================================
' WINMAIN
'==============================================================================


FUNCTION WINMAIN (BYVAL hInstance AS LONG, _
                  BYVAL hPrevInstance AS LONG, _
                  lpCmdLine AS ASCIIZ PTR, _
                  BYVAL iCmdShow AS LONG) EXPORT AS LONG

    LOCAL CmdLn AS STRING
    LOCAL ScopeMeterMode AS LONG, i AS LONG, j AS LONG, maxEpsLen AS LONG
    LOCAL gotLine AS INTEGER
    LOCAL reqStr AS STRING, tmpStr AS STRING, txBuf AS STRING *64

    ' create application window
    LOCAL Msg       AS tagMsg
    LOCAL wce       AS WndClassEx
    LOCAL hWnd      AS DWORD
    LOCAL szAppName AS ASCIIZ * 80
    szAppName = "FlukeSim 97"
    wce.cbSize      = SIZEOF(wce)
    wce.style       = 0 '%CS_HREDRAW OR %CS_VREDRAW
    wce.lpfnWndProc = CODEPTR(WndProc)
    wce.cbClsExtra    = 0
    wce.cbWndExtra    = 0
    wce.hInstance     = hInstance
    wce.lpszMenuName  = %NULL
    wce.lpszClassName = VARPTR(szAppName)
    RegisterClassEx wce
    hWnd = CreateWindow(szAppName, _               ' window class name
                        "The Hello Program", _     ' window caption
                        %WS_OVERLAPPEDWINDOW, _    ' window style
                        %CW_USEDEFAULT, _          ' initial x position
                        %CW_USEDEFAULT, _          ' initial y position
                        500, _          ' initial x size
                        800, _          ' initial y size
                        %NULL, _                   ' parent window handle
                        %NULL, _                   ' window menu handle
                        hInstance, _               ' program instance handle
                        BYVAL %NULL)               ' creation parameters
    IF hWnd = 0 THEN
        MSGBOX "Unable to create window"
        EXIT FUNCTION
    END IF
    DIALOG FONT "Arial", 8
    ShowWindow hWnd, iCmdShow
    UpdateWindow hWnd

    CONTROL ADD TEXTBOX, hWnd, 1, "FlukeSim 97 trace", 5, 5, 480, 780, %ES_MULTILINE+%ES_AUTOVSCROLL+%ES_READONLY+%WS_VSCROLL

    ' default mode: ScopeMeter 97, on COM2 (e.g. virtual serial port)
    HostPort$ = "COM2"
    ScopeMeterMode = 97

    ' crudely parse the command line params ('[COMx] [97|196]')
    CmdLn$ = TRIM$(COMMAND$)
    IF LEN(CmdLn$) > 0 THEN
        IF INSTR(CmdLn$,"97")>0 THEN ScopeMeterMode=97
        IF INSTR(CmdLn$,"196")>0 THEN ScopeMeterMode=196
        IF INSTR(CmdLn$,"COM1")>0 THEN HostPort$="COM1"
        IF INSTR(CmdLn$,"COM2")>0 THEN HostPort$="COM2"
        IF INSTR(CmdLn$,"COM3")>0 THEN HostPort$="COM3"
        IF INSTR(CmdLn$,"COM4")>0 THEN HostPort$="COM4"
        IF INSTR(CmdLn$,"COM5")>0 THEN HostPort$="COM5"
        IF INSTR(CmdLn$,"COM6")>0 THEN HostPort$="COM6"
    END IF

    CALL COMopenPort()
    CALL COMchangeBaud(1200)

    CONTROL GET TEXT hWnd, 1 TO tmpStr: CONTROL SET TEXT hWnd, 1, tmpStr + CHR$(13,10) + "Using " + HostPort$ + " and faking ScopeMeter model " + STR$(ScopeMeterMode) + CHR$(13,10)

    'CALL COMsendResponse("0", 1)
    'CALL COMsendResponse(CHR$(&H37,&H34,&H35,&H33,&H2C),0)
    'CALL COMsendResponse(EpsIm, 0)

    DO

        'DO WHILE GetMessage(Msg, %NULL, 0, 0)
        '    TranslateMessage Msg
        '    DispatchMessage Msg
        'LOOP

        DIALOG DOEVENTS
        UpdateWindow hWnd

        reqStr = COMgetRequest(1, gotLine) ' 50*100ms=5s until timeout

        IF gotLine=0 THEN ITERATE LOOP

        CONTROL GET TEXT hWnd, 1 TO tmpStr
        CONTROL SET TEXT hWnd, 1, tmpStr + "<RX> " + reqStr + CHR$(13,10)

        IF LEN(reqStr)<2 OR reqStr=CHR$(10,13) THEN
            CALL COMsendResponse("0", 1)
            CONTROL GET TEXT hWnd, 1 TO tmpStr
            CONTROL SET TEXT hWnd, 1, tmpStr + "<TX> ack 0" + CHR$(13,10)
        ELSEIF LEFT$(reqStr,2)="ID" THEN
            CALL COMsendResponse("0", 1)
            CONTROL GET TEXT hWnd, 1 TO tmpStr
            CONTROL SET TEXT hWnd, 1, tmpStr + "<TX> ack 0" + CHR$(13,10)
            CALL COMsendResponse("ScopeMeter model 97 ; V4.02 ; 92-11-03", 1)
            CONTROL GET TEXT hWnd, 1 TO tmpStr
            CONTROL SET TEXT hWnd, 1, tmpStr + "<TX> ScopeMeter model 97 ; V4.02 ; 92-11-03" + CHR$(13,10)
        ELSEIF INSTR(reqStr,"PC 19200")>0 OR INSTR(reqStr,"PC19200") THEN
            CALL COMchangeBaud(19200)
            CALL COMsendResponse("0", 1)
            CONTROL GET TEXT hWnd, 1 TO tmpStr
            CONTROL SET TEXT hWnd, 1, tmpStr + "Changed baud to 19200" + CHR$(13,10) + "<TX> ack 0" + CHR$(13,10)
        ELSEIF INSTR(reqStr,"PC 1200")>0 OR INSTR(reqStr,"PC1200") THEN
            CALL COMchangeBaud(1200)
            CALL COMsendResponse("0", 1)
            CONTROL GET TEXT hWnd, 1 TO tmpStr
            CONTROL SET TEXT hWnd, 1, tmpStr + "Changed baud to 1200" + CHR$(13,10) + "<TX> ack 0" + CHR$(13,10)
        ELSEIF INSTR(reqStr,"QW")>0 THEN
            CONTROL GET TEXT hWnd, 1 TO tmpStr
            CONTROL SET TEXT hWnd, 1, tmpStr + "<TX> contents of 97wavea.bin" + CHR$(13,10)
            i = FREEFILE
            OPEN $ROOTPATH+"97wavea.bin" FOR BINARY ACCESS READ AS i
            FOR j=1 TO LOF(i) STEP 64
                SEEK i, j
                GET$ i, 64, txBuf
                CALL COMsendResponse(txBuf, 0)
            NEXT j
            CLOSE i
        ELSEIF INSTR(reqStr,"QG129")>0 OR INSTR(reqStr,"QP")>0 THEN
            'CALL COMsendResponse("0", 1)
            'CALL COMsendResponse(CHR$(&H37,&H34,&H35,&H33,&H2C),0)
            CONTROL GET TEXT hWnd, 1 TO tmpStr
            CONTROL SET TEXT hWnd, 1, tmpStr + "<TX> ack 0" + CHR$(13,10) + "<TX> length string '7453,'" + CHR$(13,10)
            'j = 0
            'FOR i = 1 TO 98
            '    j = j + LEN(EpsIm(i))
            '    CONTROL GET TEXT hWnd, 1 TO tmpStr
            '    CONTROL SET TEXT hWnd, 1, tmpStr + CHR$(13,10) + "<TX> binary data (" + STR$(j) + "/" + STR$(maxEpsLen) + ")" ' : " + LEFT$(reqStr,64) + CHR$(13,10)
            '    CALL COMsendResponse(EpsIm(i), 0)
            '    DIALOG DOEVENTS
            '    UpdateWindow hWnd
            'NEXT i
            i = FREEFILE
            OPEN $ROOTPATH+"epsonesc.bin" FOR BINARY ACCESS READ AS i
            FOR j=1 TO LOF(i) STEP 64
                SEEK i, j
                GET$ i, 64, txBuf
                CALL COMsendResponse(txBuf, 0)
            NEXT j
            CLOSE i

        END IF

        DIALOG DOEVENTS
        UpdateWindow hWnd

    ' Ctrl-C to exit
    LOOP

END FUNCTION 'WINMAIN
