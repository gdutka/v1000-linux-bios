/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _STD_C_LIB_SUPPORT_H
#define _STD_C_LIB_SUPPORT_H

#include <Uefi.h>
#include <Library/BaseLib.h>

#define va_list   VA_LIST
#define va_arg    VA_ARG
#define va_start  VA_START
#define va_end    VA_END

#define CHAR_BIT     8   /* number of bits in a char */
#define INT_MAX      2147483647       /* max value for an int */
#define INT_MIN      (-2147483647-1)  /* min value for an int */
#define UINT_MAX     0xffffffff  /* max value for an unsigned int */
#define ULONG_MAX    0xffffffff  /* max value for an unsigned long */
#define SHRT_MAX     32767/* max value for a short */
#define LONG_MAX     2147483647  /* max value for a long */
#define LONG_MIN     (-2147483647-1) /* min value for a long */


#define EOF          (-1)

typedef unsigned short wchar_t;
typedef UINTN size_t;

// Locale convention structure
struct lconv
{
    char*    decimal_point;
    char*    thousands_sep;
    char*    grouping;
    char*    int_curr_symbol;
    char*    currency_symbol;
    char*    mon_decimal_point;
    char*    mon_thousands_sep;
    char*    mon_grouping;
    char*    positive_sign;
    char*    negative_sign;
    char     int_frac_digits;
    char     frac_digits;
    char     p_cs_precedes;
    char     p_sep_by_space;
    char     n_cs_precedes;
    char     n_sep_by_space;
    char     p_sign_posn;
    char     n_sign_posn;
    wchar_t* _W_decimal_point;
    wchar_t* _W_thousands_sep;
    wchar_t* _W_int_curr_symbol;
    wchar_t* _W_currency_symbol;
    wchar_t* _W_mon_decimal_point;
    wchar_t* _W_mon_thousands_sep;
    wchar_t* _W_positive_sign;
    wchar_t* _W_negative_sign;
};

//
// ctype.h
//
int tolower(int c);
int isdigit(int c);
int isalpha(int c);
int isupper(int c);
int isscii(int c);
int isspace(int c);

//
// setjmp.h
//

typedef BASE_LIBRARY_JUMP_BUFFER jmp_buf[1];
#define setjmp(env)   (INTN)SetJump((env))
void longjmp(jmp_buf env, int val);

//
// stdio.h
//
int sprintf ( char * str, const char * format, ... );

//
// stdlib.h
//
int  abs(register int i);
void *calloc (size_t, size_t);
//void exit(int status);
void  free (void *);
long labs(long j);
void *malloc (size_t);
void qsort (void *base, size_t num, size_t width, int (*compare)(const void *, const void *));
void *realloc (void *, size_t);
long int strtol (const char* str, char** endptr, int base);

//
// string.h
//
void *memchr (const void *, int, size_t);
int memcmp (const void *, const void *, size_t);
void *memcpy (void *, const void *, size_t);
void *memmove (void *, const void *, size_t size);
void *memset (void *, int, size_t size);
char *strcat (char *, const char *);
int  strcmp (const char *, const char *);
char *strcpy (char *, const char *);
size_t  strlen (const char *);
int strncmp (const char *, const char *, size_t);
char *strncpy (char *, const char *, size_t);
char *strrchr (const char *, int);
char *strstr (const char *, const char *);
int   strcmpi ( const char * str1, const char * str2 );
char *strdup(const char *s1);

//
// wchar.h
//
wchar_t * wcsdup(wchar_t *str);
unsigned long wcstoul(wchar_t *nptr, wchar_t **endptr , unsigned int base );
long wcstol(wchar_t  *nptr, wchar_t  **endptr , int base );
int wcscasecmp (wchar_t *s1, wchar_t *s2);
wchar_t *wcscpy (wchar_t *to, const wchar_t *from);
size_t wcslen(const wchar_t *str);
wchar_t *wcscat(wchar_t *s, const wchar_t *append);
int wcscmp(const wchar_t *s1, const wchar_t *s2);
wchar_t *wcsncpy(wchar_t *dst, const wchar_t * src,size_t n);

//
// locale.h
//
struct lconv* localeconv(void);

#if defined (MDE_CPU_X64)
#if defined(_MSC_EXTENSIONS)
#define ptrdiff_t __int64
#else
#define ptrdiff_t long long
#endif
#else
#define ptrdiff_t  int
#endif

#define assert(exp) ASSERT(exp)

#ifdef __GNUC__
#if __GNUC__ >= 4
#define OFFSET_OF(TYPE, Field) ((UINTN) __builtin_offsetof(TYPE, Field))
#endif
#endif

#ifndef OFFSET_OF
#define OFFSET_OF(TYPE, Field) ((UINTN) &(((TYPE *)0)->Field))
#endif

#define offsetof(StrucName, Member)  OFFSET_OF(StrucName, Member)


#define swprintf UnicodeSPrint

#endif
