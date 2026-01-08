count=1
cat $1 | while read line
do
	echo $count: $line
done
