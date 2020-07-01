#include "grammatical_analysis.h"
// 对词法分析包装内函数的定义
lexic_wrapper::lexic_wrapper()
{
    lex = new lexic();
    isSuccess = lex->lexic_analyze();
    position = lex->token_list.begin();
}
lexic_wrapper::~lexic_wrapper()
{
    delete lex;
}
token lexic_wrapper::getToken()
{
    if (position != lex->token_list.end()) {
        token result = *position;
        position++;
        return result;
    } else {
        return token { "", 0 };
    }
}
// 对语法分析类内函数的定义
grammar::grammar()
{
    lexic = new lexic_wrapper();
}
grammar::~grammar()
{
    delete lexic;
}
void grammar::getToken()
{
    w = lexic->getToken();
}
void grammar::error()
{
	cout << "unexpected word:"<<w.token_value<<endl;
	exit(1);
}
void grammar::E()
{
  T();
  E1();
}
void grammar::E1()
{
	if((w.token_code == PT && w.token_value == "+")||(w.token_code == PT && w.token_value == "-"))
	{
		getToken();
		T();
		E1();
	}
}
void grammar::T()
{
	F();
	T1();
}
void grammar::T1()
{
	if((w.token_code == PT && w.token_value == "*")||(w.token_code == PT && w.token_value == "/"))
	{
		getToken();
		F();
		T1();
	}
}
void grammar::F()
{
	if(w.token_code == iT)
	{
		getToken();
		D();
	}
	else if(w.token_code == CT)
	{
		getToken();
	}
	else if(w.token_code == PT && w.token_value == "(")
	{
		getToken();
		E();
		if(w.token_code == PT && w.token_value == ")")
		{
			getToken();
			return ;
		}
		else
		 {
			 error();
		 }
	}
	else
	{
		error();
	}
}
void grammar::D()
{
	if(w.token_code == PT && w.token_value=="[")
	{
		getToken();
		mathExpression();
		if(w.token_code == PT && w.token_value == "]")
		{
			getToken();
			return;
		}
		else{
			error();
		}
	}
}
void grammar::A()
{
	if(w.token_code == PT && w.token_value =="[")
	{
		getToken();
		mathExpression();
		if(w.token_code == PT && w.token_value =="]")
		{
			getToken();
			assignment();
		}
		else{
			error();
		}
	}
	assignment();
}
void grammar::B()
{
	if(w.token_code == iT && w.token_value == "else")
	{
		getToken();
		if(w.token_code == PT && w.token_value == "{")
		{
			getToken();
			functionBody();
			if(w.token_code == PT && w.token_value == "}")
			{
				getToken();
			}
			else
			{
				error();
			}
		}
		else
		{
			error();
		}
	}
}
void grammar::C()
{
	if(w.token_code == PT && w.token_value=="[")
	{
		getToken();
		mathExpression();
		if(w.token_code == PT && w.token_value =="]")
		{
			getToken();
			declaration_2();
		}
		else
		{
			error();
		}
	}
	declaration_1();
}
