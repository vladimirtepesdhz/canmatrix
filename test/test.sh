#!/bin/bash

while [ $# -ge 1 ]
do
	filename=$(basename -s .trc $1).txt
	echo '' > $filename
	grep -vE '^;' $1 | awk '{printf $2"\t";printf $4"\t";for(i=7;i<=NF;++i){printf $i" "};print ""}' | canm 0x58.cfg 0x2B1.cfg >> $filename 

	shift
done
