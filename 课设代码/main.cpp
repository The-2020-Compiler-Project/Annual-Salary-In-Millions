#include "object_code_generate.cpp"

int main()
{

    //语法语义分析

    grammar* test = new grammar();

    test->begin();

    cout << "generated quaternion:" << endl;

    test->print_quaternion_list();

    cout << endl;

    //优化

    divide();

    cout << "optimized quaternion:" << endl;

    display();

    //目标代码生成

    optimize_list = QT;

    //cout<<optimize_list.size();

    fcode* fcode_test = new fcode();

    fcode_test->to_code();

    cout << "object code:" << endl;

    fcode_test->print_fcode_array();

    system("pause");
    return 0;
}
