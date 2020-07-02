#include "lexical_analysis.h"
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
// 语法分析类
class grammar {
private:
    lexic_wrapper* lexic;
    token w;

public:
    grammar();
    ~grammar();
    void error();
    void getToken();
    void begin();
    void program();
    int type();//修改用来返回相应的类型，供符号表的填写
    bool isType();
    void functionBody();
    void expression();
    void mathExpression();
    void logicExpression();
    void declaration(int kind);//传入类型
    void declaration_1();
    void declaration_2();
    void arrayInit();
    void assignment();
    void A();
    void B();
    void C(int kind,token temp);//传入标识和相应的类型
    void D();
    void E();
    void T();
    void E1();
    void F();
    void T1();
};
