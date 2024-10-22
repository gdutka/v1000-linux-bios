set PYTHON27_PATH=C:\Python27
set CXFREEZE_PATH=%PYTHON27_PATH%\Scripts
del *.dll /Q
del *.pyd /Q
del *.exe /Q
%PYTHON27_PATH%\python.exe %CXFREEZE_PATH%\cxfreeze BuildPspDirectory.py --target-dir=.