#!/bin/bash
function setBackup() {
    if [ "$1" = "NO" ]; then
        sed -i 's/BACKUP_BIOS                             = YES/BACKUP_BIOS                             = NO/g' Project.env
    else
        sed -i 's/BACKUP_BIOS                             = NO/BACKUP_BIOS                             = YES/g' Project.env
    fi
    sleep 3
}

setBackup YES
make clean uefi64

setBackup NO
make clean uefi64

./BIOS/Tools/Make16MFlash.sh