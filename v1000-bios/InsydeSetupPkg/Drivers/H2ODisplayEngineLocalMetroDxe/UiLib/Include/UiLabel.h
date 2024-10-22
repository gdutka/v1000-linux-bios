/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _UI_LABEL_H
#define _UI_LABEL_H

typedef enum {
  UI_LABEL_DATE_TIME_FUNC_NONE = 0,
  UI_LABEL_DATE_TIME_FUNC_DATE,
  UI_LABEL_DATE_TIME_FUNC_WEEK,
  UI_LABEL_DATE_TIME_FUNC_TIME,
  UI_LABEL_DATE_TIME_FUNC_TIME_NO_SECOND,
  UI_LABEL_DATE_TIME_FUNC_MAX
} UI_LABEL_DATE_TIME_FUNC;

typedef struct _UI_LABEL UI_LABEL;
typedef struct _UI_LABEL_CLASS UI_LABEL_CLASS;

UI_LABEL_CLASS *
EFIAPI
GetLabelClass (
  VOID
  );

struct _UI_LABEL {
  UI_CONTROL                    Control;

  CHAR16                        *Text;
  UINT32                        FontSize;
  UINT32                        TextColor;
  UINT32                        TextStyle;
  BOOLEAN                       Ellipsis;
  BOOLEAN                       Marquee;
  UINT32                        MarqueeOffset;
  CHAR16                        *MarqueeText;
  BOOLEAN                       ShowHtmlText;
  UINT32                        FocusTextColor;
};

struct _UI_LABEL_CLASS {
  UI_CONTROL_CLASS              ParentClass;
};

#endif
