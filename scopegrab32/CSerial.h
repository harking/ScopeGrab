#ifndef _CSERIAL_H
#define _CSERIAL_H

#include <windows.h>
#include <sstream>
#include <process.h>

// client class with a public OnCommEvent() for callback
class MyFrame;

// serial buffer sizes
#define MAX_OUT_BUFFER 4096   // transmit buffer
#define MAX_IN_BUFFER  4096   // receive buffer

// func that runs as a separate thread and receives incoming serial data
extern DWORD ReceiverFunc(LPVOID hostClass);

// CSerial class
class CSerial {

public:
	CSerial(MyFrame* clientFrmMain);
	virtual ~CSerial();

    // user accessible funcs
public:
	BOOL openPort(BYTE portnum, DWORD baudrate,
            BYTE databits, BYTE stopbits,
            BYTE parity, BYTE handshaking);
	BOOL isOpen();
	BOOL closePort();

	BOOL  setBaudrate(DWORD baudrate);
    DWORD getBaudrate();
	BOOL  setDatabits(BYTE databits);
	BOOL  setStopbits(BYTE stopbits);

	BOOL transmitData(const BYTE* databuf, const DWORD bytecount);

	BOOL getDTR();
	BOOL setDTR(BOOL switchOn);
	BOOL getRTS();
	BOOL setRTS(BOOL switchOn);

    void  clearCommErrors();
    DWORD getModembits();
	
	DWORD getLastError() { return m_PreviousError; }
	
    // public vars for the receiver thread/function to access
public:
	struct MyFrame*  frmMain; // for accessing the callback OnCommEvent() func
	volatile HANDLE  m_portHdl;
	HANDLE           m_hdl_RxThread;
    volatile BOOL    m_RxThread_Running;
    volatile BOOL    m_RxThread_Shutdown;
    DWORD            m_RxThread_ID;
	OVERLAPPED       m_overlapRx;
	HANDLE           m_evtRead;
	OVERLAPPED       m_overlapTx;
	HANDLE           m_evtWrite;

private:
	BOOL             m_DTRon;
	BOOL             m_RTSon;
	DWORD            m_PreviousError;

};

#endif // _CSERIAL_H

