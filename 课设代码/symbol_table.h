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
	Bool,
	Const_int,
	Const_double,
	Const_char,
	Const_string,
    Array,
	};//这部分是为了填符号表的时候确定相应的类型
//类型表
struct TYPEL {
    TVAL tval;
    void* tpoint;
};
vector<TYPEL> typel_list;

//各个常数表
vector<int> const_int_list;
vector<int>::iterator const_int_list_it =const_int_list.begin();//便于索引常数表中的内容
vector<float> const_float_list;
vector<char> const_char_list;
vector<string> const_string_list;

//总表
struct SYNBL {
    string name;
    TYPEL TYPE;
    CAT cat;
    int level; //作用域标号
    int offset_add;//偏移地址
    void* addr; //改为偏移地址 注意数组类型的偏移地址有不确定性
};
vector<SYNBL> synbel_list;
vector<SYNBL>::iterator synbel_it = synbel_list.begin();//用来标记符号表填入的当前位置，便符号表的管理
//先++synbel_it，再压入符号表

//长度表，自定义类型需要指过去
vector<int> LENL;

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
int current_level=0;
//作用域栈
vector <int> current_level_stcak;//作用域栈，用来存储作用域。
vector <int>::iterator level_it =current_level_stcak.begin();//用来表示栈顶，便于释放元素
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
    vector<SYNBL>::iterator pointer;//配合着synbel_it,共同确定相应的符号表的位置
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

