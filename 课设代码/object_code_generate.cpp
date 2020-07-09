#include "object_code_generate.h"
using namespace std;
//单寄存器AX
int flag = 0; //flag=0表示寄存器AX可用
int tempNum = 1; //产生空间，当寄存器被占用时，先移入其中
string op_flag; //用来标记跳转符号
stack<int> SE; //用来反填的栈 ,反填if，while
vector<int> processed_syn_position;
int current_offset = 0;
OPERAND RDL;

int fcode::ADDR = 0;
vector<fcode> fcode_arrary; //存放生成的目标指令

void fcode::fill_addr(int p)
{
    ostringstream oss;
    string str;
    oss << p;
    str = oss.str();
    str = "C" + str;
    OP3 = str;
}
void fcode::add(string op1, string op2, string op3)
{
    OP1 = get_label() + op1;
    OP2 = op2;
    OP3 = op3;
    ADDR++;
}
void fcode::add_not_label(string op1, string op2, string op3)
{
    OP1 = op1;
    OP2 = op2;
    OP3 = op3;
}

bool fcode::is_processed(int position)
{
    for (unsigned i = 0; i < processed_syn_position.size(); i++) {
        if (position == processed_syn_position[i])
            return true;
    }
    return false;
}
int fcode::get_element_num(string name)
{
    int i = 0;
    int num = 0;
    while (name[i] != '[') {
        i++;
    }
    i++;
    while (name[i] != ']') {
        num = num * 10 + name[i] - '0';
        i++;
    }
    return num;
}
int fcode::get_length_for_type(TVAL tval)
{
    //为了和AX匹配 默认全部为一个字单元
    if (tval == TVAL::Char) {
        return 2;
    } else if (tval == TVAL::Int) {
        return 2;
    } else if (tval == TVAL::Double) {
        return 2;
    } else {
        return 2;
    }
}

void fcode::refill_addr_single(OPERAND operand)
{
    if (operand.name == "")
        return;
    int position = operand.position;
    if (!is_processed(position)) {
        processed_syn_position.push_back(position);
        if (synbl_list[position].cat == CAT::v || synbl_list[position].cat == CAT::tv) {
            if (synbl_list[position].addr.table == TABLE::synbl) //处理的对象为数组元素
            {
                int element_num = get_element_num(synbl_list[position].name);
                int array_position = synbl_list[position].addr.position;
                //若数组母元素还没有处理则先处理
                if (!is_processed(array_position)) {
                    TVAL tval_ = synbl_list[array_position].TYPE.tval;
                    int ainfl_position = synbl_list[array_position].TYPE.addr.position;
                    int size = ainfl_list[ainfl_position].up + 1;
                    TVAL tval = ainfl_list[ainfl_position].tval;
                    int each_length = get_length_for_type(tval);
                    synbl_list[array_position].offset_add = current_offset;
                    current_offset = current_offset + each_length * size;
                    processed_syn_position.push_back(array_position);
                }
                int array_offset = synbl_list[array_position].offset_add;
                TVAL tval = synbl_list[position].TYPE.tval;
                int length = get_length_for_type(tval);
                synbl_list[position].offset_add = array_offset + length * element_num; //默认处理为4个单元
            } else {
                synbl_list[position].offset_add = current_offset;
                int length = get_length_for_type(synbl_list[position].TYPE.tval);
                current_offset = current_offset + length;
            }
        }
    }
}
void fcode::refill_addr()
{
    for (unsigned i = 0; i < QT.size(); i++) {
        refill_addr_single(QT[i].operand_1);
        refill_addr_single(QT[i].operand_2);
        refill_addr_single(QT[i].operand_3);
    }
}

string fcode::get_addr_of_operand(OPERAND operand)
{
    SYNBL synbl = synbl_list[operand.position];
    if (synbl.cat == CAT::v || synbl.cat == CAT::tv) {
        int offset = synbl_list[operand.position].offset_add;
        string str = "BUFF";
        str = str + "[";
        str = str + to_string(offset);
        str = str + "]";
        str = "WORD PTR " + str;
        return str;
    } else if (synbl.cat == CAT::c) {
        if (synbl.TYPE.tval == TVAL::Int) {
            int num = const_int_double_list[synbl.addr.position];
            return to_string(num);
        } else if (synbl.TYPE.tval == TVAL::Char) {
            return const_char_list[synbl.addr.position];
        }
    }
}

string fcode::get_label()
{
    string str;
    str = "C" + to_string(fcode::ADDR) + ": ";
    return str;
}

void fcode::to_code()
{
    //cout<<QT.size();

    fcode f;
    f.add_not_label("DSEG SEGMENT", "", "");
    fcode_arrary.push_back(f);
    //中间填写定义的数据,对符号表扫一遍对变量分配存储单元

    //反填符号表中的偏移地址
    refill_addr();
    //根据偏移地址确定分配的数据段的大小;
    f.add_not_label("BUFF", "DB", "DUP(" + to_string(current_offset) + ")");
    fcode_arrary.push_back(f);
    f.add_not_label("DSEG", "ENDS", "");
    fcode_arrary.push_back(f);

    f.add_not_label("CSEG", "SEGMENT", "");
    fcode_arrary.push_back(f);
    //访问符号表，填写定义的内容
    f.add_not_label("ASSUME CS:CSEG", ",", "DS:DSEG");
    fcode_arrary.push_back(f);

    //后面是生成正常的指令
    for (auto it = QT.begin(); it != QT.end(); it++) {
        QUATERNION temp;
        temp = *it;
        //赋值语句
        if (temp.sign == SIGN::equal) {
            if (RDL.position != temp.operand_1.position) {
                fcode f1, f2;
                f1.add("MOV", "AX", get_addr_of_operand(temp.operand_1));
                f2.add("MOV", get_addr_of_operand(temp.operand_3), "AX");
                fcode_arrary.push_back(f1);
                fcode_arrary.push_back(f2);
                RDL = temp.operand_3;
            } else {
                fcode f;
                f.add("MOV", get_addr_of_operand(temp.operand_3), "AX"); //直接将AX里面的值移动到相应的变量中
                fcode_arrary.push_back(f);
                RDL = temp.operand_3;
            }
        }

        //加，减，乘，除 ，与，或
        else if (temp.sign == SIGN::add || temp.sign == SIGN::sub || temp.sign == SIGN::multi || temp.sign == SIGN::div_ || temp.sign == SIGN::and_ || temp.sign == SIGN::or_) {
            string op_temp; //记录一下是什么符号，四种符号生成的目标代码一致
            if (temp.sign == SIGN::add || temp.sign == SIGN::sub) {
                temp.sign == SIGN::add ? op_temp = "ADD" : op_temp = "SUB";
            } else if (temp.sign == SIGN::multi || temp.sign == SIGN::div_) {
                temp.sign == SIGN::multi ? op_temp = "MUL" : op_temp = "DIV";
            } else {
                temp.sign == SIGN::and_ ? op_temp = "AND" : op_temp = "OR";
            }

            if (RDL.position != temp.operand_1.position) //寄存器没被占用
            {
                fcode f1, f2;
                f1.add("MOV", "AX", get_addr_of_operand(temp.operand_1));
                f2.add(op_temp, "AX", get_addr_of_operand(temp.operand_2));

                fcode_arrary.push_back(f1);
                fcode_arrary.push_back(f2);

                //释放内存
                fcode f3;
                f3.add("MOV", get_addr_of_operand(temp.operand_3), "AX");
                fcode_arrary.push_back(f3);

                RDL = temp.operand_3;
            } else {
                fcode f1;
                f1.add(op_temp, "AX", get_addr_of_operand(temp.operand_2));
                fcode_arrary.push_back(f1);

                //释放内存
                fcode f3;
                f3.add("MOV", get_addr_of_operand(temp.operand_3), "AX");
                fcode_arrary.push_back(f3);
                RDL = temp.operand_3;
            }
        }

        //>,<,=,>=,<=,==放在一起
        else if (temp.sign == SIGN::larger || temp.sign == SIGN::smaller || temp.sign == SIGN::larger_equal || temp.sign == SIGN::smaller_equal || temp.sign == SIGN::is_equal || temp.sign == SIGN::not_equal) {
            if (temp.sign == SIGN::larger || temp.sign == SIGN::smaller) {
                temp.sign == SIGN::larger ? op_flag = "JA" : op_flag = "JB";
            } else if (temp.sign == SIGN::smaller_equal || temp.sign == SIGN::larger_equal) {
                temp.sign == SIGN::smaller_equal ? op_flag = "JNA" : op_flag = "JNB";
            } else {
                temp.sign == SIGN::is_equal ? op_flag = "JE" : op_flag = "JNE";
            }

            if (RDL.position != temp.operand_1.position) {
                fcode f1, f2;
                f1.add("MOV", "AX", get_addr_of_operand(temp.operand_1));
                f2.add("CMP", "AX", get_addr_of_operand(temp.operand_2));
                fcode_arrary.push_back(f1);
                fcode_arrary.push_back(f2);

                //释放内存
                fcode f3;
                f3.add("MOV", get_addr_of_operand(temp.operand_3), "AX");
                fcode_arrary.push_back(f3);

                RDL = temp.operand_3;

            } else {
                fcode f1;
                f1.add("CMP", "AX", temp.operand_2.name);
                fcode_arrary.push_back(f1);

                //释放内存
                fcode f3;
                f3.add("MOV", get_addr_of_operand(temp.operand_3), "AX");
                fcode_arrary.push_back(f3);
                RDL = temp.operand_3;
            }
        }

        else if (temp.sign == SIGN::if_ || temp.sign == SIGN::do_) {
            fcode f;
            SE.push(f.ADDR);
            f.add(op_flag, "AX", "?"); //回来的时候回填
            fcode_arrary.push_back(f);
        }

        else if (temp.sign == SIGN::else_) {
            fcode f;
            int num = SE.top();
            SE.pop();
            SE.push(f.ADDR);
            f.add("JMP", "_", "?"); //等待回填
            fcode_arrary[num].fill_addr(f.ADDR); //回填if
            fcode_arrary.push_back(f);
        }

        else if (temp.sign == SIGN::ie) {
            int num = SE.top();
            SE.pop();
            fcode_arrary[num].fill_addr(fcode::ADDR); //回填else
        }

        else if (temp.sign == SIGN::wh) {
            SE.push(fcode::ADDR); //压入反填的地址
        }

        else if (temp.sign == SIGN::we) {
            fcode f;
            int num = SE.top(); //先反填do
            SE.pop();
            f.add("JMP", "", "?");
            fcode_arrary[num].fill_addr(fcode::ADDR); //JMP的下一个
            num = SE.top();
            SE.pop();
            fcode_arrary.push_back(f);
            fcode_arrary[fcode::ADDR - 1].fill_addr(num); //填上上面的？
        }

        else if (temp.sign == SIGN::end_) {
            fcode f1, f2;
            f1.add("MOV", "AH", "4CH");
            f2.add("INT 21H", "", "");
            fcode_arrary.push_back(f1);
            fcode_arrary.push_back(f2);

            fcode f3, f4;
            f3.add_not_label("CSEG ENDS", "", "");
            f4.add_not_label("END C1", "", "");

            fcode_arrary.push_back(f3);
            fcode_arrary.push_back(f4);
        }
    }
}
string fcode::print_fcode_array()
{
    string result;
    for (unsigned i = 0; i < fcode_arrary.size(); i++) {
        result+= fcode_arrary[i].OP1 + ' ' + fcode_arrary[i].OP2 + ',' + fcode_arrary[i].OP3 +'\n';
    }
    return result;
}
