BASEDIR=$(dirname "$0")
TOP="${1}_Top"
echo $BASEDIR
dd if=$1 of=$TOP bs=1024 count=7868 skip=0
cat $TOP $2 $BASEDIR/crc32_empty.FD > $1
rm $TOP
sleep 2
