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
    vf,
    tv//临时变量 
    };
//按照实际进行修改
enum TVAL { Int,
    Double,
    String,
    Char,
	Bool,
    Array,
    WRONG_TYPE//用于返回类型不匹配的
    };//这部分是为了填符号表的时候确定相应的类型
//总表和符号表中的addr的修正
enum TABLE {
    const_int_double,
    const_char,
    const_string,
    lenl,
    ainfl,
    rinfl,
    synbl
    };
struct ADDR{
    TABLE table;
    int position;
};
//类型表
struct TYPEL {
    TVAL tval;
    ADDR addr;
};





//总表
struct SYNBL {
    string name;
    TYPEL TYPE;
    CAT cat;
    int level; //作用域标号
    int offset_add; //偏移地址
    ADDR addr;
};

//迭代器会失效
//vector<SYNBL>::iterator synbel_it = synbel_list.begin();//用来标记符号表填入的当前位置，便符号表的管理
//先++synbel_it，再压入符号表

//长度表，自定义类型需要指过去


//数组表
struct AINFL {
    int low;
    int up;
    int ctp; //没用复杂数据类型的时候暂时用不上
    TVAL tval;
    int clen;
};


//结构表 暂时用不到
struct RINFL {
    string name;
    int off;
    TYPEL* tp;
};


//形参表
struct FPL {
    string name;
    TYPEL* tp;
    CAT cat;
    int level; //对应活动记录中的第几层
    int off; //对应的偏移地址
};


//函数表
struct PFINFL {
    int level;
    int off;
    int FN;
    long ENTRY;
    FPL* fpl_pointer;
    int data_long; //该函数中需要开辟的空间的大小 为了目标代码生成服务
};


//标识符表 为了方便判断是否重定义以及类型是否匹配 额外设立一个标识符表
struct IDENTIFY {
    string name;
    TYPEL* tp;
    int level; //当前的作用域的层数 0为全局作用域 1为main作用域 当重作用域中返回的时候需要删除表
};

//已分配作用域标号

//作用域栈


//四元式符号
enum SIGN {
    add,
    sub,
    multi,
    div_,
    equal,//赋值 =
    is_equal,//相当 ==
    not_equal,
    larger,
    smaller,
    larger_equal,
    smaller_equal,
    and_,
    or_,
    if_,
    ie,
    wh,
    we,
    else_,
    do_,
    end_
};
//操作符栈


//操作数结构
struct OPERAND {
    string name;
    //vector<SYNBL>::iterator pointer;//配合着synbel_it,共同确定相应的符号表的位置
    int position;
};
//对象栈


//四元式结构体
struct QUATERNION {
    SIGN sign;
    OPERAND operand_1;
    OPERAND operand_2;
    OPERAND operand_3;
};

