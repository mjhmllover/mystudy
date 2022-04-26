/*
** 对于含堆内存的类，显式的列出拷贝构造函数，并使用深拷贝
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

    // 深拷贝
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

A Get(bool flag)
{
    A a;
    A b;

    if (flag)
    {
        return a;
    }
    
    return b;
}

int main()
{
    /* 
    ** 此处取false(深拷贝)，取true不再会发生coredump(也是深拷贝)
    ** 不能依赖默认拷贝构造
    */
    A a = Get(true);

    return 0;
}
