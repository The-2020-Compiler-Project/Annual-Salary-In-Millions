#include <stack>
#include <string>
#include <vector>
using namespace std;

//按照实际进行修改
enum CAT { f,
    c,
    t,
    d,
    v,
    vn,
    vf };
//按照实际进行修改
enum TVAL { Int,
    Double,
    String,
    Char,
    Array };
//类型表
struct TYPEL {
    TVAL tval;
    void* tpoint;
};
vector<TYPEL> typel_list;

//总表
struct SYNBL {
    string name;
    TYPEL* TYPE;
    CAT cat;
    int level; //作用域标号
    void* addr; //改为偏移地址 注意数组类型的偏移地址有不确定性
};
vector<SYNBL> synbel_list;

//数组表
struct AINFL {
    int low;
    int up;
    TYPEL* ctp;
    int clen;
};
vector<AINFL> ainfl_list;

//结构表 暂时用不到
struct RINFL {
    string name;
    int off;
    TYPEL* tp;
};
vector<RINFL> rinfl_list;

//形参表
struct FPL {
    string name;
    TYPEL* tp;
    CAT cat;
    int level; //对应活动记录中的第几层
    int off; //对应的偏移地址
};
vector<FPL> fpl_list;

//函数表
struct PFINFL {
    int level;
    int off;
    int FN;
    long ENTRY;
    FPL* fpl_pointer;
    int data_long; //该函数中需要开辟的空间的大小 为了目标代码生成服务
};
vector<PFINFL> pfinfl_list;

//标识符表 为了方便判断是否重定义以及类型是否匹配 额外设立一个标识符表
struct IDENTIFY {
    string name;
    TYPEL* tp;
    int level; //当前的作用域的层数 0为全局作用域 1为main作用域 当重作用域中返回的时候需要删除表
};

//已分配作用域标号
int current_level;
//作用域栈
stack<int> current_level_stcak;

//四元式符号
enum SIGN {
    add,
    sub,
    multi,
    div_,
    equal,
    not_equal,
    larger,
    smaller,
    larger_equal,
    smaller_equal,
    and_,
    or_
};
//操作符栈
stack<SIGN> sign_stack;

//操作数结构
struct OPERAND {
    string name;
    SYNBL* pointer;
};
//对象栈
stack<OPERAND> operand_stack;

//四元式结构体
struct QUATERNION {
    SIGN sign;
    OPERAND operand_1;
    OPERAND operand_2;
    OPERAND operand_3;
};
vector<QUATERNION> quaternion_list;
