#include "DAG.h"
int dagnum = 0;
int nodevalue = 0;
vector<QUATERNION> QT;
vector<OPERAND> node_list; 
vector<Dnode>NODE;

//-------------------------------------------------------
void divide() //���ֻ�����
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
            end = count; //���������
           
            if(end!=start)
            {optimize(start, end);//һ��������Ŀ�ʼ�ͽ���           
            opti_Quaternion(); //��һ���������Ż������Ԫʽ����
            }
            QT.push_back(quaternion_list[count]); //���������������Ԫʽ����
            end = count + 1; //��ΪҪ����µ�start
            continue;
        }
       
    }
}
void opti_Quaternion() //�����Ż������Ԫʽ
{
    int i, j;
    QUATERNION quat;
    for (i = 0; i < dagnum; i++) {

        if (NODE[i].isLeaf==false) //����Ҷ�ӽڵ�
        {
            quat.sign = NODE[i].oper;
            quat.operand_1 = NODE[NODE[i].lchild].label[0];
            quat.operand_2 = NODE[NODE[i].rchild].label[0];
            quat.operand_3 = NODE[i].label[0];
            QT.push_back(quat);
           
        }
        if (NODE[i].label_num > 1) {
            for (j = 1; j < NODE[i].label_num; j++) {
                if (!istemp(NODE[i].label[j])) //������ʱ����
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
void display() //����Ż������Ԫʽ
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
string sign_to_string(SIGN sign_enum) //�ַ�ת�ַ���
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
int isdefine(string op) //�ж�op�Ƿ񱻶����
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

int innode(string op) //op��node_list��λ��
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
bool istemp(OPERAND temp) //�Ƿ�����ʱ����
{string str=temp.name;
int n=str.size();
if((str[0]>=48&&str[0]<=57)&&(str[n-1]=='t'))
    
        return true;
    else
        return false;
}
bool iscv(OPERAND op) //�Ƿ��ǳ���
{
    if (synbl_list[op.position].cat == CAT::c)
        return true;
    else
        return false;
}
void del(OPERAND res, int newnum, int oldnum) //ɾ���ý��������λ��
{
    int nodeposition = innode(res.name); //��ȡ�����node_list��λ�ã�Ҫ�ı����ı��
    node_list[nodeposition].position = newnum;
    NODE[oldnum].label_num--;
    int i;
    for (i = 0; i < NODE[oldnum].label_num; i++) {
        if ((res.name == NODE[oldnum].label[i].name) && (i != 0)) //�������ɾ
        {
            for (int j = i + 1; j < NODE[oldnum].label_num; j++)
                NODE[oldnum].label[j - 1] = NODE[oldnum].label[j]; //ɾ���ñ�ǣ���ǰ��һ��
            break;
        }
    }
}

void fuzhi(OPERAND op, OPERAND res) //��ֵ��Ԫʽ
{
    int dnum; //res�Ľ����
    int isdef;
   
    isdef = isdefine(op.name); //��һ���������Ƿ��Ѵ���
    int isdefres;
    isdefres = isdefine(res.name); //����Ƿ񱻶����
    if (isdef==-1) //��һ��������û�ж��壬Ҫ���ڵ㣬������node_list
    { 
        node_list.push_back(op);
        node_list[nodevalue].position = dagnum; //�޸�����positionΪͼ���ı�ţ�����ɾ��  
        Dnode DN;
        DN.num = dagnum;
        DN.isLeaf = true;
        DN.iscval = iscv(op);
        DN.oper=SIGN::equal;
        DN.label_num = 2;
        DN.label[0] = op;
        DN.label[1] = res; //���������
        NODE.push_back(DN);
        dnum = dagnum;
        dagnum++;
        nodevalue++;
       
    } 
    else //��һ���������������ˣ��������Ϊ���ı��
    {
        dnum = isdef;
        NODE[isdef].label[NODE[isdef].label_num] = res;
        NODE[isdef].label_num++;
        
    }

   
    if (isdefres != -1) //����Ѵ��ڣ�������������Ҫɾ��
        del(res, dnum, isdefres);
    else //û�б��������Ҫ����node_list
    {
        node_list.push_back(res);
        node_list[nodevalue].position = dnum;
        nodevalue++;
    }
    if (istemp(NODE[dnum].label[0])) //�������ʱ����������λ��
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
void binaryoperator(SIGN s, OPERAND op1, OPERAND op2, OPERAND op3) //˫Ŀ����
{
int i;
    int isdef1;
    isdef1 = isdefine(op1.name); //��һ���������Ƿ��Ѵ���
    int isdef2;
    isdef2 = isdefine(op2.name); //����Ƿ񱻶����
    int isdef3;
    isdef3 = isdefine(op3.name);
    int renum; //���浱ǰ�ڵ�ı�ţ��û�������ʱ����
    double c; //���������ǳ���
    double c1, c2;
    if (iscv(op1) && iscv(op2)) //���������ǳ���,�����c
    {
        c1 = getcval(op1);
        c2 = getcval(op2);
        c = compare(c1, c2, s);
        string str = to_string(c); //��cת��Ϊ�ַ���
        int isdefc = isdefine(str); //�жϸó����Ƿ����
        if (isdefc != -1) //����
        {
            renum = isdefc;

            if (isdef3 == -1) //���������������
            {
                del(op3, isdefc, isdef3);
                //�ѵ�3������c����ͬһ���ڵ�
                NODE[isdefc].label[NODE[isdefc].label_num] = op3;
                NODE[isdefc].label_num++;
            } else //û�ж������ֱ�Ӽӽ����Ϳ�
            {
                node_list[nodevalue] = op3;
                node_list[nodevalue].position = isdefc;
                nodevalue++;
                NODE[isdefc].label[NODE[isdefc].label_num] = op3;
                NODE[isdefc].label_num++;
            }
        } else //����c�����ڣ�Ҫ����
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
            if (isdef3 != -1) //����Ѵ��ڣ�������������Ҫɾ��
            {
                del(op3, dagnum - 1, isdef3);
            } else {
                node_list.push_back(op3);
                node_list[nodevalue].position = dagnum - 1;
                nodevalue++;
            }
        }
    }

    else //������1�����ǳ���
    {Dnode DN;
           int num = -1;
        for (i = 0; i < dagnum; i++) {
            if (NODE[i].oper == s) //�����������
            {
                num = i;
                break;
            }
        }
       
        if (num != -1) //˵��������������ˣ����������Һ���
        {
            renum = num;
            if (NODE[num].lchild==isdef1) //���������op1
            {
                if (NODE[isdef3].rchild==isdef2) //�Һ�����op2��˵�������Ԫʽ�ظ���
                {
                    NODE[num].label[NODE[num].label_num] = op3;
                    NODE[num].label_num++;
                    if (isdef3 != -1) //����Ѵ��ڣ�������������Ҫɾ��
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
        else //�������û�б����壬��Ҫ���½�һ���ڵ�
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
            if (isdef1 != -1) //��һ����������������
            {
                NODE[dagnum - 1].lchild = isdef1;
                if (isdef2 != -1) //һ������������
                {
                    NODE[dagnum - 1].rchild = isdef2;
                } else //һ���壬��û��,�����½�һ���ڵ�
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
            } else //��һ����û����
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
                if (isdef2 != -1) //�ڶ�����������
                {
                    NODE[dagnum - 2].rchild = isdef2;
                } else //��һ�������ڶ���������û����
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

    if (istemp(NODE[renum].label[0])) //�������ʱ����������λ��
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

void optimize(int start, int end) //DAG�Ż�
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
