#include<iostream>
#include<vector> 
#include <sstream>
#include <stack>
#include "grammatical_analysis.cpp"
using namespace std;
//单寄存器AX
int flag=0;//flag=0表示寄存器AX可用
int tempNum = 1;//产生空间，当寄存器被占用时，先移入其中 
stack <int> SE;//用来反填的栈 ,反填if，while 

vector<QUATERNION> optimize_list;//优化后的四元式接口 
class fcode{
 	public:
        void print_fcode_array();
 		void to_code();
 		void add(string op, string op1, string op2);
 		void fill_addr(int p);//用来反填跳转地址 
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
void  fcode::to_code()
{
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
				f1.add("MOV","AX",temp.operand_1.name);
				f2.add("MOV",temp.operand_3.name,"AX");
				fcode_arrary.push_back(f1);
				fcode_arrary.push_back(f2);
			 }
			 else//说明寄存器已经被栈用，此时AX中已经为res(E) 
			 {
			 	fcode f;
			 	f.add("MOV",temp.operand_3.name,"AX");//直接将AX里面的值移动到相应的变量中 
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
            	f1.add("MOV","AX",temp.operand_1.name);
            	f2.add(op_temp,"AX",temp.operand_2.name);
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
			} 
		 }
		 //>,<,=,>=,<=,==放在一起 
		 else if(temp.sign ==SIGN::larger||temp.sign==SIGN::smaller||temp.sign==SIGN::larger_equal||
		         temp.sign==SIGN:: smaller_equal||temp.sign==SIGN::is_equal||temp.sign==SIGN::not_equal)
		{
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
			f.add("FJMP","AX","?");//回来的时候回填 
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
			f.add("JMP","_","?");
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
 	
 	
}

void fcode::print_fcode_array()
{
    for(unsigned i=0;i<fcode_arrary.size();i++)
    {
        cout<<fcode_arrary[i].OP1<<' '<<fcode_arrary[i].OP2<<' '<<fcode_arrary[i].OP3<<endl;
    }
}