# day6

  今天注意到的问题主要有两点，一个是符号表索引方面的迭代器问题，另一个是在使用git时注意到的一些vim规则。

## 1.迭代器方面的问题

### (1)对于list

  对于list中只有元素的删除操作会导致指向该元素的迭代器失效，其他元素迭代器不受影响。

### (2)对于vector

   1.对于vector元素的删除、插入操作会导致指向该元素以及后面的元素的迭代器失效。
   2.当插入（push_back）一个元素后，end操作返回的迭代器肯定失效。
   3.当插入(push_back)一个元素后，capacity返回值与没有插入元素之前相比有改变，则需要重新加载整个容器，此时first和end操作返回的迭代器都会失效。不增容不会导致迭代器失效。
   4.当进行删除操作（erase，pop_back）后，指向删除点的迭代器全部失效；指向删除点后面的元素的迭代器也将全部失效。

## 2.vim语法的问题

  在vim中保存文件时通过在命令模式中实现。要进入命令模式，可在一般模式中按：（冒号） 并等待键入命令，通过Enter可完成命令 。

:wq 保存并退出当前文件

 :w 保存当前文件并留在编辑器中 :w！ 若文本为只读模式，强制保存

 :q 退出当前文件（仅在没有未保存更改的情况下）

 :q! 退出当前文件，忽略任何未保存的修改 W用于保存写入，q用于退出，！用于强制执行