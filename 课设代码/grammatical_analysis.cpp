#include "grammatical_analysis.h"
#include "symbol_table.h"
//开始插入一些语义动作
// 对词法分析包装内函数的定义
lexic_wrapper::lexic_wrapper()
{
    lex = new lexic();
    lex->open_file();
    isSuccess = lex->lexic_analyze();
    lex->close_file();
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
        token result = { "#", -1 };
        return result;
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
    cout << "unexpected word: " << w.token_value << endl;
    exit(1);
}
void grammar::getToken()
{
    w = lexic->getToken();
}
OPERAND grammar::operand_temp_produce()
{
	temp_operand[1]++;
	string s(temp_operand);
	OPERAND temp;
	temp.name=s;
	return temp;//产生的临时变量的指针未初始化，由填符号表时来确定
}
void grammar::begin()
{
    getToken();
    program();
    if (w.token_value == "#") {
        cout << "finished,no error" << endl;
        return;
    } else {
        error();
    }
}
void grammar::program()
{ current_level_stcak.push_back(current_level);//最外层的层次号为0
    if (w.token_code == KT && w.token_value == "void") {
        getToken();
        if (w.token_code == KT && w.token_value == "main") {
            getToken();
            if (w.token_code == PT && w.token_value == "(") {
                getToken();
                if (w.token_code == PT && w.token_value == ")") {
                    getToken();
                    if (w.token_code == PT && w.token_value == "{") {
						current_level++;
						level_it++;
						current_level_stcak.push_back(current_level);//层次加一，入栈
                        getToken();
                        functionBody();
                        if (w.token_code == PT && w.token_value == "}") {
							current_level_stcak.erase(level_it);//离开，之前压入的层次号
                            level_it--;
                            getToken();
                            return;
                        } else {
                            error();
                        }
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
    } else {
        error();
    }
}
int grammar::type()
{
    if (w.token_code == KT) {
        if (w.token_value == "int" || w.token_value == "char" || w.token_value == "bool" || w.token_value == "string" || w.token_value == "double") {
			if(w.token_value=="int" )
			return Int;
			else if(w.token_value =="char")
			return Char;
			else if(w.token_value == "bool")
			return Bool;
			else if(w.token_value == "string")
			return String;
			else if(w.token_value == "double" )
			return Double;
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
    if (w.token_code == KT) {
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
    } else if (isType()) {//这部分用来判断开头是不是类型，为了填入符号表做准备
        int kind;//用来传类型
		kind=type();
        declaration(kind);//传入相应的类型给声明,填写符号表
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
    E();//算数表达式的接口直接传递到E处
}

void grammar::logicExpression()
{
    if (w.token_value == ">") {

    } else if (w.token_value == "<") {

    } else if (w.token_value == ">=") {

    } else if (w.token_value == "<=") {

    } else if (w.token_value == "&&") {

    } else if (w.token_value == "||") {

    } else if (w.token_value == "!=") {
    } else {
        return;
    }
    getToken();
    mathExpression();
}

void grammar::declaration(int kind)
{
    if (w.token_code == iT) {
		token temp=w;
        getToken();
        C(kind,temp);
        if (w.token_value == ";") {
            getToken();
        } else {
            error();
        }
    } else {
        error();
    }
}

void grammar::declaration_1()
{
    if (w.token_value == ",") {
        getToken();
        if (w.token_code == iT) {
            getToken();
            declaration_1();
        } else {
            error();
        }
    } else if (w.token_value == "=") {
        getToken();
        expression();
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
    }
}

void grammar::assignment()
{
    if (w.token_value == "=") {
        getToken();
        expression();
        if (w.token_value == ";") {
            getToken();
        } else {
            error();
        }
    } else {
        error();
    }
}
void grammar::E()
{
    T();
    E1();
}
void grammar::E1()
{
    if ((w.token_code == PT && w.token_value == "+") || (w.token_code == PT && w.token_value == "-")) {
		//将操作符保存到操作符栈中
		SIGN temp;
		if(w.token_value == "+")
		temp = add;
		else
		temp = sub;
		sign_stack.push(temp);
        getToken();
        T();//再其后生成四元式
		OPERAND one,two;//弹出两个操作数
		two = operand_stack.top();
	    operand_stack.pop();
		one = operand_stack.top();
		OPERAND operand_temp=operand_temp_produce();
		SYNBL synbl_temp;//准备将临时变量填到符号表里
		synbl_temp.name=operand_temp.name;
        synbl_temp.TYPE=two.pointer->TYPE;//访问符号表，与第一，第二操作数的类型一致
		synbl_temp.level=two.pointer->level;//地址问题先统一不处理
		if(one.pointer->cat==c && two.pointer->cat==c)//只有两个操作数均为常数时，结果为常数，否则均为变量
		synbl_temp.cat=c;
		else synbl_temp.cat=v;
		synbel_it++;
		operand_temp.pointer=synbel_it;
		synbel_list.push_back(synbl_temp);//将产生的临时变量填入符号表
		//准备产生四元式
		QUATERNION q_temp;
        q_temp.operand_1=one;
		q_temp.operand_2=two;
		q_temp.operand_3=operand_temp;
		q_temp.sign=sign_stack.top();
		sign_stack.pop();//弹出操作符
		quaternion_list.push_back(q_temp);//压入四元式
		operand_stack.push(operand_temp);//将产生的临时变量压入对象栈
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
    if ((w.token_code == PT && w.token_value == "*") || (w.token_code == PT && w.token_value == "/")) {
		SIGN temp;
		if(w.token_value == "*")
		temp = multi;
		else
		temp = div_;
		sign_stack.push(temp);
        getToken();
        F();
		OPERAND one,two;//弹出两个操作数
		two = operand_stack.top();
	    operand_stack.pop();
		one = operand_stack.top();
		OPERAND operand_temp=operand_temp_produce();
		SYNBL synbl_temp;//准备将临时变量填到符号表里
		synbl_temp.name=operand_temp.name;
        synbl_temp.TYPE=two.pointer->TYPE;//访问符号表，与第一，第二操作数的类型一致
		synbl_temp.level=two.pointer->level;//地址问题先统一不处理
		if(one.pointer->cat==c && two.pointer->cat==c)//只有两个操作数均为常数时，结果为常数，否则均为变量
		synbl_temp.cat=c;
		else synbl_temp.cat=v;
		synbel_it++;
		operand_temp.pointer=synbel_it;
		synbel_list.push_back(synbl_temp);//将产生的临时变量填入符号表
		//准备产生四元式
		QUATERNION q_temp;
        q_temp.operand_1=one;
		q_temp.operand_2=two;
		q_temp.operand_3=operand_temp;
		q_temp.sign=sign_stack.top();
		sign_stack.pop();//弹出操作符
		quaternion_list.push_back(q_temp);//压入四元式
		operand_stack.push(operand_temp);//将产生的临时变量压入对象栈
        T1();
    }
}
void grammar::F()
{
    if (w.token_code == iT) {
        getToken();
		//数组问题还没处理
        D();
    } else if (w.token_code == CT) {
		SYNBL synbel_temp;
		synbel_temp.name = w.token_value;//名字填的是常数的值
		//先均当成int常数处理，等后期再分类
		const_int_list.push_back(change_to_int(w.token_value));
		synbel_temp.cat=c;//常量
		synbel_temp.level=*level_it;
		synbel_temp.TYPE.tval=Int;
		synbel_temp.addr=const_int_list_it;//void*指针和迭代器转换之间存在问题，未解决
		OPERAND num;
		num.name = w.token_value;
        getToken();
    } else if (w.token_code == PT && w.token_value == "(") {
        getToken();
        E();
        if (w.token_code == PT && w.token_value == ")") {
            getToken();
            return;
        } else {
            error();
        }
    } else {
        error();
    }
}
void grammar::D()
{
    if (w.token_code == PT && w.token_value == "[") {
        getToken();
        mathExpression();
        if (w.token_code == PT && w.token_value == "]") {
            getToken();
            return;
        } else {
            error();
        }
    }
}
void grammar::A()
{
    if (w.token_code == PT && w.token_value == "[") {
        getToken();
        mathExpression();
        if (w.token_code == PT && w.token_value == "]") {
            getToken();
            assignment();
        } else {
            error();
        }
    }
    assignment();
}
void grammar::B()
{
    if (w.token_code == KT && w.token_value == "else") {
        getToken();
        if (w.token_code == PT && w.token_value == "{") {
            getToken();
            functionBody();
            if (w.token_code == PT && w.token_value == "}") {
                getToken();
            } else {
                error();
            }
        } else {
            error();
        }
    }
}
void grammar::C(int kind,token temp)
{
    if (w.token_code == PT && w.token_value == "[") {
        getToken();
        mathExpression();//未完成，先处理算数表达式那部分
        if (w.token_code == PT && w.token_value == "]") {
            getToken();
            declaration_2();
        } else {
            error();
        }
    }
//在此处填写符号表
	SYNBL s_temp;
	s_temp.name=temp.token_value;//变量的名字
	s_temp.TYPE.tval=Int;
	s_temp.cat = v;
	s_temp.level= *level_it;
	s_temp.addr = NULL;
	s_temp.offset_add=0;//对于变量v而言，其addr这部分需要跟着活动记录来实现
	synbel_list.push_back(s_temp);//填入符号表
	OPERAND operand_temp;
	operand_temp.name=temp.token_value;//变量的名字
	operand_temp.pointer=synbel_it;
	operand_stack.push(operand_temp);//标识符压入对象栈
    declaration_1();
}
int grammar::change_to_int(string &s)
{
	int temp;
	temp=atoi(s.c_str());
    return temp;
}
