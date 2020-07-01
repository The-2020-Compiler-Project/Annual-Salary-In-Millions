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
void grammar::error()
{
    cout << "unexpected word:" << w.token_value << endl;
    exit(1);
}
void grammar::getToken()
{
    w = lexic->getToken();
}
void grammar::begin()
{
    getToken();
    program();
    if (w.token_value == "#") {
        cout << "finished,no error";
    } else {
        error();
    }
}
void grammar::program()
{
    if (w.token_code == KT && w.token_value == "void") {
        getToken();
        if (w.token_code == KT && w.token_value == "main") {
            getToken();
            if (w.token_code == PT && w.token_value == "(") {
                getToken();
                functionBody();
                if (w.token_code == PT && w.token_value == ")") {
                    getToken();
                    return;
                } else {
                    error();
                }
            } else {
                error();
            }
        } else {
            error();
        }
    } else {
        error();
    }
}
void grammar::type()
{
    if (w.token_code == PT) {
        if (w.token_value == "int" || w.token_value == "char" || w.token_value == "bool" || w.token_value == "string" || w.token_value == "double") {
            getToken();
        } else {
            error();
        }
    } else {
        error();
    }
}

bool grammar::isType()
{
    if (w.token_code == PT) {
        if (w.token_value == "int" || w.token_value == "char" || w.token_value == "bool" || w.token_value == "string" || w.token_value == "double") {
            return true;
        }
    }
    return false;
}

void grammar::functionBody()
{
    if (w.token_code == iT) {
        getToken();
        A();
        functionBody();
    } else if (w.token_value == "if") {
        getToken();
        if (w.token_value == "(") {
            getToken();
            expression();
            if (w.token_value == ")") {
                getToken();
                if (w.token_value == "{") {
                    getToken();
                    functionBody();
                    if (w.token_value == "}") {
                        getToken();
                        B();
                        functionBody();
                    } else {
                        error();
                    }
                } else {
                    error();
                }
            } else {
                error();
            }
        } else {
            error();
        }
    } else if (w.token_value == "while") {
        getToken();
        if (w.token_value == "(") {
            getToken();
            expression();
            if (w.token_value == ")") {
                getToken();
                if (w.token_value == "{") {
                    getToken();
                    functionBody();
                    if (w.token_value == "}") {
                        getToken();
                        functionBody();
                    } else {
                        error();
                    }
                } else {
                    error();
                }
            } else {
                error();
            }
        } else {
            error();
        }
    } else if (isType()) {
        type();
        declaration();
        functionBody();
    } else {
        return;
    }
}
void grammar::expression()
{
    if (w.token_code == cT) {
        getToken();
    } else if (w.token_code == sT) {
        getToken();
    } else {
        mathExpression();
        logicExpression();
    }
}

void grammar::mathExpression()
{
    E();
}

void grammar::logicExpression()
{
    if (w.token_code == PT) {
        if (w.token_value == ">") {

        } else if (w.token_value == "<") {

        } else if (w.token_value == ">=") {

        } else if (w.token_value == "<=") {

        } else if (w.token_value == "&&") {

        } else if (w.token_value == "||") {

        } else if (w.token_value == "!=") {
        } else {
            error();
        }
        getToken();
        mathExpression();
    } else {
        return;
    }
}

void grammar::declaration()
{
    if (w.token_code == iT) {
        getToken();
        C();
        if (w.token_value == ";") {
            getToken();
        } else {
            error();
        }
    }
}

void grammar::declaration_1()
{
    if (w.token_value == ",") {
        getToken();
        if (w.token_code == iT) {
            declaration_1();
        } else {
            error();
        }
    } else if (true) {
        //判断是否是表达式
    }
}

void grammar::declaration_2()
{
    if (w.token_value == ",") {
        getToken();
        if (w.token_code == iT) {
            getToken();
            if (w.token_value == "[") {
                mathExpression();
                if (w.token_value == "]") {
                    getToken();
                    declaration_2();
                } else {
                    error();
                }
            } else {
                error();
            }
        } else {
            error();
        }
    } else if (w.token_value == "=") {
        getToken();
        if (w.token_value == "{") {
            getToken();
            expression();
            arrayInit();
            if (w.token_value == "}") {
                getToken();
                return;
            } else {
                error();
            }
        } else {
            error();
        }
    } else {
        return;
    }
}

void grammar::arrayInit()
{
    if (w.token_value == ",") {
        getToken();
        expression();
        arrayInit();
    } else {
        error();
    }
}

void grammar::assignment()
{
    if (w.token_value == "=") {
        getToken();
        expression();
        if (w.token_value == ";") {
            getToken();
            return;
        } else {
            error();
        }
    } else {
        error();
    }
}
