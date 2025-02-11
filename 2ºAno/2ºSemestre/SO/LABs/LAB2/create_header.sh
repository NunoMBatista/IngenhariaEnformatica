# $1 -> File name
# $2 -> Project name
# $3 -> User name

touch $1.c
echo // $3 > $1.c
echo // $2 >> $1.c
echo // ---------------------- >> $1.c
