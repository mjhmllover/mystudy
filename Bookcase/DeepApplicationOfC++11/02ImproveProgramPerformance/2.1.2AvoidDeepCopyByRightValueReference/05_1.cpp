/*
** 对于含堆内存的类，显式的列出拷贝构造函数，并使用深拷贝
** g++ a.cpp -fno-elide-constructors -std=c++11
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

    // 拷贝构造仍保留
    A(const A& a):m_ptr(new int(*a.m_ptr))
    {
        cout << "copy construct " << this << endl;
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
    ** 显式的定义拷贝构造函数，就不会发生coredump
    */
    A a;
    a = Get();

    return 0;
}
