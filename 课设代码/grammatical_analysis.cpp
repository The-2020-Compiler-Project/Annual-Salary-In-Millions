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
