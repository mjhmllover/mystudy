/*
** 对于含堆内存的类，默认拷贝构造被优化（返回值优化）
** 关闭返回值优化之后必coredump（g++ a.cpp -fno-elide-constructors）
*/

#include <iostream>
using namespace std;

class A
{
public:
	A():m_ptr(new int(0))
	{
        cout << "construct " << this << endl;
	}

	~A()
	{
        cout << "destruct " << this << endl;
		delete m_ptr;
	}

private:
	int* m_ptr;
};

A Get()
{
    A a;
    return a;
}

int main()
{
    /* 
    ** 拷贝构造貌似被优化了
    */
    A a = Get();

    return 0;
}
