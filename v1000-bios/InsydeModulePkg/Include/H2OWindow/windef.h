/* windef.h*/
/*
 * Copyright (c) 1999 Greg Haerr <greg@censoft.com>
 *
 * Win32 API base type definitions
 */
#ifndef _WINDEF_H
#define _WINDEF_H


#ifdef VXWORKS
/* Don't include the internal Tornado header file <ntcontext.h>, **
** as the definitions in it conflict with these definitions.     */
#define __INCntcontexth
/* Bring in the core VxWorks definitions as they could conflict **
** with the ones below if they are brought in later.            */
#include <vxWorks.h>
#endif


#ifndef NULL
#define NULL    ((void *)0)
#endif

#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

#ifndef NOMINMAX
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
#endif  /* NOMINMAX */

#define CALLBACK
#define WINAPI        EFIAPI
#define WINAPIV
#define APIENTRY    	WINAPI
#define APIPRIVATE
#define PASCAL

#define FAR
#define NEAR
#define CONST		const
#define CDECL

//#define VOID void
typedef void *PVOID;
typedef char CHAR;
typedef short SHORT;
typedef long LONG;
typedef SHORT *PSHORT;
typedef LONG *PLONG;

#ifndef VXWORKS
typedef unsigned char 		UCHAR;
typedef unsigned short 		USHORT;
typedef uint32_t		ULONG;
#ifndef __ITRON_TYPES_h_ /* FIXME RTEMS hack*/
typedef unsigned int		UINT;
#endif

#ifndef __ITRON_TYPES_h_
#ifndef COMMON_H	 /* MiniGUI hack*/
typedef int			BOOL;
#endif
#endif

#endif /* !VXWORKS*/

typedef ULONG *			PULONG;
typedef USHORT *		PUSHORT;
typedef UCHAR *			PUCHAR;
typedef char *			PSZ;
#ifndef __ITRON_TYPES_h_
typedef int                 	INT;
#endif
typedef unsigned int        	*PUINT;

typedef uint32_t       DWORD;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;

typedef float               FLOAT;
typedef FLOAT               *PFLOAT;
typedef BOOL NEAR           *PBOOL;
typedef BOOL FAR            *LPBOOL;
typedef BYTE NEAR           *PBYTE;
typedef BYTE FAR            *LPBYTE;
typedef int NEAR            *PINT;
typedef int FAR             *LPINT;
typedef WORD NEAR           *PWORD;
typedef WORD FAR            *LPWORD;
typedef long FAR            *LPLONG;
typedef DWORD NEAR          *PDWORD;
typedef DWORD FAR           *LPDWORD;
typedef void FAR            *LPVOID;
typedef CONST void FAR      *LPCVOID;

typedef unsigned short WCHAR;
typedef WCHAR *PWCHAR;
typedef WCHAR *LPWCH, *PWCH;
typedef CONST WCHAR *LPCWCH, *PCWCH;
typedef WCHAR *NWPSTR;
typedef WCHAR *LPWSTR, *PWSTR;
typedef CONST WCHAR *LPCWSTR, *PCWSTR;

typedef CHAR *PCHAR;
typedef CHAR *LPCH, *PCH;
typedef CONST CHAR *LPCCH, *PCCH;
typedef CHAR *NPSTR;
typedef CHAR *LPSTR, *PSTR;
typedef CONST CHAR *LPCSTR, *PCSTR;
//
// use WCHAR as TCHAR
//
typedef WCHAR TCHAR, *PTCHAR;
typedef unsigned char TBYTE , *PTBYTE ;
typedef LPWSTR LPTCH, PTCH;
typedef LPWSTR PTSTR, LPTSTR;
typedef LPCWSTR LPCTSTR;

#define __TEXT(quote) quote
#define TEXT(quote) __TEXT(quote)

typedef int (FAR WINAPI *FARPROC)();
typedef int (NEAR WINAPI *NEARPROC)();
typedef int (WINAPI *PROC)();

typedef UINTN WPARAM;
typedef INTN  LPARAM;
typedef INTN  LRESULT;
typedef INTN  HRESULT;

#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#define LOWORD(l)           ((WORD)(UINTN)(l))
#define HIWORD(l)           ((WORD)(((DWORD)(UINTN)(l) >> 16) & 0xFFFF))
#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))

#define PALETTEINDEX(i)     ((COLORREF)(0x01000000 | (DWORD)(WORD)(i)))

typedef DWORD   COLORREF;
typedef DWORD   *LPCOLORREF;

/* handle typedef*/
typedef PVOID HANDLE;

typedef HANDLE *PHANDLE;
typedef HANDLE NEAR         *SPHANDLE;
typedef HANDLE FAR          *LPHANDLE;
typedef HANDLE              HGLOBAL;
typedef HANDLE              HLOCAL;
typedef HANDLE              GLOBALHANDLE;
typedef HANDLE              LOCALHANDLE;

typedef WORD                ATOM;

typedef struct hwnd *	HWND;
typedef struct hdc *	HDC;
typedef struct hcursor *HCURSOR;
typedef struct hgdiobj *HGDIOBJ;
typedef struct hgdiobj *HBRUSH;
typedef struct hgdiobj *HPEN;
typedef struct hgdiobj *HFONT;
typedef struct hgdiobj *HBITMAP;
typedef struct hgdiobj *HRGN;
typedef struct hgdiobj *HPALETTE;
typedef HANDLE		HICON;
typedef HANDLE		HINSTANCE;
typedef HANDLE		HMODULE;
typedef HANDLE		HMENU;

#undef FAR
/* moved to winuser.h for resource compiler*/
/*typedef LRESULT (CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);*/

#endif
