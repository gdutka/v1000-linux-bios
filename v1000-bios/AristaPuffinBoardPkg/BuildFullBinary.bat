del .\BIOS\AristaVlh_Normal.FD
del .\BIOS\AristaVlh_Backup.FD

.\BIOS\Tools\setBackup.sh Project.env
nmake clean uefi64
.\BIOS\Tools\setNormal.sh Project.env
nmake clean uefi64
.\BIOS\Tools\Make16MFlash.bat