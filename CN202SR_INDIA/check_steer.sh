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
		version=$(cat $1 | grep -iE 'FILEVERSION')
		if [ $version == ';$FILEVERSION=2.0' ];then
			grep -vE '^;' $1 | awk '{printf $2"\t";printf $4"\t";for(i=7;i<=NF;++i){printf $i" "};print ""}' > $tempfile
		elif [ $version == ';$FILEVERSION=1.3' ];then
			grep -vE '^;' $1 | awk '{printf $2"\t";printf $5"\t";for(i=8;i<=NF;++i){printf $i" "};print ""}' > $tempfile
		elif [ $version == ';$FILEVERSION=1.1' ];then
			grep -vE '^;' $1 | awk '{printf $2"\t";printf $4"\t";for(i=6;i<=NF;++i){printf $i" "};print ""}' > $tempfile
		fi
	elif [ $suffix == "asc" ];then
		cat $1 | awk '{if($1~/^[0-9]/&&$2~/^[0-9]/&&$3~/^[0-9a-fA-F][0-9a-fA-F]*/)print}' | awk '{printf $1"\t";printf $3"\t";for(i=7;i<=14;++i){printf $i" "};print ""}' > $tempfile
	else
		continue
	fi

	echo 'steer:' >> $filename
	cat $tempfile | ./canm 0x115.cfg >> $filename
	echo '' >> $filename

	shift
done
