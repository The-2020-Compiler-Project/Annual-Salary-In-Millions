#include<stdio.h>
#include "grammatical_analysis.cpp"
using namespace std;

struct Dnode {
    int num; //节点编号
    bool isLeaf; //叶节点
    bool iscval; //是常数
    int lchild;
    int rchild;
    SIGN oper; //运算符
    int label_num;
    OPERAND label[10]; //该结点的主从标记
    Dnode();
};
Dnode::Dnode()
{
    isLeaf = true;
    iscval = false;
    label_num = 0;
    //需要给叶子节点设一个运算符
}
void divide(); //划分基本块;
int isdefine(string op); //判断op是否被定义过
int innode(string op); //op在node_list的位置
void opti_Quaternion(); //生成优化后的四元式
bool istemp(OPERAND temp); //是否是临时变量
bool iscv(OPERAND op); //是否是常数
void del(OPERAND res, int newnum, int oldnum); //删除该结果的其他位置
void fuzhi(OPERAND op, OPERAND res); //赋值四元式
double getcval(OPERAND op); //获得op的常值
double compare(double a, double b, SIGN s); //两个常数的运算结果
void binaryoperator(SIGN s, OPERAND op1, OPERAND op2, OPERAND op3); //双目运算
void swap(OPERAND& S1, OPERAND& S2); //交换
void optimize(int start, int end); //DAG优化
string sign_to_string(SIGN sign_enum); //字符转字符串
void display(); //输出优化后的四元式