/** @file

;******************************************************************************
;* Copyright (c) 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#ifndef _PROPERTY_DEF_H_
#define _PROPERTY_DEF_H_

#define DISPLAY_ENGINE_LOCAL_TEXT_GUID   { 0x7c808617, 0x7bc1, 0x4745, {0xa4, 0x58, 0x09, 0x28, 0xf8, 0xb9, 0x5e, 0x60} }
#define DISPLAY_ENGINE_LOCAL_METRO_GUID  { 0xb9e329a2, 0xaba7, 0x4f41, {0x93, 0x98, 0x46, 0xde, 0xc0, 0xae, 0xc1, 0xf7} }

#define PANEL_PROVIDER_FORMSET_GUID            {0x63da76b8, 0xc99f, 0x4651, {0xba, 0x0d, 0xf3, 0xdb, 0x82, 0xcf, 0xfc, 0xe1}}
#define PANEL_PROVIDER_FORM_ID_OWNER_DRAW      0x0002
#define PANEL_PROVIDER_FORM_ID_OWNER_DRAW_TEST 0x0003
#define PANEL_PROVIDER_FORM_ID_SAMPLE_OP_CODES 0x0004

#define COMMON_PAGE_LAYOUT_ID                  0x00000001
#define SCU_LAYOUT_ID                          0x00000002
#define FRONT_PAGE_LAYOUT_ID                   0x00000004
#define BOOT_MANAGER_LAYOUT_ID                 0x00000008
#define BOOT_FROM_FILE_LAYOUT_ID               0x00000010
#define SCU_SIMPLE_LAYOUT_ID                   0x00000020

#define SCREEN_PANEL_ID                        0x0001
#define OWNER_DRAW_PANEL_ID                    0x0002
#define TITLE_PANEL_ID                         0x0003
#define SETUP_MENU_PANEL_ID                    0x0004
#define SETUP_PAGE_PANEL_ID                    0x0005
#define HELP_TEXT_PANEL_ID                     0x0006
#define HOTKEY_PANEL_ID                        0x0007
#define QUESTION_PANEL_ID                      0x0008
#define SETUP_PAGE_2_PANEL_ID                  0x0009

#define H2O_PANEL_TYPE_SCREEN          0x0000
#define H2O_PANEL_TYPE_HOTKEY          0x0001
#define H2O_PANEL_TYPE_SETUP_MENU      0x0003
#define H2O_PANEL_TYPE_SETUP_PAGE      0x0004
#define H2O_PANEL_TYPE_OWNER_DRAW      0x0007
#define H2O_PANEL_TYPE_FORM            0x0008
#define H2O_PANEL_TYPE_DISPLAY_ELEMENT 0x0009
#define H2O_PANEL_TYPE_GUID            0xffff

//#define H2O_CONTROL_MAIN_PAGE_ONE_OF_GUID \
//{ \
//  0xd197cfcd, 0x825f, 0x4182, 0xa6, 0x8a, 0xda, 0x7f, 0x0a, 0x1a, 0x17, 0xa3 \
//}

#define H2O_HOT_KEY_ACTION_SHOW_HELP              0x00000001
#define H2O_HOT_KEY_ACTION_SELECT_PREVIOUS_ITEM   0x00000002
#define H2O_HOT_KEY_ACTION_SELECT_NEXT_ITEM       0x00000003
#define H2O_HOT_KEY_ACTION_SELECT_PREVIOUS_MENU   0x00000004
#define H2O_HOT_KEY_ACTION_SELECT_NEXT_MENU       0x00000005
#define H2O_HOT_KEY_ACTION_MODIFY_PREVIOUS_VALUE  0x00000006
#define H2O_HOT_KEY_ACTION_MODIFY_NEXT_VALUE      0x00000007
#define H2O_HOT_KEY_ACTION_DISCARD                0x00000008
#define H2O_HOT_KEY_ACTION_DISCARD_AND_EXIT       0x00000009
#define H2O_HOT_KEY_ACTION_LOAD_DEFAULT           0x0000000A
#define H2O_HOT_KEY_ACTION_SAVE                   0x0000000B
#define H2O_HOT_KEY_ACTION_SAVE_AND_EXIT          0x0000000C
#define H2O_HOT_KEY_ACTION_ENTER                  0x0000000D
#define H2O_HOT_KEY_ACTION_CALLBACK               0x0000000E
#define H2O_HOT_KEY_ACTION_GOTO                   0x0000000F
#define H2O_HOT_KEY_ACTION_SET_QUESTION_VALUE     0x00000010

#define H2O_HOT_KEY_ACTION_SCOPE_CURRENT          0x00000000
#define H2O_HOT_KEY_ACTION_SCOPE_ALL              0x00000001
#define H2O_HOT_KEY_ACTION_SCOPE_FORMSET          0x00000002
#define H2O_HOT_KEY_ACTION_SCOPE_FORM             0x00000003
#define H2O_HOT_KEY_ACTION_SCOPE_QUESTION         0x00000004

//
// Owner Draw
//
#define H2O_ITEM_GROUP_OD_LOGO                      0x00000001
#define H2O_ITEM_GROUP_OD_DATE_TIME                 0x00000002
#define H2O_ITEM_GROUP_OD_MAIN_BOARD_TEMPERATURE    0x00000003
#define H2O_ITEM_GROUP_OD_CPU_TEMPERATURE           0x00000004

#define H2O_ITEM_ACTION_OD_LOGO                     0x00000001
#define H2O_ITEM_ACTION_OD_SYSTEM_INFO              0x00000002
#define H2O_ITEM_ACTION_OD_DATE_TIME_DATE           0x00000003
#define H2O_ITEM_ACTION_OD_DATE_TIME_WEEK           0x00000004
#define H2O_ITEM_ACTION_OD_DATE_TIME_TIME           0x00000005
#define H2O_ITEM_ACTION_OD_MAIN_BOARD_TEMPERATURE   0x00000006
#define H2O_ITEM_ACTION_OD_CPU_TEMPERATURE          0x00000007

#endif

