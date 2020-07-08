# day8

  今天完成的主要工作是目标代码的生成，先打算生成一个伪代码的形式，如果有时间的话，在和优化拼接完成后再继续做进一步目标代码的生成。今天完成的任务有设计目标生成的大体流程，实现赋值语句代码生成部分。

## 1.目标代码生成流程

  1.引入标志单元 flag，表示寄存器占用情况，若 flag=0，代表寄存器 AX可用， 否则代表寄存器 RAX被占用。 生成算法： 

①置初值 FLAG=0； 

②根据当前四元式 (w,op1,op2,tk ) 和 acc 值，查表生成目标代码；

③ 读取下一条四元式；

 ④若当前语句不是基本块出口，转到②，否则结束

## 2.编写实现赋值语句代码生成部分。

1.class fcode{
 	public:
 		void to_code();
 		void add(string op, string op1, string op2);
 		void fill_addr(int p);//用来反填跳转地址 
 	private://生成的目标指令 
 		static int ADDR;//序号地址 
        string OP1;
        string OP2;
        string OP3;
 }; 

2.void  fcode::to_code()
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

}
