#include <windows.h>
#include "CSerial.h"

#include "main.h"

// ****************************************************************
// *  C'stor
// *
// *  Arguments: ptr to client frame that has a public OnCommEvent
// *             member function for callback

CSerial::CSerial(MyFrame* clientFrmMain) {
    // clear win32 error temp
	m_PreviousError = 0;

    // reset port variables
	m_DTRon = FALSE;
	m_RTSon = FALSE;
	m_portHdl = NULL;
	m_hdl_RxThread = NULL;
    m_RxThread_Running = FALSE;
    m_RxThread_Shutdown = FALSE;

	// parent window pointer
	frmMain = clientFrmMain;
}


// ****************************************************************
// *  D'stor
// *
// *  On exit, auto-close the currently open port
// *

CSerial::~CSerial() {
	this->closePort();
}






// ****************************************************************
// *  BOOL openPort(int portnum, int baudrate, ...)
// *
// *  Arguments: portnum - COM port to open (1..64)
// *             baudrate - baud rate (1200..115200)
// *             databits - num of databits (1..8)
// *             stopbits - num of stopbits (0..8)
// *             parity   - 'N'=none, 'O'=odd, 'E'=even,
// *                        'M'=mark, 'S'=space
// *                        default is 'N'
// *             handshaking - 0=none, 1=xon/xoff, 2=rts/cts, 3=xon/rts
// *
// *  Returns: TRUE if the port has been opened

BOOL CSerial::openPort(
    BYTE portnum, DWORD baudrate,
    BYTE databits, BYTE stopbits,
	BYTE parity, BYTE handshaking ) {

    // check for parameters for valid range
    if ( portnum<1 || portnum>64 ) return FALSE;
    if ( baudrate<1200 || baudrate>115200 ) return FALSE;
    if ( databits<5 || databits>8 ) return FALSE;
    if ( ONESTOPBIT!=stopbits && ONE5STOPBITS!=stopbits
         && TWOSTOPBITS!=stopbits ) return FALSE;

	// if the port is already open, close it first,
	// then re-open with the new settings
	if ( TRUE == this->isOpen() ) { this->closePort(); }

	// set main class for the rx/tx thread -> OnComm callback
	this->frmMain = frmMain;

    // try to open the system file "COMx"
	std::ostringstream portstr;
	portstr << "COM" << (int)portnum << std::flush;
	this->m_portHdl = CreateFile (
       (LPCSTR)portstr.str().c_str(),
       GENERIC_READ | GENERIC_WRITE, // open for read/write
       0, NULL, // no sharing and NULL ptr to security attrib structure
       OPEN_EXISTING, // must use this to open COM
       FILE_FLAG_OVERLAPPED, NULL // use overlapped rx/tx for speed
    );
    
	if ( INVALID_HANDLE_VALUE == this->m_portHdl ) {
		m_PreviousError = GetLastError();
		return FALSE;
	}

    // port was opened, set the buffer sizes
	SetupComm ( this->m_portHdl, MAX_IN_BUFFER, MAX_OUT_BUFFER );

	// attempt to get a copy of the current Device Control Block
    DCB commDCB; commDCB.DCBlength = sizeof(DCB);
	if ( !GetCommState(this->m_portHdl, &commDCB) ) {
		m_PreviousError = GetLastError();
        CloseHandle(m_portHdl);
		return FALSE;
	}
	
    // adjust parity
    commDCB.fParity = TRUE;
    if ( 'O'==parity ) { commDCB.Parity=ODDPARITY; }
    else if ( 'E'==parity ) { commDCB.Parity=EVENPARITY; }
    else if ( 'M'==parity ) { commDCB.Parity=MARKPARITY; }
    else if ( 'S'==parity ) { commDCB.Parity=SPACEPARITY; }
    else { commDCB.fParity=FALSE; commDCB.Parity=NOPARITY; } // default: no parity

    // adjust data rate and bits
    commDCB.BaudRate = baudrate;
    commDCB.ByteSize = databits;
    commDCB.StopBits = stopbits;
    commDCB.fBinary  = TRUE;   // windows supports only binary mode
	commDCB.fNull    = FALSE;  // don't discard '\0's

    // set handshaking
    if(handshaking>3) handshaking=0;
	switch (handshaking) {
        case 0: // none
		    commDCB.fOutX = FALSE; commDCB.fInX = FALSE;
            commDCB.fOutxCtsFlow = FALSE;
            commDCB.fRtsControl  = RTS_CONTROL_ENABLE; // normal RTS
            break;
    	case 1: // xon/xoff
            commDCB.fOutX = TRUE; commDCB.fInX = TRUE;
            commDCB.fOutxCtsFlow = FALSE;
            commDCB.fRtsControl  = RTS_CONTROL_ENABLE; // normal RTS
            break;
        case 2: // rts/cts
            commDCB.fOutX = FALSE; commDCB.fInX = FALSE;
            commDCB.fOutxCtsFlow = TRUE;
            commDCB.fRtsControl  = RTS_CONTROL_HANDSHAKE; // RTS for handshake
    		break;
        case 3: // xon/rts
            commDCB.fOutX = TRUE; commDCB.fInX = TRUE;
            commDCB.fOutxCtsFlow = TRUE;
            commDCB.fRtsControl  = RTS_CONTROL_HANDSHAKE; // RTS for handshake
            break;
	}

    // fill out remainder of DCB
	commDCB.fDtrControl        = DTR_CONTROL_ENABLE; m_DTRon = TRUE;
 	commDCB.fDsrSensitivity    = 0;
	commDCB.fOutxDsrFlow       = FALSE;
	commDCB.fTXContinueOnXoff  = 1;
	commDCB.fAbortOnError      = FALSE; // allow continuing despite a set error flag
	commDCB.fDummy2   = 0;
	commDCB.ErrorChar = '*';
	commDCB.fErrorChar = FALSE;   // don't replace parity err bytes with ErrorChar
	commDCB.EofChar   = 0x01;
	commDCB.EvtChar   = 0x02;
	commDCB.XonChar   = 0x11;
	commDCB.XonLim    = MAX_IN_BUFFER-16; // RX with up to full rx buffer, 16 spare
	commDCB.XoffChar  = 0x13;
	commDCB.XoffLim   = 16;     // halt sender when RX buffer full with 16 spare

	// finally, try to apply these changes
	if ( !SetCommState(this->m_portHdl, &commDCB) ) {
		m_PreviousError = GetLastError();
        CloseHandle(m_portHdl);
		return FALSE;
	}

    // update the port timeout settings so that the read/wait
    // operations won't time out (using event driven rx/tx)
    COMMTIMEOUTS commTimeouts;
	commTimeouts.ReadIntervalTimeout           = MAXDWORD;
	commTimeouts.ReadTotalTimeoutMultiplier    = 0;
	commTimeouts.ReadTotalTimeoutConstant      = 0;
	commTimeouts.WriteTotalTimeoutMultiplier   = 0;
	commTimeouts.WriteTotalTimeoutConstant     = 0;
	if ( !SetCommTimeouts(this->m_portHdl, &commTimeouts) ) {
		m_PreviousError = GetLastError();
        CloseHandle(m_portHdl);
		return FALSE;
	}

    // prepare the Overlapped I/O structure for the
    // write operation, with reference to a custom event
    // that has manual reset and is initially off
	this->m_evtWrite = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_overlapTx.hEvent         = m_evtWrite;
	m_overlapTx.Internal       = 0;
	m_overlapTx.InternalHigh   = 0;
	m_overlapTx.Offset         = 0;
	m_overlapTx.OffsetHigh     = 0;

    // prepare the Overlapped I/O structure for the
    // read operation, with reference to a custom event
    // that has manual reset and is initially off
	this->m_evtRead = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_overlapRx.hEvent         = m_evtRead;
	m_overlapRx.Internal       = 0;
	m_overlapRx.InternalHigh   = 0;
	m_overlapRx.Offset         = 0;
	m_overlapRx.OffsetHigh     = 0;

    // attempt to start the receiver thread
    this->m_RxThread_Running = TRUE;
    this->m_RxThread_Shutdown = FALSE;

    SECURITY_ATTRIBUTES secAttrib;
    secAttrib.lpSecurityDescriptor  = NULL;
    secAttrib.bInheritHandle        = FALSE;
    secAttrib.nLength               = sizeof(secAttrib);
    m_hdl_RxThread = (HANDLE)_beginthreadex(
        (void *)(&secAttrib),
        0,
        (unsigned (__stdcall *)(void *))&ReceiverFunc,
        this, 0,
        (unsigned int *)(&m_RxThread_ID)
    );

    // if starting the thread failed, close the port
    if ( INVALID_HANDLE_VALUE==m_hdl_RxThread || NULL==m_hdl_RxThread ) {
        closePort();
        return FALSE;
    }
    // seems like everything went fine
	return TRUE;
}

// ****************************************************************
// *  BOOL isOpen()
// *
// *  Returns: TRUE if the port has been opened

BOOL CSerial::isOpen() {
	if ( NULL==m_portHdl || INVALID_HANDLE_VALUE==m_portHdl ) return FALSE;
    if ( FALSE==m_RxThread_Running ) return FALSE;
    return TRUE;
}

// ****************************************************************
// *  BOOL closePort()
// *
// *  Calling this will close the current serial port,
// *  if it was open.
// *
// *  Returns: TRUE if closing was successful, FALSE if an error occured

BOOL CSerial::closePort() {

	if ( NULL==m_hdl_RxThread
         || INVALID_HANDLE_VALUE==m_hdl_RxThread ) {
        this->m_RxThread_Running = FALSE;
        this->m_RxThread_Shutdown = FALSE;
    }
    
    // time out all pending operations
	COMMTIMEOUTS commTimeouts;
    if ( NULL!=m_portHdl && INVALID_HANDLE_VALUE!=m_portHdl ) {
	    commTimeouts.ReadIntervalTimeout = 1;
	    commTimeouts.ReadTotalTimeoutMultiplier = 1;
	    commTimeouts.ReadTotalTimeoutConstant = 1;
        commTimeouts.WriteTotalTimeoutMultiplier = 1;
        commTimeouts.WriteTotalTimeoutConstant = 1;
        SleepEx(300,FALSE);
        if(NULL!=this->frmMain) { this->frmMain->DoEvents(); }
	    SetCommTimeouts(m_portHdl, &commTimeouts);
        SleepEx(300,TRUE);
        if(NULL!=this->frmMain) { this->frmMain->DoEvents(); }
    }
    
    // stop the receiver thread
	if ( TRUE==this->m_RxThread_Running ) {

        // signal a shutdown event to the thread and exit all
        // WaitForEvent's in the thread by resetting the port mask
        this->m_RxThread_Shutdown = TRUE; // (is set back to FALSE by the thread)
		SetCommMask ( m_portHdl, 0 );

		// give the thread a while to terminate
        for ( int k=0; (TRUE==this->m_RxThread_Shutdown) && k<20; ++k ) {
			Sleep(50);
			frmMain->DoEvents();
		}

        // close the thread handle
        CloseHandle(m_hdl_RxThread);
        m_hdl_RxThread = NULL;

        m_RxThread_Running = FALSE;
        m_RxThread_Shutdown = FALSE;
	}

	// try to close the serial port file handle and both rx/tx event handles
	if ( NULL!=m_portHdl && INVALID_HANDLE_VALUE!=m_portHdl ) {
		if ( 0==CloseHandle(m_portHdl)
             || 0==CloseHandle(m_overlapTx.hEvent)
             || 0==CloseHandle(m_overlapRx.hEvent) ) {
			m_PreviousError = GetLastError();
			return FALSE;
		}
	}

	return TRUE;
}



// ****************************************************************
// *  DWORD ReceiverFunc(LPVOID hostClass)
// *
// *  Separate thread for receiving serial port data. Don't
// *  call this directly! Does not exit until signaled
// *  to do so with CSerial::m_RxThread_Shutdown = TRUE.
// *
// *  Arguments: pointer to the host CSerial class
// *
// *  Returns: 0 when signaled to exit

DWORD ReceiverFunc(LPVOID hostClass) {

    // local buffer for the received bytes
    BYTE  rxbuf[MAX_IN_BUFFER+1];
	DWORD bytesread = 0;

    // locals
	DWORD rxevent = 0;
	DWORD commerr = 0;
	BOOL ret = FALSE;
    BOOL continueReading = TRUE;

    // get host class (of type CSerial)
	CSerial* ptrHost = (CSerial*)hostClass;

	// thread started, set the info flags
	ptrHost->m_RxThread_Running = TRUE;
    ptrHost->m_RxThread_Shutdown = FALSE;

	// modify the port event mask so that WaitCommEvent
	// returns on received characters or error conditions
	SetCommMask ( ptrHost->m_portHdl, EV_RXCHAR | EV_ERR );

    // receive chars, until thread is told to terminate
	while ( FALSE==ptrHost->m_RxThread_Shutdown ) {

		// check if there are any characters ready to be read
        rxevent = 0;
		ret = WaitCommEvent (
                ptrHost->m_portHdl,
                &rxevent, &(ptrHost->m_overlapRx)
            );

        // if the error was that the operation is pending, wait till it completes
		if ( !ret && (ERROR_IO_PENDING==GetLastError()) ) {
           // wait until some characters are available
           DWORD dword1;
		   ret = GetOverlappedResult (
                    ptrHost->m_portHdl,
                    &(ptrHost->m_overlapRx),
                    &dword1,
                    TRUE
                 );

		   if(!ret) {
                // something during the wait went wrong - skip, ignore
                // and continue to receive
                ResetEvent(ptrHost->m_overlapRx.hEvent);
                continue;
           } else {
                // wait went ok, get current state, contineu
                // with reading the bytes farther below
                GetCommMask ( ptrHost->m_portHdl, &rxevent );
           }
		}

        // operation completed, check what event this was

		if ( 0!=(rxevent&EV_ERR) ) {  // receiving error
            // clear the error event
			ClearCommError ( ptrHost->m_portHdl, &commerr, NULL );
		}

        if ( 0!=(rxevent&EV_RXCHAR) ) { // receiving success, got data
            do { // read all bytes
                // clear old errors
                ClearCommError(ptrHost->m_portHdl, &commerr, NULL);
                // read next bytes
                DWORD reqcount = MAX_IN_BUFFER;
                ResetEvent ( ptrHost->m_overlapRx.hEvent );
                continueReading = ReadFile (
                            ptrHost->m_portHdl,
                            (LPVOID)rxbuf, reqcount, &bytesread,
                            &(ptrHost->m_overlapRx) // overlapped receive/transmit
                         );

                if ( TRUE==continueReading ) { // read success
                    // use overlappedresults to get the
                    // precise nr of bytes that were read
                    GetOverlappedResult (
                            ptrHost->m_portHdl,
                            &(ptrHost->m_overlapRx),
                            &bytesread, FALSE
                        );

                    if (bytesread>0) {
                        // pass data on to the client application
	                    if (ptrHost->frmMain != NULL) {
					       ptrHost->frmMain->OnCommEvent((const BYTE*)rxbuf, (const DWORD)bytesread);
                        }
                    } else {
                        // read 0 bytes (==finished), exit loop
                        continueReading = FALSE;
                    }
                } else {
                    // file read error, exit loop
                    continueReading = FALSE;
                }
            } while(TRUE==continueReading); // read while still data left
        }

        // clear the bytes-received event
        ResetEvent(ptrHost->m_overlapRx.hEvent);

	}//(thread loop)

    // Exited the receiver loop because m_RxThread_Shutdown was set to TRUE
    // Set it back to FALSE to indicate that the thread has closed now.
    ptrHost->m_RxThread_Shutdown = FALSE;
	ptrHost->m_RxThread_Running = FALSE;

    return 0;
}



// ****************************************************************
// *  BOOL transmitData(const BYTE* databuf, const DWORD bytecount)
// *
// *  Send the data of the passed data buffer over the serial port.
// *
// *  Arguments: *databuf - BYTE pointer to the start of the data buffer
// *             bytecount - number of bytes to transmit from the buffer
// *
// *  Returns: TRUE if the databits nr was successfully changed

BOOL CSerial::transmitData(const BYTE* databuf, const DWORD bytecount) {

	DWORD result = 0;

    m_PreviousError = 0;
    
    // can transmit only on opened port
    if ( FALSE==isOpen() ) return FALSE;

    // basic checks
    if ( NULL==databuf || bytecount<1 ) return FALSE;

	// reset tx event before writing
	ResetEvent(m_overlapTx.hEvent);

    // try to write the byte(s), blocking until the write has completed
	DWORD writecount = 0;
	result = WriteFile (
                this->m_portHdl,
                databuf, bytecount,
                &writecount,
                &this->m_overlapTx
            );

    // immediate success?
    if ( result && writecount==bytecount ) {
        return TRUE; // all bytes written, done
    }

    // write failed, check the error code
    m_PreviousError = GetLastError();
    if ( ERROR_IO_PENDING!=m_PreviousError ) {
        clearCommErrors();
        return FALSE;
    }

    // IO_PENDING as an "error" code, wait for the
    // pending write operation to complete

    DWORD commErr = 0;
    ClearCommError(m_portHdl, &commErr, NULL); // clear error flags
    result = WaitForSingleObject ( this->m_overlapTx.hEvent, (1000+bytecount*10) ); // blocking wait, 1sec max plus 10ms per byte

    switch (result) {

        case WAIT_TIMEOUT:
            // the infinitely blocking wait timed out, hmm...
            m_PreviousError = GetLastError();
            clearCommErrors();
            return FALSE;

        case WAIT_OBJECT_0:

            // likely success, get the result code for the operation
	        result = GetOverlappedResult ( m_portHdl, &this->m_overlapTx, &writecount, FALSE );

	        if ( 0==result                     // wait error,
                 || writecount!=bytecount ) { // or wrote less than expected
                m_PreviousError = GetLastError();
                clearCommErrors();
                return FALSE;
            }

            // full success
            return TRUE;

        default:
            // the wait operation failed
            clearCommErrors();
	        return FALSE;
            break;

    }

    //exec shouldn't reach here
    return FALSE;
}




// ****************************************************************
// *  BOOL getDTR()
// *
// *  Returns: TRUE if the DTR pin is currently set High

BOOL CSerial::getDTR() { return m_DTRon; }

// ****************************************************************
// *  BOOL setDTR(BOOL switchOn)
// *
// *  Tries to change the state of the DTR pin,
// *  according to switchOn. TRUE will set the DTR
// *  pin high.
// *
// *  Arguments:  switchOn  -  TRUE to set DTR high
// *                           FALSE to set DTR low
// *  Returns: TRUE is setting the DTR state was successful

BOOL CSerial::setDTR(BOOL switchOn) {
	DWORD result;

    if(NULL==m_portHdl) return FALSE;

	if(TRUE==switchOn) {
 	  result = EscapeCommFunction(m_portHdl, SETDTR);
    } else {
 	  result = EscapeCommFunction(m_portHdl, CLRDTR);
    }

    if(result) {
        m_DTRon = switchOn;
        return TRUE;
    }

    m_PreviousError = GetLastError();
	return FALSE;
}

// ****************************************************************
// *  BOOL getRTS()
// *
// *  Returns: TRUE if the RTS pin is currently set High

BOOL CSerial::getRTS() { return m_RTSon; }

// ****************************************************************
// *  BOOL setRTS(BOOL switchOn)
// *
// *  Tries to change the state of the RTS pin,
// *  according to switchOn. TRUE will set the RTS
// *  pin high.
// *
// *  Arguments:  switchOn  -  TRUE to set RTS high
// *                           FALSE to set RTSlow
// *  Returns: TRUE is setting the RTS state was successful

BOOL CSerial::setRTS(BOOL switchOn) {
	DWORD result;

    if(NULL==m_portHdl) return FALSE;

	if(TRUE==switchOn) {
 	  result = EscapeCommFunction(m_portHdl, SETRTS);
    } else {
 	  result = EscapeCommFunction(m_portHdl, CLRRTS);
    }

    if(result) {
        m_RTSon = switchOn;
        return TRUE;
    }

    m_PreviousError = GetLastError();
	return FALSE;
}

// ****************************************************************
// *  BOOL setBaudrate(DWORD baudrate)
// *
// *  Change the baud rate of the currently open port.
// *
// *  Arguments: baudrate - new baudrate to set
// *
// *  Returns: TRUE if the baud rate was successfully changed

BOOL CSerial::setBaudrate(DWORD baudrate) {
    // can change only on opened port
    if ( FALSE==isOpen() ) return FALSE;

    // baud rate is valid/supported?
    if ( baudrate<1200 || baudrate>115200 ) return FALSE;

    // read the Device Control Block with the current settings
    DCB commDCB;
    commDCB.DCBlength = sizeof(DCB);
    if ( !GetCommState(m_portHdl, &commDCB) ) {
        m_PreviousError = GetLastError();
        return FALSE;
    }

    // write the new baud rate
    commDCB.BaudRate = baudrate;
    if ( !SetCommState(m_portHdl, &commDCB) ) {
        m_PreviousError = GetLastError();
        return FALSE;
    }

	return TRUE;
}

// ****************************************************************
// *  DWORD getBaudrate()
// *
// *  Returns the current baud rate of the open port.
// *
// *  Returns: 0 on error, or the actual baud rate

DWORD CSerial::getBaudrate() {
    // can read only on opened port
    if ( FALSE==isOpen() ) return 0;

    // read the Device Control Block with the current settings
    DCB commDCB;
    commDCB.DCBlength = sizeof(DCB);
    if ( !GetCommState(m_portHdl, &commDCB) ) {
        m_PreviousError = GetLastError();
        return 0;
    }
    
    // return the baudrate
    return commDCB.BaudRate;
}

// ****************************************************************
// *  BOOL setDatabits(BYTE databits)
// *
// *  Change the databits setting of the currently open port.
// *
// *  Arguments: databits - new nr of data bits
// *
// *  Returns: TRUE if the databits nr was successfully changed

BOOL CSerial::setDatabits(BYTE databits) {
    // can change only on opened port
    if ( FALSE==isOpen() ) return FALSE;

    // databit nr is valid/supported?
    if ( databits<5 || databits>8 ) return FALSE;

    // read the Device Control Block with the current settings
    DCB commDCB;
    commDCB.DCBlength = sizeof(DCB);
    if ( !GetCommState(m_portHdl, &commDCB) ) {
        m_PreviousError = GetLastError();
        return FALSE;
    }

    // update to new databit nr
    commDCB.ByteSize = databits;
    if ( !SetCommState(m_portHdl, &commDCB) ) {
        m_PreviousError = GetLastError();
        return FALSE;
    }

	return TRUE;
}

// ****************************************************************
// *  BOOL setStopbits(BYTE stopbits)
// *
// *  Change the stopbits setting of the currently open port.
// *
// *  Arguments: stopbits - new nr of stop bits
// *
// *  Returns: TRUE if the stopbits nr was successfully changed

BOOL CSerial::setStopbits(BYTE stopbits) {
    // can change only on opened port
    if ( FALSE==isOpen() ) return FALSE;

    // stopbits nr is valid/supported?
    if ( ONESTOPBIT!=stopbits && ONE5STOPBITS!=stopbits
         && TWOSTOPBITS!=stopbits ) return FALSE;

    // read the Device Control Block with the current settings
    DCB commDCB;
    commDCB.DCBlength = sizeof(DCB);
    if ( !GetCommState(m_portHdl, &commDCB) ) {
        m_PreviousError = GetLastError();
        return FALSE;
    }

    // update to new databit nr
    commDCB.StopBits = stopbits;
    if ( !SetCommState(m_portHdl, &commDCB) ) {
        m_PreviousError = GetLastError();
        return FALSE;
    }

	return TRUE;
}

// ****************************************************************
// *  DWORD getModembits()
// *
// *  Read the pin states aka "modem bits" of the currently open port.
// *
// *  Returns: a DWORD value received from Win32 API GetCommModemStatus()
// *           special values: 0=port not open, 0xFFFF=read error

DWORD CSerial::getModembits() {
    // can read only on opened port
    if ( FALSE==isOpen() ) return 0;

    // read the modem pin states
	DWORD currentBits;
    if ( !GetCommModemStatus(m_portHdl, &currentBits) ) {
        m_PreviousError = GetLastError();
        return 0xFFFF;
    }

	return currentBits;
}






// ****************************************************************
// *  void clearCommErrors()
// *
// *  Clears the serial port error flags if they were set.

void CSerial::clearCommErrors() {
    // can check only if port is open
    if ( FALSE==isOpen() ) return;

    // read the serial port event mask
    DWORD commEvt = 0;
    if ( !GetCommMask(m_portHdl, &commEvt) ) return;

    // if an error event is indicated, clear the errors
    DWORD commErr = 0;
    if ( commEvt & EV_ERR ) {
        ClearCommError(m_portHdl, &commErr, NULL);
    }
    return;
}

