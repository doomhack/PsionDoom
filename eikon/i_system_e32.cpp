// PsionDoomDoc.cpp
//
// Copyright 17/02/2019 
//


#include <unistd.h>
#include <stdarg.h>

#include <estlib.h>
#include <w32std.h>

#include "i_system_e32.h"

extern "C"
{
	#include "lprintf.h"
}

//**************************************************************************************

CWsBitmap* pFbs = NULL;
byte* pScreenBuff = NULL;

bool use_ws_bitmap = false;

unsigned int vid_width = 0;
unsigned int vid_height = 0;

unsigned int screen_width = 0;
unsigned int screen_height = 0;

unsigned int scr_offset = 0;

unsigned int y_pitch = 0;

RWsSession* wsSession = NULL;
CWsScreenDevice* wsScreen = NULL;
CWindowGc* wsContext = NULL;

RWindowGroup* wsWinGroup = NULL;

RWindow* wsWindow = NULL;

int wgToken;

TRequestStatus iWsEventStatus = 0;
TRequestStatus iRedrawEventStatus = 0;

bool clear_screen = true;

bool has_focus = true;

//**************************************************************************************

void I_InitScreen_e32()
{
	wsSession = new RWsSession();

	User::LeaveIfError(wsSession->Connect());
	
	wsScreen=new(ELeave) CWsScreenDevice(*wsSession);

	User::LeaveIfError(wsScreen->Construct());

	screen_height = wsScreen->SizeInPixels().iHeight;
	screen_width = wsScreen->SizeInPixels().iWidth;

	//Gives 480px on a 5(mx) and 320px on a Revo.
	vid_width = ((screen_height >> 3) << 4); //Height * 2 and vid_width % 16 == 0

	vid_height = screen_height;

	scr_offset = (screen_width - vid_width) >> 1;
}

//**************************************************************************************

void I_BlitScreenBmp_e32()
{
	if(!pFbs)
		return;

	wsContext->Activate(*wsWindow);
	
	TRect rect = TRect(wsWindow->Size());
	wsWindow->Invalidate(rect);        
	wsWindow->BeginRedraw(rect);

	wsContext->BitBlt(TPoint(scr_offset,0), pFbs);
		
	wsWindow->EndRedraw();
	wsContext->Deactivate();
	wsSession->Flush();
}

//**************************************************************************************

void I_StartWServEvents_e32()
{        
	iWsEventStatus = KRequestPending;

	wsSession->EventReady(&iWsEventStatus);
}

//**************************************************************************************

const int sKeyMapTable[][2] = 
{
	{EStdKeyLeftArrow, KEYD_LEFTARROW},
	{EStdKeyRightArrow, KEYD_RIGHTARROW},
	{EStdKeyUpArrow, KEYD_UPARROW},
	{EStdKeyDownArrow, KEYD_DOWNARROW},

	{EStdKeyEscape, KEYD_ESCAPE},
	{EStdKeyEnter, KEYD_ENTER},
	{EStdKeySpace, ' '},

	{EStdKeyComma, ','},
	{EStdKeyFullStop, '.'},

	{EStdKeyLeftShift, KEYD_RSHIFT},
	{EStdKeyRightShift, KEYD_RSHIFT},
	{EStdKeyLeftCtrl, KEYD_RCTRL},
	{EStdKeyRightCtrl, KEYD_RCTRL},

	{EStdKeyLeftAlt, KEYD_LALT},
	{EStdKeyRightAlt, KEYD_RALT},

	{EStdKeyCapsLock, KEYD_CAPSLOCK},

	{EStdKeyTab, KEYD_TAB},
	{EStdKeyBackspace, KEYD_BACKSPACE},
	{EStdKeyDelete, KEYD_DEL},

	{EStdKeyMinus, KEYD_MINUS},
	{EStdKeyEquals, KEYD_EQUALS},
	{EStdKeyForwardSlash, '\\'},
	{EStdKeyBackSlash, '/'},
	{EStdKeySemiColon, ';'},
	{0, 0}
};


//**************************************************************************************

int MapScanCode(int aScancode)
{
	if ((aScancode>=' ' && aScancode<='Z'))
		return tolower(aScancode);

	for (int i = 0; sKeyMapTable[i][0]; i++)
	{
		if (sKeyMapTable[i][0] == aScancode)
			return sKeyMapTable[i][1];
	}

	return 0;
}

//**************************************************************************************

void I_PollWServEvents_e32()
{
	if (iWsEventStatus != KRequestPending)
	{
		TWsEvent wsEvent;

		wsSession->GetEvent(wsEvent);
		
		TInt type = wsEvent.Type();

		if(type == EEventKeyUp || type == EEventKeyDown)
		{
			TKeyEvent* event = wsEvent.Key();

			event_t ev;
		
			ev.type = (type == EEventKeyDown) ? ev_keydown : ev_keyup;
		
			ev.data1 = MapScanCode(event->iScanCode);
			ev.data2 = 0;
			ev.data3 = 0;
			
			if(ev.data1 != 0)
				D_PostEvent(&ev);
		}
		else if(type == EEventFocusLost)
		{
			has_focus = false;
		}
		else if(type == EEventFocusGained)
		{
			has_focus = true;
			clear_screen = true;
		}
		

		iWsEventStatus = KRequestPending;
		wsSession->EventReady(&iWsEventStatus);
	}
}

//**************************************************************************************

void I_ClearWindow_e32()
{
	wsContext->Activate(*wsWindow);
	
	TRect rect = TRect(wsWindow->Size());
	wsWindow->Invalidate(rect);        
	wsWindow->BeginRedraw(rect);
	

	wsContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
	wsContext->SetBrushColor(KRgbBlack);

	wsContext->DrawRect(rect);
		
	wsWindow->EndRedraw();
	wsContext->Deactivate();
	wsSession->Flush();
}


//**************************************************************************************

bool I_CreateWindow_e32()
{
	User::LeaveIfError(wsScreen->CreateContext(wsContext));

	wsWinGroup = new RWindowGroup(*wsSession);

	User::LeaveIfError(wsWinGroup->Construct((TUint32)&wgToken));

	wsWinGroup->SetOrdinalPosition(0);

	wsWindow = new RWindow(*wsSession);

	User::LeaveIfError(wsWindow->Construct(*wsWinGroup, (TUint32)&wgToken));

	wsWindow->SetRequiredDisplayMode(EGray16);

	wsWindow->Activate();

	wsWindow->SetSize(wsScreen->SizeInPixels());
	wsWindow->SetVisible(ETrue);

	I_StartWServEvents_e32();

	return true;
}

//**************************************************************************************

void I_CreateBackBuffer_e32()
{	
	I_CreateWindow_e32();

	TPckgBuf<TScreenInfoV01> i;
	UserSvr::ScreenInfo(i);
	
	if(i().iScreenAddressValid)
	{
		pScreenBuff = (byte*)i().iScreenAddress;

		pScreenBuff += (scr_offset >> 1);
		y_pitch = (screen_width >> 1);
	}
	else
	{
		pFbs = new CWsBitmap(*wsSession);

		TInt err = pFbs->Create(TSize(vid_width, vid_height), EGray16);

		if(err != KErrNone)
		{
			//Cookie monster is unhappy. There are no cookies.
		}

		pScreenBuff = (byte*)pFbs->DataAddress();
		y_pitch = (vid_width >> 1);

		use_ws_bitmap = true;
	}
}

//**************************************************************************************

void I_FinishUpdate_e32(byte* srcBuffer, byte* pallete, const unsigned int width, const unsigned int height)
{

	if(!pScreenBuff)
		return;
	
	if(!has_focus)
		return;

	if(clear_screen)
	{
		I_ClearWindow_e32();
		clear_screen = false;
	}
		
	

	unsigned int* pSrcScanline = (unsigned int*)srcBuffer;
	unsigned int* pDstScanline = (unsigned int*)pScreenBuff;

	const unsigned int xp2 = (width >> 2);
	const unsigned int yp2 = ((y_pitch >> 2) - xp2);

	unsigned int y = 0;

	do
	{
		unsigned int* dend = (pDstScanline + xp2);

		//4bit color. The pallete has 2x of the dest color (color | color << 4)
		//so we don't have to compose each nybble. 1 x pallete[n] = 2 pixels.
		do
		{

			unsigned int ps1 = *pSrcScanline++;
			unsigned int ps2 = *pSrcScanline++;
			
			unsigned int p1 = pallete[ ps1			& 0xff];
			unsigned int p2 = pallete[(ps1 >> 8)	& 0xff];
			unsigned int p3 = pallete[(ps1 >> 16)	& 0xff];
			unsigned int p4 = pallete[(ps1 >> 24)];

			unsigned int p5 = pallete[ ps2			& 0xff];
			unsigned int p6 = pallete[(ps2 >> 8)	& 0xff];
			unsigned int p7 = pallete[(ps2 >> 16)	& 0xff];
			unsigned int p8 = pallete[(ps2 >> 24)];

			*pDstScanline++ =		p1 | (p2 << 8) | (p3 << 16) | (p4 << 24);
			*pDstScanline++ =		p5 | (p6 << 8) | (p7 << 16) | (p8 << 24);


			ps1 = *pSrcScanline++;
			ps2 = *pSrcScanline++;
			
			p1 = pallete[ ps1			& 0xff];
			p2 = pallete[(ps1 >> 8)		& 0xff];
			p3 = pallete[(ps1 >> 16)	& 0xff];
			p4 = pallete[(ps1 >> 24)];

			p5 = pallete[ ps2			& 0xff];
			p6 = pallete[(ps2 >> 8)		& 0xff];
			p7 = pallete[(ps2 >> 16)	& 0xff];
			p8 = pallete[(ps2 >> 24)];

			*pDstScanline++ =		p1 | (p2 << 8) | (p3 << 16) | (p4 << 24);
			*pDstScanline++ =		p5 | (p6 << 8) | (p7 << 16) | (p8 << 24);

		} while(pDstScanline < dend);

		pDstScanline += yp2;

	} while(++y < height);



	if(use_ws_bitmap)
	{
		I_BlitScreenBmp_e32();
	}		
}

//**************************************************************************************

int I_GetVideoWidth_e32()
{
	return vid_width >> 1;
}

//**************************************************************************************

int I_GetVideoHeight_e32()
{
	return vid_height;
}

//**************************************************************************************

void I_ProcessKeyEvents()
{
	I_PollWServEvents_e32();
}

//**************************************************************************************

#define MAX_MESSAGE_SIZE 2048

void I_Error (char *error, ...)
{
	char msg[MAX_MESSAGE_SIZE];
 

	TInt	id = 0;
	//Try to pull the STDLIB console window foreground.
	//Set the owning window of our window group to this
	//so the shell will pull it foreground.

	while (id >= 0)
	{
		id = wsSession->FindWindowGroupIdentifier(id , _L("*"));
		if (id > 0)
		{
			TThreadId threadId;
			if (wsSession->GetWindowGroupClientThreadId(id , threadId) == KErrNone)
			{
				RThread	thread;
				if (thread.Open(threadId) == KErrNone)
				{
					TFullName	name;
					name = thread.Name();
					if (name.Compare(_L("STDOUT")) == 0)
					{
						TWsEvent	event;

						wsWinGroup->SetOwningWindowGroup(id);
						thread.Close();
						break;
					}
					thread.Close();
				}
			}
		}
	}

	wsWindow->Close();
	wsWindow->Destroy();

	//delete wsWindow;
	wsWindow = NULL;
	
	
	wsWinGroup->Close();
	wsWinGroup->Destroy();

	//delete wsWinGroup;
	wsWinGroup = NULL;


	delete wsContext;
	wsContext = NULL;


	delete wsScreen;
	wsScreen = NULL;

	wsSession->Close();
	delete wsSession;



	va_list v;
	va_start(v, error);
	
	vsprintf(msg, error, v);
	
	va_end(v);

	printf("%s", msg);
	

	TPtrC8	ptr((TText8*) msg);
	RDebug::Print(_L("I_Error: %S"), &ptr);

    fflush( stderr );
	fflush( stdout );

	gets(msg);

	I_Quit_e32();
}

//**************************************************************************************

void I_Quit_e32()
{
	//Kill the application.
	RProcess().Kill(-1);
}

//**************************************************************************************



//**************************************************************************************

extern "C" int psionDoomMain();

EXPORT_C void EntryRunGame(int /*argc*/, char* /*argv[]*/)
{

	CTrapCleanup* cleanup=CTrapCleanup::New();

#ifdef __WINS__
	int crt_argc=0;
	char** crt_argv=0;
	char** crt_envp=0;

	__crt0(crt_argc, crt_argv, crt_envp);
#endif


	printf(" ");
	fflush(stdout);

	if (chdir("d:\\Doom") != 0)
		chdir("c:\\Doom");

	psionDoomMain();

	delete cleanup;
}


#ifdef __WINS__

int __cdecl E32Dll(TDllReason /*aReason*/)
{
	return(KErrNone);
}

#else

EXPORT_C int E32Main()
{
	CTrapCleanup* cleanup=CTrapCleanup::New();

	psionDoomMain();

	delete cleanup;

	return 0;
}

#endif


//**************************************************************************************