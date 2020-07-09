#include "DAG.h"
#include <iostream>
#include <sstream>
#include <stack>
#include <vector>

class fcode {
public:
    string print_fcode_array();
    void to_code();
    void add(string op, string op1, string op2);
    void add_not_label(string op, string op1, string op2); //不带标号的移动
    void fill_addr(int p); //用来反填跳转地址
    bool is_processed(int position); //用来判断是否已经填写了偏移地址
    int get_element_num(string name); //获取偏移量
    int get_length_for_type(TVAL tval); //获取类型的字节大小
    void refill_addr(); //反填符号表中的偏移地址
    void refill_addr_single(OPERAND operand); //反填单个偏移地址
    string get_addr_of_operand(OPERAND operand); //获得操作数的偏移地址
    string get_label(); //获得语句的标号
private: //生成的目标指令
    static int ADDR; //序号地址
    string OP1;
    string OP2;
    string OP3;
};
