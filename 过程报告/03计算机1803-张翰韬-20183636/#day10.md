# day10

  今天完成的任务主要有两点，一是将目标代码（伪码）和前面的程序对接了一下，二是将目标代码的生成部分进一步优化，改成了汇编语言的格式。

## 1.对接

  由于优化部分还没有完成，所以将目标代码优化的部分先和前面的语义部分生成的四元式对接一下，跑一下，看是不是能够达到预期生成的目标代码。由于前面的目标代码没有优化，所以生成的目标代码可能也会出现一些问题，而且目标代码的生成那部分活跃信息还处理。

## 2.改成汇编语言格式

生成的汇编语言格式如下：
main SEGMENT
 数据地址的分配
ASSUME CS:main , DS:main

操作指令

main ENDS

为了便于生成目标代码，所以产生的部分只有一个段，即main segment，该段既是数据段也是代码段，这样生成起来的格式比较固定。由于另一个同学所作的优化部分正在进行中，所以该段目前还没有和优化的部分整合调试，只是直接在语义分析后面连接了一下，没有bug。明天还需要进一步调试。

