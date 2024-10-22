/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#ifndef _COMPILER_SPECS_H_
#define _COMPILER_SPECS_H_

#if defined (_MSC_VER)
  #if _MSC_VER < 900

    #define CALLCONV __pascal
    #define ROMDATA __based( __segname( "_CODE" ) )
    #define _16BYTE_ALIGN   __declspec(align(16))
    #define _8BYTE_ALIGN   __declspec(align(8))
    #define _4BYTE_ALIGN   __declspec(align(4))
    #define _2BYTE_ALIGN   __declspec(align(2))
    #define _1BYTE_ALIGN   __declspec(align(1))

  #else /* _MSC_VER < 900 */

     #if defined (_M_IX86)

       #define ROMDATA
       #define CALLCONV
       #define _16BYTE_ALIGN   __declspec(align(64))
       #define _8BYTE_ALIGN   __declspec(align(8))
       #define _4BYTE_ALIGN   __declspec(align(4))
       #define _2BYTE_ALIGN   __declspec(align(2))
       #define _1BYTE_ALIGN   __declspec(align(1))

     #else /* _M_IX86, 64-bit compiler */

       #define ROMDATA
       #define CALLCONV
       #define _16BYTE_ALIGN   __declspec(align(16))
       #define _8BYTE_ALIGN   __declspec(align(8))
       #define _4BYTE_ALIGN   __declspec(align(4))
       #define _2BYTE_ALIGN   __declspec(align(2))
       #define _1BYTE_ALIGN   __declspec(align(1))

     #endif /* _M_IX86 */

  #endif /* _MSC_VER < 900 */

#elif defined __GNUC__ /* _MSC_VER */

  #define ROMDATA
  #define CALLCONV
  #define _16BYTE_ALIGN __attribute__ ((aligned (16)))
  #define _8BYTE_ALIGN __attribute__ ((aligned (8)))
  #define _4BYTE_ALIGN __attribute__ ((aligned (4)))
  #define _2BYTE_ALIGN __attribute__ ((aligned (2)))
  #define _1BYTE_ALIGN __attribute__ ((aligned (1)))


#endif /* __GNUC__, _MSC_VER */

#endif /* _COMPILER_SPECS_H_ */
