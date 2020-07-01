<h1 align="center">编译原理课程设计</h1>

<p align="center">
    <img src="https://raw.githubusercontent.com/amtoaer/images/master/Annual-Salary-In-Millions/circle_4.png" width="90">
    <img src="https://raw.githubusercontent.com/amtoaer/images/master/Annual-Salary-In-Millions/circle_1.png" width="90">
    <img src="https://raw.githubusercontent.com/amtoaer/images/master/Annual-Salary-In-Millions/circle_2.png" width="90">
    <img src="https://raw.githubusercontent.com/amtoaer/images/master/Annual-Salary-In-Millions/circle_3.png" width="90">
</p>



<p align="center">
    <img src="https://img.shields.io/badge/language-c++-blue.svg?longCache=true&style=for-the-badge">
	<img src="https://img.shields.io/badge/codeby-vscode-orange.svg?longCache=true&style=for-the-badge">
	<img src="https://img.shields.io/badge/in-progress-red.svg?longCache=true&style=for-the-badge">
</p>

## 介绍

队伍名为`年薪百万队`，由四名同学组成，分别是：

+ 计算机1803蒙龙-20184364
+ 计算机1802刘文兴-20184402
+ 计算机1803张翰韬-20183636
+ 计算机1803钟洁-20184333

## 规范

> 规范第一版（待确定），暂定7.2日起实行。

1. 代码规范
   + 在较难理解的地方写明注释
   + 使用`utf-8`编码
   + 缩进使用`tab`，长度为4
   + 换行符使用`crlf`
   + 文件结束插入一行空行，移除行尾空格
   + 使用`vscode C/C++`内置的`WebKit`风格进行代码格式化
2. 仓库规范
   + 在`develop`分支进行开发，每次大更新`merge`到`master`分支
   + `commit message`尽量做到简单明了，统一使用中文/英文

## 进度

### 第一天

完成分工、简易文法设计，完成了词法分析样例的编写（需要进一步修改）。

### 第二天

扩充词法分析，在讨论后更正了文法的一些错误，将文法改写为`LL(1)`，使用递归下降子程序法实现语法分析（WIP）。

