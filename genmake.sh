list=files.list
dst=mk
temp=

if [ $# -eq 1 ] ; then
	list=$1
fi
if [ $# -eq 2 ] ; then
	dst=$2
fi

if [ ! -f ${list} ] ; then
	echo 'file:'${list}' not exist..'
	exit 1
fi

#将文件清空
echo '' > $dst

#生成参数
echo 'DEBUG = 0' >> $dst
echo 'CC = g++' >> $dst
echo 'TARGET = main.exe' >> $dst
echo 'ifeq ($(DEBUG),0)' >> $dst
echo 'DIR_BUILD = release' >> $dst
echo 'CFLAGS =  -Wall' >> $dst 
echo 'else' >> $dst
echo 'DIR_BUILD = debug' >> $dst
echo 'CFLAGS =  -Wall -g' >> $dst
echo 'endif' >> $dst
echo 'DIR_OBJ = $(DIR_BUILD)/obj' >> $dst
echo 'DIR_BIN = $(DIR_BUILD)/bin' >> $dst
echo 'DIR_IND = $(DIR_BUILD)/ind' >> $dst
echo 'BIN_TARGET = ${DIR_BIN}/${TARGET}' >> $dst
echo 'SRC := ' >> $dst
echo 'OBJ := ' >> $dst


#生成源文件和目标文件列表
for line in $(cat ${list})
do
	echo 'SRC += '$line >> $dst
	temp=$(basename $line | sed 's,\.c\(pp\)\?$,\.o,g')
	echo 'OBJ += ${DIR_OBJ}/'$temp >> $dst
done


#生成规则
echo '' >> $dst
echo 'all:|createdir ${BIN_TARGET}' >> $dst
echo '' >> $dst
echo 'createdir:' >> $dst
echo '	@if [ ! -d $(DIR_OBJ) ]; then\' >> $dst
echo '		mkdir -p $(DIR_OBJ) ;\' >> $dst
echo '	fi' >> $dst
echo '	@if [ ! -d $(DIR_BIN) ]; then\' >> $dst
echo '		mkdir -p $(DIR_BIN) ;\' >> $dst
echo '	fi' >> $dst
echo '	@if [ ! -d $(DIR_IND) ]; then\' >> $dst
echo '		mkdir -p $(DIR_IND) ;\' >> $dst
echo '	fi' >> $dst
echo '' >> $dst
echo '${BIN_TARGET}:${OBJ}' >> $dst
echo '	$(CC) ${OBJ} -o $@' >> $dst
echo '' >> $dst

for line in $(cat ${list})
do
	temp=$(gcc -MM $line | sed ':a;N;s/\\\n//g;ta' | sed 's/^/\${DIR_OBJ}\//g')
	echo $temp >> $dst
	echo '	$(CC) $(CFLAGS) -c '$line' -o $@' >> $dst
done

#清除
echo '' >> $dst
echo '.PHONY:clean' >> $dst
echo 'clean:' >> $dst
echo '	-del ${OBJ} ${BIN_TARGET}' >> $dst
echo '	-rm -rf ${OBJ} ${BIN_TARGET}' >> $dst
