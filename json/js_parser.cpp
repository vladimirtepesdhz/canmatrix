#include <stdio.h>
#include <stdlib.h>
#include <memory.h>


#include "js_parser.h"

bool	CJsInput::Init(char const * file_path)
{
	file = fopen(file_path,"rb");
	if(NULL == file)
		return	false;
	type = JS_INPUT_FILEPATH;
	return	true;
}
bool	CJsInput::Init(FILE * file_handle)
{
	if(NULL == file_handle)
		return	false;
	file = file_handle;
	type = JS_INPUT_FILEHANDLE;
	return	true;
}
bool	CJsInput::Init(unsigned char const * p_data,int data_size)
{
	if(NULL == p_data || 0 == data_size)
		return	false;
	buffer.assign(p_data,p_data+data_size);
	offset = 0;
	type = JS_INPUT_DATABUF;
	return	true;
}

void	CJsInput::Release()
{
	if(JS_INPUT_FILEPATH == type)
	{
		if(file)
			fclose(file);
	}
	else if(JS_INPUT_DATABUF == type)
	{
		buffer.clear();
	}
	type = JS_INPUT_NONE;
	file = NULL;
	offset = 0;
}

int	CJsInput::GetChar()
{
	int v = 0;
	if(JS_INPUT_DATABUF == type)
	{
		if(offset < buffer.size())
		{
			v = buffer[offset];
			++offset;
		}
		else
			v = EOF;
	}
	else if(JS_INPUT_FILEPATH == type || JS_INPUT_FILEHANDLE == type)
	{
		v = fgetc(file);
	}
	else
		v = EOF;

	if('\n' == v)	//统计行号
		++line_num;

	return	v;
}
void	CJsInput::FallbackChar()
{
	if(JS_INPUT_DATABUF == type)
	{
		if(offset)
			--offset;
	}
	else if(JS_INPUT_FILEPATH == type || JS_INPUT_FILEHANDLE == type)
	{
		long pos = 0;
		pos = ftell(file);
		if(pos)
		{
			fseek(file,pos-1,SEEK_SET);
		}
	}
}
bool	CJsInput::IsEOF()	const
{
	if(JS_INPUT_DATABUF == type)
	{
		if(offset < buffer.size())
			return	false;
		else
			return	true;
	}
	else if(JS_INPUT_FILEPATH == type || JS_INPUT_FILEHANDLE == type)
	{
		if(0 == feof(file))
			return	false;
		else
			return	true;
	}
	else
		return	true;
}
bool	CJsInput::Seek(int pos)
{
	if(JS_INPUT_DATABUF == type)
	{
		if(pos < 0 || pos >= buffer.size())
			return	false;
		offset = pos;
		return	true;
	}
	else if(JS_INPUT_FILEPATH == type || JS_INPUT_FILEHANDLE == type)
	{
		if(0 == fseek(file,pos,SEEK_SET))
			return	true;
		else
			return	false;
	}
	else
		return	false;
}

long	CJsInput::GetPos()	const
{
	if(JS_INPUT_DATABUF == type)
	{
		return	offset;
	}
	else if(JS_INPUT_FILEPATH == type || JS_INPUT_FILEHANDLE == type)
	{
		return	ftell(file);
	}
	return	0;
}



bool	CJsToken::IsEmptyChar(int ch)
{
	switch(ch)
	{
		case ' ':
		case '\t':
		case '\b':
		case '\r':
		case '\n':
			return	true;
		break;
	}
	return	false;
}
bool	CJsToken::IsAlphaChar(int ch)
{
	if(ch >= 'a' && ch <= 'Z')
		return	true;
	if(ch >= 'A' && ch <= 'Z')
		return	true;
	return	false;
}
bool	CJsToken::IsNumChar(int ch)
{
	if(ch >= '0' && ch <= '9')
		return	true;
	return	false;
}
bool	CJsToken::IsHexChar(int ch)
{
	if(IsNumChar(ch))
		return	true;
	if(ch >= 'a' && ch <= 'f')
		return	true;
	if(ch >= 'A' && ch <= 'F')
		return	true;
	return	false;
}
bool	CJsToken::IsVarChar(int ch)
{
	if(IsAlphaChar(ch))
		return	true;
	if('_' == ch)
		return	true;
	if(ch > 0xFF && ch <= 0xFFFE)
		return	true;
	return	false;
}

void	CJsToken::Init(CJsInput * p_input)
{
	input = p_input;
}
void	CJsToken::Release()
{
	type = JS_TOKEN_NULL;
	input = NULL;
}
int	CJsToken::ParseEmpty()
{
	int cur_char = 0;
	if(NULL == input)
		return	EOF;
	while(!input->IsEOF())
	{
		cur_char = input->GetChar();
		if(EOF != cur_char && !IsEmptyChar(cur_char))
		{
			input->FallbackChar();
			return cur_char;
		}
	}
	return	EOF;
}
bool	CJsToken::ParseNext()
{
	int cur_char = 0;

	cur_char = ParseEmpty();
	pos = input->GetPos();
	line_num = input->GetLineNum();
	if(EOF == cur_char)
	{
		type = JS_TOKEN_NULL;
		return	false;
	}
	cur_char = input->GetChar();
	if('\'' == cur_char || '\"' == cur_char)
	{
		str = "";
		cur_char = input->GetChar();
		while(EOF != cur_char&&'\'' != cur_char && '\"' != cur_char)
		{
			if('\\' == cur_char)
			{
				cur_char = input->GetChar();
				switch(cur_char)
				{
					case '\'':	cur_char = '\'';	break;
					case '\"':	cur_char = '\"';	break;
					default:	str.push_back('\\');
				}
			}
			str.push_back((char)cur_char);
			cur_char = input->GetChar();
		}
		type = JS_TOKEN_STRBLK;
		return	true;
	}
	else if(':' == cur_char || '=' == cur_char)
	{
		type = JS_TOKEN_COLON;
		return	true;
	}
	else if('{' == cur_char)
	{
		type = JS_TOKEN_LEFT_BRACE;
		return	true;
	}
	else if('}' == cur_char)
	{
		type = JS_TOKEN_RIGHT_BRACE;
		return	true;
	}
	else if('[' == cur_char)
	{
		type = JS_TOKEN_LEFT_BRACKET;
		return	true;
	}
	else if(']' == cur_char)
	{
		type = JS_TOKEN_RIGHT_BRACKET;
		return	true;
	}
	else if(',' == cur_char)
	{
		type = JS_TOKEN_COMMA;
		return	true;
	}
	else
	{
		str = "";
		while(EOF != cur_char && !IsEmptyChar(cur_char))
		{
			if(('\'' == cur_char)
				||('\"' == cur_char)
				||(':' == cur_char)
				||('=' == cur_char)
				||('{' == cur_char)
				||('}' == cur_char)
				||('[' == cur_char)
				||(']' == cur_char)
				||(',' == cur_char))
			{
				input->FallbackChar();
				break;
			}
			str.push_back((char)cur_char);
			cur_char = input->GetChar();
		}
		type = JS_TOKEN_STRBLK;
		return	true;
	}
	return	true;
}

bool	CJsParser::CheckHead()
{
	unsigned char win_utf8_header[3];
	CJsToken token;

	token.Init(&input);

	//检测Windows的UTF8头标志 EF BB BF
	win_utf8_header[0] = input.GetChar();
	win_utf8_header[1] = input.GetChar();
	win_utf8_header[2] = input.GetChar();
	if((0xEF == win_utf8_header[0])
		&& (0xBB == win_utf8_header[1])
		&& (0xBF == win_utf8_header[2]))
	{
		stack.back().start_pos = input.GetPos();
	}
	else
	{
		input.Seek(0);
		input.SetLineNum(1);
	}

	//检测第一个左大括号
	if(token.ParseNext())
	{
		if(CJsToken::JS_TOKEN_LEFT_BRACE == token.GetType())
		{
			stack.back().start_pos = input.GetPos();
			stack.back().start_line = input.GetLineNum();
		}
	}
	return	true;
}

bool	CJsParser::Init(char const * filename)
{
	if(!input.Init(filename))
		return	false;
	return	CheckHead();
}
bool	CJsParser::Init(FILE * filehandle)
{
	if(!input.Init(filehandle))
		return	false;
	return	CheckHead();
}
bool	CJsParser::Init(unsigned char const * p_data,int data_size)
{
	if(!input.Init(p_data,data_size))
		return	false;
	return	CheckHead();
}

int	CJsParser::GetValInt()	const
{
	bool	neg = false;
	char const * pstr = NULL;
	int  result = 0;

	if("" == value)
		return	0;

	pstr = value.c_str();
	while(*pstr && CJsToken::IsEmptyChar(*pstr))
		++pstr;
	if('-' == *pstr)
	{
		neg = true;
		++pstr;
	}
	while(*pstr && CJsToken::IsEmptyChar(*pstr))
		++pstr;
	if('0' == pstr[0] && ('x' == pstr[1] || 'X' == pstr[1]))
	{
		result = strtoul(pstr,NULL,16);
	}
	else
	{
		result = strtoul(pstr,NULL,10);
	}
	if(neg)
		result = -result;
	return	result;
}



bool	CJsParser::TokenPass(CJsToken::EnJsTokenType start_token)
{
	vector<CJsToken::EnJsTokenType> token_stack;
	CJsToken token;

	if(!IsValid())
		return	false;

	if(CJsToken::JS_TOKEN_LEFT_BRACE == start_token || CJsToken::JS_TOKEN_LEFT_BRACKET == start_token)
		token_stack.push_back(start_token);
	else
		return	false;

	token.Init(&input);
	while(!token_stack.empty())
	{
		if(!token.ParseNext())
			break;
		switch(token.GetType())
		{
			case	CJsToken::JS_TOKEN_LEFT_BRACE:
			case	CJsToken::JS_TOKEN_LEFT_BRACKET:
			{
				token_stack.push_back(token.GetType());
			}
			break;

			case	CJsToken::JS_TOKEN_RIGHT_BRACE:
			{
				if(CJsToken::JS_TOKEN_LEFT_BRACE == token_stack.back())
					token_stack.pop_back();
			}
			break;

			case	CJsToken::JS_TOKEN_RIGHT_BRACKET:
			{
				if(CJsToken::JS_TOKEN_LEFT_BRACKET == token_stack.back())
					token_stack.pop_back();
			}
			break;
		}
	}

	return	true;
}

bool	CJsParser::ParseReset()
{
	CStackInfo * p_cur = 0;

	if(!IsValid())
		return	false;

	p_cur = &stack.back();
	input.Seek(p_cur->start_pos);
	input.SetLineNum(p_cur->start_line);

	index = -1;
	var = "";
	parse_type = JS_PARSE_NULL;
	value = "";
	
	return	true;
}

bool	CJsParser::ParseRoot()
{
	stack.resize(1);

	return	ParseReset();
}

bool	CJsParser::ParseNext()
{
	CStackInfo * p_cur = 0;
	CJsToken token;

	if(!IsValid())
		return	false;

	p_cur = &stack.back();
	token.Init(&input);

	if(JS_PARSE_OBJ == parse_type)
		TokenPass(CJsToken::JS_TOKEN_LEFT_BRACE);
	else if(JS_PARSE_ARRAY == parse_type)
		TokenPass(CJsToken::JS_TOKEN_LEFT_BRACKET);

	var = "";
	parse_type = JS_PARSE_NULL;
	value = "";

	if(JS_PARSE_OBJ == p_cur->parse_type)
	{
		if(-1 != index)
		{
			if(!token.ParseNext())
			{
				parse_type = JS_PARSE_END;
				input.Seek(token.GetPos());
				input.SetLineNum(token.GetLineNum());
				return	false;
			}
			if(CJsToken::JS_TOKEN_COMMA != token.GetType())	//解析逗号
			{
				if(CJsToken::JS_TOKEN_RIGHT_BRACE == token.GetType())
					parse_type = JS_PARSE_END;
				else
					parse_type = JS_PARSE_ERROR;
				input.Seek(token.GetPos());
				input.SetLineNum(token.GetLineNum());
				return	false;
			}
		}

		if(!token.ParseNext())
		{
			parse_type = JS_PARSE_END;
			input.Seek(token.GetPos());
			input.SetLineNum(token.GetLineNum());
			return	false;
		}
		if(CJsToken::JS_TOKEN_STRBLK != token.GetType())	//解析变量名
		{
			if(CJsToken::JS_TOKEN_RIGHT_BRACE == token.GetType())
				parse_type = JS_PARSE_END;
			else
				parse_type = JS_PARSE_ERROR;
			input.Seek(token.GetPos());
			input.SetLineNum(token.GetLineNum());
			return	false;
		}
		var = token.GetStr();

		if(!token.ParseNext())
		{
			parse_type = JS_PARSE_ERROR;
			input.Seek(token.GetPos());
			input.SetLineNum(token.GetLineNum());
			return	false;
		}
		if(CJsToken::JS_TOKEN_COLON != token.GetType())		//解析冒号/等于号
		{
			parse_type = JS_PARSE_ERROR;
			input.Seek(token.GetPos());
			input.SetLineNum(token.GetLineNum());
			return	false;
		}
	}
	else if(JS_PARSE_ARRAY == p_cur->parse_type)
	{
		if(-1 != index)
		{
			if(!token.ParseNext())
			{
				parse_type = JS_PARSE_END;
				input.Seek(token.GetPos());
				input.SetLineNum(token.GetLineNum());
				return	false;
			}
			if(CJsToken::JS_TOKEN_COMMA != token.GetType())	//解析逗号
			{
				if(CJsToken::JS_TOKEN_RIGHT_BRACKET == token.GetType())
					parse_type = JS_PARSE_END;
				else
					parse_type = JS_PARSE_ERROR;
				input.Seek(token.GetPos());
				input.SetLineNum(token.GetLineNum());
				return	false;
			}
		}
	}
	else
		return	false;

	if(!token.ParseNext())
	{
		parse_type = JS_PARSE_ERROR;
		input.Seek(token.GetPos());
		input.SetLineNum(token.GetLineNum());
		return	false;
	}
	switch(token.GetType())					
	{
		case	CJsToken::JS_TOKEN_STRBLK:	//单值变量
		{
			parse_type = JS_PARSE_SINGLE;
			value = token.GetStr();
		}
		break;

		case	CJsToken::JS_TOKEN_LEFT_BRACE:	//结构变量
		{
			parse_type = JS_PARSE_OBJ;
		}
		break;

		case	CJsToken::JS_TOKEN_LEFT_BRACKET:	//数组变量
		{
			parse_type = JS_PARSE_ARRAY;
		}
		break;

		default:
		{
			parse_type = JS_PARSE_ERROR;
			input.Seek(token.GetPos());
			input.SetLineNum(token.GetLineNum());
			return	false;
		}
	}
	++index;

	return	true;
}

bool	CJsParser::ParseSub()	//进入下层
{
	CStackInfo si;

	if(!IsValid())
		return	false;

	if(JS_PARSE_OBJ != parse_type && JS_PARSE_ARRAY != parse_type)
		return	false;

	si.start_pos = input.GetPos();
	si.start_line = input.GetLineNum();
	si.index = index;
	si.var = var;
	si.parse_type = parse_type;
	stack.push_back(si);

	index = -1;
	var = "";
	parse_type = JS_PARSE_NULL;
	value = "";

	return	true;
}

bool	CJsParser::ParseUpper()	//返回上层
{
	CStackInfo * p = 0;

	if(!IsValid())
		return	false;

	if(1 == stack.size())	//到顶了
		return	false;

	p = &stack.back();
	parse_type = p->parse_type;
	var = p->var;
	index = p->index;
	input.Seek(p->start_pos);
	input.SetLineNum(p->start_line);

	stack.pop_back();

	return	true;
}

bool	CJsParser::FindVar(char const * var_name)
{
	if(!IsValid())
		return	false;
	if(JS_PARSE_OBJ != stack.back().parse_type)
		return	false;
	ParseReset();
	while(ParseNext())
	{
		if(var == var_name)
			return	true;
	}	
	return	false;
}

bool	CJsParser::FindIndex(int idx)
{
	if(!IsValid())
		return	false;
	if(JS_PARSE_ARRAY != stack.back().parse_type)
		return	false;
	ParseReset();
	while(ParseNext())
	{
		if(idx == index)
			return	true;
	}
	return	false;
}



char const *	CJsParser::ParseSplit(char const * path_name,string * p_var,int * p_index)
{
	char const * pstr = path_name;

	*p_var = "";
	*p_index = -1;
	while(*pstr && CJsToken::IsEmptyChar(*pstr))
		++pstr;
	if('.' == *pstr)
	{
		++pstr;
	}
	while(*pstr && CJsToken::IsEmptyChar(*pstr))
		++pstr;
	if('\'' == *pstr || '\"' == *pstr)
	{
		++pstr;
		while(*pstr && '\'' != *pstr && '\"' != *pstr)
		{
			p_var->push_back(*pstr);
			++pstr;
		}
		if('\'' == *pstr || '\"' == *pstr)
		{
			++pstr;
		}
	}
	else if('[' == *pstr)
	{
		++pstr;
		while(*pstr && CJsToken::IsEmptyChar(*pstr))
			++pstr;
		if('0' == pstr[0] && ('x' == pstr[1] || 'X' == pstr[1]))	//16进制
		{
			*p_var += "0x";
			pstr += 2;
			while(CJsToken::IsHexChar(*pstr))
			{
				p_var->push_back(*pstr);
				++pstr;
			}
			*p_index = strtoul(p_var->c_str(),NULL,16);
		}
		else 	//10进制
		{
			while(CJsToken::IsNumChar(*pstr))
			{
				p_var->push_back(*pstr);
				++pstr;
			}
			*p_index = strtoul(p_var->c_str(),NULL,10);
		}
		while(*pstr && CJsToken::IsEmptyChar(*pstr))
			++pstr;
		if(']' == *pstr)
			++pstr;
	}
	else if(!CJsToken::IsEmptyChar(*pstr))
	{
		while(*pstr 
				&&(!CJsToken::IsEmptyChar(*pstr))
				&&('.' != *pstr)
				&&('\''!= *pstr)
				&&('\"'!= *pstr)
				&&('[' != *pstr)
				&&(']' != *pstr))
		{
			p_var->push_back(*pstr);
			++pstr;
		}
	}
	return	pstr;
}

bool	CJsParser::FindPath(char const * path_name)
{
	string	var_name;
	int	idx;
	int step = 0;

	char const * pstr = path_name;
	if(!IsValid())
		return	false;
	if(!ParseRoot())
		return	false;
	while(*pstr)
	{
		pstr = ParseSplit(pstr,&var_name,&idx);
		if(-1 == idx && "" == var_name)
			break;
		if(step)
		{
			if(!ParseSub())
				return	false;
		}
		if(-1 != idx)
		{
			if(!FindIndex(idx))
				return	false;
		}
		else if("" != var_name)
		{
			if(!FindVar(var_name.c_str()))
				return	false;
		}
		++step;
	}
	return	true;
}

bool	CJsParser::EnterObj()
{
	if(JS_PARSE_OBJ == parse_type)
	{
		return	ParseSub();
	}
	else
		return	false;
}

bool	CJsParser::EnterArray()
{
	if(JS_PARSE_ARRAY == parse_type)
	{
		return	ParseSub();
	}
	else
		return	false;
}


bool	CJsWriter::Init(char const * filename)
{
	FILE * f = NULL;

	if(!filename)
		return	false;

	Release();
	f = fopen(filename,"wb");
	if(NULL == f)
		return	false;
	file = f;
	type = JS_WRITE_FILEPATH;
	return	true;
}

bool	CJsWriter::Init(FILE * filehandle)
{
	if(!filehandle)
		return	false;
	Release();
	file = filehandle;
	type = JS_WRITE_FILEHANDLE;
	return	true;
}

bool	CJsWriter::Init(unsigned char * p,int size)
{
	if(NULL == p || size <= 0)
		return	false;
	Release();
	pdata = p;
	data_size = size;
	type = JS_WRITE_DATABUF;
	return	true;
}

void	CJsWriter::Release()
{
	if(JS_WRITE_FILEPATH == type)
	{
		if(file)
			fclose(file);
	}
	type = JS_WRITE_NONE;
	file = 0;
	pdata = 0;
	data_size = 0;
	js_str = "";
	indent = "";
	index = 0;
}

void	CJsWriter::WriteVar(char const * var_name)
{
	js_str += indent;
	js_str += "\"";
	js_str += var_name; 
	js_str += "\" : ";
}
void	CJsWriter::WriteValStr(char const * val_str)
{
	js_str += "\"";
	while(*val_str)
	{
		if('\"' == *val_str)
		{
			js_str += "\\\"";
		}
		else if('\'' == *val_str)
		{
			js_str += "\\\'";
		}
		else
			js_str.push_back(*val_str);
		++val_str;
	}
	js_str += "\" ";
}
void	CJsWriter::WriteArrayValStr(char const * val_str)
{
	js_str += indent;
	WriteValStr(val_str);
}
void	CJsWriter::WriteComma()
{
	js_str += ",\n";
}
void	CJsWriter::WriteLeftBrace()
{
	js_str += "\n";
	js_str += indent;
	js_str += "{\n";
	indent.push_back('\t');
}
void	CJsWriter::WriteLeftBracket()
{
	js_str += "\n";
	js_str += indent;
	js_str += "[\n";
	indent.push_back('\t');
}
void	CJsWriter::WriteRightBrace()
{
	if(indent.length())
		indent.erase(indent.end()-1);
	js_str += "\n";
	js_str += indent;
	js_str += "}\n";
}
void	CJsWriter::WriteRightBracket()
{
	if(indent.length())
		indent.erase(indent.end()-1);
	js_str += "\n";
	js_str += indent;
	js_str += "]\n";
}

bool	CJsWriter::Flush()
{
	if(JS_WRITE_DATABUF == type)
	{
		int wcnt = 0;
		wcnt = js_str.length();
		if(wcnt >= data_size)
			wcnt = data_size -1;
		memcpy(pdata,js_str.c_str(),wcnt);
		pdata[wcnt] = 0;
		return	true;
	}
	else if(JS_WRITE_FILEPATH == type || JS_WRITE_FILEHANDLE == type)
	{
		int wcnt = 0;
		wcnt = js_str.length();
		fseek(file,0,SEEK_SET);
		if(fwrite(js_str.c_str(),1,wcnt,file) != wcnt)
			return	false;
		fflush(file);
		return	true;
	}
	else
		return	false;
}
