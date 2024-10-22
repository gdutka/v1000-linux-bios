/** @file
  Header file for H2O Console Redirection Configuration

;******************************************************************************
;* Copyright (c) 2015, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _CR_CONFIG_H_
#define _CR_CONFIG_H_

#include <Guid/CrConfigHii.h>
#include <Guid/SolSetupConfigHii.h>

#define CR_CONFIGURATION_VARSTORE_ID        0x7777

#define CR_DEVICE_LABEL_START               0x1000
#define CR_DEVICE_LABEL_END                 0x1001

#define CONSOLE_REDIRECTION_ROOT_FORM_ID    0x2001
#define CR_DEVICE_FORM_ID                   0x2002
//[-start-190612-IB07401110-add]//
#define CR_DEVICE_FORM_0_ID                 0x2002
#define CR_DEVICE_FORM_1_ID                 0x2003
#define CR_DEVICE_FORM_2_ID                 0x2004
#define CR_DEVICE_FORM_3_ID                 0x2005
#define CR_DEVICE_FORM_4_ID                 0x2006
#define CR_DEVICE_FORM_5_ID                 0x2007
#define CR_DEVICE_FORM_6_ID                 0x2008
#define CR_DEVICE_FORM_7_ID                 0x2009
#define CR_DEVICE_FORM_8_ID                 0x200A
#define CR_DEVICE_FORM_9_ID                 0x200B
#define CR_DEVICE_FORM_10_ID                0x200C
//[-end-190612-IB07401110-add]//

#define CR_DEVICE_QUESTION_ID_BASE          0xC000

#define KEY_GLOBAL_TERMINAL_TYPE            0x0D00
#define KEY_GLOBAL_BAUD_RATE                0x0D01
#define KEY_GLOBAL_DATA_BITS                0x0D02
#define KEY_GLOBAL_PARITY                   0x0D03
#define KEY_GLOBAL_STOP_BITS                0x0D04
#define KEY_GLOBAL_FLOW_CONTROL             0x0D05

//[-start-190613-IB07401110-remove]//
//#define KEY_PORT_ENABLE                     0x0D06
//#define KEY_USE_GLOBAL_SETTING              0x0D07
//
//#define KEY_TERMINAL_TYPE                   0x0D08
//#define KEY_BAUD_RATE                       0x0D09
//#define KEY_DATA_BITS                       0x0D0A
//#define KEY_PARITY                          0x0D0B
//#define KEY_STOP_BITS                       0x0D0C
//#define KEY_FLOW_CONTROL                    0x0D0D
//[-end-190613-IB07401110-remove]//

//[-start-190612-IB07401110-add]//
#define KEY_PORT_ENABLE0                    0x0E00
#define KEY_PORT_ENABLE1                    0x0E01
#define KEY_PORT_ENABLE2                    0x0E02
#define KEY_PORT_ENABLE3                    0x0E03
#define KEY_PORT_ENABLE4                    0x0E04
#define KEY_PORT_ENABLE5                    0x0E05
#define KEY_PORT_ENABLE6                    0x0E06
#define KEY_PORT_ENABLE7                    0x0E07
#define KEY_PORT_ENABLE8                    0x0E08
#define KEY_PORT_ENABLE9                    0x0E09
#define KEY_PORT_ENABLE10                   0x0E0A

#define KEY_USE_GLOBAL_SETTING0             0x0E0B
#define KEY_USE_GLOBAL_SETTING1             0x0E0C
#define KEY_USE_GLOBAL_SETTING2             0x0E0D
#define KEY_USE_GLOBAL_SETTING3             0x0E0E
#define KEY_USE_GLOBAL_SETTING4             0x0E0F
#define KEY_USE_GLOBAL_SETTING5             0x0E10
#define KEY_USE_GLOBAL_SETTING6             0x0E11
#define KEY_USE_GLOBAL_SETTING7             0x0E12
#define KEY_USE_GLOBAL_SETTING8             0x0E13
#define KEY_USE_GLOBAL_SETTING9             0x0E14
#define KEY_USE_GLOBAL_SETTING10            0x0E15

#define KEY_TERMINAL_TYPE0                  0x0E16
#define KEY_TERMINAL_TYPE1                  0x0E17
#define KEY_TERMINAL_TYPE2                  0x0E18
#define KEY_TERMINAL_TYPE3                  0x0E19
#define KEY_TERMINAL_TYPE4                  0x0E1A
#define KEY_TERMINAL_TYPE5                  0x0E1B
#define KEY_TERMINAL_TYPE6                  0x0E1C
#define KEY_TERMINAL_TYPE7                  0x0E1D
#define KEY_TERMINAL_TYPE8                  0x0E1E
#define KEY_TERMINAL_TYPE9                  0x0E1F
#define KEY_TERMINAL_TYPE10                 0x0E20

#define KEY_BAUD_RATE0                      0x0E21
#define KEY_BAUD_RATE1                      0x0E22
#define KEY_BAUD_RATE2                      0x0E23
#define KEY_BAUD_RATE3                      0x0E24
#define KEY_BAUD_RATE4                      0x0E25
#define KEY_BAUD_RATE5                      0x0E26
#define KEY_BAUD_RATE6                      0x0E27
#define KEY_BAUD_RATE7                      0x0E28
#define KEY_BAUD_RATE8                      0x0E29
#define KEY_BAUD_RATE9                      0x0E2A
#define KEY_BAUD_RATE10                     0x0E2B

#define KEY_DATA_BITS0                      0x0E2C
#define KEY_DATA_BITS1                      0x0E2D
#define KEY_DATA_BITS2                      0x0E2E
#define KEY_DATA_BITS3                      0x0E2F
#define KEY_DATA_BITS4                      0x0E30
#define KEY_DATA_BITS5                      0x0E31
#define KEY_DATA_BITS6                      0x0E32
#define KEY_DATA_BITS7                      0x0E33
#define KEY_DATA_BITS8                      0x0E34
#define KEY_DATA_BITS9                      0x0E35
#define KEY_DATA_BITS10                     0x0E36

#define KEY_PARITY0                         0x0E37
#define KEY_PARITY1                         0x0E38
#define KEY_PARITY2                         0x0E39
#define KEY_PARITY3                         0x0E3A
#define KEY_PARITY4                         0x0E3B
#define KEY_PARITY5                         0x0E3C
#define KEY_PARITY6                         0x0E3D
#define KEY_PARITY7                         0x0E3E
#define KEY_PARITY8                         0x0E3F
#define KEY_PARITY9                         0x0E40
#define KEY_PARITY10                        0x0E41

#define KEY_STOP_BITS0                      0x0E42
#define KEY_STOP_BITS1                      0x0E43
#define KEY_STOP_BITS2                      0x0E44
#define KEY_STOP_BITS3                      0x0E45
#define KEY_STOP_BITS4                      0x0E46
#define KEY_STOP_BITS5                      0x0E47
#define KEY_STOP_BITS6                      0x0E48
#define KEY_STOP_BITS7                      0x0E49
#define KEY_STOP_BITS8                      0x0E4A
#define KEY_STOP_BITS9                      0x0E4B
#define KEY_STOP_BITS10                     0x0E4C

#define KEY_FLOW_CONTROL0                   0x0E4D
#define KEY_FLOW_CONTROL1                   0x0E4E
#define KEY_FLOW_CONTROL2                   0x0E4F
#define KEY_FLOW_CONTROL3                   0x0E50
#define KEY_FLOW_CONTROL4                   0x0E51
#define KEY_FLOW_CONTROL5                   0x0E52
#define KEY_FLOW_CONTROL6                   0x0E53
#define KEY_FLOW_CONTROL7                   0x0E54
#define KEY_FLOW_CONTROL8                   0x0E55
#define KEY_FLOW_CONTROL9                   0x0E56
#define KEY_FLOW_CONTROL10                  0x0E57
//[-end-190612-IB07401110-add]//

#define KEY_SCAN_F9                         0xF0D2

//
// Console Redirection Text Mode Resolution (CRTextModeResolution)
//
#define CR_TEXT_MODE_AUTO                   0
#define CR_TEXT_MODE_80X25                  1
#define CR_TEXT_MODE_80X24DF                2
#define CR_TEXT_MODE_80X24DL                3
#define CR_TEXT_MODE_128X40                 4

#endif

