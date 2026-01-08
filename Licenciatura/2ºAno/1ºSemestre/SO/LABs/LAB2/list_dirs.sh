#!/bin/bash
counter=0

# Iterate over each entry in the ls output
for dir in $(ls -a -F); do
    if [ -d $dir ]; then
        ((counter++))
        echo "Directory $count: ${dir%/}"
    fi
done
echo "Total directories = $counter"
