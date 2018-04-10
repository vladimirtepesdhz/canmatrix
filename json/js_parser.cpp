#include <stdio.h>
#include <stdlib.h>


#include "js_parser.h"

bool	CJsInput::Init(char const * file_path)
{
	file = fopen(file_path,"r");
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
		if(!IsEmptyChar(cur_char))
			return cur_char;
	}
	return	EOF;
}
bool	CJsToken::ParseNext()
{
	int cur_char = 0;
	cur_char = ParseEmpty();
	if(EOF == cur_char)
	{
		type = JS_TOKEN_NULL;
		return	false;
	}
	pos = input->GetPos();
	line_num = input->GetLineNum();
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

	p_cur = &stack.back();
	input.Seek(p_cur->start_pos);
	input.SetLineNum(p_cur->start_line);
	p_cur->cur_pos = p_cur->start_pos;
	p_cur->cur_line = p_cur->line_num;

	index = -1;
	var = "";
	parse_type = JS_PARSE_NULL;
	value = "";
	
	return	true;
}

bool	CJsParser::ParseNext()
{
	CStackInfo * p_cur = 0;
	CJsToken token;

	p_cur = &stack.back();
	token.Init(&input);

	if(JS_PARSE_OBJ == parse_type || JS_PARSE_ARRAY == parse_type)
		TokenPass(parse_type);

	var = "";
	parse_type = JS_PARSE_ERROR;
	value = "";

	if(JS_PARSE_OBJ == p_cur->type)
	{
		if(!token.ParseNext())
			return	false;
		if(CJsToken::JS_TOKEN_STRBLK != token.GetType())	//解析变量名
			return	false;
		var = token.GetStr();

		if(!token.ParseNext())
			return	false;
		if(CJsToken::JS_TOKEN_COLON != token.GetType())		//解析冒号/等于号
			return	false;

	}
	else if(JS_PARSE_ARRAY == p_cur->type)
	{
		if(-1 != index)
		{
			if(!token.ParseNext())
				return	false;
			if(CJsToken::JS_TOKEN_COMMA != token.GetType())
				return	false;
		}
	}
	else
		return	false;

	if(!token.ParseNext())
		return	false;
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

		case	CJsToken::JS_TOKEN_LEFT_BRACE:	//数组变量
		{
			parse_type = JS_PARSE_ARRAY;
		}
		break;
	}
	++index;
	p_cur->cur_pos = input.GetPos();
	p_cur->cur_line = input.GetLineNum();

	return	true;
}

bool	CJsParser::ParseSub()
{
	return	true;
}

bool	CJsParser::ParseUpper()
{
	return	true;
}

