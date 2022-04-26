/*
** 对于含堆内存的类，显式的列出拷贝构造函数，并使用深拷贝
** 右值引用将避免深拷贝，优化性能
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

    // 移动构造函数
    A(A&& a):m_ptr(a.m_ptr)
    {
        a.m_ptr = nullptr;
        cout << "move construct " << this << endl;
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
    ** 移动构造函数将避免临时对象的深拷贝
    */
    A a = Get();

    return 0;
}
