/* winuser.h*/
/*
 * Copyright (c) 1999, 2000 Greg Haerr <greg@censoft.com>
 *
 * Win32 USER structures and API
 */
#ifndef _WINUSER_H_
#define _WINUSER_H_

//#include "winctl.h"  /* required compatibility for resource compiler*/

/* moved from windef.h for resource compiler*/
typedef LRESULT (CALLBACK* WNDPROC)(HWND, UINT32, WPARAM, LPARAM);
typedef BOOL (CALLBACK* DLGPROC)(HWND, UINT32, WPARAM, LPARAM);

#define FAR

/* win api*/
LRESULT WINAPI 	DefWindowProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

/* Class styles*/
#define CS_VREDRAW          0x0001
#define CS_HREDRAW          0x0002
#define CS_DBLCLKS          0x0008
#define CS_OWNDC            0x0020
#define CS_CLASSDC          0x0040
#define CS_PARENTDC         0x0080
#define CS_NOCLOSE          0x0200
#define CS_SAVEBITS         0x0800
#define CS_BYTEALIGNCLIENT  0x1000
#define CS_BYTEALIGNWINDOW  0x2000
#define CS_GLOBALCLASS      0x4000

typedef struct tagWNDCLASSA {
    MWLIST	link;			/* microwin*/
    UINT        style;
    WNDPROC     lpfnWndProc;
    int         cbClsExtra;		/* nyi*/
    int         cbWndExtra;
    HINSTANCE   hInstance;		/* nyi*/
    HICON       hIcon;			/* nyi*/
    HCURSOR     hCursor;		/* nyi*/
    HBRUSH      hbrBackground;
    LPCWSTR     lpszMenuName;    /* nyi*/
    LPCWSTR     lpszClassName;
    WCHAR       szClassName[40];  /* microwin*/
} WNDCLASS, *PWNDCLASS, NEAR *NPWNDCLASS, FAR *LPWNDCLASS;

ATOM WINAPI	RegisterClass(CONST WNDCLASS *lpWndClass);
BOOL WINAPI UnregisterClass(LPCWSTR lpClassName, HINSTANCE hInstance);

/*
 * Message structure
 */
typedef struct tagMSG {
    MWLIST	link;			/* microwin*/
    HWND        hwnd;
    UINT        message;
    WPARAM      wParam;
    LPARAM      lParam;
    UINT64      time;
    POINT       pt;
} MSG, *PMSG, NEAR *NPMSG, FAR *LPMSG;

#define POINTSTOPOINT(pt, pts)                          \
        { (pt).x = (LONG)(SHORT)LOWORD(*(LONG*)&pts);   \
          (pt).y = (LONG)(SHORT)HIWORD(*(LONG*)&pts); }

#define POINTTOPOINTS(pt)      (MAKELONG((short)((pt).x), (short)((pt).y)))
#define MAKEWPARAM(l, h)      (WPARAM)MAKELONG(l, h)
#define MAKELPARAM(l, h)      (LPARAM)MAKELONG(l, h)
#define MAKELRESULT(l, h)     (LRESULT)MAKELONG(l, h)

/* window messages*/
#define WM_NULL                         0x0000
#define WM_CREATE                       0x0001
#define WM_DESTROY                      0x0002
#define WM_NCDESTROY			WM_DESTROY
#define WM_MOVE                         0x0003
#define WM_SIZE                         0x0005
#define WM_ACTIVATE                     0x0006
#define WM_SETFOCUS                     0x0007
#define WM_KILLFOCUS                    0x0008
#define WM_ENABLE                       0x000A
#define WM_SETREDRAW					0x000B
#define WM_SETTEXT                      0x000C
#define WM_GETTEXT                      0x000D
#define WM_GETTEXTLENGTH                0x000E
#define WM_PAINT                        0x000F
#define WM_CLOSE                        0x0010
#define WM_QUIT                         0x0012
#define WM_ERASEBKGND                   0x0014
#define WM_SHOWWINDOW                   0x0018
#define WM_CTLCOLOR                     0x0019
#define WM_NEXTDLGCTL                   0x0028
#define WM_DRAWITEM                     0x002B
#define WM_MEASUREITEM                  0x002C
#define WM_DELETEITEM 					0x002D
#define WM_VKEYTOITEM					0x002E
#define WM_CHARTOITEM					0x002F
#define WM_SETFONT          		0x0030
#define WM_GETFONT      		0x0031
#define WM_COMPAREITEM					0x0039
#define WM_WINDOWPOSCHANGED             0x0047
#define WM_NOTIFY                       0x004E
#define WM_NCCREATE                     0x0081
#define WM_NCCALCSIZE                   0x0083
#define WM_NCHITTEST                    0x0084
#define WM_NCPAINT                      0x0085
#define WM_GETDLGCODE                   0x0087
#define WM_NCMOUSEMOVE                  0x00A0
#define WM_NCLBUTTONDOWN                0x00A1
#define WM_NCLBUTTONUP                  0x00A2
#define WM_NCLBUTTONDBLCLK              0x00A3
#define WM_NCRBUTTONDOWN                0x00A4
#define WM_NCRBUTTONUP                  0x00A5
#define WM_NCRBUTTONDBLCLK              0x00A6
#define WM_KEYFIRST                     0x0100
#define WM_KEYDOWN                      0x0100
#define WM_KEYUP                        0x0101
#define WM_CHAR                         0x0102
#define WM_DEADCHAR                     0x0103	/* notimp*/
#define WM_SYSKEYDOWN                   0x0104	/* nyi*/
#define WM_SYSKEYUP                     0x0105	/* nyi*/
#define WM_SYSCHAR                      0x0106
#define WM_SYSDEADCHAR                  0x0107	/* notimp*/
#define WM_KEYLAST                      0x0108
#define WM_INITDIALOG	                0x0110
#define WM_COMMAND                      0x0111
#define WM_SYSCOMMAND                   0x0112
#define WM_TIMER                        0x0113
#define WM_HSCROLL                      0x0114
#define WM_VSCROLL                      0x0115

#define WM_ENTERIDLE                    0x0121

#define WM_CTLCOLORMSGBOX               0x0132
#define WM_CTLCOLOREDIT                 0x0133
#define WM_CTLCOLORLISTBOX              0x0134
#define WM_CTLCOLORBTN                  0x0135
#define WM_CTLCOLORDLG                  0x0136
#define WM_CTLCOLORSCROLLBAR            0x0137
#define WM_CTLCOLORSTATIC               0x0138

#define WM_MOUSEFIRST                   0x0200
#define WM_MOUSEMOVE                    0x0200
#define WM_LBUTTONDOWN                  0x0201
#define WM_LBUTTONUP                    0x0202
#define WM_LBUTTONDBLCLK                0x0203
#define WM_RBUTTONDOWN                  0x0204
#define WM_RBUTTONUP                    0x0205
#define WM_RBUTTONDBLCLK                0x0206
#define WM_MBUTTONDOWN                  0x0207
#define WM_MBUTTONUP                    0x0208
#define WM_MBUTTONDBLCLK                0x0209
#define WM_MOUSEWHEEL                   0x020A
#define WM_MOUSELAST                    0x020A
#define WM_MOUSEENTER                   0x02AF // win32 api doesn't have it
#define WM_MOUSEHOVER                   0x02A1
#define WM_MOUSELEAVE                   0x02A3
#define WM_HOTKEY                       0x0312

#define WM_CARET_CREATE    		0x03E0 /* Microwindows only*/
#define WM_CARET_DESTROY   		0x03E1 /* Microwindows only*/
#define WM_CARET_BLINK      		0x03E2 /* Microwindows only*/
#define WM_FDINPUT                      0x03F0 /* Microwindows only*/
#define WM_FDOUTPUT                     0x03F1 /* Microwindows only*/
#define WM_FDEXCEPT                     0x03F2 /* Microwindows only*/
#define WM_USER                         0x0400
#define WM_APP                          0x8000

/*
 * System Menu Command Values
 */
#define SC_SIZE         0xF000
#define SC_MOVE         0xF010
#define SC_MINIMIZE     0xF020
#define SC_MAXIMIZE     0xF030
#define SC_NEXTWINDOW   0xF040
#define SC_PREVWINDOW   0xF050
#define SC_CLOSE        0xF060
#define SC_VSCROLL      0xF070
#define SC_HSCROLL      0xF080
#define SC_MOUSEMENU    0xF090
#define SC_KEYMENU      0xF100
#define SC_ARRANGE      0xF110
#define SC_RESTORE      0xF120
#define SC_TASKLIST     0xF130
#define SC_SCREENSAVE   0xF140
#define SC_HOTKEY       0xF150

/* WM_ACTIVATE state values*/
#define WA_INACTIVE     0
#define WA_ACTIVE       1
#define WA_CLICKACTIVE  2

/* WM_NCHITTEST codes*/
#define HTERROR             (-2)
#define HTTRANSPARENT       (-1)
#define HTNOWHERE           0
#define HTCLIENT            1
#define HTCAPTION           2
#define HTSYSMENU           3
#define HTGROWBOX           4
#define HTSIZE              HTGROWBOX
#define HTMENU              5
#define HTHSCROLL           6
#define HTVSCROLL           7
#define HTMINBUTTON         8
#define HTMAXBUTTON         9
#define HTLEFT              10
#define HTRIGHT             11
#define HTTOP               12
#define HTTOPLEFT           13
#define HTTOPRIGHT          14
#define HTBOTTOM            15
#define HTBOTTOMLEFT        16
#define HTBOTTOMRIGHT       17
#define HTBORDER            18
#define HTREDUCE            HTMINBUTTON
#define HTZOOM              HTMAXBUTTON
#define HTSIZEFIRST         HTLEFT
#define HTSIZELAST          HTBOTTOMRIGHT
#define HTOBJECT            19
#define HTCLOSE             20
#define HTHELP              21

/* WM_SIZE wparam values*/
#define SIZE_RESTORED       0
#define SIZE_MINIMIZED      1
#define SIZE_MAXIMIZED      2
#define SIZE_MAXSHOW        3
#define SIZE_MAXHIDE        4

LRESULT WINAPI  CallWindowProc(WNDPROC lpPrevWndFunc, HWND hwnd, UINT Msg,
			WPARAM wParam, LPARAM lParam);
LRESULT WINAPI	SendMessage(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam);
BOOL WINAPI	PostMessage(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam);
BOOL WINAPI	PostThreadMessage(DWORD dwThreadId, UINT Msg, WPARAM wParam,
			LPARAM lParam);
VOID WINAPI	PostQuitMessage(int nExitCode);

/* PeekMessage options*/
#define PM_NOREMOVE		0x0000
#define PM_REMOVE		0x0001
#define PM_NOYIELD		0x0002

BOOL WINAPI	PeekMessage(LPMSG lpMsg, HWND hwnd, UINT uMsgFilterMin,
			UINT uMsgFilterMax, UINT wRemoveMsg);
BOOL WINAPI 	GetMessage(LPMSG lpMsg,HWND hwnd,UINT wMsgFilterMin,
			UINT wMsgFilterMax);
BOOL WINAPI 	TranslateMessage(CONST MSG *lpMsg);
LRESULT WINAPI	DispatchMessage(CONST MSG *lpMsg);

/* Hotkey stuff */
BOOL WINAPI MwRegisterHotKey(HWND hWnd, int id, UINT fsModifiers, UINT vk);
BOOL WINAPI UnregisterHotKey(HWND hWnd, int id);
BOOL MwDeliverHotkey (WPARAM VK_Code, BOOL pressed);

/* note: the following struct is in reverse order from the
 * microsoft version since WINAPI is cdecl in this implementation
 */
typedef struct tagCREATESTRUCT {
    DWORD       dwExStyle;
    LPCWSTR     lpszClass;
    LPCWSTR     lpszName;
    LONG        style;
    int         x;
    int         y;
    int         cx;
    int         cy;
    HWND        hwndParent;
    HMENU       hMenu;
    HINSTANCE   hInstance;
    LPVOID      lpCreateParams;
} CREATESTRUCT, *LPCREATESTRUCT;

typedef struct tagCOMPAREITEMSTRUCT {
	UINT	CtlType;
	UINT	CtlID;
	HWND	hwndItem;
	UINT	itemID1;
	DWORD	itemData1;
	UINT	itemID2;
	DWORD	itemData2;
	DWORD	dwLocaleId;
} COMPAREITEMSTRUCT,*LPCOMPAREITEMSTRUCT;
typedef struct tagDELETEITEMSTRUCT {
	UINT CtlType;
	UINT CtlID;
	UINT itemID;
	HWND hwndItem;
	UINT itemData;
} DELETEITEMSTRUCT,*PDELETEITEMSTRUCT,*LPDELETEITEMSTRUCT;
typedef struct tagNMHDR
{
    HWND  hwndFrom;
    UINT  idFrom;
    UINT  code;         // NM_ code
}   NMHDR;
typedef NMHDR FAR * LPNMHDR;

/* Button codes for MW_MOUSEMOVED:
 * Please note that they differ from normal Windows codes
 */
#define MK_LBUTTON	MWBUTTON_L
#define MK_RBUTTON	MWBUTTON_R
#define MK_MBUTTON 	MWBUTTON_M

/*
 * Window Styles
 */
#define WS_OVERLAPPED       0x00000000L
#define WS_POPUP            0x80000000L
#define WS_CHILD            0x40000000L
#define WS_MINIMIZE         0x20000000L
#define WS_VISIBLE          0x10000000L
#define WS_DISABLED         0x08000000L
#define WS_CLIPSIBLINGS     0x04000000L
#define WS_CLIPCHILDREN     0x02000000L
#define WS_MAXIMIZE         0x01000000L
#define WS_CAPTION          0x00C00000L     /* WS_BORDER | WS_DLGFRAME  */
#define WS_BORDER           0x00800000L
#define WS_DLGFRAME         0x00400000L
#define WS_VSCROLL          0x00200000L
#define WS_HSCROLL          0x00100000L
#define WS_SYSMENU          0x00080000L
#define WS_THICKFRAME       0x00040000L
#define WS_GROUP            0x00020000L
#define WS_TABSTOP          0x00010000L

#define WS_MINIMIZEBOX      0x00020000L
#define WS_MAXIMIZEBOX      0x00010000L

#define WS_TILED            WS_OVERLAPPED
#define WS_ICONIC           WS_MINIMIZE
#define WS_SIZEBOX          WS_THICKFRAME
#define WS_TILEDWINDOW      WS_OVERLAPPEDWINDOW

/*
 * Common Window Styles
 */
#define WS_OVERLAPPEDWINDOW (WS_OVERLAPPED     | \
                             WS_CAPTION        | \
                             WS_SYSMENU        | \
                             WS_THICKFRAME     | \
                             WS_MINIMIZEBOX    | \
                             WS_MAXIMIZEBOX)

#define WS_POPUPWINDOW      (WS_POPUP          | \
                             WS_BORDER         | \
                             WS_SYSMENU)

#define WS_CHILDWINDOW      (WS_CHILD)

/*
 * Extended Window Styles
 */
#define WS_EX_DLGMODALFRAME     0x00000001L
#define WS_EX_NOPARENTNOTIFY    0x00000004L
#define WS_EX_TOPMOST           0x00000008L
#define WS_EX_ACCEPTFILES       0x00000010L
#define WS_EX_TRANSPARENT       0x00000020L
#define WS_EX_MDICHILD          0x00000040L
#define WS_EX_TOOLWINDOW        0x00000080L
#define WS_EX_WINDOWEDGE        0x00000100L
#define WS_EX_CLIENTEDGE        0x00000200L
#define WS_EX_CONTEXTHELP       0x00000400L

#define WS_EX_RIGHT             0x00001000L
#define WS_EX_LEFT              0x00000000L
#define WS_EX_RTLREADING        0x00002000L
#define WS_EX_LTRREADING        0x00000000L
#define WS_EX_LEFTSCROLLBAR     0x00004000L
#define WS_EX_RIGHTSCROLLBAR    0x00000000L

#define WS_EX_CONTROLPARENT     0x00010000L
#define WS_EX_STATICEDGE        0x00020000L
#define WS_EX_APPWINDOW         0x00040000L
#define WS_EX_LAYERED		0x00080000L

#define WS_EX_NOACTIVATE        0x80000000L

#define WS_EX_OVERLAPPEDWINDOW  (WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE)
#define WS_EX_PALETTEWINDOW     (WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST)

#define CW_USEDEFAULT       ((int)0x80000000)

#define HWND_DESKTOP        ((HWND)0)

#define CreateWindow(lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hwndParent, hMenu, hInstance, lpParam)\
		CreateWindowEx(0L, lpClassName, lpWindowName, dwStyle, x, y,\
		nWidth, nHeight, hwndParent, hMenu, hInstance, lpParam)

HWND WINAPI	CreateWindowEx(DWORD dwExStyle, LPCWSTR lpClassName,
			LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y,
			int nWidth, int nHeight, HWND hwndParent, HMENU hMenu,
			HINSTANCE hInstance, LPVOID lpParam);
BOOL WINAPI 	DestroyWindow(HWND hwnd);
BOOL WINAPI	IsWindow(HWND hwnd);
#define IsWindowVisible(hwnd)	((BOOL)((hwnd)->unmapcount == 0))

/*
 * ShowWindow() Commands
 */
#define SW_HIDE             0
#define SW_SHOWNORMAL       1
#define SW_NORMAL           1
#define SW_SHOWMINIMIZED    2
#define SW_SHOWMAXIMIZED    3
#define SW_MAXIMIZE         3
#define SW_SHOWNOACTIVATE   4
#define SW_SHOW             5
#define SW_MINIMIZE         6
#define SW_SHOWMINNOACTIVE  7
#define SW_SHOWNA           8
#define SW_RESTORE          9
#define SW_SHOWDEFAULT      10
#define SW_FORCEMINIMIZE    11
#define SW_MAX              11

BOOL WINAPI 	ShowWindow(HWND hwnd, int nCmdShow);
BOOL WINAPI	InvalidateRect(HWND hwnd, CONST RECT *lpRect, BOOL bErase);
BOOL WINAPI	InvalidateRgn(HWND hwnd, HRGN hrgn, BOOL bErase);
BOOL WINAPI	ValidateRect(HWND hwnd, CONST RECT *lprc);
BOOL WINAPI	ValidateRgn(HWND hwnd, HRGN hrgn);
BOOL WINAPI	UpdateWindow(HWND hwnd);

BOOL WINAPI	SetLayeredWindowAttributes(HWND hwnd, COLORREF crKey,
			BYTE bAlpha, DWORD dwFlags);
#define LWA_COLORKEY	0x00000001
#define LWA_ALPHA	0x00000002

HWND WINAPI	GetFocus(VOID);
HWND WINAPI	SetFocus(HWND hwnd);
BOOL WINAPI	SetForegroundWindow(HWND hwnd);
HWND WINAPI	SetActiveWindow(HWND hwnd);
HWND WINAPI	GetActiveWindow(VOID);
BOOL WINAPI	BringWindowToTop(HWND hwnd);
HWND WINAPI	GetDesktopWindow(VOID);
HWND WINAPI	GetParent(HWND hwnd);
BOOL WINAPI	EnableWindow(HWND hwnd, BOOL bEnable);
#define IsWindowEnabled(hwnd)	((BOOL)(((hwnd)->style&WS_DISABLED) == 0))

BOOL WINAPI	AdjustWindowRectEx(LPRECT lpRect, DWORD dwStyle, BOOL bMenu,
			DWORD dwExStyle);
BOOL WINAPI	GetClientRect(HWND hwnd, LPRECT lpRect);
BOOL WINAPI	GetWindowRect(HWND hwnd, LPRECT lpRect);

BOOL WINAPI 	ClientToScreen(HWND hwnd, LPPOINT lpPoint);
BOOL WINAPI 	ScreenToClient(HWND hwnd, LPPOINT lpPoint);
int  WINAPI	MapWindowPoints(HWND hwndFrom, HWND hwndTo, LPPOINT lpPoints,
			UINT cPoints);

/*
 * Window field offsets for GetWindowLong()
 */
#define GWL_WNDPROC         (-4)
#define GWL_HINSTANCE       (-6)
#define GWL_HWNDPARENT      (-8)
#define GWL_STYLE           (-16)
#define GWL_EXSTYLE         (-20)
#define GWL_USERDATA        (-21)
#define GWL_ID              (-12)
#define GWLP_WNDPROC        (-4)
#define GWLP_HINSTANCE      (-6)
#define GWLP_HWNDPARENT     (-8)
#define GWLP_ID             (-12)
#define GWLP_USERDATA       (-21)

/*
 * Class field offsets for GetClassLongPtr()
 */
#define GCL_MENUNAME        (-8)
#define GCL_HBRBACKGROUND   (-10)
#define GCL_HCURSOR         (-12)
#define GCL_HICON           (-14)
#define GCL_HMODULE         (-16)
#define GCL_CBWNDEXTRA      (-18)
#define GCL_CBCLSEXTRA      (-20)
#define GCL_WNDPROC         (-24)
#define GCL_STYLE           (-26)
#define GCW_ATOM            (-32)
#define GCL_HICONSM         (-34)

#define GCLP_HBRBACKGROUND   (-10)

LONG WINAPI	GetWindowLong(HWND hwnd, int nIndex);
LONG WINAPI	SetWindowLong(HWND hwnd, int nIndex, LONG lNewLong);
INTN WINAPI GetWindowLongPtr (HWND hwnd, INT32 nIndex);
INTN WINAPI SetWindowLongPtr (HWND hwnd, INT32 nIndex, INTN lNewLong) ;
WORD WINAPI	GetWindowWord(HWND hwnd, int nIndex);
WORD WINAPI	SetWindowWord(HWND hwnd, int nIndex, WORD wNewWord);
BOOL WINAPI SetProp(HWND hWnd, LPCWSTR lpString, HANDLE hData);
HANDLE WINAPI GetProp(HWND hWnd, LPCWSTR lpString);
HANDLE WINAPI RemoveProp(HWND hWnd, LPCWSTR lpString);

#define GetDlgCtrlID(hwnd)	((int)(hwnd)->id)
UINTN  WINAPI  GetClassLongPtr(HWND hwnd, int nIndex);
int WINAPI	GetWindowTextLength(HWND hwnd);
int   WINAPI  GetWindowText(HWND hwnd, LPWSTR lpString, int nMaxCount);
BOOL  WINAPI  SetWindowText(HWND hwnd, LPCWSTR lpString);

BOOL WINAPI 	MoveWindow(HWND hwnd, int x, int y, int nWidth, int nHeight,
    			BOOL bRepaint);

/* SetWindowPos Flags*/
#define SWP_NOSIZE          0x0001
#define SWP_NOMOVE          0x0002
#define SWP_NOZORDER        0x0004
#define SWP_NOREDRAW        0x0008
#define SWP_NOACTIVATE      0x0010	/* nyi*/
#define SWP_FRAMECHANGED    0x0020	/* nyi*/
#define SWP_SHOWWINDOW      0x0040
#define SWP_HIDEWINDOW      0x0080
#define SWP_NOCOPYBITS      0x0100	/* nyi*/
#define SWP_NOOWNERZORDER   0x0200	/* nyi*/
#define SWP_NOSENDCHANGING  0x0400	/* nyi*/
#define SWP_DRAWFRAME       SWP_FRAMECHANGED
#define SWP_NOREPOSITION    SWP_NOOWNERZORDER
#define SWP_DEFERERASE      0x2000	/* nyi*/
#define SWP_ASYNCWINDOWPOS  0x4000	/* nyi*/

#define HWND_TOP        ((HWND)0)	/* nyi*/
#define HWND_BOTTOM     ((HWND)(UINTN)1)  /* nyi*/
#define HWND_TOPMOST    ((HWND)-1)	/* nyi*/
#define HWND_NOTOPMOST  ((HWND)-2)	/* nyi*/

/* WM_WINDOWPOSCHANGED message*/
typedef struct tagWINDOWPOS {
	HWND    hwnd;
	HWND    hwndInsertAfter;
	int     x;
	int     y;
	int     cx;
	int     cy;
	UINT    flags;
} WINDOWPOS, *LPWINDOWPOS, *PWINDOWPOS;

BOOL WINAPI	SetWindowPos(HWND hwnd, HWND hwndInsertAfter, int x, int y,
			int cx, int cy, UINT fuFlags);

typedef struct tagWINDOWPLACEMENT {
    UINT  length;
    UINT  flags;
    UINT  showCmd;
    POINT ptMinPosition;
    POINT ptMaxPosition;
    RECT  rcNormalPosition;
} WINDOWPLACEMENT;
typedef WINDOWPLACEMENT *PWINDOWPLACEMENT, *LPWINDOWPLACEMENT;
BOOL SetWindowPlacement(HWND hWnd, WINDOWPLACEMENT *lpwndpl);
BOOL GetWindowPlacement(HWND hWnd, WINDOWPLACEMENT *lpwndpl);

BOOL WINAPI	GetCursorPos(LPPOINT lpPoint);
HWND WINAPI	GetCapture(VOID);
HWND WINAPI	SetCapture(HWND hwnd);
BOOL WINAPI	ReleaseCapture(VOID);

#define GW_HWNDNEXT 2
#define GW_HWNDPREV 3
#define GW_CHILD 5
#define GW_HWNDFIRST 0
#define GW_HWNDLAST 1
#define GW_OWNER 4

HWND GetWindow(HWND hWnd,  UINT uCmd);
HWND GetMenu (HWND hWnd);
HWND GetForegroundWindow(VOID);
HWND WindowFromPoint(POINT pt);
BOOL WINAPI IsChild (HWND   Parent, HWND   Child);


/*
 * WM_NCCALCSIZE parameter structure
 */
typedef struct tagNCCALCSIZE_PARAMS {
    RECT       rgrc[3];
    /*PWINDOWPOS lppos;*/		/* removed for microwin*/
} NCCALCSIZE_PARAMS, *LPNCCALCSIZE_PARAMS;

//typedef FARPROC TIMERPROC;

typedef LRESULT (WINAPI *TIMERPROC)(HWND  Hwnd, UINT32 Msg, WPARAM WParam, LPARAM LParam);

UINT WINAPI	SetTimer(HWND hwnd, UINT idTimer, UINT uTimeout,
			TIMERPROC lpTimerFunc);
BOOL WINAPI	KillTimer(HWND hwnd, UINT idTimer);
UINT		MwGetNextTimeoutValue(void);
void		MwHandleTimers(void);

/* GetSystemMetrics indices*/
#define SM_CXSCREEN             0
#define SM_CYSCREEN             1
#define SM_CXVSCROLL            2
#define SM_CYHSCROLL            3
#define SM_CYCAPTION            4
#define SM_CXBORDER             5
#define SM_CYBORDER             6
#define SM_CXDLGFRAME           7
#define SM_CYDLGFRAME           8
#define SM_CXFIXEDFRAME		SM_CXDLGFRAME
#define SM_CYFIXEDFRAME		SM_CYDLGFRAME
#define SM_CYMENU               15
#define SM_CYVSCROLL            20
#define SM_CXHSCROLL            21
#define SM_CXFRAME              32
#define SM_CYFRAME              33
#define SM_CXSIZEFRAME		SM_CXFRAME
#define SM_CYSIZEFRAME		SM_CYFRAME

int WINAPI	GetSystemMetrics(int nIndex);

#define SPI_SETWORKAREA            47
#define SPI_GETWORKAREA            48

BOOL WINAPI SystemParametersInfo (UINT uiAction,  UINT uiParam, PVOID pvParam, UINT fWinIni);

HWND WINAPI	GetDlgItem(HWND hDlg, int nIDDlgItem);

/* WM_GETDLGCODE values */
#define DLGC_WANTARROWS      0x0001
#define DLGC_WANTTAB         0x0002
#define DLGC_WANTALLKEYS     0x0004
#define DLGC_WANTMESSAGE     0x0004
#define DLGC_HASSETSEL       0x0008
#define DLGC_DEFPUSHBUTTON   0x0010
#define DLGC_UNDEFPUSHBUTTON 0x0020
#define DLGC_RADIOBUTTON     0x0040
#define DLGC_WANTCHARS       0x0080
#define DLGC_STATIC          0x0100
#define DLGC_BUTTON          0x2000

/* ************************** Caret support **********************************/
BOOL WINAPI CreateCaret(HWND hwnd, HBITMAP hBitmap, int nWidth, int nHeight);
BOOL WINAPI DestroyCaret(VOID);
BOOL WINAPI HideCaret (HWND hwnd);
BOOL WINAPI ShowCaret(HWND hwnd);
BOOL WINAPI SetCaretPos(int nX, int nY);
BOOL WINAPI GetCaretPos(LPPOINT lpPoint);
UINT WINAPI GetCaretBlinkTime(VOID);
BOOL WINAPI SetCaretBlinkTime(UINT uMSeconds);


int WINAPI GetClassName(HWND hWnd, LPTSTR lpClassName, int nMaxCount);
HWND WINAPI GetNextDlgGroupItem(HWND hDlg, HWND hCtl, BOOL bPrevious);

/*
 * Dialog Box Command IDs
 */
#define IDOK                1
#define IDCANCEL            2
#define IDABORT             3
#define IDRETRY             4
#define IDIGNORE            5
#define IDYES               6
#define IDNO                7
#define IDCLOSE         	8
#define IDHELP          	9
#define IDTRYAGAIN         10
#define IDCONTINUE         11

/*
 * MessageBox() Flags
 */
#define MB_OK                       0x00000000L
#define MB_OKCANCEL                 0x00000001L
#define MB_ABORTRETRYIGNORE         0x00000002L
#define MB_YESNOCANCEL              0x00000003L
#define MB_YESNO                    0x00000004L
#define MB_RETRYCANCEL              0x00000005L
#define MB_CANCELTRYCONTINUE 		0x00000006L

#define MB_ICONHAND                 0x00000010L
#define MB_ICONQUESTION             0x00000020L
#define MB_ICONEXCLAMATION          0x00000030L
#define MB_ICONASTERISK             0x00000040L

#define MB_USERICON                 0x00000080L
#define MB_ICONWARNING              MB_ICONEXCLAMATION
#define MB_ICONERROR                MB_ICONHAND

#define MB_ICONINFORMATION          MB_ICONASTERISK
#define MB_ICONSTOP                 MB_ICONHAND

#define MB_DEFBUTTON1               0x00000000L
#define MB_DEFBUTTON2               0x00000100L
#define MB_DEFBUTTON3               0x00000200L
#define MB_DEFBUTTON4               0x00000300L

#define MB_APPLMODAL                0x00000000L
#define MB_SYSTEMMODAL              0x00001000L
#define MB_TASKMODAL                0x00002000L
#define MB_HELP                     0x00004000L // Help Button

#define MB_NOFOCUS                  0x00008000L
#define MB_SETFOREGROUND            0x00010000L
#define MB_DEFAULT_DESKTOP_ONLY     0x00020000L

#define MB_TOPMOST                  0x00040000L
#define MB_RIGHT                    0x00080000L
#define MB_RTLREADING               0x00100000L

#define MB_TYPEMASK                 0x0000000FL
#define MB_ICONMASK                 0x000000F0L
#define MB_DEFMASK                  0x00000F00L
#define MB_MODEMASK                 0x00003000L
#define MB_MISCMASK                 0x0000C000L

/* help structure not implemented, only used in MSGBOXCALLBACK below*/
#define HELPINFO_WINDOW    0x0001
#define HELPINFO_MENUITEM  0x0002
typedef struct tagHELPINFO {    /* Structure pointed to by lParam of WM_HELP */
    UINT    cbSize;             /* Size in bytes of this struct  */
    int     iContextType;       /* Either HELPINFO_WINDOW or HELPINFO_MENUITEM */
    int     iCtrlId;            /* Control Id or a Menu item Id. */
    HANDLE  hItemHandle;        /* hWnd of control or hMenu.     */
    DWORD   dwContextId;        /* Context Id associated with this item */
    POINT   MousePos;           /* Mouse Position in screen co-ordinates */
}  HELPINFO, FAR *LPHELPINFO;

typedef void (CALLBACK *MSGBOXCALLBACK)(LPHELPINFO lpHelpInfo);

typedef struct tagMSGBOXPARAMSA {
    UINT        cbSize;
    HWND        hwndOwner;
    HINSTANCE   hInstance;
    LPCSTR      lpszText;
    LPCSTR      lpszCaption;
    DWORD       dwStyle;
    LPCSTR      lpszIcon;
    DWORD       dwContextHelpId;
    MSGBOXCALLBACK      lpfnMsgBoxCallback;
    DWORD   dwLanguageId;
} MSGBOXPARAMSA, *PMSGBOXPARAMSA, *LPMSGBOXPARAMSA;

typedef MSGBOXPARAMSA MSGBOXPARAMS;

int WINAPI MessageBoxTimeout(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption,
		UINT uType, WORD wLanguageId, DWORD dwTime);
int MessageBoxEx(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType,
  WORD wLanguageId);
int MessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);
int MessageBoxIndirect( const MSGBOXPARAMS *lpMsgBoxParams);

/*
 *  Windows enumeration functions
 */
typedef BOOL CALLBACK (*WNDENUMPROC)(HWND, LPARAM);

BOOL WINAPI EnumChildWindows(HWND hWndParent, WNDENUMPROC lpEnumFunc,
			       LPARAM lParam);

#ifdef MW_CALL_IDLE_HANDLER
void WINAPI idle_handler(void);
#endif

#undef FAR
#endif
