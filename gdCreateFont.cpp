/*
/ gdCreateFont.cpp : convert windows fonts to gd library font format.
/
/
/
/
/*/


#include "stdafx.h"
#include "commdlg.h"
#include "stdio.h"
#include "resource.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CREATEFONT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_CREATEFONT);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_CREATEFONT);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_CREATEFONT;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, 200, 200, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}








void WriteFont( HWND hWnd )
{
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rt, rc;
	long lp;
	char txt[128];
	CHOOSEFONT cf;
	LOGFONT lf;
	HFONT hFont;

	hdc = BeginPaint(hWnd, &ps);
	hdc = GetDC (hWnd);

	memset( &cf, 0, sizeof( CHOOSEFONT ) );
	memset( &lf, 0, sizeof( LOGFONT ) );
	cf.lStructSize = sizeof( CHOOSEFONT );
	cf.hwndOwner = hWnd;
	cf.hDC = hdc;
	cf.lpLogFont = &lf;
	cf.Flags = CF_BOTH;
	cf.nFontType = REGULAR_FONTTYPE;
	if( ChooseFont( &cf ) ){
		long xmax=0,ymax=0,x,y, pix, i;
		char fontname[128], *p;
		FILE *fp;

		strcpy( fontname , lf.lfFaceName );
		if ( p = strchr( fontname, ' ' ) ) *p = 0;
		sprintf( txt, "gdfont_%s.c", fontname );


		fp = fopen( txt, "w+" );

		hFont = CreateFontIndirect( cf.lpLogFont );
		SelectObject (hdc, hFont);

		GetClientRect(hWnd, &rt);
		GetClientRect(hWnd, &rc);	rc.right += 150;

		for( lp=32;lp<256;lp++){
			FillRect( hdc, &rt, GetSysColorBrush(COLOR_3DFACE) );
			sprintf( txt, "%c", lp );
			pix = GetPixel( hdc, 0, 0 );
			DrawText(hdc, txt, (int)strlen(txt), &rt, 0);

			x=0;y=0;
			while( (i=GetPixel( hdc, x, y )) != pix ) x++;
			if ( x > xmax ) xmax = x;
			x=0;
			while( (i=GetPixel( hdc, x, y )) != pix ) y++;
			if ( y > ymax ) ymax = y;
			//GetPixel( hdx, x, y );
			//CLR_INVALID
		}

		fprintf( fp, "// %s, xmax = %d, ymax = %d\n", fontname, xmax, ymax );
		fprintf( fp, "#include \"gdfont_%s.h\"\n", fontname );
		fprintf( fp, "char gdFontdata_%s[] = {\n\n", fontname );

		for( lp=32;lp<256;lp++){
			FillRect( hdc, &rt, GetSysColorBrush(COLOR_3DFACE) );
			sprintf( txt, "%c", lp );
			DrawText(hdc, txt, (int)strlen(txt), &rt, 0);

			for( y=0;y<ymax;y++){
				for( x=0;x<xmax;x++){
					pix = GetPixel( hdc, x, y );
					if ( x ) fputc( ',',fp );
					if ( pix == 0 )
						fputc( '1',fp );
					else
						fputc( '0',fp );

				}
				fputc( '\n',fp );
			}
			fputc( '\n',fp );
			//Sleep( 20 );
		}
		fprintf( fp, "}\n\n" );

		fprintf( fp, "gdFont gdFont_%s = { 255,32,%d,%d, gdFontdata_%s };\n",fontname, xmax,ymax, fontname );

		fclose(fp);
		ReleaseDC( hWnd, hdc );
		EndPaint(hWnd, &ps);
	}
}


/*

gdFont gdFontSmallRep = {
	96,
	32,
	6,
	12,
	gdFontSmallData
};

gdFontPtr gdFontSmall = &gdFontSmallRep;
*/

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				case IDM_SAVE:
					WriteFont(hWnd);
					break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code here...
			RECT rt;
			GetClientRect(hWnd, &rt);
			//DrawText(hdc, szHello, strlen(szHello), &rt, DT_CENTER);
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}
