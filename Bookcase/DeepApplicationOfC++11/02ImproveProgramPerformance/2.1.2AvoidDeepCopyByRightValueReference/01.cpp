/*
** 对于含堆内存的类，默认拷贝构造使用深拷贝还是使用浅拷贝存疑
** 默认拷贝构造不靠谱
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
    ** 此处只能取false(深拷贝)，取true会发生coredump(浅拷贝)
    ** 怪哉！！
    */
    A a = Get(false);

    return 0;
}
