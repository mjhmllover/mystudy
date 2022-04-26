坐标polarDB，阿里云数据库内核组。
自己在过年后光速写了个小的数据库当项目，也算是弥补了项目经验上的不足。

[TOC]

# 简历面

编译器是怎么优化的
手撸内存分配器
通过一个数组来管理资源，需要使用时从数组中分配一个成员，使用完毕后从数组中释放该成员，释放后的成员可以被再次分配使用；请实现一个结构用来快速的对资源进行分配和释放。
聊项目

# 一面

说说 const char *ptr 和 char const *ptr的区别
写出一个包含以下元素的结构体的大小

```c++
char a;
char b;
double c; 
int d; 
```

static的作用
宏和inline区别
说说纯虚函数和虚函数
new和malloc区别
说出以下代码每行的作用和运行结果

```c++
char *p = (char *) malloc(10); 
sizeof(p)= 4
free(p+1)
```


手撸算法：最大子矩阵和，我是用 O ( n^ 3 ) 的做法写出来的
口述算法：N个数，有一个数出现超过N/2，寻找这个数
口述算法：链表倒数第K数
静态链接库和动态链接库的区别
epoll和select的作用
手撸多线程编程题：队列取数和放数交替操作（具体的记不太清了）
聊项目，因为自己的项目是实现了一个小的关系型数据库，正好这是数据库内核组，于是就…被花式吊打

# 二面

聊大一时候的图像检索的项目
聊数据库这个项目
手撸算法题：
按段（段内的元素不翻转）翻转链表：如链表 1->2->3->4->5->6->7->8->9，如果段大小为3，翻转后为7->8->9->4->5->6->1->2->3。
注意段大小作为参数传入。要求编写可以运行的测试用例（有main函数和足够的测试集），注意代码规范。

# 三面

Linux系统怎么看cache的相关信息
什么情况下内容会存进cache，怎么看cache的内容
聊项目。。。
算法题1：找二叉树最深的节点
算法题2：遍历二叉树最底层的节点
算法题3：在一个无向图中，如何判断两点是否联通
算法题4：在一个有向图中，如何判断两点是否联通
这一面还是比较简单的，几个算法题就是dfs、bfs来回考，判联通可以用并查集操作一下。

# 四面

聊项目
进程和线程区别
进程间的通信方式
如何调试C++多线程程序
聊人生

# 五面（hr面）

自我介绍
自己擅长什么不擅长什么
聊聊ACM经历
说说自己的学校怎么样
自己成绩怎么样
人生最大的挫折是什么

# 个人代码

因为阿里伯乐写代码是有存档的，所以就贴一下方便大家指教。

## 内存分配器

```c++
//通过一个数组来管理资源，
//需要使用时从数组中分配一个成员；
//使用完毕后从数组中释放该成员，释放后的成员可以被再次分配使用；请实现一个结构用来快速的对资源进行分配和释放。

// 10000这种数字是跟面试官沟通以后随便写的
class memory_manage {
public:
	obj* m_malloc() {
		obj* p = nullptr;
		if(l.size()) {
			p = l.front();
			l.erase(l.begin());
		} else if(cur<10000) {
			p = &arr[cur++];
		}
		return p;

	}

	void m_free(obj* p) {
		l.push_back(p);
	}

private:
	obj arr[10000];
	int cur = 0;
	list<obj*> l;
};
```



## 最大的子矩阵

```c++
N * N

-10 1 2 3
2 3 4 -100
1 2 3 5
0 0 0 0

K*M
和最大的子矩阵

O(N^2)
第一步：矩阵前缀和O(n^2)
第二步：遍历子矩阵O(n^4)

int a[N+2][N+2];
int sum[N+2][N+2];

void FindMax(int n) {
	int cur = 0,ans = 0;
	memset(sum,0,sizeof(sum));
	for(int i=1; i<=n; i++) {
		for(int j=1; j<=n; j++) {
			sum[i][j] = sum[i-1][j] + a[i][j];
		}
	}
	for(int i=1; i<=n; i++) {
		for(int j=1; j<=n; j++) {
			cur = 0;
			for(int k=1; k<=n; k++) {
				cur = cur + (sum[j][k]-sum[i-1][k]);
				if(cur<0) cur = 0;
				else ans = cur;
			}
		}
	}
	return ans;
}
```



## 多线程题

```c++
mutex g_mutex;
condition_variable cv;
queue<int> q;
bool flag = 0; // 0放，1取

int get() {
	lock_guard<mutex> mtx(g_mutex);
	cv.wait(mtx,[] {return flag==1;});
	int val = q.front();
	q.pop();
	flag = 0;
	cv.notify_one();
	return val;
}

void put(int val) {
	lock_guard<mutex> mtx(g_mutex);
	cv.wait(mtx,[] {return flag==0;});
	q.push(val);
	flag = 1;
	cv.notify_one();
	return;
}
```



## 花式翻转链表

```c++
// 评测题目: 按段（段内的元素不翻转）翻转链表：如链表 1->2->3->4->5->6->7->8->9，
// 如果段大小为3，翻转后为7->8->9->4->5->6->1->2->3。注意段大小作为参数传入。
// 要求编写可以运行的测试用例（有main函数和足够的测试集），注意代码规范。

#include <iostream>
#include <cstdio>
#include <vector>
using namespace std;

struct ListNode {
    int val;
    ListNode *next;
    ListNode(int x):val(x),next(NULL){}
};

struct BlockList {
    ListNode *LeftPre,*LeftNow,*RightNow;
    BlockList(ListNode* a,ListNode* b){
        LeftPre = a;
        LeftNow = b;
        RightNow = nullptr;
    }
};

void PrintList(ListNode* head) {
    head = head->next;
    while(head!=nullptr) {
        cout << head->val << ' ';
        head=head->next;
    }
    cout << endl;
}

ListNode* ReverseList(ListNode* head,int block_size) {
    vector<BlockList> Block;
    ListNode* p = head;
    ListNode* pre = nullptr;
    int now_size = -1;
    while(p!=nullptr) { //Record each block head
        if(now_size>=0 && now_size % block_size==0) {
            Block.push_back(BlockList(pre,p));
        }
        if(now_size>=0 && now_size % block_size==block_size-1 || p->next==nullptr) {
            Block.back().RightNow = p;
        }
        pre = p;
        p = p->next;
        now_size++;
    } 

    int block_num = Block.size();

    for(int i=0;i<block_num/2;i++) {
        BlockList left = Block[i];
        BlockList right = Block[block_num-i-1];
        left.LeftPre->next = right.LeftNow;
        ListNode* temp =  right.RightNow->next;
        right.RightNow->next = left.RightNow->next;
        right.LeftPre->next = left.LeftNow;
        left.RightNow->next = temp;
    }
    
    return head;
}

int main() {
    int ListSize,ReverseSize;
    cin >> ListSize >> ReverseSize;

    if(ListSize==0) {
        cout << "Empty List!" << endl;
        return 0;
    }

    ListNode* p = new ListNode(-1);
    ListNode* root = p;
    for(int i=0,val;i<ListSize;i++) {
        cin >> val;
        ListNode* q = new ListNode(val);
        p->next = q;
        p = p->next;
    }
    ReverseList(root,ReverseSize);
    PrintList(root);
    return 0;
}

/* 测试用例
9 3
1 2 3 4 5 6 7 8 9

3 1
1 2 3

0 0

2 2
1 2

3 4
1 2 3 

5 2
1 2 3 4 5
*/
```