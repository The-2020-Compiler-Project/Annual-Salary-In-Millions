#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
//词法分析宏定义
#define kt_num 16 //关键字表的大小
#define s_pt_num 15 //单字符界符表的大小
#define token_num 400 //token表的大小
#define d_pt_num 3 //双字符界符表的大小
#define error_s -1 //错误状态
#define end_s 0 //结束状态
//定义六种表的序号
#define iT 1 //标识符
#define cT 2 //字符
#define sT 3 //字符串
#define CT 4 //常数
#define KT 5 //关键字
#define PT 6 //界符
struct token {
    string token_value; //token值
    int token_code; //序列码
};
//****************关键字表*******************
string K_T[kt_num] = { "int", "main", "void", "if", "else",
    "return", "string", "float", "double", "case",
    "switch", "while", "break", "bool", "char",
    "default" };
//****************界符表*******************
//  单字界符
string single_PT[s_pt_num] = { "=", ">", "<", "+", "-",
    "*", "/", "{", "}", ",",
    ";", "(", ")", "[", "]" };
//双字界符
string double_PT[d_pt_num] = { ">=", "<=", "==" };
//Token表定义
class lexic {
public:
    lexic() { i_T_num = c_T_num = s_T_num = C_T_num = 1; };
    void open_file(); //打开测试文件
    void close_file(); //关闭测试文件a
    FILE* f = NULL;
    //运行中自动填充的表
    int i_T_num, c_T_num, s_T_num, C_T_num; //分别为四个表计数；
    bool lexic_analyze(); //词法分析函数
    int change_num(char buffer); //字符转换成代号
    int s_change(int s_now, char buffer); //通过有限状态自动机实现状态的转换
    int token_code(int s_before, string value); //通过所处的状态，生成token的序号并存储到相应的表中
    int token_kind(int s_before, string value); //返回token所处的类型
    vector<token> token_list; //保存词法分析所得的token串
};

/*语法分析部分开始*/
// 包装词法分析，提供getToken函数
class lexic_wrapper {
private:
    bool isSuccess;
    lexic* lex;
    vector<token>::iterator position;

public:
    lexic_wrapper();
    ~lexic_wrapper();
    token getToken();
};
// 词法分析类
class grammar {
private:
    lexic_wrapper* lexic;
    token w;

public:
    grammar();
    ~grammar();
    void getToken();
    void begin();
    void program();
    void type();
    void functionBody();
    void expression();
    void mathExpression();
    void logicExpression();
    void declaration();
    void declaration_1();
    void declaration_2();
    void arrayInit();
    void assignment();
    void A();
    void B();
    void C();
    void D();
    void E();
    void T();
    void E1();
    void F();
    void T1();
};
