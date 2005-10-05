#ifndef _CSERIAL_H
#define _CSERIAL_H

#include <sstream>
#ifdef __WIN32__
   #include <windows.h>
   #include <process.h>
#else
   // use http://www.easysw.com/~mike/serial/serial.html
   #include "linux_typedefs.h"
   #include <termios.h>
   #define ONESTOPBIT 0
   #define TWOSTOPBIT CSTOPB
#endif


// client class with a public OnCommEvent() for callback
class MyFrame;

// serial buffer sizes
#define MAX_OUT_BUFFER 4096    // transmit buffer
#define MAX_IN_BUFFER  16384   // receive buffer

// func that runs as a separate thread and receives incoming serial data
extern DWORD ReceiverFunc(LPVOID hostClass);

// CSerial class
class CSerial {

public:
   CSerial(MyFrame* clientFrmMain);
   virtual ~CSerial();

   // user accessible funcs
public:
   bool openPort(BYTE portnum, DWORD baudrate,
         BYTE databits, BYTE stopbits,
         BYTE parity, BYTE handshaking);
   bool isOpen();
   bool closePort();

   bool  setBaudrate(DWORD baudrate);
   DWORD getBaudrate();
   bool  setDatabits(BYTE databits);
   bool  setStopbits(BYTE stopbits);

   bool transmitData(const BYTE* databuf, const DWORD bytecount);

   bool getDTR();
   bool setDTR(bool switchOn);
   bool getRTS();
   bool setRTS(bool switchOn);

   void  clearCommErrors();
   DWORD getModembits();

   DWORD getLastError() { return m_PreviousError; }

   // public vars for the receiver thread/function to access
public:
   struct MyFrame*  frmMain; // for accessing the callback OnCommEvent() func
   volatile HANDLE  m_portHdl;
   HANDLE           m_hdl_RxThread;
   volatile bool    m_RxThread_Running;
   volatile bool    m_RxThread_Shutdown;
   DWORD            m_RxThread_ID;
   #ifdef __WIN32__
   OVERLAPPED       m_overlapRx;
   OVERLAPPED       m_overlapTx;
   #endif
   HANDLE           m_evtRead;
   HANDLE           m_evtWrite;

private:
   bool             m_DTRon;
   bool             m_RTSon;
   DWORD            m_PreviousError;

};

#endif // _CSERIAL_H
