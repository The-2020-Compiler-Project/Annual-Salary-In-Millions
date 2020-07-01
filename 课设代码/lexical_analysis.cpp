#include "lexical_analysis.h"

//****************关键字表*******************
string K_T[kt_num] = { "int", "main", "void", "if", "else",
    "return", "string", "float", "double", "case",
    "switch", "while", "break", "bool", "char",
    "default" };
//****************界符表*******************
//  单字界符
string single_PT[s_pt_num] = { "=", ">", "<", "+", "-",
    "*", "/", "{", "}", ",",
    ";", "(", ")", "[", "]", "!", "&", "|" };
//双字界符
string double_PT[d_pt_num] = { ">=", "<=", "==", "&&", "||", "!=" };

void lexic::open_file()
{
    f = fopen("test.txt", "r"); //  进行打开文件
    if (f == NULL) {
        cout << "文件打开失败！" << endl;
        exit(0);
    } else {
        cout << "文件打开成功！" << endl;
    }
}
void lexic::close_file()
{
    fclose(f); // 关闭打开的文件
}
bool lexic::lexic_analyze()
{
    int kind;
    int s_now = 1, s_before; //当前状态和之前状态
    int token_n = 0; //token表的下标
    char buffer; //每次读入的字符
    token temp;
    temp.token_value = ""; //初始化一下临时存放单元
    buffer = fgetc(f);
    while (buffer != '#') {
        s_before = s_now; //保存前一个状态
        s_now = s_change(s_now, buffer); //状态转换

        if (s_now == error_s) //发生错误
        {
            cout << "识别字符:" << buffer << "时发生错误!" << endl; //遇到某字符时发生错误
            return false;
        } else if (s_now == 13) //状态机开始即读到空格或换行
        {
            s_now = 1;
        } else if (s_now != end_s) //未到终止状态
        {
            temp.token_value += buffer;
        } else if (s_now == end_s) //到终止状态
        {
            temp.token_code = token_code(s_before, temp.token_value); //生成token序列码
            token_list.push_back(temp);
            fseek(f, -1, SEEK_CUR);
            temp.token_value = ""; //重新初始化一下临时变量
            s_now = 1;
        } else {
            cout << "出现非法状态！" << endl; //读到不正常的状态
            return false;
        }
        buffer = fgetc(f); //读取下一个字符
    }
    s_before = s_now;
    return true;
}
int lexic::change_num(char buffer)
{ //将字符转化为相应的代号
    int n;
    if ((buffer >= 'a' && buffer <= 'z') || (buffer >= 'A' && buffer <= 'Z'))
        n = 1;
    else if (buffer >= '0' && buffer <= '9')
        n = 2;
    else if (buffer == '.')
        n = 3;
    else if (buffer == ' ' || buffer == '\n' || buffer == '\t')
        n = 4; //空格或换行或tab
    else if (buffer == 39)
        n = 5; //''
    else if (buffer == 34)
        n = 6; //""
    else if (buffer == '>')
        n = 7; //双字符界符
    else if (buffer == '<')
        n = 8; //双字符界符
    else if (buffer == '=')
        n = 9; //双字符界符         //新增一些符号识别
    else if (buffer == '!')
        n = 11; //"!="
    else if (buffer == '|')
        n = 12; //"||"
    else if (buffer == '&')
        n = 13; //"&&"
    else
        n = 10;
    return n;
}
int lexic::s_change(int s_now, char buffer)
{ //通过自动机来实现状态转换
    int n, s_next;
    n = change_num(buffer);
    switch (s_now) {
    case 1:
        if (n == 1)
            s_next = 2;
        else if (n == 2)
            s_next = 3;
        else if (n == 5)
            s_next = 6;
        else if (n == 6)
            s_next = 9;
        else if (n == 7)
            s_next = 12; //双字符界符">="
        else if (n == 8)
            s_next = 14; //双字符界符
        else if (n == 9)
            s_next = 16; //双字符界符
        else if (n == 10)
            s_next = 18; //单字符界符
        else if (n == 4)
            s_next = 13; //开始读到空格或换行
        else if (n == 11)
            s_next = 19; //"!="
        else if (n == 12)
            s_next = 20; //"||"
        else if (n == 13)
            s_next = 21; //"&&"
        else
            s_next = error_s; //实现报错功能
        break;
    case 2:
        if ((n == 1) || (n == 2) || (buffer == 95))
            s_next = 2; //标识符字母开头-字母数字下划线组成
        else
            s_next = 0;
        break;
    case 3:
        if (n == 2)
            s_next = 3; //整数
        else if (n == 3)
            s_next = 4; //小数
        else if ((n == 4) || (n == 7) || (n == 8) || (n == 9) || (n == 10))
            s_next = 0; //遇到界符或空格或换行停止
        else
            s_next = error_s;
        break;
    case 4:
        if (n == 2)
            s_next = 5;
        else
            s_next = error_s;
        break;
    case 5:
        if (n == 2)
            s_next = 5;
        else if ((n == 4) || (n == 7) || (n == 8) || (n == 9) || (n == 10))
            s_next = 0; //遇到界符或空格或换行停止
        else
            s_next = error_s;
        break;
    case 6:
        if (n == 1)
            s_next = 7;
        else
            s_next = error_s;
        break;
    case 7:
        if (n == 5)
            s_next = 8;
        else
            s_next = -1;
        break;
    case 8:
        s_next = 0;
        break;
    case 9:
        if (n == 1)
            s_next = 10;
        else
            s_next = -1;
        break;
    case 10:
        if (n == 1)
            s_next = 10;
        else if (n == 6)
            s_next = 11;
        else
            s_next = -1; //字符串只有字母
        break;
    case 11:
        s_next = 0;
        break;
    case 12:
        if (n == 9)
            s_next = 15; //>=   //只要调到相应的空处，使得状态机增加一个状态即可。
        else
            s_next = 0;
        break;
    case 13:
        s_next = 0;
        break;
    case 14:
        if (n == 9)
            s_next = 15; //<=
        else
            s_next = 0;
        break;
    case 15:
        s_next = 0;
        break;
    case 16:
        if (n == 9)
            s_next = 17; //==
        else
            s_next = 0;
        break;
    case 17:
        s_next = 0;
        break;
    case 18: //单字符界符
        s_next = 0;
        break;
    case 19: //!=
        if (n == 9)
            s_next = 15; //!=   //只要调到相应的空处，使得状态机增加一个状态即可。
        else
            s_next = 0;
        break;
    case 20: //||
        if (n == 12)
            s_next = 15; //||   //只要调到相应的空处，使得状态机增加一个状态即可。
        else
            s_next = 0;
        break;
    case 21: //&&
        if (n == 13)
            s_next = 15; //&&   //只要调到相应的空处，使得状态机增加一个状态即可。
        else
            s_next = 0;
        break;
    default:
        cout << endl
             << "出现无法识别的字符： '" << buffer << endl; //遇到无法识别的字符
        break;
    }
    return s_next;
}
int lexic::token_code(int s_before, string value)
{
    int i = 0;
    if (s_before == 2) {
        for (i = 0; i < kt_num; i++) {
            if (K_T[i].compare(value) == 0) {
                return KT; //成功匹配到，是关键字
            }
        }
        return iT; //未查到的话则为标识符
    } else if ((s_before == 5) || (s_before == 3))
        return CT;
    else if (s_before == 8)
        return cT;
    else if (s_before == 11)
        return sT;
    else if ((s_before == 13) || (s_before == 15) || (s_before == 17)) { //统一到界符表中去查
        for (i = 0; i < d_pt_num; i++) {
            if (double_PT[i].compare(value) == 0) {
                return PT;
            }
        }
    } else if ((s_before == 12) || (s_before == 14) || (s_before == 16) || (s_before == 18) || (s_before == 19) || (s_before == 20) || (s_before == 21)) { //统一到单界符表中去查
        for (i = 0; i < s_pt_num; i++) {
            if (single_PT[i].compare(value) == 0) {
                return PT;
            }
        }
    }
}
void lexic::print_token()
{
    for (auto it = token_list.begin(); it != token_list.end(); it++) {
        cout << "<" << (*it).token_code << "," << (*it).token_value << ">" << endl;
    }
}

int main()
{
    bool test;
    lexic a;
    a.open_file();
    test = a.lexic_analyze();
    a.close_file();
    if (test)
        cout << "词法分析成功！" << endl;
    else
        cout << "词法分析失败！" << endl;
    cout << endl;
    a.print_token();
    return 0;
}
