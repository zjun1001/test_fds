ps aux |grep echo
ps aux |grep demo
ps aux |grep teaupload
MYPID=`ps aux | grep echo | awk {'print $2'}`
if [ -z "$MYPID" ]
then
        echo "echo have not start"
        exit 1
fi
kill -9 $MYPID
#echo "已经杀死 $MYPID"

demPID=`ps aux | grep demo | awk {'print $2'}`
if [ -z "$demPID" ]
then
        echo "demo have not start"
        exit 1
fi
kill -9 $demPID
#echo "已经杀死 $demPID"

uploadPID=`ps aux | grep teaupload | awk {'print $2'}`
if [ -z "$uploadPID" ]
then
        echo "upload have not start"
        exit 1
fi
kill -9 $uploadPID
#echo "已经杀死 $MYPID"
