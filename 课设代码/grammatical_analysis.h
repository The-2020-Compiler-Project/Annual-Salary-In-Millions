#ifndef GRAM
#define GRAM

#include "lexical_analysis.h"
#include "symbol_table.h"

extern vector<AINFL> ainfl_list;
extern vector<RINFL> rinfl_list;
extern vector<FPL> fpl_list;
extern vector<PFINFL> pfinfl_list;
extern int current_level;
extern stack<SIGN> sign_stack;
extern stack<OPERAND> operand_stack;
extern vector<string> err_msg_list;
extern vector<QUATERNION> quaternion_list;
extern vector<TYPEL> typel_list;
extern vector<double> const_int_double_list;
extern vector<string> const_char_list;
extern vector<string> const_string_list;
extern vector<int> LENL;
extern vector<SYNBL> synbl_list;
extern vector<int> current_level_stack;

/*语法分析部分开始*/
// 包装词法分析，提供getToken函数
class lexic_wrapper {
private:
    bool isSuccess;
    lexic* lex;
    vector<token>::iterator position;

public:
    lexic_wrapper(string);
    ~lexic_wrapper();
    token getToken();
};
// 语法分析类
class grammar {
private:
    lexic_wrapper* lexic;
    token w;
    int temp_operand_num = 0;

public:
    grammar(string);
    ~grammar();
    void error(string err_msg);
    void getToken();
    void begin();
    string errorManage();
    void program(); //完成
    TVAL type(); //修改用来返回相应的类型，供符号表的填写
    bool isType();
    void functionBody(); //完成
    void expression(); //完成
    void mathExpression(); //完成
    void logicExpression(); //完成
    void declaration(TVAL tval); //传入类型 //完成
    void declaration_1(TVAL kind); //传入类型 //完成
    void declaration_2(TVAL kind); //传入类型 //完成
    void arrayInit(OPERAND operand_array, TVAL tval, int max_subscrip, int current_subscrip); //完成
    void assignment(); //完成
    void A(); //完成
    void B(); //完成
    void C(TVAL kind, token temp); //传入标识和相应的类型 //完成
    void D(); //完成
    void E(); //完成
    void T(); //完成
    void E1(); //完成
    void F(); //完成
    void T1(); //完成

    OPERAND operand_temp_produce(); //产生临时变量 //需要修改
    int change_to_int(string& s); //用于处理int形式填写符号表
    TYPEL type_deduction(TVAL tval_1, TVAL tval_2, SIGN sign = add); //用来进行类型的推到
    int push_into_synbel_list(SYNBL synbel); //填写符号表并且返回位置
    int is_iT_defined(string it_name); //判断标识符是否已经定义了
    void push_into_const_int_double_list(OPERAND one, OPERAND two, OPERAND three, SIGN sign); //填写常数表，如果临时变量为常数，则需要额外填写常数表，目前还没有做bool型的
    TVAL CT_type_deduction(string str); //判断常数为double还是int类型
    bool is_iT_defined_in_current_level(string name); //判断标识符在当前作用域中是否已经定义过
    int change_type_to_length(TVAL tval); //获得类型的字节长度
    string print_quaternion_list(); //输出四元式
    string change_sign_to_string(SIGN sign_enum); //将枚举型的sign转为string
};
#endif
