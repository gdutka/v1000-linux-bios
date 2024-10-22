/** @file
  This function optionally updates an image and/or its display coordinates and optionally
  displays the image.

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
  This function optionally updates an image and/or its display coordinates and optionally displays 
  the image.
  If the function displays the image OR the function wants to prevent the kernel from 
  displaying the image, this function should return EFI_SUCCESS.  
  If the function does not display the image, but updates the image data or image type, this 
  function should return EFI_MEDIA_CHANGED. The caller will free the original image data and the 
  returned image data.  
  If the function does not display the image, but updates the image coordinates, this function 
  should return EFI_MEDIA_CHANGED. The caller will free the original image data.  
  If the function does not wish to make any changes to the image data or image coordinates OR the 
  function is not implemented, it should return EFI_UNSUPPORTED.

  @param[in out]  Id                The logo identifier describes how the kernel plans to use the logo.
                                    On entry, points to an unsigned integer that specifies the logo identifier. 
                                    On exit, points to an updated unsigned integer.
                                    0 = Logo
                                    1 = CPU Badge
                                    2 = Chipset Badge
                                    255 = OEM
  @param[in out]  PosX              On entry, points to an integer that specifies the horizontal offset relative to the origin specified by OriginX. 
                                    On exit, points to an updated integer.
  @param[in out]  PosY              Points to an integer that specifies the vertical offset relative to the origin specified by OriginY. 
                                    On exit, points to an updated integer.
  @param[in out]  OriginX           On entry, points to an enumerated value that specifies 
                                    the horizontal origin. 
                                    On exit, points to an updated enumerated value.
                                    0 = Left edge is 0.
                                    1 = Left edge is the width of the screen, less the width of the 
                                    logo.
                                    2 = Left edge is the width of the screen, less the width of the 
                                    logo, divided by 2.
  @param[in out]  OriginY           On entry, points to an enumerated value that specifies the 
                                    vertical origin. 
                                    On exit, points to an updated enumerated value.
                                    0 = Top edge is 0.
                                    1 = Top edge is the height of the screen, less the height of the logo.
                                    2 = Top edge is the height of the screen, less the height of the logo, divided by 2.
  @param[in out]  ImageType         On entry, points to an enumerated value that specifies the format of the image data pointed to by ImageData.
                                    On exit, points to an updated enumerated value.
                                    0 = BMP
                                    1 = JPEG
                                    2 = GIF
                                    3 = PNG
  @param[in out]  ImageData         On entry, points to a pointer to a buffer that contains the raw image data. 
                                    On exit, points to a pointer to an updated buffer that contains the raw image data.
  @param[in out]  ImageSize         On entry, points to an unsigned integer that specifies the size of ImageData, in bytes. 
                                    On exit, points to an updated unsigned integer.
 
  @retval     EFI_UNSUPPORTED       The function is not implemented or the image data and attributes have not been updated. 
                                    The kernel should display the image at the specified coordinates and free the image specified by 
                                    ImageData.
  @retval     EFI_SUCCESS           Image has been successfully handled by this function. 
                                    The kernel should free the image specified by ImageData and should not display the image.
  @retval     EFI_MEDIA_CHANGED     Image data or attributes have been updated by this function. 
                                    The kernel should display the image at the specified coordinates and free the image passed in to the function, 
                                    as well as the image specified by ImageData.
  @retval     Others                Based on OEM design.
**/
EFI_STATUS
OemSvcUpdateLogo (
  IN OUT UINT8                                 *Id,
  IN OUT INT16                                 *PosX,
  IN OUT INT16                                 *PosY,
  IN OUT UINT8                                 *OriginX,
  IN OUT UINT8                                 *OriginY,
  IN OUT UINT8                                 *ImageType,
  IN OUT UINT8                                 **ImageData,
  IN OUT UINT32                                *ImageSize
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}
