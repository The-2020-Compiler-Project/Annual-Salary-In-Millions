# day2

今天又是忙碌的一天。唉，人生太难了！

今天主要完成了以下任务：

## 1.完成了词法分析的代码

经过昨天晚上和同组成员的讨论，最终确定将token序列存在一个vector向量中，今天改了一下，并确定了对应的类型码：

#define iT 1//标识符 
#define cT 2//字符 
#define sT 3//字符串 
#define CT 4//常数 
#define KT 5//关键字 
#define PT 6//界符 

大题将词法分析这部分的编程结束，但还是存在一点小问题，双界符不全一部分不能识别，后期应该再改改词法分析中的自动机，把剩下的几个双界符添加进去

## 2.设计了翻译文法的初稿

中午设计了一下翻译文法的初稿：

**********************翻译文法设计**********************
数据结构注意：
1.需要先建立符号表结构（数组表是必须的，感觉函数表可以先不设计，关键在于关键字查重和  数组元素的生成和判断合法问题），
同时应该注意数组的定义是否越界引用，判读合法性问题，然后翻译文法在运行的时候，需要对符号表进行访问和回填
主要是为了判断标识符是否被多次定义（重名）。
2.建议语法翻译分析的时候配合着符号表建立，例如识别到数组元素后，直接加入到数组表中，也就是语法分析和符号表的
建立同时进行。
3。需要两个辅助栈，符号栈和对象栈（辅助地址等的回填），在对象栈出栈的时候主要判断是否为对应的类型，例如如果当前需要一个整形，那么对象栈
应该弹出一个整型。
4.需要一个存储四元式序列的存储结构，建议使用vector里面放上结构体来存储四元式。
5-------最重要一点-------
我感觉语法分析，语法翻译和符号表的建立可以放在一起，在检测token构成的语句是否合法时，便为合法的变量等分配内存单元，存储到相应的表中

*****************************************************************
1） 程序--> void main (){函数体}     //目前没有子函数，无语义动作
************************************************
2）函数体--> 类型 ① 变量声明语句 函数体
| 标识符 ② 赋值语句 函数体
| 标识符 ② [ ③ 算术表达式 ④ ] 赋值语句 函数体
| if(表达式 ⑤ ){ 函数体 } ⑥ 函数体
| if(表达式 ⑤ ){ 函数体 } ⑥ else ⑦ {函数体} ⑧ 函数体
| While ⑨ (表达式 ⑩ ) {函数体} ⑪ 函数体
| ?
①将当前类型 Token 压入符号栈中。②查表看当前标识符是
否已经定义过，若没有定义将错误信息写 入错误流，返回负数标识错
误。否则创建一个指向当前纪录的 Token， 压入对象栈中，供赋值语
句使用； ③判断当前对象栈栈顶元素类型是否是数组，若不是处理错
误并 返回负数； ④判断当前对象栈顶元素类型是否是整型，若不是
处理错误并返回负数；否则，根据栈顶元素（数组偏移量)和次栈顶元素
（数组首 地址)，生成目标内存单元中间变量同时弹出这两个元素，-------------主要为了计算数组中的表达式例如a[2+3]
之后将中间变量 Token 压入对象栈中；内存单元地址值加上此中间变-----------内存单元的分配有点问题，我不太确定以什么方式实现，我感觉直接在符号表里象征的标记一下地址就行
量类型长度使其指向下一个可用位置。 ⑤根据对象栈栈顶元素生成判
断 then 四元式，弹出对象栈栈顶 元素并将指向新生成四元式的-------------生成四元式的步骤5，7，8，9.，10，11
Token 压入对象栈等待地址回填； ⑥修改对象栈栈顶元素指向的四元
式的地址字段为当前四元式 下一个地址（地址回填），弹出对象栈顶
元素； ⑦将对象栈栈顶元素的指向的四元式的地址字段继续加 1（使
其跳过 else 语句），生成 else 四元式，将指向新生成四元式的 Token
压入对象栈等待地址回填； ⑧生成标号 ifend 四元式，修改对象栈
栈顶元素指向的四元式的 地址为当前四元式所处地址（地址回填），
弹出对象栈顶元素； ⑨生成标号 while 四元式并压入对象栈中； ⑩
根据对象栈栈顶元素生成判断 while 四元式，弹出对象栈栈顶 元素
并将指向新生成四元式的 Token 压入对象栈等待地址回填； ⑪生成
标号 whend 四元式，其地址为对象栈中次栈顶元素四元式 的位置，
将对象栈栈顶元素所指向的四元式的地址修改为新生成四元 式的下
一个位置，再次弹出栈顶元素；
***************************************************
3）变量声明语句--> 标识符 ① 变量声明语句_1 ;②
| 标识符 ① [整型常数 ③ ] 变量声明语句_2 ;②
 ①在符号表中查找，看是否存在同名的标识符，若存在说明发
生重定义错误，进行错误处理。否则根据当前标识符和符号栈中类型信-------------无四元式，主要是生成符号表
息，向符号表中填写，并创建一个指向该条新增纪录的 Token 压入对
象栈中等待被初始化，内存单元地址值加上此类型 长度使其指向下一
个可用位置。 ②将符号栈栈顶元素弹出，因为声明语句已经结束类型
信息没有意义了。 ③在数组表中新增一条纪录，其中开始下标默认为-------设计到数组表的设计和运行
0，结束下标为当前整型常数，基本类型为符号栈栈顶元素，长度为整型
常量乘上基本类型长度。并将此类型向类型表中注册。然后根据 对象
栈中栈顶元素 Token 取到符号表中的新添纪录，将其类型 Token 指
向类型表中的这条新增记录。内存单元地址值加上数组 长度使其指向
下一个可用位置。
*********************************************************
4）类型--> int |char |bool | string | double--------------------------无操作
 步骤2已经将相应的类型token压栈
***************************************************
5）变量声明语句_1--> , ① 标识符 ② 变量声明语句_1
| =表达式 ③ | ? ④
①、④弹出对象栈栈顶元素，因为他已经过了初始化时机； ②--------------赋值部分，只涉及赋值四元式的生成
在符号表中查找，看是否存在同名的标识符，若存在说明 发生重定义
错误，进行错误处理。否则根据当前标识符和符号栈 中类型信息，向
符号表中填写，并创建一个指向该条新增纪录的 Token 压入对象栈中
等待被初始化，内存单元地址值加上此类型 长度使其指向下一个可用
位置。 ③赋值判断规则：整型可以赋值给实型，整数 0 和 1 当作
bool 类型可以赋值给 bool 变量，其他的赋值语句类型必须完全类型----这部分和c语言的文法有一些出入，可以保留，感觉也可以删掉，主要为了支持 bool a=0;这种初始化
匹配，否则就是错误。根据赋值判断对象栈顶元素和是否能赋值 给次
栈顶元素，若不能处理错误，否则生成赋值四元式。
********************************************************************
6）变量声明语句_2--> ,①标识符②[算数表达式③]变量声明语句_2
| ={表达式 ④ 数组初始化 ⑤}
| = 表达式 ⑥
| ? ⑦
①、⑦弹出对象栈栈顶元素，因为他已经过了初始化时机； ②        ------只有在第一次定义的时候才初始化，后期跳过初始化，ppt上有涉及这个问题，当时有文法的修改专门避免多次定义
在符号表中查找，看是否存在同名的标识符，若存在说明 发生重定义
错误，进行错误处理。否则根据当前标识符和符号栈 中类型信息，向
符号表中填写，并创建一个指向该条新增纪录的 Token 压入对象栈中
等待被初始化，内存单元地址值加上此类型 长度使其指向下一个可用***********///该部分为定义语句部分主要涉及到内存分配问题，我感觉很难搞，内存分配和符号表设计很棘手
位置。 ③在数组表中新增一条纪录，其中开始下标默认为 0，结束
下标为当前整型常数，基本类型为符号栈栈顶元素，长度为整型 常量----四元式生成：只涉及到赋值语句
乘上基本类型长度。并将此类型向类型表中注册。然后根据 对象栈中
栈顶元素 Token 取到符号表中的新添纪录，将其类型 Token 指向类
型表中的这条新增记录。内存单元地址值加上数组 长度使其指向下一
个可用位置。 ④创建一个临时的数组下标置初值为零，创建一个中间
变量 其地址为当前数组首地址加上数组下标乘上数组成员类型长度
之积，和一般赋值语句类似将当前对象栈栈顶元素的值赋值给中 间变
量并生成一个赋值四元式，数组下标加一。⑤创建一个中间变量其地址-----涉及到中间变量的地址分配问题，我一直很矛盾地址分配问题的实际实现方式，因此只给出大概逻辑
为当前数组首地址加上数组下标 乘上数组成员类型长度之积，和一般
赋值语句类似将当前对象栈 栈顶元素的值赋值给中间变量并生成一
个赋值四元式，数组下标 加一，若下标大于数组长度减一，数组越界，
处理错误。 ⑥赋值判断规则：整型可以赋值给实型，整数 0 和 1 当
作 bool 类型可以赋值给 bool 变量，其他的赋值语句类型必须完全
类型 匹配，否则就是错误。根据赋值判断对象栈顶元素和是否能赋值----目前涉及0，1可以给bool形
给次栈顶元素，若不能处理错误，否则生成赋值四元式。
******************************************************************
7）数组初始化--> ,表达式 ① 数组初始化| ？
  ①创建一个中间变量其地址为当前数组首地址加上数组下标
乘上数组成员类型长度之积，和一般赋值语句类似将当前对象栈 栈顶------赋值四元式
元素的值赋值给中间变量并生成一个赋值四元式，数组下标 加一，若
下标大于数组长度减一，数组越界，处理错误。
***************************************************************
8）赋值语句--> =表达式 ① ;
 ①赋值判断规则：整型可以赋值给实型，整数 0 和 1 当作--------这部分涉及上不涉及太大的语义动作，主要判断合法问题，类型兼容问题
bool 类型可以赋值给 bool 变量，其他的赋值语句类型必须完全类型-------如果合法且类型兼容，则可以直接赋值，生成相应的四元式，否则报错
匹配，否则就是错误。根据赋值判断对象栈顶元素和是否能赋值 给次
栈顶元素，若不能处理错误，否则生成赋值四元式保存到语 义分析得-------赋值语句涉及生成四元式
到的四元式表中
********************************************************************
9）表达式-->算数表达式 逻辑表达式------------不涉及生成四元式
| 字符常量①|字符串常量①
 ①将当前常量压入对象栈中。
*********************************************
10） 逻辑表达式--> >①算数表达式 ②
| >= ①算数表达式 ②
| <①算数表达式 ②
| <= ①算数表达式 ②
| ==①算数表达式 ②
| &&①算数表达式 ②
| ||①算数表达式 ②
| != ①算数表达式 ②
| ?
 ①将逻辑符号保存到符号栈中 ②对象栈中栈顶元素是第二个
操作数，次栈顶元素是第一个 操作数，符号栈栈顶元素（操作符），操-----------生成四元式的是2，放在表达式之后实现
作结果保存到临时生成 中间变量中，根据上述创建一个四元式存入语
义分析得到的四元 式表中。对象栈中弹出两个元素，符号栈弹出一个------临时变量的生成序号应该有一个函数专门涉及实现并保存，专门有人负责临时变量的生成
元素，同时将 中间变量压入对象栈中。
******************************************************************
11）E--> T E1
E1--> +①T②E1 | -①T②E1 | ?
T--> F T1
T1--> *①F②T1 | /①F②T1 | ?
F-->标识符 ③[算术表达式 ④ ] | 标识符 ③
| 整型常数 ⑤| 实型常数 ⑤ | (E)
 ①将操作符保存到符号栈中。 ②对象栈栈顶元素作为第二个
操作数和对象栈次栈顶元素作 为第一个操作数，符号栈栈顶元素作---------重点还是判断是否合法，这部分应该是连着语法分析和翻译一起做了。涉及到蒙龙提到的类型问题
为操作符，判断第一第二个 操作数是否是数值，若有一个说明错误处----------我还是坚持以string的形式存储，但判断的时候考虑string中是什么类型的东西
理错误。生成一个中间 变量累保存结果，其类型取两个操作数中类型
级别最高。根据上 面所述的四个数据生成四元式并保存到四元式表中。------生成四元式的操作主要为2（涉及到+，-，*，/才会生成四元式），其他的均是准备工作和表达式之间的转换，推导
对象栈中弹 出两个元素，符号栈中弹出一个元素。 ③查表看当前标
识符是否已经定义过，若没有定义将错误信息写入错误流，返回负数标
识错误。否则创建一个指向当前纪录的 Token，压入对象栈中，供赋值
语句使用； ④判断当前对象栈次栈顶元素类型是否是数组，若不是处理
错误并返回负数；判断当前对象栈顶元素类型是否是整型，若不 是处
理错误并返回负数；否则，根据栈顶元素（数组偏移量)和次栈顶元素
（数组首地址)，生成目标内存单元中间变量同时弹 出这两个元素，
之后将中间变量 Token 压入对象栈中；内存单元地址值加上此中间变
量类型长度使其指向下一个可用位置。⑤将当前常数 Token 压入对象
栈中。 

## 3.修改了一下之前写的.md文件

之前没注意.md在编写的时候#后面得加一个空格，导致格式出现问题，今天订正了一下

## 注：

设计的文法和翻译文法还是存在问题，还得继续讨论改进！果然文法的设计很难.............

