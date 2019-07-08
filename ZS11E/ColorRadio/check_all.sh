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

	echo '前方碰撞报警开关_req:' >> $filename
	cat $tempfile | ./canm 前方碰撞报警开关_req.cfg >> $filename
	echo '' >> $filename
	echo '前方碰撞报警开关_cmd:' >> $filename
	cat $tempfile | ./canm 前方碰撞报警开关_cmd.cfg >> $filename
	echo '' >> $filename

	echo '前向碰撞报警灵敏度_req:' >> $filename
	cat $tempfile | ./canm 前向碰撞报警灵敏度_req.cfg >> $filename
	echo '' >> $filename
	echo '前向碰撞报警灵敏度_cmd:' >> $filename
	cat $tempfile | ./canm 前向碰撞报警灵敏度_cmd.cfg >> $filename
	echo '' >> $filename

	echo '车道偏离警报开关_req:' >> $filename
	cat $tempfile | ./canm 车道偏离警报开关_req.cfg >> $filename
	echo '' >> $filename
	echo '车道偏离警报开关_cmd:' >> $filename
	cat $tempfile | ./canm 车道偏离警报开关_cmd.cfg >> $filename
	echo '' >> $filename

	echo '车道偏离警报灵敏度_req:' >> $filename
	cat $tempfile | ./canm 车道偏离警报灵敏度_req.cfg >> $filename
	echo '' >> $filename
	echo '车道偏离警报灵敏度_cmd:' >> $filename
	cat $tempfile | ./canm 车道偏离警报灵敏度_cmd.cfg >> $filename
	echo '' >> $filename

	echo '车道偏离警报提示音开关_req:' >> $filename
	cat $tempfile | ./canm 车道偏离警报提示音开关_req.cfg >> $filename
	echo '' >> $filename
	echo '车道偏离警报提示音开关_cmd:' >> $filename
	cat $tempfile | ./canm 车道偏离警报提示音开关_cmd.cfg >> $filename
	echo '' >> $filename

	echo '车道偏离警报提震动开关_req:' >> $filename
	cat $tempfile | ./canm 车道偏离警报提震动开关_req.cfg >> $filename
	echo '' >> $filename
	echo '车道偏离警报提震动开关_cmd:' >> $filename
	cat $tempfile | ./canm 车道偏离警报提震动开关_cmd.cfg >> $filename
	echo '' >> $filename

	echo '后向驾驶辅助总开关_req:' >> $filename
	cat $tempfile | ./canm 后向驾驶辅助总开关_req.cfg >> $filename
	echo '' >> $filename
	echo '后向驾驶辅助总开关_cmd:' >> $filename
	cat $tempfile | ./canm 后向驾驶辅助总开关_cmd.cfg >> $filename
	echo '' >> $filename

	echo '后视镜盲区监测开关_req:' >> $filename
	cat $tempfile | ./canm 后视镜盲区监测开关_req.cfg >> $filename
	echo '' >> $filename
	echo '后视镜盲区监测开关_cmd:' >> $filename
	cat $tempfile | ./canm 后视镜盲区监测开关_cmd.cfg >> $filename
	echo '' >> $filename

	echo '变道辅助开关_req:' >> $filename
	cat $tempfile | ./canm 变道辅助开关_req.cfg >> $filename
	echo '' >> $filename
	echo '变道辅助开关_cmd:' >> $filename
	cat $tempfile | ./canm 变道辅助开关_cmd.cfg >> $filename
	echo '' >> $filename

	echo '倒车后方盲区监测开关_req:' >> $filename
	cat $tempfile | ./canm 倒车后方盲区监测开关_req.cfg >> $filename
	echo '' >> $filename
	echo '倒车后方盲区监测开关_cmd:' >> $filename
	cat $tempfile | ./canm 倒车后方盲区监测开关_cmd.cfg >> $filename
	echo '' >> $filename

	echo '陡坡缓降控制开关_req:' >> $filename
	cat $tempfile | ./canm 陡坡缓降控制开关_req.cfg >> $filename
	echo '' >> $filename
	echo '陡坡缓降控制开关_cmd:' >> $filename
	cat $tempfile | ./canm 陡坡缓降控制开关_cmd.cfg >> $filename
	echo '' >> $filename

	echo '倒车雷达开关_req:' >> $filename
	cat $tempfile | ./canm 倒车雷达开关_req.cfg >> $filename
	echo '' >> $filename
	echo '倒车雷达开关_cmd:' >> $filename
	cat $tempfile | ./canm 倒车雷达开关_cmd.cfg >> $filename
	echo '' >> $filename

	echo '行人警示开关_req:' >> $filename
	cat $tempfile | ./canm 行人警示开关_req.cfg >> $filename
	echo '' >> $filename
	echo '行人警示开关_cmd:' >> $filename
	cat $tempfile | ./canm 行人警示开关_cmd.cfg >> $filename
	echo '' >> $filename

	echo 'SLIF_Warning_req:' >> $filename
	cat $tempfile | ./canm SLIF_Warning_req.cfg >> $filename
	echo '' >> $filename
	echo 'SLIF_Warning_cmd:' >> $filename
	cat $tempfile | ./canm SLIF_Warning_cmd.cfg >> $filename
	echo '' >> $filename

	echo 'Speed_Control_req:' >> $filename
	cat $tempfile | ./canm Speed_Control_req.cfg >> $filename
	echo '' >> $filename
	echo 'Speed_Control_cmd:' >> $filename
	cat $tempfile | ./canm Speed_Control_cmd.cfg >> $filename
	echo '' >> $filename

	echo 'MG_Pilot_req:' >> $filename
	cat $tempfile | ./canm MG_Pilot_req.cfg >> $filename
	echo '' >> $filename
	echo 'MG_Pilot_cmd:' >> $filename
	cat $tempfile | ./canm MG_Pilot_cmd.cfg >> $filename
	echo '' >> $filename

	shift
done
