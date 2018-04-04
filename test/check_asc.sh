#!/bin/bash

while [ $# -ge 1 ]
do
	filename=$(basename -s .asc $1).txt
	echo '' > $filename
	grep -E '^ ' $1 | awk '{if($2 == "1" && $3 != "Statistic:")print}' | awk '{printf $1"\t";printf $3"\t";for(i=7;i<=14;++i){printf $i" "};print ""}' | canm custom.cfg >> $filename 

	shift
done
