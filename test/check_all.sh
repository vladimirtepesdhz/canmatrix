#!/bin/bash

tempfile=temp.txt

while [ $# -ge 1 ]
do
	dir=$(dirname $1)
	suffix=$1
	suffix=${suffix##*.}
	filename=${dir}'/'$(basename -s '.'$suffix  $1).txt
	echo '' > $filename

	echo $1' -> '$filename

	suffix=$(echo $suffix | tr [A-Z] [a-z])
	if [ $suffix == "trc" ];then
		grep -vE '^;' $1 | awk '{printf $2"\t";printf $4"\t";for(i=7;i<=NF;++i){printf $i" "};print ""}' > $tempfile
	elif [ $suffix == "asc" ];then
		grep -E '^ ' $1 | awk '{if($2 == "1" && $3 != "Statistic:")print}' | awk '{printf $1"\t";printf $3"\t";for(i=7;i<=14;++i){printf $i" "};print ""}' > $tempfile
	else
		return 1
	fi


	echo 'engine info:' >> $filename
	cat $tempfile | ./canm engine.cfg >> $filename 
	echo '' >> $filename

	echo '0x0E3:' >> $filename
	cat $tempfile | ./canm 0x0E3.cfg >> $filename
	echo '' >> $filename

#	echo '0x2B1:' >> $filename
#	cat $tempfile | ./canm 0x2B1.cfg >> $filename
#	echo '' >> $filename
#
#	echo '0x2C8:' >> $filename
#	cat $tempfile | ./canm 0x2C8.cfg >> $filename
#	echo '' >> $filename
#
#	echo '0x2AD:' >> $filename
#	cat $tempfile | ./canm 0x2AD.cfg >> $filename
#	echo '' >> $filename

	echo 'remote info:' >> $filename
	cat $tempfile | ./canm remote.cfg >> $filename
	echo '' >> $filename

	echo '0x297:' >> $filename
	cat $tempfile | ./canm 0x297.cfg >> $filename
	echo '' >> $filename

	shift
done
