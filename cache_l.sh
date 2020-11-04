#!/bin/bash
n=2
m=1
echo $1
for i in `seq 1 12`;
do
        echo "zoom" $i
        for x in `seq 0 $m`
	do
        for y in `seq 0 $m`
	do
    if [ $1 = $i ]
     	then
    	if [ $((y%3)) -eq 0 ]
		then
			echo "zoom " $1 " x " $x " y " $y
        	url="http://127.0.0.1:8081/"$i"/"$x"/"$y".svg"
        	wget  -q -O /dev/null $url 
		else
        	url="http://127.0.0.1:8081/"$1"/"$x"/"$y".svg"
			#echo "p zoom " $1 " x " $x " y " $y
        	wget  -q -O /dev/null $url & 
		fi
    fi
	done
	done
        n=$(($n*2))
	m=$(($n - 1))
done
