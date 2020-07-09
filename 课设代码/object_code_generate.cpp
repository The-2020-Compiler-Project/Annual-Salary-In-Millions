#include<iostream>
#include<vector> 
#include <sstream>
#include <stack>
#include "DAG.cpp"
using namespace std;
//单寄存器AX
int flag=0;//flag=0表示寄存器AX可用
int tempNum = 1;//产生空间，当寄存器被占用时，先移入其中
string op_flag;//用来标记跳转符号 
stack <int> SE;//用来反填的栈 ,反填if，while
vector<int> processed_syn_position;
int current_offset=0;
OPERAND RDL;


vector<QUATERNION> optimize_list;//优化后的四元式接口
class fcode{
 	public:
        void print_fcode_array();
 		void to_code();
 		void add(string op, string op1, string op2);
 		void fill_addr(int p);//用来反填跳转地址
		bool is_processed(int position);//用来判断是否已经填写了偏移地址
		int get_element_num(string name);//获取偏移量
		int get_length_for_type(TVAL tval);//获取类型的字节大小
		void refill_addr();//反填符号表中的偏移地址
		void refill_addr_single(OPERAND operand);//反填单个偏移地址
		string get_addr_of_operand(OPERAND operand);//获得操作数的偏移地址
 	private://生成的目标指令
 		static int ADDR;//序号地址
        string OP1;
        string OP2;
        string OP3;
 };
int fcode::ADDR=0;
vector<fcode> fcode_arrary;//存放生成的目标指令
void fcode::fill_addr(int p)
{
    ostringstream oss;
    string str;
    oss << p;
    str = oss.str();
    OP3=str;
}
void fcode::add(string op1, string op2, string op3)
{
	    OP1 = op1;
        OP2 = op2;
        OP3 = op3;
        ADDR++;
}

bool fcode::is_processed(int position)
{
	for(unsigned i=0;i<processed_syn_position.size();i++)
	{
		if(position==processed_syn_position[i])
			return true;
	}
	return false;
}
int fcode::get_element_num(string name)
{
	int i=0;
	int num=0;
	while(name[i]!='[')
	{
		i++;
	}
	i++;
	while(name[i]!=']')
	{
		num=num*10+name[i]-'0';
		i++;
	}
	return num;
}
int fcode::get_length_for_type(TVAL tval)
{
	if(tval==TVAL::Char)
	{
		return 1;
	}
	else if(tval==TVAL::Int)
	{
		return 2;
	}
	else if(tval==TVAL::Double)
	{
		return 4;
	}
	else
	{
		return 4;
	}
	
}

void fcode::refill_addr_single(OPERAND operand)
{
	if(operand.name=="")
		return;
	int position = operand.position;
	if(!is_processed(position))
	{
		processed_syn_position.push_back(position);
		if(synbl_list[position].cat==CAT::v||synbl_list[position].cat==CAT::tv)
		{
			if(synbl_list[position].addr.table==TABLE::synbl)//处理的对象为数组元素
			{
				int element_num=get_element_num(synbl_list[position].name);
				int array_position=synbl_list[position].addr.position;
				//若数组母元素还没有处理则先处理
				if(!is_processed(array_position))
				{
					int ainfl_position= synbl_list[position].TYPE.addr.position;
					int size=ainfl_list[ainfl_position].up+1;
					TVAL tval=ainfl_list[ainfl_position].tval;
					int each_length=get_length_for_type(tval);
					synbl_list[position].offset_add=current_offset;
					current_offset=current_offset+each_length*size;
					processed_syn_position.push_back(array_position);
				}
				int array_offset=synbl_list[array_position].offset_add;
				TVAL tval=synbl_list[position].TYPE.tval;
				int length=get_length_for_type(tval);
				synbl_list[position].offset_add=array_offset+length*element_num;//默认处理为4个单元
			}
			else
			{
				synbl_list[position].offset_add=current_offset;
				int length=get_length_for_type(synbl_list[position].TYPE.tval);
				current_offset=current_offset+length;
			}
		}	
	}
}
void fcode::refill_addr()
{
	for(unsigned i=0;i<quaternion_list.size();i++)
	{
		refill_addr_single(quaternion_list[i].operand_1);
		refill_addr_single(quaternion_list[i].operand_2);
		refill_addr_single(quaternion_list[i].operand_3);
	}
}

string fcode::get_addr_of_operand(OPERAND operand)
{
	int offset=synbl_list[operand.position].offset_add;
	string str="BUFF"+'['+to_string(offset)+']';
	return str;
}

void  fcode::to_code()
{
	fcode f;
	f.add("DSEG","SEGMENT","");
	fcode_arrary.push_back(f);
	//中间填写定义的数据,对符号表扫一遍对变量分配存储单元

	//反填符号表中的偏移地址
	refill_addr();
	//根据偏移地址确定分配的数据段的大小;
	f.add("BUFF","DB","DUP("+to_string(current_offset)+")");
	f.add("DSEG","ENDS","");


	f.add("CSEG","SEGMENT","");
	//访问符号表，填写定义的内容 
	f.add("ASSUME CS:CSEG",",","DS:DSEG");
	fcode_arrary.push_back(f); 


	//后面是生成正常的指令 
 	for(auto it= optimize_list.begin();it!=optimize_list.end();it++)
 	{
 		QUATERNION temp;
 	    temp = *it;
 	    //赋值语句
 	    if(temp.sign==SIGN::equal)
 	    {
 	    	if(!flag)//寄存器空闲
 	    	{
 	    		fcode f1,f2;
				string offset=get_addr_of_operand(temp.operand_1);
				f1.add("MOV","AX",offset);
				f2.add("MOV",get_addr_of_operand(temp.operand_3),"AX");
				fcode_arrary.push_back(f1);
				fcode_arrary.push_back(f2);
			 }
			 else//说明寄存器已经被栈用，此时AX中已经为res(E)
			 {
			 	fcode f;
			 	f.add("MOV",get_addr_of_operand(temp.operand_3),"AX");//直接将AX里面的值移动到相应的变量中
			 	fcode_arrary.push_back(f);
			 }
		 }
		 //加，减，乘，除 ，与，或
		 else if(temp.sign == SIGN::add||temp.sign == SIGN::sub||temp.sign == SIGN::multi||temp.sign== SIGN::div_||
		         temp.sign == SIGN::and_ ||temp.sign == SIGN:: or_)
		 {
		 	string op_temp;//记录一下是什么符号，四种符号生成的目标代码一致
			 if (temp.sign == SIGN::add||temp.sign ==SIGN:: sub)
                {
                    temp.sign == SIGN::add? op_temp = "ADD" : op_temp= "SUB";
                }
            else if (temp.sign == SIGN::multi||temp.sign== SIGN::div_)
                {
                    temp.sign ==SIGN:: multi?  op_temp = "MUL" : op_temp= "DIV";
                }
            else
            {
            	temp.sign == SIGN::and_? op_temp = "AND" : op_temp= "OR";
			}
            if(!flag)//寄存器没被占用
            {
            	fcode f1,f2;
            	f1.add("MOV","AX",get_addr_of_operand(temp.operand_1));
            	f2.add(op_temp,"AX",get_addr_of_operand(temp.operand_2));
            	fcode_arrary.push_back(f1);
            	fcode_arrary.push_back(f2);
            	flag=1;//此时结果放在寄存器中，寄存器被占用
			}
			else{
				fcode f1,f2,f3;
				ostringstream oss;
                string str;
                oss << tempNum;
                str = "M" + oss.str();
                tempNum++;
				f1.add("MOV",str,"AX");//先把占用的寄存器中的数据移出去
				f2.add("MOV","AX",temp.operand_1.name);
				f3.add(op_temp,"AX",temp.operand_2.name);
				fcode_arrary.push_back(f1);
				fcode_arrary.push_back(f2);
				fcode_arrary.push_back(f3);
				flag=1;
			}
		 }
		 //>,<,=,>=,<=,==放在一起
		 else if(temp.sign ==SIGN::larger||temp.sign==SIGN::smaller||temp.sign==SIGN::larger_equal||
		         temp.sign==SIGN:: smaller_equal||temp.sign==SIGN::is_equal||temp.sign==SIGN::not_equal)
		{
			 if (temp.sign ==SIGN::larger||temp.sign==SIGN::smaller)
                    {
                        temp.sign ==SIGN::larger ? op_flag = "JA" : op_flag = "JB";
                    }
                    else if (temp.sign==SIGN:: smaller_equal|| temp.sign==SIGN::larger_equal)
                    {
                        temp.sign==SIGN:: smaller_equal ? op_flag = "JNA" : op_flag = "JNB";
                    }
                    else
                    {
                       temp.sign==SIGN::is_equal ? op_flag = "JE" : op_flag = "JNE";
                    }
			if(!flag)//寄存器没有被占用
			{
				fcode f1,f2;
				f1.add("MOV","AX",temp.operand_1.name);
				f2.add("CMP","AX",temp.operand_2.name);
				fcode_arrary.push_back(f1);
				fcode_arrary.push_back(f2);
				flag=1;
			 }
			 else
			 {
			 	fcode f1,f2,f3;
			 	ostringstream oss;
                string str;
                oss << tempNum;
                str = "M" + oss.str();
                tempNum++;
                f1.add("MOV",str,"AX");
                f2.add("MOV","AX",temp.operand_1.name);
                f3.add("CMP","AX",temp.operand_2.name);
                fcode_arrary.push_back(f1);
                fcode_arrary.push_back(f2);
                fcode_arrary.push_back(f3);
                flag=1;
			 }
		}
		else if(temp.sign==SIGN::if_||temp.sign==SIGN::do_)
		{
			fcode f;
			SE.push(f.ADDR);
			f.add(op_flag,"AX","?");//回来的时候回填
			fcode_arrary.push_back(f);
		}
		else if(temp.sign==SIGN::else_)
		{
			fcode f;
			int num=SE.top();
			SE.pop();
			SE.push(f.ADDR);
			f.add("JMP","_","?");//等待回填
			fcode_arrary[num].fill_addr(f.ADDR);//回填if
			fcode_arrary.push_back(f);
		}
		else if(temp.sign == SIGN::ie)
		{
			int num=SE.top();
			SE.pop();
			fcode_arrary[num].fill_addr(fcode::ADDR);//回填else
		}
		else if(temp.sign==SIGN::wh)
		{
			SE.push(fcode::ADDR);//压入反填的地址
		}
		else if(temp.sign==SIGN::we)
		{
			fcode f;
			int num=SE.top();//先反填do
			SE.pop();
			f.add("JMP","","?");
			fcode_arrary[num].fill_addr(fcode::ADDR);//JMP的下一个
			num=SE.top();
			SE.pop();
			fcode_arrary.push_back(f);
			fcode_arrary[fcode::ADDR-1].fill_addr(num);//填上上面的？
		}
		else
		{
			cout<<"erro QUATERNION!";
		}
	}
	f.add("main","ENDS","");
	fcode_arrary.push_back(f);//结尾 
}
void fcode::print_fcode_array()
{
    for(unsigned i=0;i<fcode_arrary.size();i++)
    {
        cout<<fcode_arrary[i].OP1<<' '<<fcode_arrary[i].OP2<<' '<<fcode_arrary[i].OP3<<endl;
    }
}