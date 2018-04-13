#ifndef	_JS_PARSER_H_
#define	_JS_PARSER_H_

#include <vector>
#include <string>
using	namespace	std;

class	CJsInput
{
public:
	typedef enum 	_EnJsInputType
	{
		JS_INPUT_NONE
		,JS_INPUT_FILEPATH		//文件名/路径
		,JS_INPUT_FILEHANDLE	//文件句柄/标准输入
		,JS_INPUT_DATABUF		//数据缓冲区
	}EnJsInputType;
private:
	EnJsInputType type;
	FILE * file;
	vector<unsigned char> buffer;
	int offset;
	int line_num;
public:
	CJsInput()
	{
		type = JS_INPUT_NONE;
		file = NULL;
		offset = 0;
		line_num = 1;
	}
	~CJsInput()
	{
		if(JS_INPUT_FILEPATH == type)
		{
			if(file)
				fclose(file);
		}
	}
	EnJsInputType	GetType()	const	{return	type;}
	int	GetLineNum()	const	{return	line_num;}
	void	SetLineNum(int ln)	{line_num = ln;}
	bool	Init(char const * file_path);
	bool	Init(FILE * file_handle);
	bool	Init(unsigned char const * p_data,int data_size);
	void	Release();
	int		GetChar();
	void	FallbackChar();
	bool	IsEOF()	const;
	bool	Seek(int pos);
	long	GetPos()	const;
};

class	CJsToken
{
public:
	typedef enum 	_EnJsTokenType
	{
		JS_TOKEN_NULL	//无效类型
		,JS_TOKEN_STRBLK	//字符串，或有效数据
		,JS_TOKEN_COLON	//冒号,等于号
		,JS_TOKEN_LEFT_BRACE	//左大括号
		,JS_TOKEN_RIGHT_BRACE	//右大括号
		,JS_TOKEN_LEFT_BRACKET		//左中括号
		,JS_TOKEN_RIGHT_BRACKET		//右中括号
		,JS_TOKEN_COMMA		//逗号
		,JS_TOKEN_ERROR		//错误类型
	}EnJsTokenType;
public:
	static	bool	IsEmptyChar(int ch);
	static	bool	IsAlphaChar(int ch);
	static	bool	IsNumChar(int ch);
	static	bool	IsHexChar(int ch);
	static	bool	IsVarChar(int ch);
private:
	CJsInput * input;
	EnJsTokenType type;
	string	str;
	int pos;
	int line_num;
public:
	CJsToken()
	{
		type = JS_TOKEN_NULL;
		input = NULL;
		pos = 0;
		line_num = 1;
	}
	EnJsTokenType	GetType()	const	{return	type;}
	string const &	GetStr()	const	{return	str;}
	int GetPos()	const	{return	pos;}
	int	GetLineNum()	const	{return	line_num;}
	void	Init(CJsInput * p_input);
	void	Release();
	int		ParseEmpty();
	bool	ParseNext();
};

class	CJsParser
{
public:
	typedef enum 	_EnJsParseType
	{
		JS_PARSE_NULL		//未解析
		,JS_PARSE_SINGLE	//单变量
		,JS_PARSE_OBJ		//变量
		,JS_PARSE_ARRAY		//数组变量
		,JS_PARSE_END		//检索到结尾
		,JS_PARSE_ERROR		//解析出错
	}EnJsParseType;
	struct	CStackInfo
	{
	public:
		long	start_pos;
		int		start_line;
		string	var;
		int	index;
		EnJsParseType parse_type;
		CStackInfo()
		{
			start_pos = 0;
			start_line = 1;
			index = -1;
			parse_type = JS_PARSE_NULL;
		}
	};
private:
	CJsInput input;
	vector<CStackInfo>	stack;

	string	var;
	int index;
	EnJsParseType	parse_type;
	string	value;
public:
	CJsParser()
	{
		stack.push_back(CStackInfo());
		stack.back().parse_type = JS_PARSE_OBJ;
		index = -1;
		parse_type = JS_PARSE_NULL;
	}
	~CJsParser()
	{
	}
	bool	Init(char const * filename)	{return	input.Init(filename);}
	bool	Init(FILE * filehandle)	{return	input.Init(filehandle);}
	bool	Init(unsigned char const * p_data,int data_size)	{return	input.Init(p_data,data_size);}
	bool	IsValid()	const	{return	(CJsInput::JS_INPUT_NONE != input.GetType());}

	string	const &	GetVar()	const	{return	var;}
	int	GetIndex()	const	{return	index;}
	EnJsParseType GetParseType()	const	{return	parse_type;}
	string	const & GetValue()	const	{return	value;}
	int	GetValInt()	const;

	long	GetCurPos()	const	{return	input.GetPos();}
	int		GetCurLine()	const	{return	input.GetLineNum();}

	bool	TokenPass(CJsToken::EnJsTokenType start_token);
	bool	ParseReset();
	bool	ParseRoot();
	bool	ParseNext();
	bool	ParseUpper();
	bool	ParseSub();

	bool	FindVar(char const * var_name);
	bool	FindIndex(int idx);

	static	char const *	ParseSplit(char const * path_name,string * p_var,int * p_index);
	bool	FindPath(char const * path_name);
	bool	EnterObj();
	bool	EnterArray();
};

class	CJsWriter
{
public:
	typedef enum 	_EnJsWriteType
	{
		JS_WRITE_NONE
		,JS_WRITE_FILEPATH
		,JS_WRITE_FILEHANDLE
		,JS_WRITE_DATABUF
	}EnJsWriteType;
private:
	EnJsWriteType type;
	FILE * file;
	unsigned char * pdata;
	int data_size;

	string	js_str;

	string	indent;
	int index;
public:
	CJsWriter()
	{
		type = JS_WRITE_NONE;
		file = NULL;
		pdata = NULL;
		data_size = 0;
		index = 0;
	}
	~CJsWriter()
	{
		if(JS_WRITE_FILEPATH == type)
		{
			if(file)
				fclose(file);
		}
	}
	string const &	GetJsonStr()	const	{return	js_str;}

	bool	Init(char const * filename);
	bool	Init(FILE * filehandle);
	bool	Init(unsigned char * p,int size);
	void	Release();
	
	void	WriteVar(char const * var_name);
	void	WriteValStr(char const * val_str);
	void	WriteArrayValStr(char const * val_str);
	void	WriteComma();
	void	WriteLeftBrace();
	void	WriteLeftBracket();
	void	WriteRightBrace();
	void	WriteRightBracket();
	bool	Flush();
};

#endif
