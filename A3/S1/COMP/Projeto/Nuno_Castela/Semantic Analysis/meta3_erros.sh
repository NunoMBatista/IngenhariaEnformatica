# sort errors before diff, to make the order irrelevant
for i in meta3/*errors*.dgo; do
    echo "[$i]";
    ./gocompiler < "$i" | sort | diff "${i/%.dgo}.out" -;
done
