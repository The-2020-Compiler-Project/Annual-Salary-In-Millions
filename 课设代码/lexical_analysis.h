//调试大于等于
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
//词法分析宏定义
#define kt_num 16 //关键字表的大小
#define s_pt_num 18 //单字符界符表的大小
#define token_num 400 //token表的大小
#define d_pt_num 6 //双字符界符表的大小
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
    void print_token();
    vector<token> token_list; //保存词法分析所得的token串
};
