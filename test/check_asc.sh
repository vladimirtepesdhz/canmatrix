#!/bin/bash

tempfile=temp.txt

while [ $# -ge 1 ]
do
	filename=$(basename -s .asc $1).txt
	echo '' > $filename

	echo $1' -> '$filename

	grep -E '^ ' $1 | awk '{if($2 == "1" && $3 != "Statistic:")print}' | awk '{printf $1"\t";printf $3"\t";for(i=7;i<=14;++i){printf $i" "};print ""}' > temp.txt

	echo 'engine info:' >> $filename
	cat $tempfile | ./canm engine.cfg >> $filename 
	echo '' >> $filename

	echo '0x2B1:' >> $filename
	cat $tempfile | ./canm 0x2B1.cfg >> $filename
	echo '' >> $filename

	echo '0x2C8:' >> $filename
	cat $tempfile | ./canm 0x2C8.cfg >> $filename
	echo '' >> $filename

	echo '0x2AD:' >> $filename
	cat $tempfile | ./canm 0x2AD.cfg >> $filename
	echo '' >> $filename

	shift
done
