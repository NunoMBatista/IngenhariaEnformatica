cur=1
fact=1

while [ $cur -le $1 ]; do
	fact=$((cur * fact))
	cur=$((cur + 1))
done
echo $fact
