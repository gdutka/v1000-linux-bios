:: *****************************************************************************
:: *
:: * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
:: *
:: ******************************************************************************

set CXFREEZE_PATH=C:\Python27\Scripts
del *.dll /Q
del *.pyd /Q
del *.exe /Q
%CXFREEZE_PATH%\cxfreeze.bat CheckAblVersion.py  --target-dir=.