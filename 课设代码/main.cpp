#include "object_code_generate.cpp"



int main()
{

    //�﷨�������

    grammar* test = new grammar();

    test->begin();

    cout<<"���ɵ���ԪʽΪ��"<<endl;

    test->print_quaternion_list();

    cout<<endl;

    //�Ż�

    divide();

    cout<<"�Ż������ԪʽΪ��"<<endl;

    display();

    //Ŀ���������

    /*optimize_list=quaternion_list;

    fcode* fcode_test = new fcode();

    fcode_test->to_code();

    cout<<"���ɵ�Ŀ�����Ϊ��"<<endl;

    fcode_test->print_fcode_array();*/

    system("pause");
    return 0;
}
