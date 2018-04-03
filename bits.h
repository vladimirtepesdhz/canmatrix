#ifndef	_BITS_H_
#define	_BITS_H_

typedef	unsigned char	u8;
typedef	unsigned short	u16;
typedef	unsigned int	u32;
typedef	enum _bool_t
{
	t_false
	,t_true
}bool_t;

u32	GenMask(int n);
u32	GetBits_LE32(unsigned char const * pdata,int data_size,int offset,int bits_len);
u32	GetBits_BE32(unsigned char const * pdata,int data_size,int offset,int bits_len);
void	SetBits_LE32(u8 * pdata,int data_size,int offset,int bits_len,u32 value);
void	SetBits_BE32(u8 * pdata,int data_size,int offset,int bits_len,u32 value);
void	ShiftLeft_LE(u8 * pdata,int data_size,int shift,bool_t rotate);
void	ShiftRight_LE(u8 *pdata,int data_size,int shift,bool_t rotate);
void	ShiftLeft_BE(u8 * pdata,int data_size,int shift,bool_t rotate);
void	ShiftRight_BE(u8 * pdata,int data_size,int shift,bool_t rotate);
#endif
