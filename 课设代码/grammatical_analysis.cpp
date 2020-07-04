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
void grammar::error(string err_msg="")
{
    err_msg_list.push_back(err_msg);
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
{   current_level=0;
    current_level_stcak.push_back(current_level);//最外层的层次号为0
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
						//level_it++; 迭代器失效
						current_level_stcak.push_back(current_level);//层次加一，入栈
                        getToken();
                        functionBody();
                        if (w.token_code == PT && w.token_value == "}") {
							current_level_stcak.pop_back();
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
        
        
        //判断是否定义了
        int position=is_iT_defined(w.token_value);
        if(position<0){//若没有定义则进行报错
            error(w.token_value+" not defined");
        }

        //压入符号栈
        OPERAND iT_operand;
        iT_operand.name=w.token_value;
        iT_operand.position=position;
        operand_stack.push(iT_operand);

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

        //获得操作数
        OPERAND one=operand_stack.top();
        operand_stack.pop();
        OPERAND three=operand_stack.top();
        operand_stack.pop();

        //判断类型是否匹配
        TYPEL typel=type_deduction(synbl_list[one.position].TYPE.tval,synbl_list[three.position].TYPE.tval);
        if(typel.tval==TVAL::WRONG_TYPE){
            error(one.name+" and "+three.name+" type mismatch");
        }

        //构建赋值四元式
        QUATERNION quaternion;
        quaternion.sign=SIGN::equal;
        quaternion.operand_1=one;
        quaternion.operand_3=three;
        quaternion_list.push_back(quaternion);



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

        //弹出两个操作数
		OPERAND one,two;
		two = operand_stack.top();
	    operand_stack.pop();
		one = operand_stack.top();
        operand_stack.pop();
        
        //准备将临时变量填到符号表里
		OPERAND operand_temp=operand_temp_produce();
		SYNBL synbl_temp;
		synbl_temp.name=operand_temp.name;
        synbl_temp.TYPE=type_deduction(synbl_list[one.position].TYPE.tval,synbl_list[two.position].TYPE.tval);//结果的类型应该是通过两个操作数的类型进行推到
        if(synbl_temp.TYPE.tval==TVAL::WRONG_TYPE)//类型不匹配
            error("wrong type");
        synbl_temp.level=current_level_stcak.back();//level应该是作用域
		if(synbl_list[one.position].cat==c && synbl_list[two.position].cat==c)//只有两个操作数均为常数时，结果为常数，否则均为变量
            synbl_temp.cat=c;
		else synbl_temp.cat=v;
        operand_temp.position=push_into_synbel_list(synbl_temp);//用来替代迭代器

		//准备产生四元式
		QUATERNION q_temp;
        q_temp.operand_1=one;
		q_temp.operand_2=two;
		q_temp.operand_3=operand_temp;
		q_temp.sign=sign_stack.top();
		sign_stack.pop();//弹出操作符
		quaternion_list.push_back(q_temp);//压入四元式
		operand_stack.push(operand_temp);//将产生的临时变量压入对象栈

        //如果临时变量是常数的话，还需要填写常数表
        if(synbl_temp.cat==CAT::c){
            push_into_const_int_double_list(one,two,operand_temp,q_temp.sign);
        }
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

        //弹出两个操作数
		OPERAND one,two;
		two = operand_stack.top();
	    operand_stack.pop();
		one = operand_stack.top();
        operand_stack.pop();

        //准备将临时变量填到符号表里
		OPERAND operand_temp=operand_temp_produce();
		SYNBL synbl_temp;
		synbl_temp.name=operand_temp.name;
        synbl_temp.TYPE=type_deduction(synbl_list[one.position].TYPE.tval,synbl_list[two.position].TYPE.tval);//访问符号表，与第一，第二操作数的类型一致
        if(synbl_temp.TYPE.tval==TVAL::WRONG_TYPE)//类型不匹配的情况
            error("wrong type");
		synbl_temp.level=current_level_stcak.back();
		if(synbl_list[one.position].cat==c && synbl_list[two.position].cat==c)//只有两个操作数均为常数时，结果为常数，否则均为变量
		synbl_temp.cat=c;
		else synbl_temp.cat=v;
		operand_temp.position=push_into_synbel_list(synbl_temp);//压入符号表

		//准备产生四元式
		QUATERNION q_temp;
        q_temp.operand_1=one;
		q_temp.operand_2=two;
		q_temp.operand_3=operand_temp;
		q_temp.sign=sign_stack.top();
		sign_stack.pop();//弹出操作符
		quaternion_list.push_back(q_temp);//压入四元式
		operand_stack.push(operand_temp);//将产生的临时变量压入对象栈

        //如果临时变量是常数的话，还需要填写常数表
        if(synbl_temp.cat==CAT::c){
            push_into_const_int_double_list(one,two,operand_temp,q_temp.sign);
        }

        T1();
    }
}
void grammar::F()
{
    if (w.token_code == iT) {
        int position=is_iT_defined(w.token_value);
        if(position<0){//若没有定义则进行报错
            error(w.token_value+" not defined");
        }
        //压入符号栈
        OPERAND iT_operand;
        iT_operand.name=w.token_value;
        iT_operand.position=position;
        operand_stack.push(iT_operand);

        getToken();

        D();

    } else if (w.token_code == CT) {

        //判断常数的类型
        TVAL CT_tval=CT_type_deduction(w.token_value);

        //判断符号表中是否已经有该常数了
        int position=0;
        for(;position<synbl_list.size();position++){
            if(synbl_list[position].name==w.token_value&&synbl_list[position].TYPE.tval==CT_tval)
                break;
        }

        OPERAND CT_operand;
        //已经在常数表中了则不需要填写符号表
        if(position<synbl_list.size()){
            //压入操作数栈
            CT_operand.name=w.token_value;
            CT_operand.position=position;
            operand_stack.push(CT_operand);
        }else{//不在符号表中则需要填写符号表
            //填入符号表
            SYNBL synbel_temp;
		    synbel_temp.name = w.token_value;
            synbel_temp.cat=CAT::c;
            synbel_temp.level=0;
            synbel_temp.TYPE.tval=CT_tval;
            synbel_temp.addr.table=TABLE::const_int_double;
            synbel_temp.addr.position=const_int_double_list.size();
            const_int_double_list.push_back(atof(w.token_value.c_str()));
            //压入操作数栈
            CT_operand.name=w.token_value;
            CT_operand.position=push_into_synbel_list(synbel_temp);
            operand_stack.push(CT_operand);
        }
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
        //弹出操作数栈顶元素判断是否为数组类型
        OPERAND iT_operand=operand_stack.top();
        operand_stack.pop();
        if(synbl_list[iT_operand.position].TYPE.tval!=TVAL::Array){
            error(w.token_value+" is not array but used as array");
        }

        getToken();

        mathExpression();

        //弹出对象栈栈顶元素作为数组的下标 判断是否为整数和是否越界
        OPERAND subscript_operand=operand_stack.top();
        operand_stack.pop();
        SYNBL subscript_synbel=synbl_list[subscript_operand.position];
        if(subscript_synbel.TYPE.tval!=TVAL::Int){
            error("subscrip is not integer");
        }
        //如果为常数可以判断是否越界
        if(subscript_synbel.cat==CAT::c){
            int ainfl_position=synbl_list[iT_operand.position].TYPE.addr.position;
            int up_bound=ainfl_list[ainfl_position].up;
            if(const_int_double_list[subscript_synbel.addr.position]>up_bound){
                 error(iT_operand.name+" Array out of bounds");
            }
        }
        
        //数组元素操作填写符号表 构建数组元素的操作数并且压入对象栈
        SYNBL array_element_synbl;
        array_element_synbl.name=iT_operand.name+"["+subscript_operand.name+"]";
        array_element_synbl.cat=CAT::v;
        array_element_synbl.addr.table=TABLE::synbl;
        array_element_synbl.addr.position=iT_operand.position;
        array_element_synbl.level=current_level_stcak.back();
        int anifl_position=synbl_list[iT_operand.position].TYPE.addr.position;
        array_element_synbl.TYPE.tval=ainfl_list[anifl_position].tval;
        OPERAND array_element_operand;
        array_element_operand.name=array_element_synbl.name;
        array_element_operand.position=push_into_synbel_list(array_element_synbl);
        operand_stack.push(array_element_operand);

        


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
        //弹出操作数栈顶元素判断是否为数组类型
        OPERAND iT_operand=operand_stack.top();
        operand_stack.pop();
        if(synbl_list[iT_operand.position].TYPE.tval!=TVAL::Array){
            error(w.token_value+" is not array but used as array");
        }

        getToken();

        mathExpression();

        //弹出对象栈栈顶元素作为数组的下标 判断是否为整数和是否越界
        OPERAND subscript_operand=operand_stack.top();
        operand_stack.pop();
        SYNBL subscript_synbel=synbl_list[subscript_operand.position];
        if(subscript_synbel.TYPE.tval!=TVAL::Int){
            error("subscrip is not integer");
        }
        //如果为常数可以判断是否越界
        if(subscript_synbel.cat==CAT::c){
            int ainfl_position=synbl_list[iT_operand.position].TYPE.addr.position;
            int up_bound=ainfl_list[ainfl_position].up;
            if(const_int_double_list[subscript_synbel.addr.position]>up_bound){
                 error(iT_operand.name+" Array out of bounds");
            }
        }

        //数组元素操作填写符号表 构建数组元素的操作数并且压入对象栈
        SYNBL array_element_synbl;
        array_element_synbl.name=iT_operand.name+"["+subscript_operand.name+"]";
        array_element_synbl.cat=CAT::v;
        array_element_synbl.addr.table=TABLE::synbl;
        array_element_synbl.addr.position=iT_operand.position;
        array_element_synbl.level=current_level_stcak.back();
        int anifl_position=synbl_list[iT_operand.position].TYPE.addr.position;
        array_element_synbl.TYPE.tval=ainfl_list[anifl_position].tval;
        OPERAND array_element_operand;
        array_element_operand.name=array_element_synbl.name;
        array_element_operand.position=push_into_synbel_list(array_element_synbl);
        operand_stack.push(array_element_operand);

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
	synbl_list.push_back(s_temp);//填入符号表
	OPERAND operand_temp;
	operand_temp.name=temp.token_value;//变量的名字
	operand_temp.pointer=synbel_it;
	operand_stack.push(operand_temp);//标识符压入对象栈
    declaration_1();
}


//用来进行类型的推到 目前仅支持int double bool的推到 char和string默认返回WRONG_TYPE
TYPEL grammar::type_deduction(TVAL tval_1,TVAL tval_2)
{
    TVAL new_tval;
    if(tval_1==TVAL::Int && tval_2==TVAL::Int)
    {
        new_tval=TVAL::Int;
    }
    else if((tval_1==TVAL::Int && tval_2==TVAL::Double)||(tval_1==TVAL::Double && tval_2==TVAL::Int)||(tval_1==TVAL::Double && tval_2==TVAL::Double))
    {
        new_tval=TVAL::Double;
    }
    else if((tval_1==TVAL::Bool && tval_2==TVAL::Bool))
    {
        new_tval=TVAL::Bool;
    }
    else
    {
        new_tval=TVAL::WRONG_TYPE;
    }
    TYPEL new_typel;
    new_typel.tval=new_tval;
    return new_typel;
} 

//压入符号表
int grammar::push_into_synbel_list(SYNBL synbel)
{
    synbl_list.push_back(synbel); 
    return (int)synbl_list.size()-1;
}

//判断标识符是否已经定义 若定义了则返回符号表中的位置 没有定义返回 -1
int grammar::is_iT_defined(string iT_name)
{
    int position=-1;
    int current_level_=-1;
    for(unsigned i=0;i<synbl_list.size();i++){
        if(synbl_list[i].name==iT_name){
            for(unsigned t=0;t<current_level_stcak.size();t--){
                if(synbl_list[i].level==current_level_stcak[t]){
                    if(synbl_list[i].level>current_level_){ //优先返回深度更深的作用域的变量
                        position=i;
                        current_level_=synbl_list[i].level;
                    }
                    break;
                }
            }
        }
    }
    return position;
}

//填写常数表，如果临时变量为常数，则需要额外填写常数表，目前还没有做bool型的
void grammar::push_into_const_int_double_list(OPERAND one,OPERAND two,OPERAND three,SIGN sign)
{
    double operand_3;
    int operand_1=const_int_double_list[synbl_list[one.position].addr.position];
    int operand_2=const_int_double_list[synbl_list[two.position].addr.position];
    if(sign==SIGN::add){
        operand_3=operand_1+operand_1;
    }
    else if(sign==SIGN::sub){
        operand_3=operand_1-operand_1;
    }
    else if(sign==SIGN::div_){
        operand_3=operand_1/operand_1;
    }
    else if(sign==SIGN::multi){
        operand_3=operand_1*operand_1;
    }
    synbl_list[three.position].addr.table=TABLE::const_int_double;
    synbl_list[three.position].addr.position=const_int_double_list.size();
    const_int_double_list.push_back(operand_3);
}

//判断常数为double还是int类型
TVAL grammar::CT_type_deduction(string str)
{
    int CT_int=atoi(str.c_str());
    double CT_double=atof(str.c_str());
    if(CT_int<CT_double){
        return TVAL::Double;
    }
    else{
        return TVAL::Int;
    }
}
