#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <time.h>

#include <string>
#include <vector>
#include <map>
using	namespace	std;

#include "bits.h"
#include "json/js_parser.h"

#define	numof(a)	(sizeof(a)/sizeof((a)[0]))

typedef	struct 	_StCanIdName
{
	u16 can_id;
	char const * id_name;
}StCanIdName;

typedef	struct 	_StCanSignal
{
	char const *	sign_name;
	u16	can_id;
	u8	start_bit;
	u8	bits_len;
	u8	min_v;
	u8	max_v;
}StCanSignal;

typedef struct 	_StCanFrame
{
	u16 can_id;
	u8	can_data[8];
}StCanFrame;

StCanIdName	g_can_id_table[] = 
{
	{0x058,		"TBOX_2"}
	,{0x2B1,	"TBOX_3"}
	,{0x2AC,	"BCM_2"}
	,{0x2A6,	"PKE_2"}
	,{0x2C8,	"AIC_1"}
	,{0x2AD,	"DATC_3"}
	,{0xE3,		"GW_18"}
};
StCanSignal	g_signal_table[] = 
{
	//TBOX_2(0x58)
	{"EncryptMessage_Tbox_2",		0x058,	0,8,	0,0xFF}
	//TBOX_3(0x2B1)
	,{"AutoCtrlReq_TBOX",			0x2B1,	8,2,	1,2	}
	,{"ExpectTempCtrlReq_FR_TBOX",	0x2B1,	11,5,	0x00,0x1C}
	,{"BlowLeveCtrllReq_TBOX",		0x2B1,	16,4,	2,8}
	,{"ACPowerCtrlReq_TBOX",		0x2B1,	20,2,	1,2}
	,{"RecirculationCtrlReq_TBOX",	0x2B1,	22,2,	1,2}
	,{"BlowModeCtrlReq_TBOX",		0x2B1,	24,3,	1,5}
	,{"CompressorCtrlReq_TBOX",		0x2B1,	32,2,	1,2}
	,{"SeatHeaCtrltReq_FR_TBOX",	0x2B1,	34,3,	1,2}
	,{"SeatHeatCtrlReq_FL_TBOX",	0x2B1,	37,3,	1,2}
	,{"RearWindowHeatCtrlReq_Tbox",	0x2B1,	42,2,	1,2}
	,{"EngineStartTimeSet",			0x2B1,	44,3,	1,7}
	//BCM_2(0x2AC)
	,{"IgnKeyPos_BCM",				0x2AC,	1,3,	1,3}
	,{"AntiTheftSt",				0x2AC,	41,2,	1,2}
	//PKE_2(0x2A6)
	,{"StartSource",				0x2A6,	32,3,	1,3}
	,{"DecryptResult",				0x2A6,	37,2,	1,2}
	//AIC_1(0x2C8)
	,{"DriverSeatHeatFeedback",		0x2C8,	16,3,	1,4}
	,{"PassengerSeatHeatFeedback",	0x2C8,	19,3,	1,4}
	//DATC_3(0x2AD)
	,{"PowerCtrl_FeedBack",			0x2AD,	0,3,	1,4}
	,{"AutoCtrl_FeedBack",			0x2AD,	3,3,	1,4}
	,{"ExpectTempCtrl_FeedBack_FR",	0x2AD,	6,2,	1,2}
	,{"BlowModeCtrl_FeedBack",		0x2AD,	8,3,	1,6}
	,{"BlowLevelCtrl_FeedBack",		0x2AD,	11,4,	1,8}
	,{"RecirculationCtrl_FeedBack",	0x2AD,	16,3,	1,4}
	,{"CompressorCtrl_FeedBack",	0x2AD,	19,3,	1,4}
	,{"RearWindowHeatCtrlFeedback",	0x2AD,	24,3,	1,4}
	//GW_18(0xE3)
	,{"EngineSt_EMS",				0xE3,	32,3,	1,7}
};
map<string,StCanSignal*>	g_signal_map;
map<u16,string>	g_can_id_map;
typedef	map<string,StCanSignal*>::iterator TySignal;
typedef map<u16,string>::iterator	TyCanId;
vector<StCanSignal*>	g_signal_req;

bool_t	CanMatrixInit()
{
	int iter=0;
	for(iter=0;iter<numof(g_can_id_table);++iter)
	{
		g_can_id_map.insert(pair<u16,string>(g_can_id_table[iter].can_id,g_can_id_table[iter].id_name));
	}
	for(iter=0;iter<numof(g_signal_table);++iter)
	{
		g_signal_map.insert(pair<string,StCanSignal*>(g_signal_table[iter].sign_name,&g_signal_table[iter]));
	}
	return	t_true;
}

#define	INPUT_BUFFER_MAX	4096

int	main(int argc,char * argv[])
{
#if 0
	int iter=0;
	int len = 0;
	char buffer[INPUT_BUFFER_MAX];

	CanMatrixInit();

	for(iter=1;iter<argc;++iter)
	{
		FILE * file = NULL;
		file = fopen(argv[iter],"r");
		if(file)
		{
			//while(fgets(buffer,sizeof(buffer),file)
			while(0 == feof(file))
			{
				buffer[0] = 0;
#if 0
				fgets(buffer,sizeof(buffer),file);
				if(0 == buffer[0] || '\n' == buffer[0])
					continue;
				len = strlen(buffer);
				if(len >= 2)
				{
					if('\r' == buffer[len - 2] || '\n' == buffer[len - 2])
						buffer[len - 2] = 0;
				}
				if(len >= 1)
				{
					if('\r' == buffer[len - 1] || '\n' == buffer[len - 1])
						buffer[len - 1] = 0;
				}
#else
				fscanf(file,"%s",buffer);
				if(0 == buffer[0] || '\n' == buffer[0])
					continue;
				//fprintf(stdout,"%s\n",buffer);
#endif
				TySignal i = g_signal_map.find(buffer);
				if(g_signal_map.end() != i)
				{
					g_signal_req.push_back(i->second);
				}
			}
			fclose(file);
		}
	}

	while(0 == feof(stdin))
	{
		vector<char const *>	str_split;
		char * p_str = buffer;
		StCanFrame frame;
		int size = 0;
		bool_t print_frame = t_false;
		string str_line;

		//读入行数据
		buffer[0] = 0;
		fgets(buffer,sizeof(buffer),stdin);
		if(0 == buffer[0]||'\n' == buffer[0])
			continue;
		str_line = buffer;

		//分解行数据
		p_str = strtok(p_str," \t");
		while(NULL != p_str)
		{
			str_split.push_back(p_str);
			p_str = strtok(NULL," \t\r");
		}

		//解析行数据  由时间戳，CAN id ,8字节数据组成
		memset(&frame,0,sizeof(frame));
		if(str_split.size() >=2 )
		{
			frame.can_id = strtoul(str_split[1],NULL,16);
			if(str_split.size() < 8+2)
				size = str_split.size() - 2;
			else
				size = 8;
			for(int iter=0;iter<size;++iter)
			{
				frame.can_data[iter] = strtoul(str_split[iter+2],NULL,16);
			}
		}

		//解析报文数据
		if(0 != frame.can_id)
		{
			for(int iter=0;iter<g_signal_req.size();++iter)
			{
				StCanSignal * p = g_signal_req[iter];
				if(p->can_id == frame.can_id)
				{
					u8 value = 0;

					value = GetBits_LE32(frame.can_data,sizeof(frame.can_data),p->start_bit,p->bits_len);
					if(value >= p->min_v && value <= p->max_v)
					{
						if(t_false == print_frame)
						{
							TyCanId i;

							print_frame = t_true;
							i = g_can_id_map.find(frame.can_id);
							if(g_can_id_map.end() != i)
							{
								fprintf(stdout," %s\t",i->second.c_str());
								fprintf(stdout,"%s",str_line.c_str());
							}
						}
						fprintf(stdout,"\t%s = 0x%x\n",p->sign_name,(int)value);
					}
				}
			}
		}
	}

#endif
	if(argc > 1)
	{
		CJsInput input;
		CJsToken token;

		if(!input.Init(argv[1]))
		{
			fprintf(stdout,"无法打开文件%s\n",argv[1]);
			return	1;
		}
		token.Init(&input);
		while(token.ParseNext())
		{
			fprintf(stdout,"line:%d\t",token.GetLineNum());
			switch(token.GetType())
			{
				case	CJsToken::JS_TOKEN_NULL:
				{
					fprintf(stdout,"JS_TOKEN_NULL\n");
				}
				break;

				case	CJsToken::JS_TOKEN_STRBLK:
				{
					fprintf(stdout,"JS_TOKEN_STRBLK %s\n",token.GetStr().c_str());
				}
				break;

				case	CJsToken::JS_TOKEN_COLON:
				{
					fprintf(stdout,"JS_TOKEN_COLON :\n");
				}
				break;

				case	CJsToken::JS_TOKEN_LEFT_BRACE:
				{
					fprintf(stdout,"JS_TOKEN_LEFT_BRACE {\n");
				}
				break;

				case	CJsToken::JS_TOKEN_RIGHT_BRACE:
				{
					fprintf(stdout,"JS_TOKEN_RIGHT_BRACE }\n");
				}
				break;

				case	CJsToken::JS_TOKEN_LEFT_BRACKET:
				{
					fprintf(stdout,"JS_TOKEN_LEFT_BRACKET [\n");
				}
				break;

				case	CJsToken::JS_TOKEN_RIGHT_BRACKET:
				{
					fprintf(stdout,"JS_TOKEN_RIGHT_BRACKET ]\n");
				}
				break;

				case	CJsToken::JS_TOKEN_COMMA:
				{
					fprintf(stdout,"JS_TOKEN_COMMA ,\n");
				}
				break;

				default:
				{
					fprintf(stdout,"JS_TOKEN_ERROR\n");
				}
			}
		}
	}
	return	0;
}
