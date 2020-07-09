#include "object_code_generate.cpp"



int main()
{

    //语法语义分析

    grammar* test = new grammar();

    test->begin();

    cout<<"生成的四元式为："<<endl;

    test->print_quaternion_list();

    cout<<endl;

    //优化

    divide();

    cout<<"优化后的四元式为："<<endl;

    display();

    //目标代码生成

    optimize_list=QT;

    //cout<<optimize_list.size();

    fcode* fcode_test = new fcode();

    fcode_test->to_code();

    cout<<"生成的目标代码为："<<endl;

    fcode_test->print_fcode_array();

    system("pause");
    return 0;
}
