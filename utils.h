#ifndef	_UTILS_H_
#define	_UTILS_H_

//#include "app/public/public_def.h"
//#include "../can_def.h"
#include "bits.h"

#define	numof(a)	(sizeof(a)/sizeof((a)[0]))
#define	TRUE	true
#define	FALSE	false

u16	FromLE16(u8 const * p_data);
u32	FromLE24(u8 const * p_data);
u32	FromLE32(u8 const * p_data);
u16	FromBE16(u8 const * p_data);
u32	FromBE24(u8 const * p_data);
u32	FromBE32(u8 const * p_data);
void	ToLE16(u8 * p_data,u16 v);
void	ToLE24(u8 * p_data,u32 v);
void	ToLE32(u8 * p_data,u32 v);
void	ToBE16(u8 * p_data,u16 v);
void	ToBE24(u8 * p_data,u32 v);
void	ToBE32(u8 * p_data,u32 v);


u32	GetBits_M(u8 const * p_data,int data_size,int bits_start,int bits_len);
void	SetBits_M(u8 * p_data,int data_size,int bits_start,int bits_len,u32 value);
u32	GetBits_I(u8 const * p_data,int data_size,int bits_start,int bits_len);
void	SetBits_I(u8 * p_data,int data_size,int bits_start,int bits_len,u32 value);

#define	IsPrintChar(ch)	(((ch) >= 0x20 && (ch)<=0x7E)||((ch) >= 0xA1 && (ch)<=0xFE))
bool	IsEmptyChar(char ch);
bool	IsNumChar(char ch);
bool	IsAlphaChar(char ch);
bool	IsHexChar(char ch);
int	CharToHex(char ch);
char const *	ParseEmpty(char const * pstr);

int	ParseHex(char const * p_hexstr,u8 * p_data,int data_size);
s32	ParseInt(char const * p_str);
int	StringSplit(char * pstr,char const * p_split[],int buf_num);
int	StringEscConv(char * p_str,char const * esc_char);	//处理转义符

#endif
