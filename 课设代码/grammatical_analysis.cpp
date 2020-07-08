#include "grammatical_analysis.h"
//开始插入一些语义动�?
// 对词法分析包装内函数的定�?

vector<AINFL> ainfl_list;
vector<RINFL> rinfl_list;
vector<FPL> fpl_list;
vector<PFINFL> pfinfl_list;
int current_level=0;
stack<SIGN> sign_stack;
stack<OPERAND> operand_stack;
vector<string> err_msg_list;
vector<QUATERNION> quaternion_list;
vector<TYPEL> typel_list;
vector<double> const_int_double_list;
vector<string> const_char_list;
vector<string> const_string_list;
vector<int> LENL;
vector<SYNBL> synbl_list;
vector <int> current_level_stack;   


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
    cout<<err_msg.back();
    cout << "unexpected word: " << w.token_value << endl;
    system("pause");
    //exit(1);
}
void grammar::getToken()
{
    w = lexic->getToken();
}
OPERAND grammar::operand_temp_produce()
{
	temp_operand_num++;
	string s=to_string(temp_operand_num)+"t";
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
    current_level_stack.push_back(current_level);//最外层的层次号�?0
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
						//level_it++; 迭代器失�?
						current_level_stack.push_back(current_level);//层次加一，入�?
                        getToken();
                        functionBody();
                        if (w.token_code == PT && w.token_value == "}") {
							current_level_stack.pop_back();
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
TVAL grammar::type()
{
    if (w.token_code == KT) {
        if (w.token_value == "int" || w.token_value == "char" || w.token_value == "bool" || w.token_value == "string" || w.token_value == "double") {
			if(w.token_value=="int" ){
                getToken();
			    return TVAL::Int;
            }
			else if(w.token_value =="char"){
                getToken();
                return TVAL::Char;
            }
			else if(w.token_value == "bool"){
                getToken();
                return TVAL::Bool;
            }
			else if(w.token_value == "string"){
                getToken();
                return TVAL::String;
            }
			else if(w.token_value == "double" ){
                return TVAL::Double;
                getToken();
            }
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
        
        
        //判断是否定义�?
        int position=is_iT_defined(w.token_value);
        if(position<0){//若没有定义则进行报错
            error(w.token_value+" not defined");
        }

        //压入符号�?
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

                //生成if四元�?
                OPERAND condition=operand_stack.top();
                operand_stack.pop();
                QUATERNION if_quaternion;
                if_quaternion.sign=SIGN::if_;
                if_quaternion.operand_1=condition;
                quaternion_list.push_back(if_quaternion);

                getToken();
                if (w.token_value == "{") {
                    //作用域栈++
                    current_level++;
                    current_level_stack.push_back(current_level);
                    getToken();
                    functionBody();
                    //生成ie四元�?
                    QUATERNION ie_quaternion;
                    ie_quaternion.sign=SIGN::ie;
                    quaternion_list.push_back(ie_quaternion);

                    if (w.token_value == "}") {

                        //作用域栈--
                        current_level_stack.pop_back();

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

        //生成wh四元�?
        QUATERNION wh_quaternion;
        wh_quaternion.sign=SIGN::wh;
        quaternion_list.push_back(wh_quaternion);

        getToken();
        if (w.token_value == "(") {
            getToken();
            expression();
            if (w.token_value == ")") {

                //生成do四元�?
                OPERAND condition=operand_stack.top();
                operand_stack.pop();
                QUATERNION do_quaternion;
                do_quaternion.sign=SIGN::do_;
                do_quaternion.operand_1=condition;
                quaternion_list.push_back(do_quaternion);

                getToken();
                if (w.token_value == "{") {
                    
                    //作用域栈++
                    current_level++;
                    current_level_stack.push_back(current_level);

                    getToken();
                    functionBody();
                    if (w.token_value == "}") {

                        //作用域栈--
                        current_level_stack.pop_back();

                        getToken();
                        functionBody();

                        //生成ie四元�?
                        QUATERNION we_quaternion;
                        we_quaternion.sign=SIGN::we;
                        quaternion_list.push_back(we_quaternion);

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
    } else if (isType()) {//这部分用来判断开头是不是类型，为了填入符号表做准�?
        TVAL kind;//用来传类�?
		kind=type();//gettoken在type�?
        declaration(kind);//传入相应的类型给声明,填写符号�?
        functionBody();
    } else {
        return;
    }
}
void grammar::expression()
{
    if (w.token_code == cT) {
        
        //判断符号表中是否已经有该常数
        int position=0;
        for(;position<synbl_list.size();position++){
            if(synbl_list[position].name==w.token_value&&synbl_list[position].TYPE.tval==TVAL::Char)
                break;
        }

        OPERAND cT_operand;
        //已经在常数表中了则不需要填写符号表
        if(position<synbl_list.size()){
            //压入操作数栈
            cT_operand.name=w.token_value;
            cT_operand.position=position;
            operand_stack.push(cT_operand);
        }else{//不在符号表中则需要填写符号表
            //填入符号�?
            SYNBL synbel_temp;
		    synbel_temp.name = w.token_value;
            synbel_temp.cat=CAT::c;
            synbel_temp.level=0;
            synbel_temp.TYPE.tval=TVAL::Char;
            synbel_temp.addr.table=TABLE::const_char;
            synbel_temp.addr.position=const_char_list.size();
            const_char_list.push_back(w.token_value);
            //压入操作数栈
            cT_operand.name=w.token_value;
            cT_operand.position=push_into_synbel_list(synbel_temp);
            operand_stack.push(cT_operand);
        }

        getToken();
    } else if (w.token_code == sT) {
        
        //判断符号表中是否已经有该常数
        int position=0;
        for(;position<synbl_list.size();position++){
            if(synbl_list[position].name==w.token_value&&synbl_list[position].TYPE.tval==TVAL::String)
                break;
        }

        OPERAND ST_operand;
        //已经在常数表中了则不需要填写符号表
        if(position<synbl_list.size()){
            //压入操作数栈
            ST_operand.name=w.token_value;
            ST_operand.position=position;
            operand_stack.push(ST_operand);
        }else{//不在符号表中则需要填写符号表
            //填入符号�?
            SYNBL synbel_temp;
		    synbel_temp.name = w.token_value;
            synbel_temp.cat=CAT::c;
            synbel_temp.level=0;
            synbel_temp.TYPE.tval=TVAL::String;
            synbel_temp.addr.table=TABLE::const_string;
            synbel_temp.addr.position=const_string_list.size();
            const_char_list.push_back(w.token_value);
            //压入操作数栈
            ST_operand.name=w.token_value;
            ST_operand.position=push_into_synbel_list(synbel_temp);
            operand_stack.push(ST_operand);
        }

        getToken();
    } else {
        mathExpression();
        logicExpression();
    }
}

void grammar::mathExpression()
{
    E();//算数表达式的接口直接传递到E�?
}

void grammar::logicExpression()
{
    if (w.token_value == ">") {
        sign_stack.push(SIGN::larger);

    } else if (w.token_value == "<") {
        sign_stack.push(SIGN::smaller);

    } else if (w.token_value == ">=") {
        sign_stack.push(SIGN::larger_equal);

    } else if (w.token_value == "<=") {
        sign_stack.push(SIGN::smaller_equal);

    } else if (w.token_value == "&&") {
        sign_stack.push(SIGN::and_);

    } else if (w.token_value == "||") {
        sign_stack.push(SIGN::or_);

    } else if (w.token_value == "!=") {
        sign_stack.push(SIGN::not_equal);
    } else if (w.token_value == "=="){
         sign_stack.push(SIGN::is_equal);

    } else {
        return;
    }
    getToken();
    mathExpression();

    //生成四元�?
    //弹出两个操作�?
    OPERAND one,two;
    two = operand_stack.top();
    operand_stack.pop();
    one = operand_stack.top();
    operand_stack.pop();

    //准备将临时变量填到符号表�?
    OPERAND operand_temp=operand_temp_produce();
    SYNBL synbl_temp;
    synbl_temp.name=operand_temp.name;
    synbl_temp.TYPE=type_deduction(synbl_list[one.position].TYPE.tval,synbl_list[two.position].TYPE.tval);//访问符号表，与第一，第二操作数的类型一�?
    if(synbl_temp.TYPE.tval==TVAL::WRONG_TYPE||synbl_temp.TYPE.tval==TVAL::Char||synbl_temp.TYPE.tval==TVAL::String)//类型不匹配的情况
        error("wrong type");
    synbl_temp.level=current_level_stack.back();
    if(synbl_list[one.position].cat==c && synbl_list[two.position].cat==c)//只有两个操作数均为常数时，结果为常数，否则均为变�?
    synbl_temp.cat=CAT::c;
    else synbl_temp.cat=CAT::v;
    operand_temp.position=push_into_synbel_list(synbl_temp);//压入符号�?

    //准备产生四元�?
    QUATERNION q_temp;
    q_temp.operand_1=one;
    q_temp.operand_2=two;
    q_temp.operand_3=operand_temp;
    q_temp.sign=sign_stack.top();
    sign_stack.pop();//弹出操作�?
    quaternion_list.push_back(q_temp);//压入四元�?
    operand_stack.push(operand_temp);//将产生的临时变量压入对象�?

    //如果临时变量是常数的话，还需要填写常数表
    if(synbl_temp.cat==CAT::c){
        push_into_const_int_double_list(one,two,operand_temp,q_temp.sign);
    }
    

}

void grammar::declaration(TVAL tval)
{
    if (w.token_code == iT) {
        //判断在当前作用域中是否已经定义过
        if(is_iT_defined_in_current_level(w.token_value)){
            error(w.token_value+" redifined");
        }
		token temp=w;
        getToken();
        C(tval,temp);
        if (w.token_value == ";") {
            getToken();
        } else {
            error();
        }
    } else {
        error();
    }
}

void grammar::declaration_1(TVAL tval)
{
    if (w.token_value == ",") {

        //弹出对象栈栈顶元�?
        operand_stack.pop();

        getToken();
        if (w.token_code == iT) {

            //判断是否已经定义�?
            if(is_iT_defined_in_current_level(w.token_value)){
                error(w.token_value+" redefined");
            }
            //填写符号�?
            SYNBL synbl;
            synbl.name=w.token_value;
            synbl.cat=CAT::v;
            synbl.level=current_level_stack.back();
            synbl.TYPE.tval=tval;
            //压入操作数栈
            OPERAND operand;
            operand.name=w.token_value;
            operand.position=push_into_synbel_list(synbl);
            operand_stack.push(operand);
            declaration_1(tval);

            getToken();
            declaration_1(tval);
        } else {
            error();
        }
    } else if (w.token_value == "=") {
        getToken();
        expression();

        //获得操作�?
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

    }
}
void grammar::declaration_2(TVAL tval)
{
    if (w.token_value == ",") {

        //弹出对象栈栈顶元�?
        operand_stack.pop();

        getToken();
        if (w.token_code == iT) {

            //判断是否已经定义�?
            if(is_iT_defined_in_current_level(w.token_value)){
                error(w.token_value+" redefined");
            }

            //保存标识�?
            token temp=w;

            getToken();
            if (w.token_value == "[") {
                getToken();
                mathExpression();
                if (w.token_value == "]") {

                    //弹出栈顶元素作为数组的大�?
                    OPERAND subscrip_operand= operand_stack.top();
                    operand_stack.pop();
                    //判断是否为整形常�?
                    if(synbl_list[subscrip_operand.position].cat!=CAT::c){
                        error(temp.token_value+" declar with not const");
                    }else if (synbl_list[subscrip_operand.position].TYPE.tval!=TVAL::Int){
                        error(temp.token_value+" declar with not integer");
                    }
                    
                    //获取数组大小
                    int const_position=synbl_list[subscrip_operand.position].addr.position;
                    int subscrip=const_int_double_list[const_position];
                    //如果�?0
                    if(subscrip==0){
                        error(w.token_value+" array declear with 0");
                    }
                    
                    //填写符号�?
                    SYNBL array_synbl;
                    array_synbl.name=temp.token_value;
                    array_synbl.cat=CAT::v;
                    array_synbl.level=current_level_stack.back();
                    //array_synbl.addr.table=TABLE::lenl;////填写长度�?
                    //填写数组�?
                    array_synbl.TYPE.tval=TVAL::Array;
                    AINFL ainfl;
                    ainfl.tval=tval;
                    ainfl.low=0;
                    ainfl.up=subscrip-1;
                    ainfl.clen=change_type_to_length(tval);
                    array_synbl.TYPE.addr.position=ainfl_list.size();
                    array_synbl.TYPE.addr.table=TABLE::ainfl;
                    ainfl_list.push_back(ainfl);

                    //压入操作数栈
                    OPERAND operand;
                    operand.name=temp.token_value;
                    operand.position=push_into_synbel_list(array_synbl);
                    operand_stack.push(operand);

                    getToken();
                    declaration_2(tval);
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

        //弹出对象栈栈顶元素，获得需要初始化的数组A的下标以及偏移地址和类型，通过一个整�? i 来记录当前初始化的个�?
        OPERAND operand_array=operand_stack.top();
        operand_stack.pop();
        //活动数组的上�?
        int anifl_positon=synbl_list[operand_array.position].TYPE.addr.position;
        int max_subscrip=ainfl_list[anifl_positon].up;
        int current_subscrip=0;

        //数组元素操作填写符号�? 构建数组元素的操作数并且压入对象�? 存在数组元素重复填写符号表的问题
        SYNBL array_element_synbl;
        array_element_synbl.name=operand_array.name+"["+to_string(current_subscrip)+"]";
        array_element_synbl.cat=CAT::v;
        array_element_synbl.addr.table=TABLE::synbl;
        array_element_synbl.addr.position=operand_array.position;
        array_element_synbl.level=current_level_stack.back();
        array_element_synbl.TYPE.tval=tval;
        OPERAND array_element_operand;
        array_element_operand.name=array_element_synbl.name;
        array_element_operand.position=push_into_synbel_list(array_element_synbl);
        operand_stack.push(array_element_operand);

        getToken();
        if (w.token_value == "{") {
            getToken();
            expression();

            //获得操作�?
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

            //初始化的位置 ++
            current_subscrip++;

            //数组元素操作填写符号�? 构建数组元素的操作数并且压入对象�? 存在数组元素重复填写符号表的问题
            SYNBL array_element_synbl;
            array_element_synbl.name=operand_array.name+"["+to_string(current_subscrip)+"]";
            array_element_synbl.cat=CAT::v;
            array_element_synbl.addr.table=TABLE::synbl;
            array_element_synbl.addr.position=operand_array.position;
            array_element_synbl.level=current_level_stack.back();
            array_element_synbl.TYPE.tval=tval;
            OPERAND array_element_operand;
            array_element_operand.name=array_element_synbl.name;
            array_element_operand.position=push_into_synbel_list(array_element_synbl);
            operand_stack.push(array_element_operand);

            arrayInit(operand_array,tval,max_subscrip,current_subscrip);
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
void grammar::arrayInit(OPERAND operand_array,TVAL tval,int max_subscrip,int current_subscrip)
{
    if (w.token_value == ",") {
        if(current_subscrip>max_subscrip){
            error("Assignment exceeds");
        }
        getToken();
        expression();

        //获得操作�?
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

        //初始化的位置 ++
        current_subscrip++;

        //数组元素操作填写符号�? 构建数组元素的操作数并且压入对象�? 存在数组元素重复填写符号表的问题 符号表中可能有多余的元素 和错误元�?
        SYNBL array_element_synbl;
        array_element_synbl.name=operand_array.name+"["+to_string(current_subscrip)+"]";
        array_element_synbl.cat=CAT::v;
        array_element_synbl.addr.table=TABLE::synbl;
        array_element_synbl.addr.position=operand_array.position;
        array_element_synbl.level=current_level_stack.back();
        array_element_synbl.TYPE.tval=tval;
        OPERAND array_element_operand;
        array_element_operand.name=array_element_synbl.name;
        array_element_operand.position=push_into_synbel_list(array_element_synbl);
        operand_stack.push(array_element_operand);

        arrayInit(operand_array,tval,max_subscrip,current_subscrip);
    }else{
        operand_stack.pop();
    }
    
}
void grammar::assignment()
{
    if (w.token_value == "=") {
        getToken();
        expression();

        //获得操作�?
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
		temp = SIGN::add;
		else
		temp = SIGN::sub;
		sign_stack.push(temp);
        getToken();

        T();//再其后生成四元式

        //弹出两个操作�?
		OPERAND one,two;
		two = operand_stack.top();
	    operand_stack.pop();
		one = operand_stack.top();
        operand_stack.pop();
        
        //准备将临时变量填到符号表�?
		OPERAND operand_temp=operand_temp_produce();
		SYNBL synbl_temp;
		synbl_temp.name=operand_temp.name;
        synbl_temp.TYPE=type_deduction(synbl_list[one.position].TYPE.tval,synbl_list[two.position].TYPE.tval);//结果的类型应该是通过两个操作数的类型进行推到
        if(synbl_temp.TYPE.tval==TVAL::WRONG_TYPE)//类型不匹�?
            error("wrong type");
        synbl_temp.level=current_level_stack.back();//level应该是作用域
		if(synbl_list[one.position].cat==c && synbl_list[two.position].cat==c)//只有两个操作数均为常数时，结果为常数，否则均为变�?
            synbl_temp.cat=CAT::c;
		else synbl_temp.cat=CAT::v;
        operand_temp.position=push_into_synbel_list(synbl_temp);//用来替代迭代�?

		//准备产生四元�?
		QUATERNION q_temp;
        q_temp.operand_1=one;
		q_temp.operand_2=two;
		q_temp.operand_3=operand_temp;
		q_temp.sign=sign_stack.top();
		sign_stack.pop();//弹出操作�?
		quaternion_list.push_back(q_temp);//压入四元�?
		operand_stack.push(operand_temp);//将产生的临时变量压入对象�?

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
		temp = SIGN::multi;
		else
		temp = SIGN::div_;
		sign_stack.push(temp);
        getToken();

        F();

        //弹出两个操作�?
		OPERAND one,two;
		two = operand_stack.top();
	    operand_stack.pop();
		one = operand_stack.top();
        operand_stack.pop();

        //准备将临时变量填到符号表�?
		OPERAND operand_temp=operand_temp_produce();
		SYNBL synbl_temp;
		synbl_temp.name=operand_temp.name;
        synbl_temp.TYPE=type_deduction(synbl_list[one.position].TYPE.tval,synbl_list[two.position].TYPE.tval);//访问符号表，与第一，第二操作数的类型一�?
        if(synbl_temp.TYPE.tval==TVAL::WRONG_TYPE)//类型不匹配的情况
            error("wrong type");
		synbl_temp.level=current_level_stack.back();
		if(synbl_list[one.position].cat==c && synbl_list[two.position].cat==c)//只有两个操作数均为常数时，结果为常数，否则均为变�?
		synbl_temp.cat=CAT::c;
		else synbl_temp.cat=CAT::v;
		operand_temp.position=push_into_synbel_list(synbl_temp);//压入符号�?

		//准备产生四元�?
		QUATERNION q_temp;
        q_temp.operand_1=one;
		q_temp.operand_2=two;
		q_temp.operand_3=operand_temp;
		q_temp.sign=sign_stack.top();
		sign_stack.pop();//弹出操作�?
		quaternion_list.push_back(q_temp);//压入四元�?
		operand_stack.push(operand_temp);//将产生的临时变量压入对象�?

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
        //压入符号�?
        OPERAND iT_operand;
        iT_operand.name=w.token_value;
        iT_operand.position=position;
        operand_stack.push(iT_operand);

        getToken();

        D();

    } else if (w.token_code == CT) {

        //判断常数的类�?
        TVAL CT_tval=CT_type_deduction(w.token_value);

        //判断符号表中是否已经有该常数�?
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
            //填入符号�?
            SYNBL synbel_temp;
		    synbel_temp.name = w.token_value;
            synbel_temp.cat=CAT::c;
            synbel_temp.level=0;//常数的为0
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
        //如果为常数可以判断是否越�?
        if(subscript_synbel.cat==CAT::c){
            int ainfl_position=synbl_list[iT_operand.position].TYPE.addr.position;
            int up_bound=ainfl_list[ainfl_position].up;
            if(const_int_double_list[subscript_synbel.addr.position]>up_bound){
                 error(iT_operand.name+" Array out of bounds");
            }
        }
        
        //数组元素操作填写符号�? 构建数组元素的操作数并且压入对象�?
        SYNBL array_element_synbl;
        array_element_synbl.name=iT_operand.name+"["+subscript_operand.name+"]";
        array_element_synbl.cat=CAT::v;
        array_element_synbl.addr.table=TABLE::synbl;
        array_element_synbl.addr.position=iT_operand.position;
        array_element_synbl.level=current_level_stack.back();
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
        //如果为常数可以判断是否越�?
        if(subscript_synbel.cat==CAT::c){
            int ainfl_position=synbl_list[iT_operand.position].TYPE.addr.position;
            int up_bound=ainfl_list[ainfl_position].up;
            if(const_int_double_list[subscript_synbel.addr.position]>up_bound){
                 error(iT_operand.name+" Array out of bounds");
            }
        }


        //数组元素可能重复入符号表�?

        //数组元素操作填写符号�? 构建数组元素的操作数并且压入对象�?
        SYNBL array_element_synbl;
        array_element_synbl.name=iT_operand.name+"["+subscript_operand.name+"]";
        array_element_synbl.cat=CAT::v;
        array_element_synbl.addr.table=TABLE::synbl;
        array_element_synbl.addr.position=iT_operand.position;
        array_element_synbl.level=current_level_stack.back();
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
    }else{
        assignment();
    }
}
void grammar::B()
{
    if (w.token_code == KT && w.token_value == "else") {
        
        //生成else四元�?
        QUATERNION else_quaternion;
        else_quaternion.sign=SIGN::else_;
        quaternion_list.push_back(else_quaternion);

        getToken();
        if (w.token_code == PT && w.token_value == "{") {

            //作用域栈++
            current_level++;
            current_level_stack.push_back(current_level);

            getToken();
            functionBody();
            if (w.token_code == PT && w.token_value == "}") {

                //作用域栈--
                current_level_stack.pop_back();

                getToken();
            } else {
                error();
            }
        } else {
            error();
        }
    }
}
void grammar::C(TVAL kind,token temp)
{
    if (w.token_code == PT && w.token_value == "[") {
        getToken();
        mathExpression();//未完成，先处理算数表达式那部�?
        if (w.token_code == PT && w.token_value == "]") {

            //弹出栈顶元素作为数组的大�?
            OPERAND subscrip_operand= operand_stack.top();
            operand_stack.pop();
            //判断是否为整形常�?
            if(synbl_list[subscrip_operand.position].cat!=CAT::c){
                error(temp.token_value+" declar with not const");
            }else if (synbl_list[subscrip_operand.position].TYPE.tval!=TVAL::Int){
                error(temp.token_value+" declar with not integer");
            }
            //获取数组大小
            int const_position=synbl_list[subscrip_operand.position].addr.position;
            int subscrip=const_int_double_list[const_position];
            
            //填写符号�?
            SYNBL array_synbl;
            array_synbl.name=temp.token_value;
            array_synbl.cat=CAT::v;
            array_synbl.level=current_level_stack.back();
            //array_synbl.addr.table=TABLE::lenl;////填写长度�?
            //填写数组�?
            array_synbl.TYPE.tval=TVAL::Array;
            AINFL ainfl;
            ainfl.tval=kind;
            ainfl.low=0;
            ainfl.up=subscrip-1;
            ainfl.clen=change_type_to_length(kind);
            array_synbl.TYPE.addr.position=ainfl_list.size();
            array_synbl.TYPE.addr.table=TABLE::ainfl;
            ainfl_list.push_back(ainfl);

            //压入操作数栈
            OPERAND operand;
            operand.name=temp.token_value;
            operand.position=push_into_synbel_list(array_synbl);
            operand_stack.push(operand);

            getToken();
            declaration_2(kind);
        } else {
            error();
        }
    }else{

        //填写符号�?
        SYNBL synbl;
        synbl.name=temp.token_value;
        synbl.cat=CAT::v;
        synbl.level=current_level_stack.back();
        synbl.TYPE.tval=kind;
        //压入操作数栈
        OPERAND operand;
        operand.name=temp.token_value;
        operand.position=push_into_synbel_list(synbl);
        operand_stack.push(operand);
        declaration_1(kind);
    }
    
}


//用来进行类型的推�? 目前仅支持int double bool的推�? char和string默认返回WRONG_TYPE
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
    else if((tval_1==TVAL::Char && tval_2==TVAL::Char))
    {
        new_tval=TVAL::Char;
    }
    else if((tval_1==TVAL::String && tval_2==TVAL::String))
    {
        new_tval=TVAL::String;
    }
    else
    {
        new_tval=TVAL::WRONG_TYPE;
    }
    TYPEL new_typel;
    new_typel.tval=new_tval;
    return new_typel;
} 

//压入符号�?
int grammar::push_into_synbel_list(SYNBL synbel)
{
    synbl_list.push_back(synbel); 
    return (int)synbl_list.size()-1;
}

//判断标识符是否已经定�? 若定义了则返回符号表中的位置 没有定义返回 -1
int grammar::is_iT_defined(string iT_name)
{
    int position=-1;
    int current_level_=-1;
    for(unsigned i=0;i<synbl_list.size();i++){
        if(synbl_list[i].name==iT_name){
            for(unsigned t=0;t<current_level_stack.size();t++){
                if(synbl_list[i].level==current_level_stack[t]){
                    if(synbl_list[i].level>current_level_){ //优先返回深度更深的作用域的变�?
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

//判断标识符在当前作用域中是否已经定义�?
bool grammar::is_iT_defined_in_current_level(string name)
{
    for(int i=0;i<synbl_list.size();i++){
        if(synbl_list[i].name==name&&synbl_list[i].level==current_level_stack.back()){
            return true;
        }
    }
    return false;
}

//获得类型的字节长�?
int grammar::change_type_to_length(TVAL tval)
{
    switch (tval)
    {
    case TVAL::Bool :
        return 1;
        break;
    case TVAL::Int :
        return 1;
        break;
    case TVAL::Double :
        return 2;
        break;
    case TVAL::Char :
        return 1;
        break;
    case TVAL::String :
        return 1;
        break;
    
    default:
        break;
    }
}

//输出四元�?
void grammar::print_quaternion_list()
{
    for(int i=0;i<quaternion_list.size();i++)
    {
        QUATERNION q=quaternion_list[i];
        string sign=change_sign_to_string(q.sign);
        string one=q.operand_1.name;
        if(q.operand_1.name=="")
            one="_";
        string two=q.operand_2.name;
        if(q.operand_2.name=="")
            two="_";
        string three=q.operand_3.name;
        if(q.operand_3.name=="")
            three="_";
        cout<<'('<<sign<<','<<one<<','<<two<<','<<three<<')'<<endl;
    }
}

//将枚举型的sign转为string
string grammar::change_sign_to_string(SIGN sign_enum)
{
    string sign;
    switch (sign_enum)
    {
    case SIGN::add :
        sign='+';
        break;

    case SIGN::sub :
        sign='-';
        break;
    
    case SIGN::multi :
        sign='*';
        break;

    case SIGN::div_ :
        sign='-';
        break;
    
    case SIGN::equal :
        sign='=';
        break;
    
    case SIGN::is_equal :
        sign="==";
        break;
    
    case SIGN::larger :
        sign='>';
        break;
    
    case SIGN::smaller :
        sign='<';
        break;
    
    case SIGN::larger_equal :
        sign=">=";
        break;
    
    case SIGN::smaller_equal :
        sign="<=";
        break;

    case SIGN::and_ :
        sign="AND";
        break;
    
    case SIGN::or_ :
        sign="OR";
        break;

    case SIGN::if_ :
        sign="IF";
        break;

    case SIGN::ie :
        sign="IE";
        break;
    
    case SIGN::wh :
        sign="WHILE";
        break;

    case SIGN::we :
        sign="WE";
        break;
    
    case SIGN::else_ :
        sign="ELSE";
        break;

    case SIGN::do_ :
        sign="DO";
        break;
        
    default:
        break;
    }
    return sign;
}