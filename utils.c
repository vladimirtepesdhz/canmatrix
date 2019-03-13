#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <memory.h>

//#include "app/public/public_def.h"
#include "utils.h"

u16	FromLE16(u8 const * p_data)
{
	u16 res = 0;
	res += p_data[1];
	res <<= 8;
	res += p_data[0];
	return	res;
}

u32	FromLE24(u8 const * p_data)
{
	u32	res = 0;
	res += p_data[2];
	res <<= 8;
	res += p_data[1];
	res <<= 8;
	res += p_data[0];
	return	res;
}


u32	FromLE32(u8 const * p_data)
{
	u32	res = 0;
	res += p_data[3];
	res <<= 8;
	res += p_data[2];
	res <<= 8;
	res += p_data[1];
	res <<= 8;
	res += p_data[0];
	return	res;
}

u16	FromBE16(u8 const * p_data)
{
	u16 res = 0;
	res += p_data[0];
	res <<= 8;
	res += p_data[1];
	return	res;
}

u32	FromBE24(u8 const * p_data)
{
	u32	res = 0;
	res += p_data[0];
	res <<= 8;
	res += p_data[1];
	res <<= 8;
	res += p_data[2];
	return	res;
}


u32	FromBE32(u8 const * p_data)
{
	u32	res = 0;
	res += p_data[0];
	res <<= 8;
	res += p_data[1];
	res <<= 8;
	res += p_data[2];
	res <<= 8;
	res += p_data[3];
	return	res;
}

void	ToLE16(u8 * p_data,u16 v)
{
	p_data[0] = (v & 0xFF);
	v >>= 8;
	p_data[1] = (v & 0xFF);
}

void	ToLE24(u8 * p_data,u32 v)
{
	p_data[0] = (v & 0xFF);
	v >>= 8;
	p_data[1] = (v & 0xFF);
	v >>= 8;
	p_data[2] = (v & 0xFF);
}


void	ToLE32(u8 * p_data,u32 v)
{
	p_data[0] = (v & 0xFF);
	v >>= 8;
	p_data[1] = (v & 0xFF);
	v >>= 8;
	p_data[2] = (v & 0xFF);
	v >>= 8;
	p_data[3] = (v & 0xFF);
}

void	ToBE16(u8 * p_data,u16 v)
{
	p_data[1] = (v & 0xFF);
	v >>= 8;
	p_data[0] = (v & 0xFF);
}

void	ToBE24(u8 * p_data,u32 v)
{
	p_data[2] = (v & 0xFF);
	v >>= 8;
	p_data[1] = (v & 0xFF);
	v >>= 8;
	p_data[0] = (v & 0xFF);
}


void	ToBE32(u8 * p_data,u32 v)
{
	p_data[3] = (v & 0xFF);
	v >>= 8;
	p_data[2] = (v & 0xFF);
	v >>= 8;
	p_data[1] = (v & 0xFF);
	v >>= 8;
	p_data[0] = (v & 0xFF);
}

u8	const	g_mask_m[8] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
//u8	const	g_mask_m[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
u8	const	g_mask_i[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

//Motorola顺序格式
u32	GetBits_M(u8 const * p_data,int data_size,int bits_start,int bits_len)
{
	int iter = 0;
	int	byte_offset = 0;
	int bit_offset = 0;
	u32	res = 0;

	for(iter=0;iter<bits_len;++iter)
	{
		byte_offset = ((bits_start + iter) >> 3);
		bit_offset = ((bits_start + iter) & 0x07);
		if(byte_offset >= data_size)
			return	res;
		res <<= 1;
		if(p_data[byte_offset] & g_mask_m[bit_offset])
			res += 1;
	}
	return	res;
}

void	SetBits_M(u8 * p_data,int data_size,int bits_start,int bits_len,u32 value)
{
	int iter = 0;
	int byte_offset = 0;
	int bit_offset = 0;
	u32	mask = (((u32)1)<<(bits_len-1));

	for(iter=0;iter<bits_len;++iter)
	{
		byte_offset = ((bits_start + iter) >> 3);
		bit_offset = ((bits_start + iter) & 0x07);
		if(byte_offset >= data_size)
			return;
		if(value & mask)
			p_data[byte_offset] |= g_mask_m[bit_offset];
		else
			p_data[byte_offset] &= ~(g_mask_m[bit_offset]);
		mask >>= 1;
	}
}

//intel顺序格式
u32	GetBits_I(u8 const * p_data,int data_size,int bits_start,int bits_len)
{
	int iter = 0;
	int byte_offset = 0;
	int bit_offset = 0;
	u32	res = 0;
	u32	mask = 0x00000001;

	for(iter=0;iter<bits_len;++iter)
	{
		byte_offset = ((bits_start + iter) >> 3);
		bit_offset = ((bits_start + iter) & 0x07);
		if(byte_offset >= data_size)
			return	res;
		if(p_data[byte_offset] & g_mask_i[bit_offset])
			res += mask;
		mask <<= 1;
	}
	return	res;
}

void	SetBits_I(u8 * p_data,int data_size,int bits_start,int bits_len,u32 value)
{
	int iter = 0;
	int byte_offset = 0;
	int bit_offset = 0;
	u32 mask = 0x00000001;

	for(iter=0;iter<bits_len;++iter)
	{
		byte_offset = ((bits_start + iter) >> 3);
		bit_offset = ((bits_start + iter) & 0x07);
		if(byte_offset >= data_size)
			return;
		if(value & mask)
			p_data[byte_offset] |= g_mask_i[bit_offset];
		else
			p_data[byte_offset] &= ~(g_mask_i[bit_offset]);
		mask <<= 1;
	}
}

bool	IsEmptyChar(char ch)
{
	switch(ch)
	{
		case	' ':
		case	'\t':
		case	'\r':
		case	'\n':
			return	TRUE;
	}
	return	FALSE;
}

bool	IsNumChar(char ch)
{
	if(ch >= '0' && ch <= '9')
		return	TRUE;
	return	FALSE;
}

bool	IsAlphaChar(char ch)
{
	if(ch >= 'A' && ch <= 'Z')
		return	TRUE;
	if(ch >= 'a' && ch <= 'z')
		return	TRUE;
	return	FALSE;
}

bool	IsHexChar(char ch)
{
	if(IsNumChar(ch))
		return	TRUE;
	if(ch >= 'A' && ch <= 'F')
		return	TRUE;
	if(ch >= 'a' && ch <= 'f')
		return	TRUE;
	return	FALSE;
}

int	CharToHex(char ch)
{
	if(IsNumChar(ch))
		return	(ch - '0');
	else if((ch >= 'A' && ch <= 'F'))
		return	(ch - 'A' + 10);
	else if((ch >= 'a' && ch <= 'f'))
		return	(ch - 'a' + 10);
	return	0;
}

char const *	ParseEmpty(char const * pstr)
{
	while(IsEmptyChar(*pstr))
		++pstr;
	return	pstr;
}

int	ParseHex(char const * p_hexstr,u8 * p_data,int data_size)
{
	u8 cur_v = 0;
	int i_dst = 0;
	if(!p_hexstr)
		return	0;
	while(*p_hexstr && i_dst < data_size)
	{
		p_hexstr = ParseEmpty(p_hexstr);
		if(0 == *p_hexstr)
			break;
		if(*p_hexstr)
		{
			cur_v = CharToHex(*p_hexstr);
			++p_hexstr;
		}
		if(IsEmptyChar(*p_hexstr))
		{
			p_hexstr = ParseEmpty(p_hexstr);
			if(0 == *p_hexstr)
				break;
		}
		else if(*p_hexstr)
		{
			cur_v <<= 4;
			cur_v += CharToHex(*p_hexstr);
			++p_hexstr;
		}
		p_data[i_dst] = cur_v;
		++i_dst;
	}
	return	i_dst;
}

s32	ParseInt(char const * p_str)
{
	s32	sum = 0;
	bool neg = FALSE;
	p_str = ParseEmpty(p_str);
	if('-' == *p_str)
	{
		neg = TRUE;
		++p_str;
		p_str = ParseEmpty(p_str);
	}
	if('0' == p_str[0] && ('x' == p_str[1] || 'X' == p_str[1]))
	{
		p_str += 2;
		sum = strtoul(p_str,NULL,16);
	}
	else
	{
		sum = strtoul(p_str,NULL,10);
	}
	if(neg)
		sum = -sum;
	return	sum;
}

int	StringSplit(char * pstr,char const * p_split[],int buf_num)
{
	int cnt = 0;

	if(!pstr)
		return	0;
	if(!p_split)
		return	0;
	if(!buf_num)
		return	0;

	while(*pstr)
	{
		pstr = (char *)ParseEmpty(pstr);
		if('\0' == *pstr)
			break;
		if('\"' == *pstr || '\'' == *pstr)
		{
			++pstr;
			p_split[cnt] = pstr;
			while(*pstr && '\"' != *pstr && '\'' != *pstr)
			{
				if('\\' == *pstr)
				{
					if('\'' == pstr[1] || '\"' == pstr[1])
					{
						++pstr;
					}
				}
				++pstr;
			}
		}
		else
		{
			p_split[cnt] = pstr;
			while(!IsEmptyChar(*pstr))
			{
				++pstr;
			}
		}
		if(*pstr)
		{
			*pstr = '\0';
			++pstr;
		}
		++cnt;
		if(cnt >= buf_num)
			break;
	}
	return	cnt;
}

int	StringEscConv(char * p_str,char const * p_esc_char)	//处理转义符
{
	int iter = 0;
	char * p_src = p_str;
	char * p_dst = p_str;
	char const * p_esc = NULL;
	
	if(!p_str)
		return	0;
	if(!p_esc_char)
		p_esc_char = "\"\'";
	
	while(*p_src)
	{
		if('\\' == *p_src)
		{
			p_esc = p_esc_char;
			while(*p_esc)
			{
				if(p_src[1] == *p_esc)
				{
					++p_src;
					switch(*p_esc)
					{
						case	'\"':	break;
						case	'\'':	break;
						case	'\\':	break;
						case	't':	*p_src = '\t';	break;
						case	'b':	*p_src = '\b';	break;
						case	'r':	*p_src = '\r';	break;
						case	'n':	*p_src = '\n';	break;
					}
					break;
				}
				++p_esc;
			}
		}
		if(p_src != p_dst)
		{
			*p_dst = *p_src;
		}
		++p_src;
		++p_dst;
	}
	*p_dst = '\0';
	return	(int)(p_dst - p_str);
}

