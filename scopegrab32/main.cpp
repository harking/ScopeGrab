
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
// ***  Licence: GNU GPL                                                ***
// ***                                                                  ***
// ***  PROGRAM DESCRIPTION                                             ***
// ***  This program was inspipired by Steven J. Merrifields similarly  ***
// ***  named ScopeGrab "Fluke ScopeMeter Screen Capture" tool          ***
// ***  for unix/linux. You can find the source for his program at      ***
// ***    http://www.labyrinth.net.au/~steve/fluke/scopegrab.html       ***
// ***                                                                  ***
// ***  ScopeGrab32 runs under Windows 95/98/.../2003, as the "32" in   ***
// ***  the name indicates (win32...) .                                 ***
// ***                                                                  ***
// ***  The program requires an optical cable for the Fluke             ***
// ***  ScopeMeter. Fluke is selling a good quality cable kit, but      ***
// ***  it has its (high) price, too. It is possible to build your      ***
// ***  own cable. For schematics and infos, see e.g. my page at        ***
// ***    http://www.hut.fi/~jwagner/fluke/                             ***
// ***                                                                  ***
// ***  For now, the program supports monochrome screenshots from both  ***
// ***  190 and 190C series, and maybe 123 too. Color screenshots of    ***
// ***  the 190C are PNG format, which is not yet handled by this       ***
// ***  program (hey, now you have the source code, add it yourself!:)  ***
// ***                                                                  ***
// ***                                                                  ***
// ***  USING THE PROGRAM                                               ***
// ***  Plug in the serial port cable into the PC serial port and       ***
// ***  the Fluke optical port, switch on the Fluke, and start          ***
// ***  ScopeGrab32. It will automatically check COM1 for the           ***
// ***  presence of a ScopeMeter. You may also select a different       ***
// ***  serial port or baud rate (1200 baud is the default for Fluke).  ***
// ***                                                                  ***
// ***  When a ScopeMeter is detected, the name is displayed below      ***
// ***  the serial port combo boxes, for example "FLUKE 196".           ***
// ***                                                                  ***
// ***  To make a screenshot of the current screen visible on your      ***
// ***  Fluke, click the Get Screen button. The image builds up while   ***
// ***  it is being downloaded.                                         ***
// ***                                                                  ***
// ***  Once downloaded, "To Clipboard" button can be clicked to copy   ***
// ***  the picture to the clipboard. After this you can for example    ***
// ***  paste it into your project report document, etc.                ***
// ***                                                                  ***
// ***  "Save Image" allows saving the picture to a Windows Bitmap      ***
// ***  *.bmp file.                                                     ***
// ***                                                                  ***
// ***  I also added a "Save Postscript" button, though I don't really  ***
// ***  know why! ;-) This will save the original postscript data       ***
// ***  that the ScopeMeter transmitted. However, at least with my      ***
// ***  Fluke 196 and firmware 4.08, the postscript is faulty and the   ***
// ***  left side of the image is not visible. The image may be garbled ***
// ***  a bit, too. Maybe in Fluke 190C series this is already fixed(?) ***
// ***                                                                  ***
// ***                                                                  ***
// ***  COMPILING                                                       ***
// ***  The program compiles only for Windows systems, as it calls      ***
// ***  the Win32 API serial port functions. Porting to linux should    ***
// ***  work, if you rewrite the CSerial class.                         ***
// ***                                                                  ***
// ***  This source was developed and compiled using:                   ***
// ***                                                                  ***
// ***   a) Dev-C++ 5.0 beta 9                                          ***
// **       http://www.bloodshed.net/dev/devcpp.html                    ***
// ***                                                                  ***
// ***   b) wxWindows (wxWidgets) 2.4.2, DevPak for Dev-C++             ***
// ***        http://michel.weinachter.free.fr/                         ***
// ***        or www.google.com "wxwindows 2.4.2 devpak"                ***
// ***      (note: a newer wxWindows DevPak may already be included     ***
// ***       with the DevC++ setup program)                             ***
// ***                                                                  ***
// ***   The source may compile also with a newer wxWindows DevPak.     ***
// ***   If not, use the DevC++ Tools=>Packet Manager to remove the     ***
// ***   new DevPak and then install the 2.4.2 one.                     ***
// ***                                                                  ***
// ************************************************************************


#include "main.h"

// instantiate the wxWidgets application MyAp (similar to WinMain())
IMPLEMENT_APP(MyApp)

// ----------------------------------------------------
// -------------      GUI EVENT MAP       -------------
// ----------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame,wxFrame)

    EVT_MENU(ID_MENU_ABOUT,MyFrame::OnMenuAbout)
    EVT_MENU(ID_MENU_EXIT,MyFrame::OnMenuExit)
  
    EVT_COMBOBOX(ID_COMBO_COM,MyFrame::evtChangeComPort)
    EVT_COMBOBOX(ID_COMBO_BAUD,MyFrame::evtChangeComPort)
  
    EVT_BUTTON(ID_BTN_SEND, MyFrame::evtSendCommand)
    EVT_BUTTON(ID_BTN_RECONNECT, MyFrame::evtReconnect)
    EVT_TEXT_ENTER(ID_TXTCTL_CONSOLECMD, MyFrame::evtSendCommand)
    EVT_BUTTON(ID_BTN_GETSCREENSHOT, MyFrame::evtGetScreenshot)
    EVT_BUTTON(ID_BTN_CLIPBOARDIMAGE, MyFrame::evtClipboardImage)
    EVT_BUTTON(ID_BTN_SAVEIMAGE, MyFrame::evtSaveImage)
    EVT_BUTTON(ID_BTN_SAVEPOSTSCRIPT, MyFrame::evtSavePostscript)

    EVT_TIMER(ID_RTSTIMER, MyFrame::evtRtsTimer)
  
END_EVENT_TABLE()

// ----------------------------------------------------
// -------------        CONSTANTS         -------------
// ----------------------------------------------------

// list of baud rates that are supported by Fluke ScopeMeters
int fluke_baudrates[] = { 1200, 2400, 4800, 9600, 19200, 38400, 57600 };

// array for storing serial port ID's
int combo_portIDs[MAX_COMPORT_COUNT];


// ----------------------------------------------------
// -------------     HELPER FUNCTIONS     -------------
// ----------------------------------------------------

void MyFrame::DoEvents() {
	// allow win32 messages through
	MSG msg;
    while ( TRUE==::PeekMessage((LPMSG)&msg, NULL, 0, 0, PM_REMOVE) ) {
       ::TranslateMessage(&msg); ::DispatchMessage(&msg);
	}
}

// ----------------------------------------------------
// ---------   PROGRAM ENTRY POINT AND INIT   ---------
// ----------------------------------------------------

bool MyApp::OnInit()
{
    // create the main application window, resized in the constructor
	frameMain = new MyFrame(NULL, -1, wxT("ScopeGrab32 for the Fluke ScopeMeter series"));
	// and show it (the frames, unlike simple controls, are not shown when
	// created initially)
	frameMain->Center(wxBOTH);
	wxIcon progIcon = wxIcon("A", wxBITMAP_TYPE_ICO_RESOURCE);
	frameMain->SetIcon(progIcon);
	frameMain->Show(TRUE);
	// success: wxApp::OnRun() will be called which will enter the main message
	// loop and the application will run. If we returned FALSE here, the
	// application would exit immediately.
	return TRUE;
}


//
// Main application window/frame constructor
//
MyFrame::MyFrame(wxWindow* parent,wxWindowID id,const wxString& title,const wxPoint& pos,const wxSize& size,long style,const wxString& name)
VwX_INIT_OBJECTS_MyFrame
 {
   // init window
   Create(parent,id,title,pos,size,style,name);
   if((pos==wxDefaultPosition)&&(size==wxDefaultSize)){
      SetSize(1,1,WINDOW_WIDTH,WINDOW_HEIGHT);
   }
   if((pos!=wxDefaultPosition)&&(size==wxDefaultSize)){
      SetSize(WINDOW_WIDTH,WINDOW_HEIGHT);
   }
   this->SetThemeEnabled(false);

   // init window elements
   initBefore();
   VwXinit();
   
   return;
}

//
// Main window destructor
//
MyFrame::~MyFrame()
{
  return;
}


//
// Program internal vars init
//
void MyFrame::initBefore()
{
    CurrRxString = "";
    strPrevSavePath = "";
    strPostscript = "";
    ReceivedStrings.Empty();
    bRxAsciiMode = TRUE;
    bRxReceiverActive = FALSE;
    bFlukeDetected = FALSE;
    RxErrorCounter = 0;
    mScopemeterType = SCOPEMETER_NONE;
}





// ----------------------------------------------------
// -------------   MAIN WINDOW GUI INIT   -------------
// ----------------------------------------------------

void MyFrame::VwXinit()
{
    // create a serial RX/TX class and mutex
    mySerial = new CSerial(this);
    tmrToggleRTS = new wxTimer(this, ID_RTSTIMER);

    // -- add GUI components

    Show(false);
    SetBackgroundColour(wxSystemSettings::GetColour((wxSystemColour)wxSYS_COLOUR_3DLIGHT));

    // -- menu and status bar
    wxMenuItem *itemmenu;
    m_menuBar=new wxMenuBar();
    m_menuFile=new wxMenu();
        m_menuBar->Append(m_menuFile,wxT("File"));
    m_menuHelp=new wxMenu();
        m_menuBar->Append(m_menuHelp,wxT("Help"));
    itemmenu = new wxMenuItem(NULL,ID_MENU_EXIT,wxT("E&xit"),wxT(""),0);
        m_menuFile->Append(itemmenu);
    itemmenu = new wxMenuItem(NULL,ID_MENU_ABOUT,wxT("About"),wxT(""),0);
        m_menuHelp->Append(itemmenu);
    SetMenuBar(m_menuBar);
        statusBar=new wxStatusBar(this,-1,0,"statusBar");
        statusBar->SetFieldsCount(1,NULL);
        statusBar->SetStatusText("  ",0);
    SetStatusBar(statusBar);
  
    // -- fluke serial comm
    st_1=new wxStaticText(this,-1,wxT(""),wxPoint(11,11),wxSize(30,13),wxNO_BORDER|wxTRANSPARENT_WINDOW|wxALIGN_LEFT);
        st_1->SetLabel(wxT("Serial port setup:"));
    comboCOM=new wxComboBox(this,ID_COMBO_COM,wxT(""),wxPoint(115,6),wxSize(65,21),0,NULL);
    comboBaud=new wxComboBox(this,ID_COMBO_BAUD,wxT(""),wxPoint(190,6),wxSize(80,21),0,NULL);
    btnReconnect=new wxButton(this,ID_BTN_RECONNECT,wxT(""),
        wxPoint(280,6),wxSize(65,21));
        btnReconnect->SetLabel(wxT("Connect"));
        btnReconnect->SetTitle(wxT("Connect"));

    st_2=new wxStaticText(this,-1,wxT(""),wxPoint(11,35),wxSize(30,13),wxNO_BORDER|wxTRANSPARENT_WINDOW|wxALIGN_LEFT);
        st_2->SetLabel(wxT("Fluke ScopeMeter:"));
    stFlukeID=new wxStaticText(this,-1,wxT(""),wxPoint(115,35),wxSize(130,19),wxNO_BORDER|wxTRANSPARENT_WINDOW|wxALIGN_LEFT);
        stFlukeID->SetLabel(wxT("<not detected>"));

    nbNote=new wxNotebook(this,-1,wxPoint(6,60),
        wxSize((155+FLUKESCREEN_WIDTH),(140+FLUKESCREEN_HEIGHT)));
        nbNote->SetTitle(wxT("State"));

    // -- screen capture
    pn_1=new wxPanel(nbNote,-1,wxPoint(4,22),
        wxSize((140+FLUKESCREEN_WIDTH),(120+FLUKESCREEN_HEIGHT)),wxNO_BORDER);
        nbNote->AddPage(pn_1,"Screen capture",false);
        pn_1->SetTitle(wxT("Screen capture"));
    imgScreenshot=new wxImage(FLUKESCREEN_WIDTH,FLUKESCREEN_HEIGHT);
    sbmpScreenshot=new wxStaticBitmap(pn_1,ID_SCREENSHOT,
            wxBitmap(imgScreenshot,IMAGE_BITDEPTH),
            wxPoint(5,5),wxSize(FLUKESCREEN_WIDTH,FLUKESCREEN_HEIGHT),
            wxTHICK_FRAME,"sbmpScreenshot");
    btnGetScreenshot=new wxButton(pn_1,ID_BTN_GETSCREENSHOT,wxT(""),
        wxPoint(35+FLUKESCREEN_WIDTH,10),wxSize(90,25));
        btnGetScreenshot->SetLabel(wxT("Get Screen"));
        btnGetScreenshot->SetTitle(wxT("Get Screen"));
    btnCopyScreenshot=new wxButton(pn_1,ID_BTN_CLIPBOARDIMAGE,wxT(""),
        wxPoint(35+FLUKESCREEN_WIDTH,10+30),wxSize(90,25));
        btnCopyScreenshot->SetLabel(wxT("To Clipboard"));
        btnCopyScreenshot->SetTitle(wxT("To Clipboard"));
    btnSaveScreenshot=new wxButton(pn_1,ID_BTN_SAVEIMAGE,wxT(""),
        wxPoint(35+FLUKESCREEN_WIDTH,10+2*30),wxSize(90,25));
        btnSaveScreenshot->SetLabel(wxT("Save Image"));
        btnSaveScreenshot->SetTitle(wxT("Save Image"));
    btnSavePostscript=new wxButton(pn_1,ID_BTN_SAVEPOSTSCRIPT,wxT(""),
        wxPoint(35+FLUKESCREEN_WIDTH,10+3*30),wxSize(90,25));
        btnSavePostscript->SetLabel(wxT("Save Postscript"));
        btnSavePostscript->SetTitle(wxT("Save Postscript"));

    // -- serial trace/console
    pn_2=new wxPanel(nbNote,-1,wxPoint(4,22),
        wxSize((140+FLUKESCREEN_WIDTH),(120+FLUKESCREEN_HEIGHT)),wxNO_BORDER);
        nbNote->AddPage(pn_2,"Serial console",false);
        pn_2->SetTitle(wxT("Serial console"));
    txtSerialTrace=new wxTextCtrl(pn_2,-1,wxT(""),
        wxPoint(1,1),wxSize(FLUKESCREEN_WIDTH+145,FLUKESCREEN_HEIGHT-30),
        wxTE_RICH|wxTE_MULTILINE|wxTE_READONLY);
        txtSerialTrace->SetFont(wxFont(8,wxDEFAULT,wxNORMAL,wxNORMAL,
            FALSE,"Arial",wxFONTENCODING_SYSTEM));
    st_3=new wxStaticText(pn_2,-1,wxT(""),
        wxPoint(1,FLUKESCREEN_HEIGHT-10),wxSize(50,13),
        wxNO_BORDER|wxTRANSPARENT_WINDOW|wxALIGN_LEFT);
        st_3->SetLabel(wxT("Command:"));
    txtCommandToSend=new wxTextCtrl(pn_2,ID_TXTCTL_CONSOLECMD,wxT(""),
        wxPoint(55,FLUKESCREEN_HEIGHT-12),
        wxSize(320,18),wxTE_DONTWRAP|wxTE_PROCESS_ENTER);
    btnSendCommand=new wxButton(pn_2,ID_BTN_SEND,wxT(""),
        wxPoint(380,FLUKESCREEN_HEIGHT-12),wxSize(60,18));
        btnSendCommand->SetLabel(wxT("send"));
        btnSendCommand->SetTitle(wxT("Send"));


    // -- populate the serial ports and baud list

    #ifdef __WIN32__
    // (direct copy from MSDN example code)
    // serial ports first:
    DWORD dwNeeded = 0, dwReturned = 0;
    PPORT_INFO_2 pStart, pInfo = NULL;
    EnumPorts(NULL, 2, (LPBYTE) pInfo, 0, &dwNeeded, &dwReturned);
    pInfo = (PPORT_INFO_2) malloc(dwNeeded);
    EnumPorts(NULL, 2, (LPBYTE) pInfo, dwNeeded, &dwNeeded, &dwReturned);
    pStart = pInfo;
    wxString strRest;
    long portID;
    int portCnt = 0;
    for (DWORD j = 0; j < dwReturned && portCnt<MAX_COMPORT_COUNT; j++) {
        wxString wxtmp = wxString(pInfo->pPortName);
        if (wxtmp.StartsWith("COM", &strRest)) {
            strRest.ToLong(&portID, 10);
		    combo_portIDs[portCnt] = (int)portID;
            comboCOM->Append( wxtmp.Left(wxtmp.Length() - 1).c_str(), &combo_portIDs[portCnt]);
            portCnt++;
        }
        pInfo++;
    }
    if (pStart) {
        free(pStart);
    }
    #else
    // serial port listing for linux
    for (int i=0; i<8 && i<MAX_COMPORT_COUNT; ++i) {
        combo_portIDs[i] = i;
        comboCOM->Append(wxString::Format("/dev/ttyS%d",i).c_str(), &combo_portIDs[portCnt])
    }
    #endif
    comboCOM->SetSelection(0);

    // next, add ScopeMeter supported baud rates
    for(unsigned int i=0;i<(sizeof(fluke_baudrates)/sizeof(int));++i) {
        comboBaud->Append(
            wxString::Format("%d", fluke_baudrates[i]), &fluke_baudrates[i]
        );
    }
    comboBaud->SetSelection(0);
  
    // apply default port settings COM1 1200baud) now
    this->ChangeComPort();
  
    return;
}

//
// Safe GUI handling, enable or disable the input controls
//
void MyFrame::GUI_Up()
{
    comboBaud->Enable(TRUE); comboCOM->Enable(TRUE);
    m_menuBar->EnableTop(0,TRUE);
    btnGetScreenshot->Enable(TRUE); btnSaveScreenshot->Enable(TRUE);
    btnCopyScreenshot->Enable(TRUE); btnSavePostscript->Enable(TRUE);
    btnReconnect->Enable(TRUE); btnSendCommand->Enable(TRUE);
    return;
}

void MyFrame::GUI_Down()
{
    comboBaud->Enable(FALSE); comboCOM->Enable(FALSE);
    m_menuBar->EnableTop(0,FALSE);
    btnGetScreenshot->Enable(FALSE); btnSaveScreenshot->Enable(FALSE);
    btnCopyScreenshot->Enable(FALSE); btnSavePostscript->Enable(FALSE);
    btnReconnect->Enable(FALSE); btnSendCommand->Enable(FALSE);
    return;
}



//
// Button click: try to connect to the Fluke again, with the current
//               serial port selections (port nr and baud rate)
//
void MyFrame::evtReconnect(wxCommandEvent& event)
{
    this->ChangeComPort();
    return;
}

//
// Combo box selection: user changed the serial baud or port
//
void MyFrame::evtChangeComPort(wxCommandEvent& event)
{
    // call ChangeComPort() to apply the settings, but skip
    // duplicate events
    static BOOL firstCall = TRUE;
    if ( TRUE==firstCall ) { this->ChangeComPort(); }
    firstCall = !firstCall;
}

//
// Apply new port settings
//
void MyFrame::ChangeComPort()
{
    BOOL ret;
    BOOL respOk;
    
    bFlukeDetected = FALSE;
    mScopemeterType = SCOPEMETER_NONE;

    // get the current/new port number
    int idx = comboCOM->GetSelection();
    if ( NULL==comboCOM->GetClientData(idx) ) return;
    int port = * (int*)comboCOM->GetClientData(idx);
    
    // also get the baud rate
    idx = comboBaud->GetSelection();
    if ( NULL==comboBaud->GetClientData(idx) ) return;
    int baud = * (int*)comboBaud->GetClientData(idx);

    // (re)open the port
    // fluke comm is always 8 databit, 1 stopbit, no parity
    if(NULL==mySerial) { mySerial = new CSerial(this); }
    wxBusyCursor wait;
    tmrToggleRTS->Stop();
    mySerial->closePort();
    CurrRxString.Clear();
    ReceivedStrings.Clear();
    ret = mySerial->openPort(port, baud, 8, ONESTOPBIT, 'N', 0);
    if(FALSE==ret)
    {
        // show error in status bar
        wxString errStr = wxString::Format(
            "error opening COM%d @ %d baud - system error 0x%04lX",
            port, baud, mySerial->getLastError() );
        statusBar->SetStatusText(errStr,0);

    } else {

        wxString infoStr = wxString::Format("COM%d @ %d baud opened", port, baud);

        // init circuit supply voltage 'generator'
        tmrToggleRTS->Start(TIMER_TOGGLERATE, FALSE); // given rate, not single-shot
        DoEvents();
        SleepEx(400,FALSE);
        DoEvents();

        // try to get identification from the Fluke : <acknowledge><cr><infos><cr>
        wxString response = QueryFluke("ID",TRUE,1000,&respOk); // just the <ack>
        if(TRUE==respOk) {
           response = GetFlukeResponse(250); // <infos> string
        }
        if(response.Freq(';')>1) {
            bFlukeDetected = TRUE;
            // show ID in text label
            stFlukeID->SetLabel(response.BeforeFirst(';'));
            // ... and in the status bar
            infoStr = infoStr + " - ident=" + response;
            statusBar->SetStatusText(infoStr,0);
            // next, try to extract the scopemeter series info
            if(1==(response.Contains(" 192") || response.Contains(" 196") || response.Contains(" 199"))) {
                mScopemeterType = SCOPEMETER_190_SERIES;
            } else if(1==(response.Contains(" 91") || response.Contains(" 92") || response.Contains(" 96"))) {
                mScopemeterType = SCOPEMETER_90_SERIES;
            } else if(1==(response.Contains(" 97") || response.Contains(" 99"))) {
                mScopemeterType = SCOPEMETER_97_SERIES;
            } else if(1==(response.Contains(" 123") || response.Contains(" 124"))) {
                mScopemeterType = SCOPEMETER_120_SERIES;
            } else {
                // unsupported model
                mScopemeterType = SCOPEMETER_NONE;
                bFlukeDetected = FALSE;
                infoStr = "(unsupported) " + infoStr;
                statusBar->SetStatusText(infoStr,0);
            }
        } else {
            bFlukeDetected = FALSE;
            // update text label and status bar
            stFlukeID->SetLabel("<not detected>");
            infoStr = infoStr + " - no ScopeMeter";
            statusBar->SetStatusText(infoStr,0);
        }
    }

    return;
}

//
// Button click: send user query string to the ScopeMeter
//
void MyFrame::evtSendCommand(wxCommandEvent& event)
{
    if ( NULL==mySerial || FALSE==mySerial->isOpen() ) {
        txtSerialTrace->AppendText("Error: port not opened, can't send the command.\r\n");
        txtCommandToSend->SetValue("");
        return;
    }
    
    // send the command and dump all response data (can be multiple lines)
    BOOL respOk;
    GUI_Down();
    wxString sendStr = txtCommandToSend->GetValue();
    wxString response = QueryFluke(sendStr,TRUE,1000,&respOk); // <ack>
    while ( response.Length() > 0 ) {
        response = GetFlukeResponse(1000); // <possible command info data>
    }
    GUI_Up();
    return;
}

//
// Button click: Get a screenshot from the scopemeter
//
void MyFrame::evtGetScreenshot(wxCommandEvent& event)
{
    BOOL gotImage = FALSE;
    BOOL imageStart = FALSE;
    char currbyte = 0;
    int y_counter = 0, x_counter = 0;
    BYTE color = 0;
    wxBitmap* bmpTempBitmap = NULL;
    
    if ( FALSE==bFlukeDetected ) {
        statusBar->SetStatusText("screenshot: no ScopeMeter detected",0);
        return;
    }

    wxString command="", response = "", strOldBaud="";
    BOOL respOk=FALSE;

    GUI_Down();
    
    wxBusyCursor wait; // displays hourglass, reverts back to normal automatically
    
    // increase baud rate to 19200 (first the Fluke, then the PC)
    DWORD oldbaud = mySerial->getBaudrate();
    if ( oldbaud < 19200 ) {
        txtSerialTrace->AppendText("Temporarily setting Fluke baudrate to 19200...\r\n");
        statusBar->SetStatusText("screenshot: changing Fluke to 19200 baud",0);

        switch(mScopemeterType) {
            case SCOPEMETER_190_SERIES:
            case SCOPEMETER_120_SERIES:
                command = "PC 19200";
                strOldBaud = wxString::Format("PC %ld", oldbaud);
                break;
            case SCOPEMETER_90_SERIES:
                command = "PC19200,N,8,1,XONXOFF";
                strOldBaud = wxString::Format("PC%ld,N,8,1,XONXOFF", oldbaud);
                break;
            case SCOPEMETER_97_SERIES:
                command = "PC 19200,N,8,1,XONXOFF";
                strOldBaud = wxString::Format("PC %ld,N,8,1,XONXOFF", oldbaud);
                break;
        }
        response = QueryFluke(command,TRUE,1000,&respOk); // <ack><cr>

        if ( TRUE==respOk ) {
            txtSerialTrace->AppendText("Temporarily setting PC baudrate to 19200...\r\n");
            statusBar->SetStatusText("screenshot: changing PC to 19200 baud",0);
            mySerial->setBaudrate(19200);
        } else {
            txtSerialTrace->AppendText("Couldn't set Fluke baurdate to 19200.\r\n");
            statusBar->SetStatusText("screenshot: failed to set ScopeMeter to 19200 baud",0);
            GUI_Up();
            return;
        }
    }

    // get screenshot / Query Print screen 0 and postscript (3) format
    switch(mScopemeterType) {
        case SCOPEMETER_190_SERIES:
        case SCOPEMETER_120_SERIES:
            command = "QP 0,3";
            statusBar->SetStatusText("screenshot: requesting postscript image",0);
            break;
        case SCOPEMETER_90_SERIES:
            command = "QP"; // ???correct??
            statusBar->SetStatusText("screenshot: requesting screenshot",0);
            break;
        case SCOPEMETER_97_SERIES:
            command = "QG129";
            statusBar->SetStatusText("screenshot: requesting screen graphics",0);
            break;
    }
    response = QueryFluke(command,TRUE,1000,&respOk); // first just the <ack>

    // wait for the complete graphics data to arrive, in postscript format,
    // split over multiple lines. process the data as it comes in.
    this->strPostscript = "";
    while ( response.Length() > 0 ) {
        // get next line
        response = GetFlukeResponse(1000); // <printer or image data><cr>

        // store to postscript temp var
        this->strPostscript.Append(response);
        this->strPostscript.Append("\r\n");
        
        // scan up to the
        //   "... { currentfile 60 string readstring pop } image" line
        // in the response - the image data starts on the next line
        if ( FALSE==imageStart ) {
            if ( response.Find("image") >= 0 ) {
                imageStart = TRUE;
                if ( NULL == this->imgScreenshot ) {
                    imgScreenshot=new wxImage(FLUKESCREEN_WIDTH,FLUKESCREEN_HEIGHT);
                }
                this->imgScreenshot->Create(FLUKESCREEN_WIDTH,FLUKESCREEN_HEIGHT);
                this->imgScreenshot->SetMask(FALSE);
                statusBar->SetStatusText("screenshot: receiving image...",0);
            }
            continue;
        }

        // a line with "showpage" indicates the end of the image data
        if ( response.Find("showpage") >= 0 ) {
            gotImage = TRUE;
            break;
        }

        // process the current line
        response = response.MakeUpper(); // (upper case HEX)
        for ( unsigned int idx=0; idx<response.Length(); ++idx ) {

            // valid hex values?
            currbyte = response.GetChar(idx);
            if ( !(currbyte>='0' && currbyte<='9') &&
                 !(currbyte>='A' && currbyte<='F') )   continue;

//                currbyte = remap(currbyte); // ...necessary..??
                
            // hex to dec
            if(currbyte>='0' && currbyte<='9') { currbyte -= '0'; }
            else { currbyte = 10+(currbyte-'A'); }
                
            // one character (0..F) marks 4 pixels (bits), with MSb on the left
            int bitnr=0, bitval=0x08;
            for ( bitnr=3; bitnr>=0; --bitnr, bitval/=2 ) {
                color = 0xFF; // bit set: white
                if ( 0==(currbyte&bitval) ) color = 0x00; // bit cleared: black
                this->imgScreenshot->SetRGB(x_counter, y_counter, color, color, color);
                ++x_counter;
            }
        }

        // update image and advance to next line
        // parse the hex data in the current line
        if ( x_counter>=FLUKESCREEN_WIDTH ) {
            x_counter = 0;
            ++y_counter;
        }
        if ( NULL!=bmpTempBitmap ) { delete bmpTempBitmap; }
        bmpTempBitmap = new wxBitmap(imgScreenshot,IMAGE_BITDEPTH);
        sbmpScreenshot->SetBitmap(*bmpTempBitmap);
        sbmpScreenshot->Refresh();
    }

    // restore old baud
    if ( oldbaud < 19200 ) {
        txtSerialTrace->AppendText("Restoring old baudrate on Fluke...\r\n");
        statusBar->SetStatusText("screenshot: restoring baudrate",0);
        response = QueryFluke(strOldBaud,TRUE,1000,&respOk); // <ack><cr>
        if ( FALSE==respOk ) {
            txtSerialTrace->AppendText("Couldn't set Fluke baudrate.\r\n");
        } else {
            mySerial->setBaudrate(oldbaud);
        }
    }
    
    // show the final result of the operation
    if ( TRUE==gotImage ) {
        statusBar->SetStatusText("screenshot: complete image received",0);
        txtSerialTrace->AppendText("Screenshot complete.\r\n");
    } else {
        if ( FALSE==imageStart ) {
            txtSerialTrace->AppendText("Screenshot error: the response did not contain any image\r\n");
            statusBar->SetStatusText("screenshot: no image received",0);
        } else {
            txtSerialTrace->AppendText("Screenshot error: ran out of data before image end\r\n");
            statusBar->SetStatusText("screenshot: incomplete image received",0);
        }
    }

    GUI_Up();
    return;
}

//
// Button click: Save the current image to a BMP
//
void MyFrame::evtSaveImage(wxCommandEvent& event)
{
    wxString filePath;
    // screenshot available?
    if ( strPostscript.Length()<128 ) {
        statusBar->SetStatusText("save image: error, no screenshot available!",0);
        return;
    }
    // check if a previous saving path is remembered
    if ( strPrevSavePath.Length()<1 ) {
        strPrevSavePath = wxGetCwd(); // default to current directory
    }
    // open a Save File dialog for *.BMP
    wxFileDialog* saveDialog = new wxFileDialog (
        (wxWindow *)this, "Save screenshot to image file",
        strPrevSavePath, "", "Windows Bitmap (*.bmp)|*.bmp",
        wxSAVE|wxOVERWRITE_PROMPT
      );
    if ( wxID_CANCEL==saveDialog->ShowModal() ) { return; }
    filePath = saveDialog->GetPath();
    saveDialog->Destroy();
    // try to save the .BMP file
    if ( FALSE==this->imgScreenshot->SaveFile (filePath,wxBITMAP_TYPE_BMP ) ) {
        statusBar->SetStatusText("save BMP: error during saving!",0);
    } else {
        statusBar->SetStatusText("save BMP: done.",0);
    }
    // store the path name for the next Save File dialog
    strPrevSavePath = filePath.BeforeLast('\\');
    return;
}

//
// Button click: Copy the current image to the clipboard
//
void MyFrame::evtClipboardImage(wxCommandEvent& event)
{
    // screenshot available?
    if ( strPostscript.Length()<128 ) {
        statusBar->SetStatusText("copy to clipbrd: error, no screenshot available!",0);
        return;
    }
    // create a cut&paste'able copy of the currently visible bitmap
    wxBitmapDataObject* bmpDataObj =
         new wxBitmapDataObject ( sbmpScreenshot->GetBitmap() );
    // try to place it on the clipboard
    if( TRUE==wxTheClipboard->Open() ) {
        wxTheClipboard->SetData ( bmpDataObj );
        wxTheClipboard->Flush(); // keeps image on clipbrd even after this app exits
        wxTheClipboard->Close();
        statusBar->SetStatusText("copy to clipbrd: done.",0);
    } else {
        statusBar->SetStatusText("copy to clipbrd: error, couldn't open clipboard!",0);
    }
    return;
}

//
// Button click: Save to postscript file
//
void MyFrame::evtSavePostscript(wxCommandEvent& event)
{
    wxString filePath;
    // screenshot available?
    if ( strPostscript.Length()<128 ) {
        statusBar->SetStatusText("save postscript: error, no screenshot available!",0);
        return;
    }
    // check if a previous saving path is remembered
    if ( strPrevSavePath.Length()<1 ) {
        strPrevSavePath = wxGetCwd(); // default to current directory
    }
    // open a Save File dialog for *.BMP
    wxFileDialog* saveDialog = new wxFileDialog (
        (wxWindow *)this, "Save screenshot to Postscript file",
        strPrevSavePath, "", "Postscript (*.ps)|*.ps",
        wxSAVE|wxOVERWRITE_PROMPT
      );
    if ( wxID_CANCEL==saveDialog->ShowModal() ) { return; }
    filePath = saveDialog->GetPath();
    saveDialog->Destroy();
    // try to save the .PS file
    wxFile outputFile(filePath,wxFile::write);
    if ( TRUE==outputFile.IsOpened() ) {
        outputFile.Seek(0, wxFromStart);
        if ( TRUE==outputFile.Write(this->strPostscript,wxConvLibc) ) {
            statusBar->SetStatusText("save postscript: done",0);
        } else {
            statusBar->SetStatusText("save postscript: error, file write failed",0);
        }
        outputFile.Flush();
        outputFile.Close();
    } else {
        statusBar->SetStatusText("save postscript: error, couldn't create file!",0);
    }
    // store the path name for the next Save File dialog
    strPrevSavePath = filePath.BeforeLast('\\');
    return;
}


//
// Menu: display the About dialog
//
void MyFrame::OnMenuAbout(wxMenuEvent& event)
{
    wxMessageBox("ScopeGrab32 v1.0 alpha\r\n\rAn open source tool for communicating\r\n"
        "with a Fluke Scopemeter and also do a\r\nscreen capture.\r\nNamed after "
        "Steven Merrifield's\r\nScopeGrab for Unix.\r\n\r\n(C) 2004 Jan Wagner",
        "About ScopeGrab32",
        wxOK | wxICON_INFORMATION, this);
    return;
}

//
// Menu: exit the program
//
void MyFrame::OnMenuExit(wxMenuEvent& event)
{
    mySerial->closePort();
    Close(); // exit the app
}



// ----------------------------------------------------
// -------------    SERIAL PORT FUNCS     -------------
// ----------------------------------------------------

//
// Timer event: pulse the RTS pin to generate
// Scopemeter DIY cable / circuit supply voltages
//
void MyFrame::evtRtsTimer(wxTimerEvent& event)
{
    // safety checks
    if ( NULL==mySerial ) return;
    if ( FALSE==mySerial->isOpen() ) {
        tmrToggleRTS->Stop();
        return;
    }
    mySerial->setRTS(!mySerial->getRTS());
}

//
// Callback func for CSerial class, receive new data bytes
//
void MyFrame::OnCommEvent(const BYTE* rxbuf, const DWORD rxbytes)
{
   // checks
   if ( NULL==rxbuf || 0==rxbytes ) return;
   
   // set flag to indicate that the rx line is still alive
   bRxReceiverActive = TRUE;
   
   // wxCriticalSectionLocker buflock(csRxBufLock);
   BYTE currByte;
   for ( DWORD i=0; i<rxbytes; ++i ) {

        currByte = *(rxbuf+i);

        // handle characters in ASCII mode
        if ( TRUE==this->bRxAsciiMode ) {

            // a <cr> carriage return indicates the end of a block
            if ( 13==currByte ) { // '\r'
                // insert at the start of the responses list, remove at end
                if(CurrRxString.Length()>0) {
                    ReceivedStrings.Insert(CurrRxString, 0, 1);
                }
                CurrRxString.Clear();
                continue;
            }
            if ( 10==currByte ) { // '\n'
                continue;
            }
        
            // check for valid characters
            if ( currByte<32 || currByte>127 ) {
                // illegal char, inc error counter, skip char
                if(RxErrorCounter<0xFFFF) { ++RxErrorCounter; }
                continue;
            }
        }
        
        // append character to current string
        CurrRxString.Append(currByte, 1);
        CurrRxString.Shrink();
   }
   
   return;
}

//
// Wait to receive a complete response from the ScopeMeter
//
wxString MyFrame::GetFlukeResponse(DWORD msTimeout)
{
    wxString response="";
    DWORD cnt;
    
    if ( msTimeout<50 || msTimeout>3000 ) { msTimeout=1000; }
    if ( NULL==mySerial || FALSE==mySerial->isOpen() ) { return wxString(""); }

    // return a response that was received earlier?
    size_t itemCount = ReceivedStrings.GetCount();
    if ( itemCount>0 ) {
        response = ReceivedStrings.Item(itemCount-1); // get last
        ReceivedStrings.Remove(itemCount-1, 1); // remove from end
        goto cleanUpResponse;
   }
   
    // wait for response
    itemCount = 0;
    cnt = 1+(msTimeout/50);
    this->bRxReceiverActive = FALSE; // flag used to detect rx activity
    while ( cnt>0 ) {
        // wait
        SleepEx(50,FALSE);
        DoEvents();
        // check receiver activity - did OnCommEvent() set the flag?
        if ( TRUE==this->bRxReceiverActive ) {
            this->bRxReceiverActive = FALSE;
            cnt = 1+(msTimeout/50); // reset back to max timeout
        } else {
            --cnt; // no activity, dec timeout
        }
        // check current count of available responses
        itemCount = ReceivedStrings.GetCount();
        if ( itemCount>0 ) break;
    }

    // binary mode responses
    if ( (0==itemCount) && (FALSE==this->bRxAsciiMode) ) {
        response = CurrRxString; // return all currently received bytes
        CurrRxString = "";
        goto cleanUpResponse;
    }
    
    // timeout in ascii mode
    if ( 0==itemCount ) return wxString("");

    // got at least 1 response string, remove it from array end
    response = ReceivedStrings.Item(itemCount-1); // get last
    ReceivedStrings.Remove(itemCount-1, 1); // remove from end

    // log and return a cleaned-up response
cleanUpResponse:
    response.Replace("\r", "", TRUE);
    response.Replace("\n", "", TRUE);
    response.Shrink();
    if ( response.Length()>0 ) {
        txtSerialTrace->AppendText("Response: " + response + "\r\n");
    }
    
    return response;
}

//
// Send a query to the ScopeMeter and return the ACK response.
// The remaining response data, if it exists, can be retrieved
// with GetFlukeResponse
//
wxString MyFrame::QueryFluke(wxString cmdString, BOOL bRxAsciiMode,
    DWORD msTimeout, BOOL* ResponseIsOK)
{
    wxString response;
    
    // clean up the command string
    cmdString = (cmdString.Trim(TRUE)).Trim(FALSE);
    cmdString.Replace("\r", " ", TRUE);
    cmdString.Replace("\t", " ", TRUE);
    cmdString.Replace("\n", " ", TRUE);
    while ( cmdString.Replace("  ", " ", TRUE) > 0);
    cmdString = cmdString.MakeUpper();
    // add cmd terminator char (carriage return)
    cmdString.Append("\r");
    
    // set the response mode (ASCII or binary)
    this->bRxAsciiMode = bRxAsciiMode;
    
    // clear current rx buffer
    RxErrorCounter = 0;
    CurrRxString = "";
    bRxReceiverActive = FALSE;
    
    // send the command
    txtSerialTrace->AppendText("Query: " + cmdString + "\n");
    wxCharBuffer dataToSend = cmdString.mb_str();
    if ( FALSE == mySerial->transmitData(
            (const BYTE*)((const char*)dataToSend), cmdString.Length())
        )
    {
       txtSerialTrace->AppendText(
         wxString::Format("Query Error: transmitData failed, win32 error 0x%lX\r\n",
            mySerial->getLastError()) );
    }
    
    // get the <ack> response
    response = GetFlukeResponse(msTimeout);

    // handle the response
    if ( RxErrorCounter>0 ) {
       txtSerialTrace->AppendText(wxString::Format("Response ACK: encountered %ld errors\r\n", RxErrorCounter));
    }
    
    if ( 0 == response.Length() ) {

        txtSerialTrace->AppendText("Response ACK timeout\r\n");
        if ( NULL!=ResponseIsOK ) *ResponseIsOK = FALSE;

    } else if ( 1 == response.Length() ) {
        char ackCode = response.GetChar(0);
        if ( NULL!=ResponseIsOK ) *ResponseIsOK = FALSE;
        switch(ackCode) {
         case '0': if ( NULL!=ResponseIsOK ) { *ResponseIsOK = TRUE; }
                  break; // ACK0, No error
         case '1': txtSerialTrace->AppendText("Response ACK1: Syntax Error\r\n"); break;
         case '2': txtSerialTrace->AppendText("Response ACK2: Execution Error\r\n"); break;
         case '3': txtSerialTrace->AppendText("Response ACK3: Synchronization Error\r\n"); break;
         case '4': txtSerialTrace->AppendText("Response ACK4: Communication Error\r\n"); break;
         default : txtSerialTrace->AppendText(
                wxString::Format("Response ACK error: ACK%c is an unknown ACK code\r\n", ackCode) );
        }

    } else { // response ack length>1

        txtSerialTrace->AppendText("Response error: expected an ACK code. Instead got <"+ response + ">\r\n");
        if ( NULL!=ResponseIsOK ) *ResponseIsOK = FALSE;

    }
    
    return response;
}

