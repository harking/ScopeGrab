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

// ************************************************************************
// ***                                                                  ***
// ***  ScopeGrab32 - A tool for the Fluke ScopeMeter series            ***
// ***  (C) 2004 Jan Wagner                                             ***
// ***                                                                  ***
// ***  Version: 2.2.0L alpha                                           ***
// ***                                                                  ***
// ***  Licence: GNU GPL                                                ***
// ***                                                                  ***
// ***  CSerialLinux.cpp :                                              ***
// ***   serial port wrapper and handler class that provides the        ***
// ***   necessary serial port access functions to the main             ***
// ***   scopegrab32 program. Intended for POSIX systems.               ***
// ***                                                                  ***
// ***  A lot of the serial programming info can be found in:           ***
// ***   Serial Programming Guide for POSIX Operating Systems           ***
// ***   http://www.easysw.com/~mike/serial/serial.html                 ***
// ***                                                                  ***
// ***  The threading ported from Win32 requires kernel 2.6 or newer    ***
// ***  with NPTL Native POSIX Thread Library for Linux support.        ***
// ***   http://www-128.ibm.com/developerworks/eserver/library/         ***
// ***          es-MigratingWin32toLinux.html                           ***
// ***                                                                  ***
// ************************************************************************


#include "CSerial.h"
#include "main.h"

// ****************************************************************
// *  C'stor
// *
// *  Arguments: ptr to client frame that has a public OnCommEvent
// *             member function for callback

CSerial::CSerial(MyFrame* clientFrmMain) {

   // clear error temp
   m_PreviousError = 0;

   // reset port variables
   m_DTRon = false;
   m_RTSon = false;
   m_portHdl = INVALID_HANDLE_VALUE;
   m_hdl_RxThread = NULL;
   m_RxThread_Running = false;
   m_RxThread_Shutdown = false;

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
// *  bool openPort(int portnum, int baudrate, ...)
// *
// *  Arguments: portnum - COM port to open (0..64)
// *             baudrate - baud rate (1200..115200)
// *             databits - num of databits (1..8) !ignored
// *             stopbits - num of stopbits (0..8) !ignored
// *             parity   - 'N'=none, 'O'=odd, 'E'=even,
// *                        'M'=mark, 'S'=space    !ignored
// *                        default is 'N'
// *             handshaking - 0=none, 1=xon/xoff, 2=rts/cts, 3=xon/rts
// *                           !ignored
// *             portStr  - string with device path (e.g. "/dev/ttyS0")
// *
// *  Note: this function implements only limited functionality,
// *        i.e. the minimum required for ScopeGrab32 which is
// *        'no parity' 8N1, no handshake, and settable baud
// *
// *  Returns: true when the port has been opened

bool CSerial::openPort(
   BYTE portnum, DWORD baudrate,
   BYTE databits, BYTE stopbits,
   BYTE parity, BYTE handshaking, const char* portStr ) {

   int i = 0;
   
   m_PreviousError = 0;
      
   // check for parameters for valid range
   if ( NULL==portStr && (portnum>64) ) return false;
   if ( baudrate<1200 || baudrate>115200 ) return false;
   //if ( databits<5 || databits>8 ) return false;
   //if ( ONESTOPBIT!=stopbits && ONE5STOPBITS!=stopbits
   //      && TWOSTOPBITS!=stopbits ) return false;

   // if the port is already open, close it first,
   // then re-open with the new settings
   if ( true == this->isOpen() ) { this->closePort(); }

   // set main class for the rx/tx thread -> OnComm callback
   this->frmMain = frmMain;

   // try to open the specified system device (usually /dev/ttySxx)
   if ( NULL==portStr ) {
      std::ostringstream ostr;
      ostr << "/dev/ttyS" << (int)portnum << std::flush;
      portStr = (char *)ostr.str().c_str();
   }
   this->m_portHdl = open(portStr, O_RDWR | O_NOCTTY);
   if ( INVALID_HANDLE_VALUE == m_portHdl ) {
      m_PreviousError = errno;
      perror(portStr);
      return false;
   } 
   
   fcntl(m_portHdl, F_SETFL, 0); // use a blocking read()

   // port was opened, get properties block
   if ( -1==tcgetattr(m_portHdl, &m_serialopt) ) {
      m_PreviousError = errno;
      perror("openPort() tcgetattr");
      return false;
   }

   // adjust databits, stopbits, parity suitable
   // for scopegrab ('8N1')
   m_serialopt.c_cflag &= ~PARENB;
   m_serialopt.c_cflag &= ~CSTOPB;
   m_serialopt.c_cflag &= ~CSIZE;
   m_serialopt.c_cflag |= CS8;
   
   // disable hardware flow control / handshaking
   #ifdef CNEW_RTSCTS
   m_serialopt.c_cflag &= ~CNEW_RTSCTS;
   #else
      #ifdef CRTSCTS
      m_serialopt.c_cflag &= ~CRTSCTS;
      #endif
   #endif
   
   // configure for raw binary data (i.e. disable ascii/echo mode)
   m_serialopt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

   // configure the input options
   m_serialopt.c_iflag &= ~IGNCR; // don't ignore CR
   m_serialopt.c_iflag &= ~(INLCR | ICRNL | IUCLC | IMAXBEL); // no remapping
   m_serialopt.c_iflag &= ~(IXON | IXOFF | IXANY); // no software flow control
   m_serialopt.c_iflag &= ~(INPCK | ISTRIP); // no parity check nor stripping
   m_serialopt.c_iflag |= IGNBRK; // ignore break conditions

   // configure output options
   m_serialopt.c_oflag &= ~OPOST; // no postprocess, just raw outputting
   
   // set read timing
   m_serialopt.c_cc[VMIN] = 1;   // wait for 1 byte at a time
   m_serialopt.c_cc[VTIME] = 0;  // blocking wait (no timeout)
   
   // set the baudrate (convert to Bxxxx constant first)
   baudrate = baudToValue(baudrate);
   cfsetispeed(&m_serialopt, baudrate);
   cfsetospeed(&m_serialopt, baudrate);

   // enable receiver
   m_serialopt.c_cflag |= (CLOCAL | CREAD);

   // finally, apply these settings
   m_serialopt.c_cflag |= (CLOCAL | CREAD);
   tcflush(m_portHdl,TCIFLUSH);
   if ( -1==tcsetattr(m_portHdl, TCSANOW, &m_serialopt) ) {
      m_PreviousError = errno;
      perror("openPort() tcsetattr");
      close(m_portHdl);
      return false;
   }   
   
   // attempt to start the receiver thread
   this->m_RxThread_Running = true;
   this->m_RxThread_Shutdown = false;

   pthread_attr_t attr;
   if ( pthread_attr_init(&attr) ) {
      m_PreviousError = errno;
      perror("openPort() phtread_attr_init");
      close(m_portHdl);
      return false;
   }

   m_hdl_RxThread = &m_RxThread;
   i = pthread_create(m_hdl_RxThread, &attr,
         (void*(*)(void*))&ReceiverThread,
         this // pass current CSerial object as param (for callback)
      );

   // if starting the thread failed, close the port
   if ( i ) {
      m_PreviousError = errno;
      perror("openPort() pthread_create");
      close(m_portHdl);
      return false;
   }

   // seems like everything went fine
   return true;
}

// ****************************************************************
// *  bool isOpen()
// *
// *  Returns: true if the port has been opened

bool CSerial::isOpen() {
   if ( INVALID_HANDLE_VALUE==m_portHdl || INVALID_HANDLE_VALUE==m_portHdl ) {
      return false;
   }
   if ( false==m_RxThread_Running ) {
      return false;
   }
   return true;
}

// *******************************************  *********************
// *  bool closePort()
// *
// *  Calling this will close the current serial port,
// *  if it was open.
// *
// *  Returns: true if closing was successful, FALSE if an error occured

bool CSerial::closePort() {

   m_PreviousError = 0;

   if ( NULL==m_hdl_RxThread ) {
         this->m_RxThread_Running = false;
         this->m_RxThread_Shutdown = false;
   }

   // time out all pending operations
   // ...
   
   // stop the receiver thread
   if ( true==this->m_RxThread_Running ) {

      // signal a shutdown event to the thread and exit all
      // WaitForEvent's in the thread by setting timeouts
      this->m_RxThread_Shutdown = true; // (is set back to false by the thread)
      tcgetattr(m_portHdl, &m_serialopt);
      m_serialopt.c_cc[VMIN] = 1;   // wait for 1 byte
      m_serialopt.c_cc[VTIME] = 1;  // wait 1x0,1s
      tcsetattr(m_portHdl, TCSANOW, &m_serialopt);      
      
      // give the thread a while to terminate by itself (1s=20*50ms)
      for ( int k=0; (true==this->m_RxThread_Shutdown) && k<20; ++k ) {
         usleep(50000);
         frmMain->DoEvents();
      }

      // cancel the receiver thread
      pthread_cancel(*m_hdl_RxThread);
      m_hdl_RxThread = NULL;

      m_RxThread_Running = false;
      m_RxThread_Shutdown = false;
   }

   // try to close the serial port file handle and both rx/tx event handles
   if ( INVALID_HANDLE_VALUE!=m_portHdl && INVALID_HANDLE_VALUE!=m_portHdl ) {
      if(-1==close(m_portHdl)) {
         m_PreviousError = errno;
         return false;
      }
   }
   
   m_portHdl = INVALID_HANDLE_VALUE;
   return true;
}



// ****************************************************************
// *  void ReceiverThread(void* hostClass)
// *
// *  Separate thread for receiving serial port data. Don't
// *  call this directly! Does not exit until signaled
// *  to do so with CSerial::m_RxThread_Shutdown = true.
// *
// *  Arguments: host class
// *
// *  Returns: 0 when signaled to exit

void CSerial::ReceiverThread(void* hostClass) {

   // local buffer for the received bytes
   BYTE  rxbuf[MAX_IN_BUFFER+1];
   int   bytesread = 0;

   // get host class
   if ( NULL==hostClass ) { pthread_exit(0); }
   CSerial* host = (CSerial*)hostClass;   
   
   // thread started, set the info flags
   host->m_RxThread_Running = true;
   host->m_RxThread_Shutdown = false;
   
   // allow asynchronous (immediate) cancelling of this thread
   pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
      
   // receive chars, until thread is told to terminate
   while ( false==host->m_RxThread_Shutdown ) {
   
      // get more bytes (blocking call)
      bytesread = read(host->m_portHdl, rxbuf, MAX_IN_BUFFER);
   
      if (bytesread>0) {
         // forward data to the client application
         if (host->frmMain != NULL) {
            host->frmMain->OnCommEvent((const BYTE*)rxbuf, (const DWORD)bytesread);
         }
      }
      
   }
   
   // Exited the receiver loop because m_RxThread_Shutdown was set to true
   // Set it back to false to indicate that the thread has closed now.
   host->m_RxThread_Shutdown = false;
   host->m_RxThread_Running = false;

   pthread_exit(0);
}



// ****************************************************************
// * bool transmitData(const BYTE* databuf, const DWORD bytecount)
// *
// *  Send the data of the passed data buffer over the serial port.
// *
// *  Arguments: *databuf - BYTE pointer to the start of the data buffer
// *             bytecount - number of bytes to transmit from the buffer
// *
// *  Returns: true if the databits nr was successfully changed

bool CSerial::transmitData(const BYTE* databuf, const DWORD bytecount) {

   unsigned int written = 0;

   m_PreviousError = 0;
  
   // can transmit only on opened port
   if ( false==isOpen() ) return false;

   // basic checks
   if ( NULL==databuf || bytecount<1 ) return false;

   // write the data
   written = write(m_portHdl, databuf, bytecount);
   if ( written<bytecount ) {
      if ( written<0 ) {
         m_PreviousError = errno;
         perror("transmitData()");
      }
      return false;
   }
   
   return true;
}




// ****************************************************************
// *  bool getDTR()
// *
// *  Returns: true if the DTR pin is currently set High

bool CSerial::getDTR() { return m_DTRon; }

// ****************************************************************
// *  bool setDTR(bool switchOn)
// *
// *  Tries to change the state of the DTR pin,
// *  according to switchOn. true will set the DTR
// *  pin high.
// *
// *  Arguments:  switchOn  -  true to set DTR high
// *                           false to set DTR low
// *  Returns: true is setting the DTR state was successful

bool CSerial::setDTR(bool switchOn) {

   int result;
   int status;
   
   if ( INVALID_HANDLE_VALUE==m_portHdl ) return false;

   if ( -1==ioctl(m_portHdl, TIOCMGET, &status) ) return false;
   
   if ( true==switchOn ) {
      status |= TIOCM_DTR;
   } else {
      status &= ~TIOCM_DTR;
   }
   result = ioctl(m_portHdl, TIOCMSET, &status);

   if ( -1==result ) {
      m_PreviousError = errno;
      return false;
   }

   m_DTRon = switchOn;
   return true;
}

// ****************************************************************
// *  bool getRTS()
// *
// *  Returns: true if the RTS pin is currently set High

bool CSerial::getRTS() { return m_RTSon; }

// ****************************************************************
// *  bool setRTS(bool switchOn)
// *
// *  Tries to change the state of the RTS pin,
// *  according to switchOn. true will set the RTS
// *  pin high.
// *
// *  Arguments:  switchOn  -  true to set RTS high
// *                           false to set RTSlow
// *  Returns: true is setting the RTS state was successful

bool CSerial::setRTS(bool switchOn) {

   int result;
   int status;
   
   if ( INVALID_HANDLE_VALUE==m_portHdl ) return false;

   if ( -1==ioctl(m_portHdl, TIOCMGET, &status) ) return false;
   
   if ( true==switchOn ) {
      status |= TIOCM_RTS;
   } else {
      status &= ~TIOCM_RTS;
   }
   result = ioctl(m_portHdl, TIOCMSET, &status);

   if ( -1==result ) {
      m_PreviousError = errno;
      return true;
   }
   
   m_RTSon = switchOn;
   return true;
}

// ****************************************************************
// *  bool setBaudrate(DWORD baudrate)
// *
// *  Change the baud rate of the currently open port.
// *
// *  Arguments: baudrate - new baudrate to set
// *
// *  Returns: true if the baud rate was successfully changed

bool CSerial::setBaudrate(DWORD baudrate) {
   
   // can change only on opened port
   if ( INVALID_HANDLE_VALUE==m_portHdl ) return false;

   // baud rate is valid/supported?
   if ( baudrate<1200 || baudrate>115200 ) return false;

   // get the current config
   if ( -1==tcgetattr(m_portHdl, &m_serialopt) ) {
      m_PreviousError = errno;
      return false;
   }  
   
   // write the new baud rate
   baudrate = baudToValue((unsigned int)baudrate);
   cfsetispeed(&m_serialopt, baudrate);
   cfsetospeed(&m_serialopt, baudrate);
   
   // apply, with data flushing
   if ( -1==tcsetattr(m_portHdl, TCSAFLUSH, &m_serialopt) ) {
      m_PreviousError = errno;
      return false;      
   }

   return true;
}

// ****************************************************************
// *  DWORD getBaudrate()
// *
// *  Returns the current baud rate of the open port.
// *
// *  Returns: 0 on error, or the actual baud rate

DWORD CSerial::getBaudrate() {
   
   // can read only on opened port
   if ( false==isOpen() ) return 0;

   // get the current config
   if ( -1==tcgetattr(m_portHdl, &m_serialopt) ) {
      m_PreviousError = errno;
      return false;
   }  

   // return the baudrate
   return cfgetospeed(&m_serialopt);
}

// ****************************************************************
// *  bool setDatabits(BYTE databits)
// *
// *  Change the databits setting of the currently open port.
// *
// *  Arguments: databits - new nr of data bits
// *
// *  !! ignored func
// *
// *  Returns: true if the databits nr was successfully changed

bool CSerial::setDatabits(BYTE databits) {
   /*
   // can change only on opened port
   if ( false==isOpen() ) return false;

   // databit nr is valid/supported?
   if ( databits<5 || databits>8 ) return false;

   // read the Device Control Block with the current settings
   DCB commDCB;
   commDCB.DCBlength = sizeof(DCB);
   if ( !GetCommState(m_portHdl, &commDCB) ) {
      m_PreviousError = errno;
      return false;
   }

   // update to new databit nr
   commDCB.ByteSize = databits;
   if ( !SetCommState(m_portHdl, &commDCB) ) {
      m_PreviousError = errno;
      return false;
   }
   */
   return true;
}

// ****************************************************************
// *  bool setStopbits(BYTE stopbits)
// *
// *  Change the stopbits setting of the currently open port.
// *
// *  Arguments: stopbits - new nr of stop bits
// *
// *  !! ignored func
// *
// *  Returns: true if the stopbits nr was successfully changed

bool CSerial::setStopbits(BYTE stopbits) {
   /*
   // can change only on opened port
   if ( false==isOpen() ) return false;

   // stopbits nr is valid/supported?
   if ( ONESTOPBIT!=stopbits && ONE5STOPBITS!=stopbits
         && TWOSTOPBITS!=stopbits ) return false;

   // read the Device Control Block with the current settings
   DCB commDCB;
   commDCB.DCBlength = sizeof(DCB);
   if ( !GetCommState(m_portHdl, &commDCB) ) {
      m_PreviousError = errno;
      return false;
   }

   // update to new databit nr
   commDCB.StopBits = stopbits;
   if ( !SetCommState(m_portHdl, &commDCB) ) {
      m_PreviousError = errno;
      return false;
   }
   */
   return true;
}

// ****************************************************************
// *  DWORD getModembits()
// *
// *  Read the pin states aka "modem bits" of the currently open port.
// *
// *  Returns: status value received from ioctl() call

DWORD CSerial::getModembits() {

   int status;
   
   // can read only on opened port
   if ( false==isOpen() ) return 0;

   if ( -1==ioctl(m_portHdl, TIOCMGET, &status) ) return 0;

   return (DWORD)status;
}






// ****************************************************************
// *  void clearCommErrors()
// *
// *  Clears the serial port error flags if they were set.
// *  !! ignored

void CSerial::clearCommErrors() {
   
   // can check only if port is open
   if ( false==isOpen() ) return;
   /*
   // read the serial port event mask
   DWORD commEvt = 0;
   if ( !GetCommMask(m_portHdl, &commEvt) ) return;

   // if an error event is indicated, clear the errors
   DWORD commErr = 0;
   if ( commEvt & EV_ERR ) {
      ClearCommError(m_portHdl, &commErr, NULL);
   }
   */
   return;
}



// ****************************************************************
// *  int baudToValue(baud)
// *
// *  Returns: posix baud rate constant that corresponds to
// *           the specified integer baud rate
// *

int CSerial::baudToValue(unsigned int baud) {
   switch(baud) {
      case 300:
         return B300;
      case 600:
         return B600;
      case 1200:
         return B1200;
      case 2400:
         return B2400;
      case 4800:
         return B4800;
      case 9600:
         return B9600;
      case 19200:
         return B19200;
      case 38400:
         return B38400;
      case 57600:
         return B57600;
      default:
         return 0;
   }
}


// ****************************************************************
// *  unsigned int valueToBaud(val)
// *
// *  Returns: converts posix baud rate constant into corresponding
// *           integer baud rate
// *

unsigned int CSerial::valueToBaud(int value) {
   switch(value) {
      case B300:
         return 300;
      case B600:
         return 600;
      case B1200:
         return 1200;
      case B2400:
         return 2400;
      case B4800:
         return 4800;
      case B9600:
         return 9600;
      case B19200:
         return 19200;
      case B38400:
         return 38400;
      case B57600:
         return 57600;
      default:
         return 0;
   }
}
