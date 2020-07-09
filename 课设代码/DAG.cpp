#include "DAG.h"
int dagnum = 0;
int nodevalue = 0;
vector<QUATERNION> QT;
vector<OPERAND> node_list; 
vector<Dnode>NODE;

//-------------------------------------------------------
void divide() //划分基本块
{
    int count;
    int start = 0;
    int end = 0;
int exists;
    for (count = 0; count < quaternion_list.size(); count++) {
        start = end;
        if(quaternion_list[count].sign==SIGN::ie||quaternion_list[count].sign==SIGN::else_||quaternion_list[count].sign==SIGN::if_||quaternion_list[count].sign==SIGN::we||
        quaternion_list[count].sign==SIGN::wh||quaternion_list[count].sign==SIGN::do_||quaternion_list[count].sign==SIGN::end_)
        exists=1;
        else
        {
            exists=0;
        }
        
        if (exists) 
        {
            dagnum = 0;
            nodevalue = 0;
            NODE.clear();
            
            node_list.clear();
            end = count; //基本块结束
           
            if(end!=start)
            {optimize(start, end);//一个基本块的开始和结束           
            opti_Quaternion(); //将一个基本块优化后的四元式加入
            }
            QT.push_back(quaternion_list[count]); //将基本块结束的四元式加入
            end = count + 1; //因为要获得新的start
            continue;
        }
       
    }
}
void opti_Quaternion() //生成优化后的四元式
{
    int i, j;
    QUATERNION quat;
    for (i = 0; i < dagnum; i++) {

        if (NODE[i].isLeaf==false) //不是叶子节点
        {
            quat.sign = NODE[i].oper;
            quat.operand_1 = NODE[NODE[i].lchild].label[0];
            quat.operand_2 = NODE[NODE[i].rchild].label[0];
            quat.operand_3 = NODE[i].label[0];
            QT.push_back(quat);
           
        }
        if (NODE[i].label_num > 1) {
            for (j = 1; j < NODE[i].label_num; j++) {
                if (!istemp(NODE[i].label[j])) //不是临时变量
                {
                    quat.sign = SIGN::equal;
                    quat.operand_1 = NODE[i].label[0];
                    quat.operand_2.name = "";
                    quat.operand_3 = NODE[i].label[j];
                    QT.push_back(quat);
                }
            }
        }
       


    }
}
void display() //输出优化后的四元式
{
    int i;
    
    for (i = 0; i < QT.size(); i++) {
        QUATERNION q = QT[i];
        string sign = sign_to_string(q.sign);
        string op1 = q.operand_1.name;
        if (q.operand_1.name == "")
            op1 = "_";
        string op2 = q.operand_2.name;
        if (q.operand_2.name == "")
            op2 = "_";
        string op3 = q.operand_3.name;
        if (q.operand_3.name == "")
            op3 = "_";
        cout << '(' << sign << ',' << op1 << ',' << op2 << ',' << op3 << ')' << endl;
    }
}
string sign_to_string(SIGN sign_enum) //字符转字符串
{
    string sign;
    switch (sign_enum) {
    case SIGN::add:
        sign = '+';
        break;

    case SIGN::sub:
        sign = '-';
        break;

    case SIGN::multi:
        sign = '*';
        break;

    case SIGN::div_:
        sign = '-';
        break;

    case SIGN::equal:
        sign = '=';
        break;

    case SIGN::is_equal:
        sign = "==";
        break;

    case SIGN::larger:
        sign = '>';
        break;

    case SIGN::smaller:
        sign = '<';
        break;

    case SIGN::larger_equal:
        sign = ">=";
        break;

    case SIGN::smaller_equal:
        sign = "<=";
        break;

    case SIGN::and_:
        sign = "AND";
        break;

    case SIGN::or_:
        sign = "OR";
        break;

    case SIGN::if_:
        sign = "IF";
        break;

    case SIGN::ie:
        sign = "IE";
        break;

    case SIGN::wh:
        sign = "WHILE";
        break;

    case SIGN::we:
        sign = "WE";
        break;

    case SIGN::else_:
        sign = "ELSE";
        break;

    case SIGN::do_:
        sign = "DO";
        break;
    case SIGN::end_:
    sign="END";
    break;

    default:
        break;
    }
    return sign;
}
int isdefine(string op) //判断op是否被定义过
{
    int t;
    OPERAND v;
    for (auto t = node_list.begin(); t != node_list.end(); t++) {
        v = *t;
        if (op == v.name)
            return v.position;
    }
    return -1;
}

int innode(string op) //op在node_list的位置
{
    int t;
    OPERAND v;
    int i = 0;
    int count = node_list.size();
    for (i = 0; i < count; i++) {

        if (op == node_list[i].name)
            return i;
    }
    return -1;
}
bool istemp(OPERAND temp) //是否是临时变量
{string str=temp.name;
int n=str.size();
if((str[0]>=48&&str[0]<=57)&&(str[n-1]=='t'))
    
        return true;
    else
        return false;
}
bool iscv(OPERAND op) //是否是常数
{
    if (synbl_list[op.position].cat == CAT::c)
        return true;
    else
        return false;
}
void del(OPERAND res, int newnum, int oldnum) //删除该结果的其他位置
{
    int nodeposition = innode(res.name); //获取结果在node_list的位置，要改变他的编号
    node_list[nodeposition].position = newnum;
    NODE[oldnum].label_num--;
    int i;
    for (i = 0; i < NODE[oldnum].label_num; i++) {
        if ((res.name == NODE[oldnum].label[i].name) && (i != 0)) //主标记免删
        {
            for (int j = i + 1; j < NODE[oldnum].label_num; j++)
                NODE[oldnum].label[j - 1] = NODE[oldnum].label[j]; //删除该标记，往前移一个
            break;
        }
    }
}

void fuzhi(OPERAND op, OPERAND res) //赋值四元式
{
    int dnum; //res的结点编号
    int isdef;
   
    isdef = isdefine(op.name); //第一个操作数是否已存在
    int isdefres;
    isdefres = isdefine(res.name); //结果是否被定义过
    if (isdef==-1) //第一个操作数没有定义，要建节点，并加入node_list
    { 
        node_list.push_back(op);
        node_list[nodevalue].position = dagnum; //修改他的position为图结点的编号，方便删除  
        Dnode DN;
        DN.num = dagnum;
        DN.isLeaf = true;
        DN.iscval = iscv(op);
        DN.oper=SIGN::equal;
        DN.label_num = 2;
        DN.label[0] = op;
        DN.label[1] = res; //结果赋给它
        NODE.push_back(DN);
        dnum = dagnum;
        dagnum++;
        nodevalue++;
       
    } 
    else //第一个操作数被定义了，将结果作为他的标记
    {
        dnum = isdef;
        NODE[isdef].label[NODE[isdef].label_num] = res;
        NODE[isdef].label_num++;
        
    }

   
    if (isdefres != -1) //结果已存在，如果不是主标记要删除
        del(res, dnum, isdefres);
    else //没有被定义过，要加入node_list
    {
        node_list.push_back(res);
        node_list[nodevalue].position = dnum;
        nodevalue++;
    }
    if (istemp(NODE[dnum].label[0])) //如果是临时变量，交换位置
    {
        swap(NODE[dnum].label[0], NODE[dnum].label[NODE[dnum].label_num - 1]);
    }
    if (iscv(NODE[dnum].label[0]))
        NODE[dnum].iscval = true;
    else
        NODE[dnum].iscval = false;
        return;
}
double getcval(OPERAND op)
{

    return const_int_double_list[synbl_list[op.position].addr.position];
}
double compare(double a, double b, SIGN s)
{
    if (s == SIGN::add)
        return a + b;
    if (s == SIGN::sub)
        return a - b;
    if (s == SIGN::multi)
        return a * b;
    if (s == SIGN::div_)
        return a / b;
    if (s == SIGN::equal)
        return (a == b);
    if (s == SIGN::not_equal)
        return (a != b);
    if (s == SIGN::larger)
        return (a > b);
    if (s == SIGN::smaller)
        return (a < b);
    if (s == SIGN::larger_equal)
        return (a >= b);
    if (s == SIGN::smaller_equal)
        return (a <= b);
    if (s == SIGN::and_)
        return (a and b);
    if (s == SIGN::or_)
        return (a or b);
}
void binaryoperator(SIGN s, OPERAND op1, OPERAND op2, OPERAND op3) //双目运算
{
int i;
    int isdef1;
    isdef1 = isdefine(op1.name); //第一个操作数是否已存在
    int isdef2;
    isdef2 = isdefine(op2.name); //结果是否被定义过
    int isdef3;
    isdef3 = isdefine(op3.name);
    int renum; //保存当前节点的编号，用户更换临时变量
    double c; //两个数都是常数
    double c1, c2;
    if (iscv(op1) && iscv(op2)) //两个数都是常数,计算出c
    {
        c1 = getcval(op1);
        c2 = getcval(op2);
        c = compare(c1, c2, s);
        string str = to_string(c); //将c转换为字符串
        int isdefc = isdefine(str); //判断该常数是否存在
        if (isdefc != -1) //存在
        {
            renum = isdefc;

            if (isdef3 == -1) //第三个数定义过了
            {
                del(op3, isdefc, isdef3);
                //把第3个数与c放在同一个节点
                NODE[isdefc].label[NODE[isdefc].label_num] = op3;
                NODE[isdefc].label_num++;
            } else //没有定义过，直接加进来就可
            {
                node_list[nodevalue] = op3;
                node_list[nodevalue].position = isdefc;
                nodevalue++;
                NODE[isdefc].label[NODE[isdefc].label_num] = op3;
                NODE[isdefc].label_num++;
            }
        } else //常数c不存在，要申请
        {
            renum = dagnum;

            NODE[dagnum].num = dagnum;
            NODE[dagnum].isLeaf = true;
            NODE[dagnum].iscval = true;
            NODE[dagnum].label_num = 2;
            NODE[dagnum].label[0].name = str;
            NODE[dagnum].label[1] = op3;
            dagnum++;
            node_list[nodevalue].name = str;
            node_list[nodevalue].position = dagnum;
            nodevalue++;
            if (isdef3 != -1) //结果已存在，如果不是主标记要删除
            {
                del(op3, dagnum - 1, isdef3);
            } else {
                node_list.push_back(op3);
                node_list[nodevalue].position = dagnum - 1;
                nodevalue++;
            }
        }
    }

    else //至少有1个不是常数
    {Dnode DN;
           int num = -1;
        for (i = 0; i < dagnum; i++) {
            if (NODE[i].oper == s) //该运算符有了
            {
                num = i;
                break;
            }
        }
       
        if (num != -1) //说明运算符被定义了，读他的左右孩子
        {
            renum = num;
            if (NODE[num].lchild==isdef1) //如果左孩子是op1
            {
                if (NODE[isdef3].rchild==isdef2) //右孩子是op2，说明这个四元式重复了
                {
                    NODE[num].label[NODE[num].label_num] = op3;
                    NODE[num].label_num++;
                    if (isdef3 != -1) //结果已存在，如果不是主标记要删除
                    {
                        del(op3, num, isdef3);
                    } else {
                        node_list.push_back(op3);
                        node_list[nodevalue].position = num;
                        nodevalue++;
                    }
                }
            }
        } 
        else //该运算符没有被定义，需要重新建一个节点
        {
            renum = dagnum;
            DN.num = dagnum;
            DN.isLeaf = false;
            DN.oper = s;
            DN.iscval = iscv(op3);
            DN.label_num = 1;
            DN.label[0] = op3;
            NODE.push_back(DN);
            node_list.push_back(op3);
            node_list[nodevalue].position=dagnum;
            nodevalue++;
            dagnum++;
            if (isdef1 != -1) //第一个操作数被定义了
            {
                NODE[dagnum - 1].lchild = isdef1;
                if (isdef2 != -1) //一二都被定义了
                {
                    NODE[dagnum - 1].rchild = isdef2;
                } else //一定义，二没有,给二新建一个节点
                {
                    DN.num = dagnum;
                    DN.isLeaf = true;
                    DN.iscval = iscv(op2);
                    DN.label_num = 1;
                    DN.label[0] = op2;
                    NODE.push_back(DN);
                    NODE[dagnum - 1].rchild = dagnum;
                    node_list.push_back(op2);
                    node_list[nodevalue].position = dagnum;
                    dagnum++;
                    nodevalue++;
                }
            } else //第一个数没定义
            {
                DN.num = dagnum;
                DN.isLeaf = true;
                DN.iscval = iscv(op1);
                DN.label_num = 1;
                DN.label[0] = op1;
                NODE.push_back(DN);
                NODE[dagnum - 1].lchild = dagnum;
                node_list.push_back(op1);
                node_list[nodevalue].position = dagnum;
                dagnum++;
                nodevalue++;
                if (isdef2 != -1) //第二个数定义了
                {
                    NODE[dagnum - 2].rchild = isdef2;
                } else //第一个数，第二个数，都没定义
                {
                    DN.num = dagnum;
                    DN.isLeaf = true;
                    DN.iscval = iscv(op2);
                    DN.label_num = 1;
                    DN.label[0] = op2;
                    NODE.push_back(DN);
                    NODE[dagnum - 2].rchild = dagnum;
                    node_list.push_back(op2);
                    node_list[nodevalue].position = dagnum;
                    dagnum++;
                    nodevalue++;
                }
            }
        }
    }

    if (istemp(NODE[renum].label[0])) //如果是临时变量，交换位置
    {
        swap(NODE[renum].label[0], NODE[renum].label[NODE[renum].label_num - 1]);
    }
    if (iscv(NODE[renum].label[0]))
        NODE[renum].iscval = true;
    else
        NODE[renum].iscval = false;
}

void swap(OPERAND& S1, OPERAND& S2)
{
    OPERAND S;
    S.name = S1.name;
    S.position = S1.position;
    S1.name = S2.name;
    S1.position = S2.position;
    S2.name = S.name;
    S2.position = S.position;
}

void optimize(int start, int end) //DAG优化
{
    int i = 0;
    int type;
    for (i = start; i < end; i++) {
        OPERAND oper1 = quaternion_list[i].operand_1;
        OPERAND oper2 = quaternion_list[i].operand_2;
        OPERAND oper3 = quaternion_list[i].operand_3;
        SIGN s = quaternion_list[i].sign;
        if (quaternion_list[i].sign == SIGN::equal) //(=,B,_A)
        {
            fuzhi(oper1, oper3);
        }
        else
        {
            binaryoperator(s, oper1, oper2, oper3);
        }
       
    }
}
