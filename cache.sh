#!/bin/bash
n=2
m=1
for i in `seq 1 11`;
do
        echo "zoom" $i
        for x in `seq 0 $m`
	do
        for y in `seq 0 $m`
	do
		echo "zoom " $i " x " $x " y " $y
                url="http://127.0.0.1:8081/"$i"/"$x"/"$y".svg"
                wget -q -O /dev/null $url  
	done
	done
        n=$(($n*2))
	m=$(($n - 1))
done
