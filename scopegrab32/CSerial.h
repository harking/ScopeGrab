/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef _CSERIAL_H
#define _CSERIAL_H

#include <sstream>
#ifdef __WIN32__
   #include <windows.h>
   #include <process.h>
#else
   #include "linux_typedefs.h"
   #include <termios.h>
   #include <termio.h>
   #include <fcntl.h>
   #include <errno.h>
   #include <unistd.h>
   #include <pthread.h>
   #define ONESTOPBIT   0
   #define ONE5STOPBITS 0
   #define TWOSTOPBITS  CSTOPB
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
         BYTE parity, BYTE handshaking, const char* portStr);
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
   volatile bool    m_RxThread_Running;
   volatile bool    m_RxThread_Shutdown;
   #ifdef __WIN32__
   DWORD            m_RxThread_ID;
   OVERLAPPED       m_overlapRx;
   OVERLAPPED       m_overlapTx;
   HANDLE           m_hdl_RxThread;
   volatile HANDLE m_portHdl;
   HANDLE           m_evtRead;
   HANDLE           m_evtWrite;
   #else
   int               m_portHdl;
   struct termios    m_serialopt;
   pthread_t         m_RxThread;
   pthread_t*        m_hdl_RxThread;
   #endif

private:
   bool             m_DTRon;
   bool             m_RTSon;
   DWORD            m_PreviousError;

   #ifndef __WIN32__
   int   baudToValue(unsigned int baud);
   unsigned int valueToBaud(int value);
   static void ReceiverThread(void* hostClass);
   #endif
};

#endif // _CSERIAL_H
