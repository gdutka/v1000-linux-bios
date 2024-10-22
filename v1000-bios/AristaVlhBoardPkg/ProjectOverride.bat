::/*****************************************************************************
:: *
:: * Copyright 2008 - 2015 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
:: *
:: * AMD is granting you permission to use this software (the Materials)
:: * pursuant to the terms and conditions of your Software License Agreement
:: * with AMD.  This header does *NOT* give you permission to use the Materials
:: * or any rights under AMD's intellectual property.  Your use of any portion
:: * of these Materials shall constitute your acceptance of those terms and
:: * conditions.  If you do not agree to the terms and conditions of the Software
:: * License Agreement, please do not use any portion of these Materials.
:: *
:: * CONFIDENTIALITY:  The Materials and all other information, identified as
:: * confidential and provided to you by AMD shall be kept confidential in
:: * accordance with the terms and conditions of the Software License Agreement.
:: *
:: * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
:: * PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
:: * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
:: * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
:: * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
:: * IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
:: * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
:: * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
:: * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
:: * RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
:: * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
:: * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
:: * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
:: *
:: * AMD does not assume any responsibility for any errors which may appear in
:: * the Materials or any other related information provided to you by AMD, or
:: * result from use of the Materials or any related information.
:: *
:: * You agree that you will not reverse engineer or decompile the Materials.
:: *
:: * NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
:: * further information, software, technical information, know-how, or show-how
:: * available to you.  Additionally, AMD retains the right to modify the
:: * Materials at any time, without notice, and is not obligated to provide such
:: * modified Materials to you.
:: *
:: * U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
:: * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
:: * subject to the restrictions as set forth in FAR 52.227-14 and
:: * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
:: * Government constitutes acknowledgement of AMD's proprietary rights in them.
:: *
:: * EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
:: * direct product thereof will be exported directly or indirectly, into any
:: * country prohibited by the United States Export Administration Act and the
:: * regulations thereunder, without the required authorization from the U.S.
:: * government nor will be used for any purpose prohibited by the same.
:: ******************************************************************************
:: */
@REM
@REM Amd Override batch file
@REM This batch file will overrid every file within the
@REM <platform_directory>/Override folder.  It will match the override folder
@REM with the equivalent file in %WORKSPACE%.  If a matching original file is
@REM not found a message will be printed without causing an error.  The
@REM Override folder must have the same directory structure relative to the
@REM %WORKSPACE% folder.
@REM
@SETLOCAL ENABLEDELAYEDEXPANSION

:: 1. List the path of the file relative to %WORKSPACE%
:: 2. Make sure there are no spaces in the path listed
:: 3. Make sure each line ends with a carat, except for the last line.
@SET RELATIVE_FILE_LIST=


@SET OVERRIDE_DIR=%CD%\Override\
@SET BAKUPEXT=bak

@if /I "%1" == "PREBUILD" (
@PUSHD Override

  @FOR %%A IN (%RELATIVE_FILE_LIST%) DO @(
    @SET REL_FILE=%%A
    @SET ORIGINAL_FILE=%WORKSPACE%\!REL_FILE!
    @SET BACKUP_FILE=!ORIGINAL_FILE!.%BAKUPEXT%
    @IF EXIST !ORIGINAL_FILE! (
      ECHO.OVERRIDE:!ORIGINAL_FILE!
      ECHO.    WITH:%OVERRIDE_DIR%!REL_FILE!
      REM It is OK to fail ren. If it is alredy there, it is the backup.
      @ren !ORIGINAL_FILE! *.*.%BAKUPEXT% > NUL
      @copy /Y !REL_FILE! !ORIGINAL_FILE! > NUL || GOTO ERROR
    ) ELSE (
      @ECHO.NOT FOUND=%WORKSPACE%\!REL_FILE!
    )

  )
@POPD
)

@if /I "%1" == "POSTBUILD" (
@PUSHD Override

  @FOR %%A IN (%RELATIVE_FILE_LIST%) DO @(
    @SET REL_FILE=%%A
    @SET ORIGINAL_FILE=%WORKSPACE%\!REL_FILE!
    @SET BACKUP_FILE=!ORIGINAL_FILE!.%BAKUPEXT%
    @IF EXIST !BACKUP_FILE! (
      @ECHO.RESTORE=!ORIGINAL_FILE!
      @COPY /Y !BACKUP_FILE! !ORIGINAL_FILE! > NUL || GOTO ERROR
      @DEL !BACKUP_FILE! > NUL || GOTO ERROR
    ) ELSE (
      @ECHO.NOT FOUND=!BACKUP_FILE!
    )

  )
@POPD

)
@GOTO :EOF


:ERROR
  ECHO.ERROR
  EXIT /B 1
