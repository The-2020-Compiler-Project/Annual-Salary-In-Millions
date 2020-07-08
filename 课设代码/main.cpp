#include "object_code_generate.cpp"


int main()
{
    grammar* test = new grammar();
    test->begin();

    optimize_list=quaternion_list;

    cout<<"生成的四元式为："<<endl;

    test->print_quaternion_list();

    cout<<endl;

    fcode* fcode_test = new fcode();

    fcode_test->to_code();

    cout<<"生成的目标代码为："<<endl;
    fcode_test->print_fcode_array();

    system("pause");
    return 0;
}
