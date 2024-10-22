BASEDIR=$(dirname "$0")
echo $BASEDIR
dd if=$BASEDIR/$1.fd of=$BASEDIR/$1_Normal.FD bs=1024 count=8192 skip=0
sleep 2