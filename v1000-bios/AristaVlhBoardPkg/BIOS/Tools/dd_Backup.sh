BASEDIR=$(dirname "$0")
echo $BASEDIR
dd if=$BASEDIR/$1.fd of=$BASEDIR/$1_Backup.FD bs=1024 count=8192 skip=8192
sleep 2
