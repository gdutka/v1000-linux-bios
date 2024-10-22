/** @file
  This function returns the Nth logo associated with the specified ID. 
  Logos of a specific Id are numbered from 0 to N, with no gaps.

;******************************************************************************
;* Copyright (c) 2016, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/DxeOemSvcKernelLib.h>


/**
  This function returns the Nth logo associated with the specified ID. 
  Logos of a specific Id are numbered from 0 to N, with no gaps.
  
  If the function displayed the image, this function should return EFI_SUCCESS.
  If the function provided image data, this function should return EFI_MEDIA_CHANGED.
  If the function cannot return any more images, this function should return EFI_UNSUPPORTED.

  @param[in]  Id                    Enumerated value that specifies the type of logo desired.
                                    0 = Logo
                                    1 = CPU Badge
                                    2 = Chipset Badge
                                    255 = OEM
  @param[in]  Index                 Unsigned integer that specifies the logo desired. 
                                    Logos of each type are specified using values from 0, 1, 2 and 
                                    so on, with no gaps, until the final logo.
  @param[out] PosX                  On exit, points to an integer that specifies the 
                                    horizontal offset relative to the origin specified by OriginX.                                  
  @param[out] PosY                  On exit, points to an integer that specifies the 
                                    vertical offset relative to the origin specified by OriginY.
  @param[out] OriginX               On exit, points to an enumerated value that specifies the 
                                    horizontal origin.
                                    0 = Left edge is 0.
                                    1 = Left edge is the width of the screen, less the width of the logo.
                                    2 = Left edge is the width of the screen, less the width of the logo, 
                                        divided by 2.
  @param[out] OriginY               On exit, points to an enumerated value that specifies the 
                                    vertical origin.
                                    0 = Top edge is 0.
                                    1 = Top edge is the height of the screen, less the height of the logo.
                                    2 = Top edge is the height of the screen, less the height of the logo, 
                                        divided by 2.
  @param[out] ImageType             On exit, points to an enumerated value that specifies the format 
                                    of the image data pointed to by ImageData.
                                    0 = BMP
                                    1 = JPEG
                                    2 = GIF
                                    3 = PNG
  @param[out] ImageData             On exit, points to a pointer to a buffer that contains the raw 
                                    image data.
  @param[out] ImageSize             On exit, points to an unsigned integer that specifies the size 
                                    of ImageData, in bytes.
 
  @retval     EFI_UNSUPPORTED       The function is not implemented or the image data and attributes
                                    have not been updated. The kernel should display the image at the
                                    specified coordinates and free the image specified by ImageData.
  @retval     EFI_SUCCESS           Image has been successfully handled by this function. The kernel 
                                    should free the image specified by ImageData and should not 
                                    display the image.
  @retval     EFI_MEDIA_CHANGED     Image data has been provided by this function. The kernel should 
                                    display the image specified by ImageData.
  @retval     Others                Based on OEM design.
**/
EFI_STATUS
OemSvcGetLogo (
  IN     UINT8                                 Id,
  IN     UINT32                                Index,
  OUT    INT16                                 *PosX,
  OUT    INT16                                 *PosY,
  OUT    UINT8                                 *OriginX,
  OUT    UINT8                                 *OriginY,
  OUT    UINT8                                 *ImageType,
  OUT    UINT8                                 **ImageData,
  OUT    UINT32                                *ImageSize
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}
