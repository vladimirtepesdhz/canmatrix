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
#include "utils.h"
#include "json/js_parser.h"

#define	numof(a)	(sizeof(a)/sizeof((a)[0]))

#define	INPUT_BUFFER_MAX	4096

typedef	enum	_EnEndiaType
{
	EndiaType_Intel
	,EndiaType_Motorola
}EnEndiaType;

typedef	struct 	_StCanIdName
{
	int can_id;
	char const * id_name;
}StCanIdName;

typedef	struct 	_StCanSignal
{
	char const *	sign_name;
	int	can_id;
	u8	start_bit;
	u8	bits_len;
	u8	min_v;
	u8	max_v;
}StCanSignal;

typedef struct 	_StCanFrame
{
	float timestamp;
	int	can_id;
	u8	can_data[8];
}StCanFrame;

typedef struct 	_StFrameStatic
{
	float start_time;
	float end_time;
	int frame_cnt;
}StFrameStatic;

class	CCanFrameInfo
{
public:
	int can_id;
	string	name;
	string	enc_type;
	CCanFrameInfo()
	{
		can_id = 0;
	}
	CCanFrameInfo(int id,char const * n,char const * e)
	{
		can_id = id;
		name = n;
		enc_type = e;
	}
};

class	CCanValName
{
public:
	int value;
	string name;

	CCanValName()
	{
		value = 0;
	}
	CCanValName(int v,char const * n)
	{
		value = v;
		name = n;
	}
};

class	CCanSignal
{
public:
	string	sign_name;
	int can_id;
	int start_bit;
	int bits_len;
	int min_v;
	int max_v;
	vector<CCanValName>	val_name;
	
	CCanSignal()
	{
		can_id = 0;
		start_bit = 0;
		bits_len = 0;
		min_v = 0;
		max_v = 0;
	}
};

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

EnEndiaType	g_endia_type = EndiaType_Intel;

unsigned char	g_sk3[8] = {0};

vector<CCanFrameInfo>	g_frame_info;
vector<CCanSignal>	g_signal_data;

map<int,CCanFrameInfo*>	g_can_frame_map;
map<string,CCanSignal*>	g_signal_map;

typedef	map<string,CCanSignal*>::iterator TySignal;
typedef map<int,CCanFrameInfo*>::iterator	TyCanFrame;

vector<CCanSignal*>	g_signal_req;

u32	GetBits(u8 const * p_data,int data_size,int bits_start,int bits_len)
{
	if(EndiaType_Motorola == g_endia_type)
	{
		return	GetBits_M(p_data,data_size,bits_start,bits_len);
	}
	else
	{
		return	GetBits_I(p_data,data_size,bits_start,bits_len);
	}
}

void	SetBits(u8 * p_data,int data_size,int bits_start,int bits_len,u32 value)
{
	if(EndiaType_Motorola == g_endia_type)
	{
		SetBits_M(p_data,data_size,bits_start,bits_len,value);
	}
	else
	{
		SetBits_I(p_data,data_size,bits_start,bits_len,value);
	}
}

void	SK3_enc(unsigned char * p_data_in,unsigned char * p_sk3,unsigned char * p_data_out);
void	SK3_dec(unsigned char * p_data_in,unsigned char * p_sk3,unsigned char * p_data_out);

bool	CanMatrixInit()
{
	int iter=0;
#if 0
	for(iter=0;iter<numof(g_can_id_table);++iter)
	{
		g_can_id_map.insert(pair<u16,string>(g_can_id_table[iter].can_id,g_can_id_table[iter].id_name));
	}
	for(iter=0;iter<numof(g_signal_table);++iter)
	{
		g_signal_map.insert(pair<string,StCanSignal*>(g_signal_table[iter].sign_name,&g_signal_table[iter]));
	}
#endif

#if 0
	for(iter=0;iter<numof(g_can_id_table);++iter)
	{
		g_can_id_map.insert(pair<int,string>(g_can_id_table[iter].can_id,g_can_id_table[iter].id_name));
	}
	for(iter=0;iter<numof(g_signal_table);++iter)
	{
		CCanSignal * p = NULL;
		g_signal_data.push_back(CCanSignal());
		p = &g_signal_data.back();
		p->sign_name = g_signal_table[iter].sign_name;
		p->can_id = g_signal_table[iter].can_id;
		p->start_bit = g_signal_table[iter].start_bit;
		p->bits_len = g_signal_table[iter].bits_len;
		p->min_v = g_signal_table[iter].min_v;
		p->max_v = g_signal_table[iter].max_v;
	}
	for(iter=0;iter<numof(g_signal_table);++iter)
	{
		g_signal_map.insert(pair<string,CCanSignal*>(g_signal_table[iter].sign_name,&g_signal_data[iter]));
	}
#endif

	CJsParser jp;

	if(jp.Init("sk3.json"))
	{
		if(jp.FindPath("sk3"))
		{
			if(jp.EnterArray())
			{
				for(iter=0;iter<sizeof(g_sk3);++iter)
				{
					if(!jp.ParseNext())
						break;
					g_sk3[iter] = jp.GetValInt();
				}
			}
		}
		else
			fprintf(stdout,"cannot find var sk3\n");
		jp.Release();
	}
	else
		fprintf(stdout,"cannot find sk3.json\n");
	
	if(!jp.Init("default.json"))
	{
		fprintf(stdout,"cannot load default.json\n");
		return	1;
	}
	
	if(jp.FindPath("endia_type"))
	{
		if(CJsParser::JS_PARSE_SINGLE == jp.GetParseType())
		{
			if("intel" == jp.GetValue())
			{
				g_endia_type = EndiaType_Intel;
			}
			else if("motorola" == jp.GetValue())
			{
				g_endia_type = EndiaType_Motorola;
			}
		}
	}
	
	if(jp.FindPath("CAN_frame"))
	{
		if(jp.EnterArray())
		{
			while(jp.ParseNext())
			{
				if(jp.EnterObj())
				{
					int can_id = 0;
					CCanFrameInfo * p = NULL;
					string name;

					g_frame_info.push_back(CCanFrameInfo());
					p = &g_frame_info.back();

					if(jp.FindVar("can_id"))
						p->can_id = jp.GetValInt();
					if(jp.FindVar("name"))
						p->name = jp.GetValue();
					if(jp.FindVar("enc_type"))
						p->enc_type = jp.GetValue();

					jp.ParseUpper();
				}
			}
		}
	}
	else
	{
		fprintf(stdout,"cannot find var : CAN_frame\n");
	}
	for(iter=0;iter<g_frame_info.size();++iter)
	{
		g_can_frame_map.insert(pair<int,CCanFrameInfo*>(g_frame_info[iter].can_id,&g_frame_info[iter]));
	}
	if(jp.FindPath("signals"))
	{
		if(jp.EnterArray())
		{
			while(jp.ParseNext())
			{
				if(jp.EnterObj())
				{
					CCanSignal * p = NULL;
					g_signal_data.push_back(CCanSignal());
					p = &g_signal_data.back();

					if(jp.FindVar("name"))
						p->sign_name = jp.GetValue();
					if(jp.FindVar("can_id"))
						p->can_id = jp.GetValInt();
					if(jp.FindVar("start_bit"))
						p->start_bit = jp.GetValInt();
					if(jp.FindVar("bits_len"))
						p->bits_len = jp.GetValInt();
					if(jp.FindVar("min_v"))
						p->min_v = jp.GetValInt();
					if(jp.FindVar("max_v"))
						p->max_v = jp.GetValInt();
					if(jp.FindVar("val_name"))
					{
						if(jp.EnterArray())
						{
							while(jp.ParseNext())
							{
								if(jp.EnterObj())
								{
									int value = 0;
									string name;

									if(jp.FindVar("value"))
										value = jp.GetValInt();
									if(jp.FindVar("name"))
										name = jp.GetValue();
									p->val_name.push_back(CCanValName(value,name.c_str()));

									jp.ParseUpper();
								}
							}
							jp.ParseUpper();
						}
					}
					jp.ParseUpper();
				}
			}
		}
	}
	else
	{
		fprintf(stdout,"cannot find var : signals\n");
	}
	for(iter=0;iter<g_signal_data.size();++iter)
	{
		g_signal_map.insert(pair<string,CCanSignal*>(g_signal_data[iter].sign_name,&g_signal_data[iter]));
	}

#if 0
	fprintf(stdout,"g_can_id_map:\n");
	for(TyCanId I=g_can_id_map.begin();I != g_can_id_map.end(); ++I)
	{
		fprintf(stdout,"can_id == 0x%x name == %s\n",I->first,I->second.c_str());
	}
	fprintf(stdout,"g_signal_data:\n");
	for(iter=0;iter<g_signal_data.size();++iter)
	{
		CCanSignal * p = &g_signal_data[iter];
		fprintf(stdout,"[%d]:\n{\n",iter);
		fprintf(stdout,"sign_name == %s\n",p->sign_name.c_str());
		fprintf(stdout,"can_id == 0x%x\n",p->can_id);
		fprintf(stdout,"start_bit == %d\n",p->start_bit);
		fprintf(stdout,"bits_len == %d\n",p->bits_len);
		fprintf(stdout,"min_v == %d\n",p->min_v);
		fprintf(stdout,"max_v == %d\n",p->max_v);
		if(p->val_name.size())
		{
			for(int j=0;j<p->val_name.size();++j)
			{
				CCanValName * pval = &(p->val_name[j]);
				fprintf(stdout,"\tvalue == %d name == %s\n",pval->value,pval->name.c_str());
			}
		}
		fprintf(stdout,"}\n");
	}
#endif
	return	true;
}

bool	CanMatrixLoadCfg(char const * file_name)
{
#if 0
	char buffer[INPUT_BUFFER_MAX];
	FILE * file = NULL;
	file = fopen(file_name,"r");
	if(file)
	{
		//while(fgets(buffer,sizeof(buffer),file)
		while(0 == feof(file))
		{
			buffer[0] = 0;
			fscanf(file,"%s",buffer);
			if(0 == buffer[0] || '\n' == buffer[0])
				continue;
			//fprintf(stdout,"%s\n",buffer);
			TySignal i = g_signal_map.find(buffer);
			if(g_signal_map.end() != i)
			{
				g_signal_req.push_back(i->second);
			}
		}
		fclose(file);
		return	true;
	}
	else
		return	false;
#endif
	CJsParser jp;
	if(jp.Init(file_name))
	{
		if(jp.FindPath("request"))
		{
			if(jp.EnterArray())
			{
				while(jp.ParseNext())
				{
					string sig_name = jp.GetValue();
					TySignal i = g_signal_map.find(sig_name);
					if(g_signal_map.end() != i)
					{
						g_signal_req.push_back(i->second);
					}
				}
				jp.ParseUpper();
			}
		}
	}
}

bool	CanMatrixPrintSignal(StCanFrame * p_frame,StFrameStatic * p_static,vector<int>* p_modify)
{
	bool first_print = false;
	bool valid_signal = false;
	unsigned char	dec[8];
	for(int iter=0;iter<g_signal_req.size();++iter)
	{
		CCanSignal * p = g_signal_req[iter];
		CCanFrameInfo * p_fi = NULL;
		TyCanFrame I = g_can_frame_map.find(p->can_id);
		if(g_can_frame_map.end() != I)
			p_fi = I->second;
		if(p->can_id == p_frame->can_id)
		{
			int value = 0;

			memcpy(dec,p_frame->can_data,sizeof(dec));
			if(p_fi->enc_type.length())
			{
				if("TBOX_2" == p_fi->enc_type)
				{
					SK3_dec(p_frame->can_data,g_sk3,dec);
				}
			}
			//value = GetBits_LE32(dec,sizeof(dec),p->start_bit,p->bits_len);
			value = GetBits(dec,sizeof(dec),p->start_bit,p->bits_len);
			if(value >= p->min_v && value <= p->max_v)
			{
				valid_signal = true;
				if(false == first_print)
				{
					first_print = true;

					fprintf(stdout," <from:%.3f\tto:%.3f\tcount:%d>\t",
						p_static->start_time,p_static->end_time,p_static->frame_cnt);
					if(p_fi)
						fprintf(stdout,"%s\t",p_fi->name.c_str());
					fprintf(stdout,"%X\t[",p_frame->can_id);
					//fprintf(stdout,"%s",str_line.c_str());
					for(int j=0;j<sizeof(p_frame->can_data);++j)
					{
						fprintf(stdout,"%02.2X ",(int)(p_frame->can_data[j]));
					}
					fprintf(stdout,"]\n");
					if(p_fi->enc_type.length())
					{
						if("TBOX_2" == p_fi->enc_type)
						{
							fprintf(stdout,"\tdec:[");
							for(int j=0;j<sizeof(dec);++j)
							{
								fprintf(stdout,"%02.2X ",dec[j]);
							}
							fprintf(stdout,"]\n");
						}
					}
				}
				//fprintf(stdout,"\t%s = 0x%X ",p->sign_name.c_str(),(int)value);
#if 1
				fprintf(stdout,"\t");
				if(p_modify && ((*p_modify)[iter] == 1))
				{
					fprintf(stdout,"* ");
				}
				fprintf(stdout,"%s = 0x%X ",p->sign_name.c_str(),(int)value);
#endif
				for(int j=0;j<p->val_name.size();++j)
				{
					if(value == p->val_name[j].value)
					{
						fprintf(stdout,"(%s)",p->val_name[j].name.c_str());
						break;
					}
				}
				fprintf(stdout,"\n");
			}
		}
	}
	return	valid_signal;
}

//检查报文中是否有要找的信号量
bool	CanMatrixCheckSignal(StCanFrame * p_frame)
{
	if(0 == p_frame->can_id)
	{
		return	false;
	}

	for(int iter=0;iter<g_signal_req.size();++iter)
	{
		CCanSignal * p = g_signal_req[iter];
		if(p->can_id == p_frame->can_id)
		{
			int value = 0;

			value = GetBits_LE32(p_frame->can_data,sizeof(p_frame->can_data),p->start_bit,p->bits_len);
			if(value >= p->min_v && value <= p->max_v)
			{
				return	true;
			}
		}
	}
	return	false;
}

void	CanMatrixGetSignals(StCanFrame * p_frame,vector<unsigned int>* p_signals)
{
	p_signals->resize(g_signal_req.size());
	for(int iter=0;iter<g_signal_req.size();++iter)
	{
		CCanSignal * p = g_signal_req[iter];
		(*p_signals)[iter] = 0;
		if(p_frame->can_id == p->can_id)
		{
			//(*p_signals)[iter] = GetBits_LE32(unsigned char const * pdata,int data_size,int offset,int bits_len);
			(*p_signals)[iter] = GetBits((u8 const *)p_frame->can_data,sizeof(p_frame->can_data),p->start_bit,p->bits_len);
		}
	}
}

void	SK3_enc(unsigned char * p_data_in,unsigned char * p_sk3,unsigned char * p_data_out)
{
	int iter=0;

	unsigned char	random[4];
	for(iter=0;iter<sizeof(random);++iter)
	{
		random[iter] = rand() & 0xFF;
	}
	/*
	tempArray[0] = aFrame.data[0];
        tempArray[1] = aFrame.data[1];
        tempArray[2] = aFrame.data[2];
        tempArray[3] = aFrame.data[3];

        G2_Secrecy_Module(tempArray, secrecySK3, encryptionByte);

        tempArray[0] = aFrame.data[0] ^ encryptionByte[0];
        tempArray[1] = aFrame.data[1] ^ encryptionByte[1];
        tempArray[2] = aFrame.data[2] ^ encryptionByte[2];
        tempArray[3] = aFrame.data[3] ^ encryptionByte[3];

        aFrame.data[0] = secrecySK3[0] ^ tempArray[0];
        aFrame.data[1] = secrecySK3[1] ^ tempArray[1];
        aFrame.data[2] = secrecySK3[2] ^ tempArray[2];
        aFrame.data[3] = secrecySK3[3] ^ tempArray[3];
        aFrame.data[4] = secrecySK3[4] ^ encryptionByte[0];
        aFrame.data[5] = secrecySK3[5] ^ encryptionByte[1];
        aFrame.data[6] = secrecySK3[6] ^ encryptionByte[2];
        aFrame.data[7] = secrecySK3[7] ^ encryptionByte[3];
	*/
	p_data_out[0] = p_data_in[0] ^ random[0] ^ p_sk3[0];
	p_data_out[1] = p_data_in[1] ^ random[1] ^ p_sk3[1];
	p_data_out[2] = p_data_in[2] ^ random[2] ^ p_sk3[2];
	p_data_out[3] = p_data_in[3] ^ random[3] ^ p_sk3[3];
	p_data_out[4] = random[0] ^ p_sk3[4];
	p_data_out[5] = random[1] ^ p_sk3[5];
	p_data_out[6] = random[2] ^ p_sk3[6];
	p_data_out[7] = random[3] ^ p_sk3[7];
}

void	SK3_dec(unsigned char * p_data_in,unsigned char * p_sk3,unsigned char * p_data_out)
{
	int iter=0;
	for(iter=0;iter<8;++iter)
	{
		p_data_out[iter] = p_data_in[iter] ^ p_sk3[iter];
	}
	for(iter=0;iter<4;++iter)
	{
		p_data_out[iter] ^= p_data_out[iter+4];
	}
}



char const * g_parse_type_str[] =
{
	"JS_PARSE_NULL"
	,"JS_PARSE_SINGLE"
	,"JS_PARSE_OBJ"
	,"JS_PARSE_ARRAY"
	,"JS_PARSE_END"
	,"JS_PARSE_ERROR"
};
int	main(int argc,char * argv[])
{
#if 1
	int iter=0;
	int len = 0;
	char buffer[INPUT_BUFFER_MAX];
	StCanFrame last_frame;
	StFrameStatic frame_stat;
	vector<unsigned int>	last_sign;
	vector<unsigned int>	cur_sign;
	vector<int>				modify;

	if(!CanMatrixInit())
	{
		fprintf(stdout,"init failed..\n");
		return	1;
	}
	for(iter=1;iter<argc;++iter)
	{
		CanMatrixLoadCfg(argv[iter]);
	}
	last_sign.resize(g_signal_req.size());
	cur_sign.resize(g_signal_req.size());
	modify.resize(g_signal_req.size(),1);

	last_frame.timestamp = 0.0;
	last_frame.can_id = 0;
	memset(last_frame.can_data,0,sizeof(last_frame.can_data));
	frame_stat.start_time = 0.0;
	frame_stat.end_time = 0.0;
	frame_stat.frame_cnt = 0;
	while(0 == feof(stdin))
	{
		vector<char const *>	str_split;
		char * p_str = buffer;
		StCanFrame frame;
		int size = 0;
		bool valid_signal = false;
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
			frame.timestamp = atof(str_split[0]);
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

		//对比重复报文的数据
		if(true == CanMatrixCheckSignal(&frame))
		{
			//if((last_frame.can_id == frame.can_id)&&(0 == memcmp(last_frame.can_data,frame.can_data,sizeof(frame.can_data))))
			CanMatrixGetSignals(&frame,&cur_sign);
			if((last_frame.can_id == frame.can_id)&&(last_sign == cur_sign))
			{
				frame_stat.end_time = frame.timestamp;
				++frame_stat.frame_cnt;
			}
			else
			{
				//解析报文数据
				if(0 != last_frame.can_id)
				{
					valid_signal = CanMatrixPrintSignal(&last_frame,&frame_stat,&modify);
				}
#if 1
				if(last_frame.can_id != frame.can_id)
				{
					modify.clear();
					modify.resize(g_signal_req.size(),1);
				}
				else
				{
					//modify.resize(g_signal_req.size(),0);
					for(int iter=0;iter<g_signal_req.size();++iter)
					{
						if(last_sign[iter] == cur_sign[iter])
						{
							modify[iter] = 0;
						}
						else
						{
							modify[iter] = 1;
						}
					}
				}
#endif
				frame_stat.start_time = frame.timestamp;
				frame_stat.end_time = frame.timestamp;
				frame_stat.frame_cnt = 1;
			}
			last_frame = frame;
			last_sign.assign(cur_sign.begin(),cur_sign.end());
		}
	}
	CanMatrixPrintSignal(&last_frame,&frame_stat,&modify);

#endif

#if 0
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
#endif

#if 0
	CJsParser jp;
	char	buffer[4096];
#if 0
	typedef enum 	_EnJsParseType
	{
		JS_PARSE_NULL		//未解析
		,JS_PARSE_SINGLE	//单变量
		,JS_PARSE_OBJ		//变量
		,JS_PARSE_ARRAY		//数组变量
		,JS_PARSE_END		//检索到结尾
		,JS_PARSE_ERROR		//解析出错
	}EnJsParseType;
#endif

	if(argc > 1)
	{
		if(!jp.Init(argv[1]))
		{
			fprintf(stdout,"无法打开文件%s\n",argv[1]);
			return	1;
		}
		for(;;)
		{
			bool res = true;
			buffer[0] = 0;
			fscanf(stdin,"%s",buffer);
			if(0 == strcasecmp(buffer,"exit"))
			{
				break;
			}
			else if(0 == strcasecmp(buffer,"next"))
			{
				res = jp.ParseNext();
			}
			else if(0 == strcasecmp(buffer,"sub"))
			{
				res = jp.ParseSub();
			}
			else if(0 == strcasecmp(buffer,"upper"))
			{
				res = jp.ParseUpper();
			}
			else if(0 == strcasecmp(buffer,"reset"))
			{
				res = jp.ParseReset();
			}
			else
			{
			}
			if(false == res)
				fprintf(stdout,"operation failed..\n");
			fprintf(stdout,"pos=%d line=%d\tvar=%s index=%d ",jp.GetCurPos(),jp.GetCurLine(),jp.GetVar().c_str(),jp.GetIndex());
			fprintf(stdout,"parse_type=%s value=%s\n",g_parse_type_str[(int)jp.GetParseType()],jp.GetValue().c_str());
		}
	}
#endif

#if 0
	char buffer[4096];
	for(;;)
	{
		string var_name;
		int idx = -1;
		char const * pstr = 0;
		
		buffer[0] = 0;
		fgets(buffer,sizeof(buffer),stdin);
		fprintf(stdout,"%s\n",buffer);
		pstr = buffer;
		while(*pstr)
		{
			pstr = CJsParser::ParseSplit(pstr,&var_name,&idx);
			if(-1 != idx)
				fprintf(stdout,"[%d]",idx);
			else if("" != var_name)
				fprintf(stdout,".\"%s\"",var_name.c_str());
			else
				break;
		}
		fprintf(stdout,"\n");
	}
#endif

#if 0
	CJsParser jp;
	if(argc <= 1)
		return	1;
	if(!jp.Init(argv[1]))
	{
		fprintf(stdout,"无法打开文件%s\n",argv[1]);
		return	1;
	}

	char buffer[4096];
	for(;;)
	{
		buffer[0] = 0;
		fgets(buffer,sizeof(buffer),stdin);

		if(jp.FindPath(buffer))
		{
			fprintf(stdout,"pos=%d line=%d\tvar=%s index=%d ",jp.GetCurPos(),jp.GetCurLine(),jp.GetVar().c_str(),jp.GetIndex());
			fprintf(stdout,"parse_type=%s value=%s\n",g_parse_type_str[(int)jp.GetParseType()],jp.GetValue().c_str());
		}
		else
			fprintf(stdout,"cannot find var %s \n",buffer);
	}
#endif

#if 0
	char buffer[4096];
	CJsWriter	jw;
	if(argc <= 1)
		return	1;
	if(!jw.Init(argv[1]))
	{
		fprintf(stdout,"无法打开文件%s\n",argv[1]);
		return	1;
	}
	jw.WriteVar("var1");
	jw.WriteValStr(" test 1 \"");	jw.WriteComma();

	jw.WriteVar("obj1");
	jw.WriteLeftBrace();
		jw.WriteVar("sub1");	jw.WriteValStr("123");	jw.WriteComma();
		jw.WriteVar("sbu2");	jw.WriteValStr("true");
	jw.WriteRightBrace();

	jw.WriteComma();
	
	jw.WriteVar("obj2");
	jw.WriteLeftBrace();
		jw.WriteVar("sub3");
		jw.WriteLeftBrace();
			jw.WriteVar("sub4");	jw.WriteValStr("ABCCD");	jw.WriteComma();
			jw.WriteVar("sub5");
			jw.WriteLeftBracket();
				for(int iter=0;iter<10;++iter)
				{
					if(iter)
						jw.WriteComma();
					sprintf(buffer,"%d",iter);
					jw.WriteArrayValStr(buffer);
				}
			jw.WriteRightBracket();
		jw.WriteRightBrace();
	jw.WriteRightBrace();

	jw.WriteComma();

	jw.WriteVar("array");
	jw.WriteLeftBracket();
		for(int iter=0;iter<10;++iter)
		{
			if(iter)
				jw.WriteComma();
			sprintf(buffer,"%d",(iter*100));
			jw.WriteArrayValStr(buffer);
		}
	jw.WriteRightBracket();

	jw.Flush();
	jw.Release();
#endif
	return	0;
}
