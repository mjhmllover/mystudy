# 第二章  SQLite概况

- 学习目标

  读完本章后，你将学到：

  - SQLite数据库系统及其功能
  - SQLite支持哪些显著特性
  - SQLite如何在数据库文件中存储表
  - 如何编写、编译和执行SQLite应用程序
  - 一些最常用的SQLite API
  - 模块化的SQLite架构
  - SQLite的局限性

- 本章简介

  SQLite是一个小型、零配置、定制、可嵌入、线程安全、易于维护、面向事务、基于SQL的关系数据库管理系统。它将整个数据库存储在一个包含所有表和索引的文件中。它将所有表组织成单独的$B^+$-树，并将索引组织成B-树。它支持核心事务属性，即原子性、一致性、隔离性和持久性。它使用基于锁的并发控制方案和基于日志的恢复方案。

  SQLite支持ANSI SQL-92大部分特性和许多特定于SQLite的命令。此外，它还提供了一个很好的框架，您可以在其中定义定制的SQL函数、聚合器和排序序列。它还支持基于UTF-8和UTF-16标准的Unicode文本编码。

  本章涉及SQLite的几乎所有功能。本文高度概述了SQLite如何与SQL应用程序配合使用。它通过展示一些简单的应用程序，让您熟悉一些SQLite API，这些API用于SQLite和数据库应用程序之间的正常交互。它还介绍了SQLite源代码的结构和组织。总的来说，本章是SQLite的简要介绍。

## SQLite概论

许多数据库管理系统（DBMS）是在过去几十年中发展起来的。DB2、lnformix、Ingres、MySQL、Oracle、PostgreSQL、SQL Server和Sybase是在商业上成功应用于企业数据库应用程序的几个例子。成功的嵌入式数据库系统包括Sybase iAnywhere、InterSystems Cache和Microsoft Jet。SQLite[22]是关系数据库管理系统（RDBMS）家族的最新成员，也是一个非常成功的嵌入式数据库系统。SQLite于2000年5月29日首次公开发布alpha代码，其功能非常有限。SQLite 1.0于2000年8月17日发布。从那时起，它经历了漫长的旅程。SQLite 2.0.0于2001年9月20日发布，SQLite 3.0.0于2004年6月18日发布。截至2011年9月19日，本书的最新版本是SQLite 3.7.8。这本书是基于这个特别的版本。SQLite开发团队继续发布新版本。您可以访问SQLite主页http://www.SQLite.org获取最新版本。你可以在http://www.sqlite.org/changes.html网页上找到其功能开发的时间顺序。(当你拿到这本书的时候，SQLite肯定会有一个不同的更新版本。但是，核心功能和处理数据的方式预计不会有太大改变。在这里获得的知识将有助于学习新的特征。通过阅读这本书，您肯定会对设计和开发嵌入式数据库系统需要什么有一个大致的了解。我鼓励你自己开发一个（**正有此意！！**）。)

### SQLite的显著特征

SQLite完全是使用ANSI C编程语言开发的。它是一个易于维护、速度相当快、基于SQL的RDBMS。它有以下一些值得称赞的显著特征：

- 零配置。在使用SQLite数据库管理软件之前，您不需要执行任何单独的安装或设置进行环境初始化。开始运行SQLite没有特定的步骤。没有配置文件来控制不同的行为。数据库不需要任何管理。您可以从它的主页http://www.sqlite.org/download.html下载SQLite源代码，使用您喜欢的C编译器将其编译成一个可执行库，并开始将该库作为数据库应用程序的一部分使用。您可以从数量有限的平台中获得库。
- 可嵌入的。您不需要专用于SQLite的单独服务器进程。SQLite库可以嵌入到您自己的应用程序中。应用程序不需要包含任何进程间通信方案来与SQLite交互。
- 为应用程序提供接口。SQLite为C应用程序操作数据库提供了一个SQL环境。它为动态SQL提供了一组调用级应用程序编程接口（API）函数；您可以动态地组装SQL语句，并将它们传递给接口执行。此外，还可以使用许多回调功能。对应用程序没有特殊的预处理和编译要求；一个普通的C编译器将完成这项工作。
- 支持事务。SQLite支持核心事务属性，即原子性、一致性、隔离性和持久性（ACID）。在系统崩溃或电源故障时，数据库用户或管理员无需执行任何操作即可恢复数据库。当SQLite读取数据库时，它会以用户透明的方式自动对数据库执行必要的恢复操作。
- 线程安全的。SQLite是一个线程安全的库，应用程序进程中的许多线程可以并发地访问相同或不同的数据库。SQLite负责数据库级线程并发。
- 轻量的。当所有SQLite功能都启用时，SQLite库的占用空间约为324KB（在Linux上使用gee -Os时为331835字节）。通过在从源代码构建库时禁用所有高级功能，可以将占用空间减少到190KB左右。
- 可定制的。SQLite提供了一个很好的框架，您可以在其中定义和使用定制的SQL函数、聚合函数和排序序列。
- 支持Unicode。SQLite支持基于UTF-8和UTF-16标准的Unicode文本编码。UTF16支持小端和大端形式。
- 内存零泄漏。如果应用程序严格遵循推荐的与SQLite库交互的协议，那么该库将被声明永远不会泄漏内存。
- 内存要求低。虽然SQLite可以使用无限量的堆栈和堆空间，但它可以在最小的4KB堆栈空间和大约100KB的堆空间下运行。这项功能对于受少量主内存限制的小型设备（如手机）非常有效。但是，可用内存越多，SQLite性能越好。
- 支持多平台。SQLite运行在Linux、Windows、Mac OS X、OS/2、OpenBSD和其他一些操作系统上。它还可以在Android、Symbian、Palm和VxWroks等嵌入式操作系统上运行。
- 单一数据库文件。每个数据库都完全存储在一个本地文件中；用户数据和元数据存储在同一个文件中。单文件方法简化了将数据库从一个地方移动/复制到另一个地方的过程。（尽管SQLite在操作数据库时使用了许多临时文件。）
- 跨平台的。SQLite允许在不同平台之间移动数据库文件。例如，您可以在Linux x86机器上创建一个数据库，并在ARM、Windows或MAC平台上使用相同的数据库（通过复制），而无需进行任何更改。数据库在所有受支持的平台上的行为相同。您可以在32位和64位机器上，或者在大端和小端系统之间使用相同的数据库，而不会出现任何问题。
- 向后兼容的。SQLite 3是向后兼容的。这意味着库的任何更高版本都可以使用由早期库版本创建的数据库。SQLite开发团队努力保持库向后兼容。但是，版本3库不能与版本2数据库一起使用。

### 使用简单

SQLite不同于当今大多数其他SQL数据库管理系统，因为它的主要设计目标是简单。SQLite开发团队相信KISS的理念：keep it simple and splendid。他们努力使SQLite保持简单，即使它偶尔会导致某些功能的实现比较低效。本质上，SQLite是

- 管理简单的，
- 操作简单的，
- 很容易嵌入到C应用程序中的，
- 维护简单的，
- 定制简单的，定制的应用也是简单的，
- 实现ACID属性的方法也是简单的。

简单性：简单的软件是易于实现、测试、维护、增强、集成、编制文档等的软件。SQLite满足这些条件。

为了实现简单性，SQLite开发团队选择牺牲一些数据库用户认为有用的许多DBMS特性，如高事务并发性、细粒度访问控制、许多内置函数、存储过程、一些SQL语言功能（如对象关系）、TB或PB可伸缩性等等。

可靠性：SQLite非常可靠。可靠性似乎是它的简单性的结果。

### SQL特性和SQLite命令

SQLite支持ANSI SQL-92数据定义和数据操作功能的大部分特性，以及一些特定于SQLite的命令。（这些命令类似于SQL语句，但它们本身并不操纵用户数据。）可以使用标准数据定义SQL构造创建表、索引、触发器和视图。可以使用INSERT、DELETE、UPDATE和SELECT SQL构造操作存储的信息。下面是SQLite 3.7.8发行版支持的SQL特性列表。(未来的每个新版本都可能有额外的功能。最新支持的功能可从SQLite网页http://www.sqlite.org/lang.html获得。)

1. 数据定义语言（DDL）：
   - 创建表、索引、视图和触发器；
   - 删除表、索引、视图和触发器；
   - 部分支持ALTER TABLE（重命名表和添加列）；
   - 唯一、非空和检查（域）约束；
   - 外键约束；
   - 自增，按列排序；
   - 冲突消除。
2. 数据操作语言（DML）：
   - 支持INSERT，DELETE，UPDATE和SELECT语句；
   - 支持子查询，包括相关子查询；
   - 支持group by，order by，offset-limit和排序；
   - 支持INNER JOIN，LEFT OUTER JOIN，NATURAL JOIN；
   - 支持UNION，UNION ALL，INTERSECT，EXCEPT；
   - 支持命名参数与参数绑定；
   - 支持行触发器。
3. 事务相关的命令：
   - BEGIN；
   - COMMIT；
   - ROLLBACK；
   - SAVEPOINT；
   - ROLLBACK TO；
   - RELEASE。
4. SQLite命令：
   - reindex；
   - attach，detach；
   - explain；
   - pragma。

SQL标准指定了大量关键字，这些关键字不能用作表、视图、索引、列、约束或数据库的名称。SQLite放宽了这一限制，允许您使用关键字作为标识符，方法是将关键字用反引号、单引号、双引号或“[”和“]”对括起来。此外，SQLite提供了一个很好的框架，您可以在其中定义和使用定制的SQL函数、聚合函数和排序序列。pragma是特殊的SQLite命令，用于更改SQLite库的行为或查询库中的内部（非表）元数据。SQLite attach命令帮助事务同时在多个数据库上工作。此类事务也符合ACID标准。

以下ANSI SQL-92特性在SQLite 3.7.8版本时还不支持。(当前列表见http:/ /www.sqlite.org/omitted.html网页。)

1. ALTER TABLE的许多特性，如重命名或删除列，添加或删除约束；
2. 语句级的触发器；
3. RIGHT或FULL OUTER JOIN；
4. 更新视图；
5. 授权和回收权限。

### 数据库存储

SQLite将整个数据库存储在一个普通的本地文件中，该文件可以放在在本地文件系统目录中的任何位置。我们经常说，文件是数据库的同义词，因为没有其他文件存储有关数据库本身的信息。有权限读取文件的用户可以从数据库中读取任何内容。对文件和容器目录具有写入权限的用户可以更改数据库中的任何内容。只要本地操作系统/文件系统允许文件增长，数据库就可以增长。SQLite在Linux系统上可支持超大文件（大于2GB）（如果这些系统有此选项的话）。它将所有表和索引分别组织为单独的$B^+$-树和B-树。它使用一个单独的日志文件来保存事务恢复信息，这些信息可在事务中止或系统故障时使用。

### 并发限制

SQLite允许多个应用程序同时访问同一个数据库。但是，它支持事务之间有限形式的并发。它允许一个数据库上有任意数量的并发读事务，但只允许一个独占的写事务。它不支持更细的数据粒度(如表、页、行、列或单元格)上的并发。

### SQLite的使用

SQLite是一个非常成功的嵌入式RDBMS。它已广泛应用于底层到中层的数据库应用程序，如web服务、手机、PDA、机顶盒、独立设备。你甚至可以在初学者数据库课程中向学生教授关系数据库和SQL语言时使用它。您还可以在高级数据库管理课程或数据库项目中使用它作为参考技术。它是免费提供的，没有许可证约束，因为它是完全公开的。（尽管有各种可选的专有部件，比如智能卡的SQLite、加密解决方案，您需要从SQLite的所有者Hwaci订购。学校里的学生们可能就不需要担心这些专有部件了。）

**网络服务器：**一个基于SQLite正常工作的web服务器平均每天可支持多达100000次访问；SQLite开发团队已经证明，SQLite甚至可以允许每天1000000次的访问。

SQLite是开源的，可以在公共资源中获得（有关开源的更多信息，请访问http://opensource.org)。您可以从网页http://www.sqlite.org/download.html下载SQLite源代码，使用您最喜欢的C编译器将其编译成可执行库，然后开始在数据库应用程序中使用该库。SQLite支持在Linux、Windows、MAC OS X、OS/2、Solaris、OpenBSD和其他一些操作系统上运行。在该本书中，我仅限于使用SQLite 3.7.8的Linux版本。该版本是截至2011年9月19日的最新版本。

## SQLite应用示例

在本节中，我将向您介绍一些简单的数据库应用程序，说明SQLite的各种核心功能。您将熟悉一些最重要、最常用的SQLite API函数和API常量。除第2.2.2节讨论一些API外，以下小节将介绍这些应用程序。您可能还记得SQLite是一个可嵌入的库，它被嵌入到应用程序处理地址空间中。图2.1描述了SQLite应用程序的通用模式。如图所示，SQLite库嵌入到应用程序进程中，进程堆空间的一部分用于存储SQLite的运行时数据。当然，SQLite在调用其API函数时使用堆栈。

此处略过图2.1。

### 一个简单的应用示例

让我们通过研究一个非常简单的应用程序来开始对SQLite土地的探索。如下所示，展示了一个典型的SQLite应用程序。它是一个典型的C程序，调用SQLite API函数来处理单个SQLite数据库。它演示了通过执行SQL查询来使用SQLite访问数据库的简单方法。

```c
#include <stdio.h>
#include "sqlite3.h"

int main(void)
{
	sqli te3* db = O; /* connection handle */
	sqli te3_stmt* stmt = O; /* statement handle */
	int retcode;

	retcode = sqli te3_open("MyDB", &db); /* Open a database named MyDB */
	if (retcode != SQLITE_OK) {
		sqli te3_close (db);
		fprintf(stderr, "Could not open the MyDB database\n");
		return retcode;
	}
	
	retcode = sqlite3_prepare(db, "select SID from Students order by SID", -1, &stmt, O);
	if (retcode != SQLITE_OK) {
		sqli te3_close (db);
		fprintf(stderr, "Could not compile a select statement\n");
		return retcode;
	}
	
	while (sqli te3_step(stmt) == SQLITE_ROW) {
		int i = sqli te3_column_int(stmt, O);
		printf("SID = %d\n", i);
	}
	sqli te3_f inalize(stmt );
	sqli te3_close( db);
	return SQLITE_OK;
}
```

您可以编译上面的示例应用程序并执行它。本文档中显示的示例输出是在Linux机器上生成的，但是这些示例可以在运行SQLite的其他平台上运行。

**注意：**这些例程假定你已经准备好了sqlite3可执行程序，libsqlite3.so（Windows平台的sqlite3.dll或者Mac OS X平台的libsqlite3.dylib） 和接口定义文件sqlite3.h 。您可以从http://www.sqlite.org获取这些源代码或者二进制可执行文件（二进制文件仅适用于Linux、MAC OS X和Windows。）如果将这三个文件（sqlite3、共享库和sqlite3.h）放在与示例应用程序相同的目录中，您可能会发现使用这些示例将更容易。

例如，假设您在一个Linux系统上，并且您已经将示例程序保存为app1.c，并将app1.c、libsqlite3.so、sqlite3和sqlite3.h放在同一个目录下，使用GNC C编译器gcc执行下述命令编译该C文件：

```shell
gcc app1.c -o ./app1 -lsqlite3 -L
```

它将在当前工作目录中生成一个名为app1的二进制文件。您可以执行应用程序以查看输出。要引入SQLite库，可能需要在Linux系统上的LD_library_PATH环境变量中包含工作目录名。是的，您运行了app1，但它不会产生任何输出；这是因为当前工作目录中没有应用程序所需的“MyDB”数据库。

**注意：**SQLite源代码和应用程序都必须使用同一个编译器进行编译。如果您已将SQLite作为软件包安装，或者您的操作系统发行版附带了预安装的SQLite，则可能需要使用一组不同的编译器参数。例如，在Ubuntu上，可以执行命令`sudo aptitude install sqlite3 libsqlite3-dev`命令安装SQLite，再使用命令`cc appl.c -o ./appl -lsqlite3`编译示例应用程序。因为SQLite已包含在最新版本的Mac OS X中，所以同样的编译命令也适用于该版本.

app1应用程序在当前工作目录中打开了MyDB数据库文件。数据库至少需要一个名为Students的表；此表必须至少有一个名为SID的整数列。在下一个示例应用程序中，您将学习如何在数据库中创建新表，以及如何在表中插入行，但目前，您可以使用sqlite3应用程序的命令创建和填充表：

```shell
./sqlite3 MyDB "create table students (SID integer)"
./sqlite3 MyDB "insert into students values (200)"
./sqlite3 MyDB "insert into students values (100)"
./sqlite3 MyDB "insert into students values (300)"
```

如果你现在运行app1，你会看到如下输出：

```shell
SID = 100
SID = 200
SID = 300
```

**注意：**在Linux、Unix和Mac OS X上，当您在命令提示符下输入app1时，可能需要在app1前加上`./`前缀，如`./app1`。

打开数据库后，应用程序首先准备SQL语句`select SID from Students order by SID`。然后，它将遍历语句生成的行集，一个接一个地获取SID值，并打印这些值。最后，它关闭准备好的语句和数据库。

SQLite是一个调用级接口库，在运行时嵌入到应用程序进程地址空间中。该库将所有SQLite API实现为C函数。所有API函数名的前缀都是sqlite3\_（API常量的前缀都是SQLITE\_），它们的签名都在sqlite3.h中声明。其中一些用于app1应用程序，即sqlite3_open、sqlite3_prepare、sqlite3_step、sqlite3_column_int、sqlite3_finalize和sqlite3_close。应用程序还使用一些助记符API常量，即SQLITE_OK和SQLITE_ROW，来比较API函数返回的值。在我介绍其他SQLite应用程序之前，下一小节将讨论一些关键的SQLite API。

### SQLite API

SQLite接口定义了一组API（一组C函数和一组命名常量）。API函数是应用程序和SQLite库之间正常通信的唯一方式。（SQLite还使用驻留在应用程序空间中的回调C函数。）我在上一小节中概述了几个API函数。在这里，我介绍了一组基本的API函数，它们在SQLite应用程序中最常用。有关这些函数和其他API函数的详细讨论可在SQLite网页（http://www.sqlite.org/capi3ref.html）上找到。总共大约有185个API函数。网页（http://www.sqlite.org/c3ref/funclist.html）上提供了所有API函数和常量的列表。

1. **sqlite3_open：**此函数有两个参数，一个输入，另一个输出。输入是一个数据库文件名。通过执行open函数，应用程序可以打开与SQLite库的新连接或会话，以访问给定的数据库文件。在本书中，我将其称为库连接。（应用程序可能有其他开放库连接来访问相同或不同的数据库。SQLite会明确地对待这些库连接，就SQLite而言，它们彼此独立。）在库连接内部，函数打开数据库文件。如果文件不存在，该函数会自动创建数据库文件；默认文件权限为0644。如果成功打开（或创建）数据库，则此函数将SQLITE_OK返回给应用程序。否则，API将返回错误码给应用程序。

   **懒惰式打开文件：**当打开或创建一个数据库文件时，SQLite遵循一种懒惰的方式——实际的打开或创建被延迟，直到访问该文件进行读取。如果数据库文件确实存在，SQLite会在需要时自动将数据库恢复到一致状态。惰性文件创建使您有机会使用pragma命令（重新）定义各种数据库设置参数的值。（这些设置参数将在第3章中讨论。）

   open函数通过输出参数（在上例中为db）返回连接句柄（指向sqlite3类型对象的指针），该句柄用于在库连接上应用进一步的操作（对于当前打开的SQLite会话而言）。句柄表示此库连接的完整状态。

   图2.3展示了一个典型的场景，其中一个应用程序打开了到SQLite库的两个连接，以访问同一个数据库文件。库连接彼此独立，它们各自由独立的sqlite3对象表示。SQLite库中独立的sqlite3对象表示并管理单个库连接。如图所示，一个连接有三个prepare语句，而另一个连接没有。我接下来讨论prepare语句。

   此处略过图2.3。

   **更新的API：**最近，SQLite开发团队不鼓励使用这个open函数；他们建议改为使用sqlite3_open_v2函数。还有许多其他的*_v2 API函数。为了保证应用程序呈现出简单性，我在本书中避免使用这些较新的API函数。

2. **sqlite3_prepare：**该函数编译一条SQL语句，并生成一个等效的内部对象（类型为sqlite3_stmt）。这个对象在数据库文献中通常被称为预处理语句，并在SQLite中作为字节码程序实现。字节码程序是由数据库引擎执行的SQL语句的抽象表示。我将在第175页第7.2节讨论字节码编程语言。我在本书中交替使用字节码程序和预处理语句这两个术语来表示相同的意思。此函数在成功时返回SQLITE_OK，在失败时返回相应的错误代码。

   prepare函数通过形参（上例中为stmt）返回语句句柄（指向sqlite3_stmt对象的指针），句柄用于进一步操作prepare语句。在前面的示例程序中，我预备了`select SID from Students order by SID`语句作为语句句柄。此句柄的作用类似于打开的游标，用于获取select语句生成的结果行集，一次取一行。通过执行API函数sqlite3_step向前移动光标，我将在下面讨论这个函数。

3. **sqlite3_step：**在通过调用sqlite3_prepare函数准备SQL语句后，必须调用sqlite3_step函数一次或多次以执行预处理语句。对step函数的每次调用都会执行字节码程序，直到它到达一个断点（因为它产生了一个新的输出行），或者直到它停止（因为没有更多的行）。函数在前一种情况下返回调用者SQLITE_ROW，在后一种情况下返回调用者SQLITE_DONE。在前一种情况下，应用程序可以使用适当的API函数sqlite3_column\_\*读取行的列值。（API函数sqlite3_column\_\*相关信息请参阅列表中的下一项。）再次调用step函数以检索下一行。step函数移动光标的位置以获取SELECT语句的结果。最初，光标指向输出行集的第一行之前。每次执行step函数都会将光标指针移动到行集的下一行。光标仅在前进方向上移动。对于不返回行的SQL语句（例如UPDATE、INSERT、DELETE、CREATE和DROP），因为没有要处理的行，step函数始终返回SQLITE_DONE。最终，step函数返回SQLITE_DONE。（如果不先调用sqlite3_reset函数，将程序执行重置回初始状态，则不应在此语句句柄上再次调用step函数。本章后续将简要讨论重置函数。）

   如果在执行step函数的过程中出现错误，则返回代码为SQLITE_BUSY，或SQLITE_ERROR，或SQLITE_MISUSE。SQLITE_BUSY意味着引擎试图访问一个繁忙（也称为锁定）的数据库，并且没有注册回调函数来重新解决这种情况，或者回调函数决定中断执行。应用程序可能会在稍后再次调用step函数，以重试执行预处理语句。SQLITE_ERROR意味着发生了运行时错误（例如违反约束）；不应在此语句句柄上再次调用step函数。SQLITE_MISUSE意味着step函数被不当调用。也许它是在一个已经完成处理（或关闭）的预处理语句上调用的，或者在一个之前返回SQLITE_ERROR或SQLITE_DONE的语句上调用的。

4. **sqlite3_column_*：**如果sqlite_step函数返回SQLITE_ROW，您可以通过执行sqlite3_column_* API函数之一来检索行中每一列的值。SQL/SQLite和C语言之间的数据类型不匹配由引擎自动处理：列函数在两种语言之间转换数据，并将数据从存储类型转换为请求的类型。（例如，如果值的内部表示是FLOAT，并且应用程序请求文本输出值，SQLite在内部使用sprintf()进行值转换。）

   可用来从列中读取数据的列API函数有以下五个：sqlite3_column_int，sqlite3_column_int64，sqlite3_column_double，sqlite3_column_text和sqlite3_column_blob。每个函数名的最后一部分指示应用程序可以从SQLite库中获得什么样的值。在前面的示例应用程序中，每个输出行都是一个整数值，我们通过执行返回整数值的sqlite3_column_int函数来读取SID列的值。（如果语句句柄当前未指向有效行，或者列索引超出范围，则这些函数生成的输出未定义。最左边的列有索引0、下一列为1、再下一列为2，依此类推。可以使用sqlite3_column_count API函数获取列总数。对于非select语句，该函数返回0。）Blob和text值需要应用程序知道其大小。SQLite具有sqlite3_column_bytes函数，该函数以字节数返回列值的大小。

5. **sqlite3_finalize：**此函数关闭并销毁语句句柄和相关的预处理语句。也就是说，它擦除字节码程序，并释放分配给语句句柄的所有资源。语句句柄将变得无效，并且不能重复使用。如果语句执行成功或根本没有执行，那么finalize函数将返回SQLITE_OK。如果前一次的语句执行失败，那么该函数将返回一个错误码。finalize函数可以在执行准备好的语句期间的任何时候调用。如果在调用此例程时引擎尚未完成语句执行，这就像在执行过程中遇到错误或中断一样。SQLite将回滚不完整的更新并中止执行，返回的结果码将是SQLITE_ABORT。

6. **sqlite3_close：**此函数关闭库连接，并释放分配给该连接的所有资源。连接句柄将变得无效。此函数在成功时返回SQLITE_OK，在失败时返回其他错误码。如果有预处理的语句尚未完成，则返回SQLITE_BUSY，连接保持打开状态。

7. **Other useful functions：**上面讨论的六个（类别）API函数是SQLite库的核心，SQLite库主要处理两个数据结构，即sqlite3和sqlite3_stmt。其他广泛使用的API函数是sqlite3_bind_*和sqlite3_reset。在一条SQL语句字符串中（输入到sqlite3_prepare函数），可以用SQL位置参数标记“?”（或编号或命名参数，如?NNN、AAA、@AAA或$AAA，其中NNN是整数，AAA是字母数字标识符）替换一个或多个文字值。它们将成为预处理语句的输入参数。对于未编号或未命名的参数，最左边的参数索引为1。对于编号参数，索引就是编号。对于命名参数，可以通过调用API函数sqlite3_bind_parameter_index的方式获得索引。可以使用bind函数设置这些参数的值。（如果在多个位置使用命名或编号的参数，则所有位置使用相同的界限值。）如果参数没有绑定值，则采用SQL的NULL值。有以下七个可用的绑定的API函数：sqlite3_bind_null、sglite3_bind_int、sqlite3_bind_int64、sqlite3_bind_double、sqlite3_bind_text、sqlite3_bind_blob和sqlite3_bind_value。每个函数名的最后一部分指示可以使用该函数将哪些类型的值绑定到参数。（sqlite3_bind_value函数用于绑定通用值。）

   reset函数将语句句柄（即预处理语句）重置回初始状态，但有一个例外：所有绑定了值的参数都保留其值。语句预处理由应用程序重新执行，并在重新执行中重用这些绑定值。但是，应用程序可以在开始重新执行之前再次执行绑定函数，以新值替换部分或所有这些值。或者，可以通过执行sqlite3_clear_bindings API函数删除所有绑定值。reset函数对于重复查询非常有用。

8. **Return values：**所有API函数都返回零或正整数值。SQLite开发团队强烈建议使用助记符来检查返回值，而不是硬编码的整数值。返回值SQLITE_OK表示成功；SQLITE_ROW表示sqlite3_step函数在SELECT语句返回的行集中找到了一个新行；SQLITE_DONE表示语句执行已完成。自SQLite 3.7.8发布以来，共有28个主要的和一些扩展的成功和错误返回码。由于返回码是SQLite接口的一部分，所以它们的值不会在不同的小版本之间发生变化。

总之，应用程序预处理SQL语句，如果需要，它会将值绑定到预处理语句，一次或多次遍历预处理语句，重置预处理语句，以便使用相同或不同的绑定值再次执行该语句。应用程序最终完成语句以销毁预处理语句。

API字符集：上述API函数处理UTF-8编码的输入文本。有一些独立的API函数只处理UTF-16编码的文本。

### 直接执行SQL

如下所示，展示了另一个SQLite应用程序，它可以从命令行运行，以交互方式操作数据库。该命令有两个参数：第一个是数据库文件名，第二个是SQL语句。它首先打开数据库文件，然后通过执行sqlite3_exec API函数将语句应用于数据库，最后关闭数据库文件。exec函数直接执行SQL语句，而无需应用程序像前面的示例应用程序中所做的那样，手动执行prepare、step和finalize API函数。如果语句有输出，exec函数将为每个输出行执行回调函数，并让应用程序进一步处理该行。您必须对给定的数据库文件具有读取权限，并且根据查询类型，您可能需要对该文件及其所在的目录具有写入权限。

```c
#include <stdio.h>
#include "sqlite3.h"

static int callback(void *unused, int argc, char **argv, char **colName)
{
	int i;
	for(i = 0; i < argc; i++ ){ // Loop over each column in the current row
		printf("%s = %s\n", colName[i], argv[i] ? argv[i] : "NULL");
	}
	print£(" \n");
	return 0;
}

int main(int argc, char **argv){
	sqlite3* db = 0;
	char* errMsg = 0;
	int re;
	if (argc != 3){
		fprintf(stderr, "Usage: %s DATABASE-NAME SQL-STATEMENT\n", argv[0]);
		return -1;
	}
	re= sqlite3_open(argv[l], &db);
	if (re != SQLITE_OK){
		fprintf(stderr, "Can't open database %s: %s\n", argv[l], sqlite3_errmsg(db));
		sqli te3_close( db);
		return -2;
	}
	re = sqlite3_exec( db, argv[2], callback, 0, &errMsg);
	if (re ! = SQLITE_OK){
		fprintf(stderr, "SQL execution error: %s\n", errMsg);
	}
	sqlite3_close(db);
	return re;
}
```

**sqlite3_exec：**此函数直接执行一个或多个SQL语句。（两个连续的SQL语句之间用分号分隔。在内部，它按照从左到右的输入顺序依次编译和执行这些语句。如果任何语句执行导致错误，则不会执行其余语句。如果语句具有SQL参数标记，则取SQL空值。如果语句生成输出，则exec函数为每个输出行调用了用户指定的回调函数。回调函数的签名可以在图2.4中找到。exec函数很方便的糅合了prepare、step、（column）和finalize函数。尽管如此，SQLite开发团队不鼓励使用该功能，因为他们可能会在未来的版本中删除它。

**sqlite3_errmsg：**如果在API函数执行过程中发生错误，可以通过调用此函数获得有关错误的更多信息。函数返回库连接上发生的最后一个错误。该消息基本上是对错误的英语描述。

您可以将应用程序代码编译成可执行文件，例如app2。现在可以发出对数据库进行操作的SQL语句。假设您正在当前工作目录中的同一个MyDB数据库上工作。通过执行以下命令行，可以在Students表中插入新行：

```shell
./app2 MyDB "insert into Students values(lO0)"
./app2 MyDB "insert into Students values(lO)"
./app2 MyDB "insert into Students values(lO00)"
```

如果现在运行上一个应用程序（app1），你将看到如下输出：

```shell
SID = 10
SID = 100
SID = 100
SID = 200
SID = 300
SID = 1000
```

还可以在数据库中创建新表；例如，`/app2 MyDBExtn "create table Courses (name vaIchar，SID integer)"`，即在当前工作目录下的新数据库MyDBExtn中创建表Courses。

注意：SQLite有一个前面提到的交互式命令行实用程序（sqlite3），您可以使用它发出SQL命令。可以从SQLite下载网页下载预编译的二进制版本，也可以从源代码处编译。这个app2示例本质上是sqlite3的一个基本实现。

### 多线程应用程序

SQLite可以在单线程或多线程模式下使用。对于后者，进程中的许多线程可以通过同一个库连接并发访问相同或不同的数据库。但是，要使其成为线程安全库，它的构建方式必须有所不同。

**线程模式：**线程模式由预处理器宏SQLITE_THREADSAFE控制。为了线程安全，必须编译SQLITE源代码，将宏设置为1表示序列化，将宏设置为2表示正常的多线程。如果宏设置为0，则库处于单线程模式。这意味着一个进程中的多个线程可以使用同一个SQLite库，但一个线程创建的SQLite（连接和语句）句柄不能被另一个线程安全使用；同时由多个线程使用SQLite也是不安全的。在前两种情况下，这一限制被放宽，库被称为“线程安全”。在正常的多线程模式（安全值为2）下，虽然多个线程可以使用同一个库连接，但不能同时使用；它们可以相互独占地使用连接；不过，它们可以同时使用不同的连接。在序列化多线程模式中，没有这样的限制。默认为序列化模式。您可以调用sqlite3_threadsafe API函数来确定您使用的SQLite库是否是线程安全的。如果编译时选项是多线程或序列化的，则可以在库开始时或运行时使用sqlite3_open_v2或sqlite3_config API函数更改此选项。

如下所示，提供了一个非常简单的多线程应用程序。应用程序创建10个线程，每个线程都试图在同一个MyDB数据库的Students表中插入一行。SQLite实现了一个基于锁的并发方案，因此一些INSERT语句可能会由于锁冲突而失败。应用程序不需要担心并发控制和数据库一致性问题；它不能破坏数据库。SQLite负责并发控制和一致性问题。但是，您需要检查失败，并在代码中适当地处理它们（例如，您可以重试失败的语句，或者通知用户它失败了，并让她决定下一步做什么）。

```c
#include <stdio.h>
#include <pthread.h>
#include "sqlite3.h"

void* myInsert(void* arg)
{
    sqlite3* db = 0;
    sqlite3_stmt* stmt = 0;
    int val = (int)arg;
    char SQL[lO0];
    int re;
    re= sqlite3_open("MyDB", &db);/* Open a database named MyDB */
    if (re != SQLITE_OK) {
    	fprintf(stderr, "Thread[%d] fails to open the MyDB database\n", val);
    	goto errorRet;
    }
    sprintf(SQL, "insert into Students values(%d)", val); /* Dynamically compose a SQL */
    re = sqlite3_prepare(db, SQL, -1, &stmt, 0); /* Prepare the insert statement */
    if (re != SQLITE_OK) {
        fprintf(stderr, "Thread[%d] fails to prepare SQL: %s; return code %d\n", val, SQL, re);
        goto errorRet;
    }
    re = sqlite3_step(stmt);
    if (re != SQLITE_DONE) {
    	fprintf(stderr, "Thread[%d] fails to execute SQL: %s; return code %d\n", val, SQL, re);
    } else {
    	printf("Thread[%d] successfully executes SQL: %s\n", val, SQL);
    }
    sqlite3_finalize(stmt);
    errorRet:
        sqlite3_close(db);
        return (void*)rc;
}

int main(void)
{
    pthread_t t[lO];
    int i;
    for (i = 0; i < 10; i++)
    	pthread_create(&t[i], 0, myInsert, (void*)i); /* pass the value of i */
    for (i = 0; i < 10; i++) pthread_join(&t[i], 0); /* wait for all threads to finish */
    return 0;
}
```

**警告！：**此应用程序可能无法在Windows和Mac OS X上“开箱即用”。您可能需要使用线程支持的选项重新编译SQLite，或直接获取pthread库，以使应用程序在这些平台上工作。Mac OS X包含pthread库，您可以在http://sourceware.org/pthreads-win32/上获取Windows的pthread库。

在示例应用程序中，每个线程打开自己与同一数据库的连接，并处理连接句柄。这曾经是SQLite早期版本中的工作模型。对于这些版本，SQLite开发团队不建议跨线程使用任何SQLite句柄。尽管SQLite API可以在跨线程使用句柄的情况下工作，但不能保证它们的正确性。事实上，在Linux的某些版本中，SQLite库可能会被破坏并生成coredump。

在SQLite 3.3.1及其后续版本中，对上述中线程之间共享库连接的限制有所放宽。线程可以在互斥模式下安全地使用库连接（在正常的多线程模式下）。这意味着您可以将连接从一个线程切换到另一个线程，只要前一个线程不持有连接上的任何本机文件锁。如果线程没有挂起的事务，并且已经重置或完成了连接上的所有语句，那么可以安全地假设没有锁被持有。在序列化模式下。没有这样的限制。

**Fork警告！：**在Unix/Linux系统下，不能将open的SQLite数据库跨fork系统调用带入子进程。如果这样做，可能会出现数据库损坏或应用程序崩溃等问题。

### 使用多个数据库

如下所示，显示了一个典型的SQLite应用程序，它可以在两个数据库上运行。（我简化了代码，不包括函数调用的错误检查。应用程序首先打开MyDB数据库，然后将MyDBExtn数据库连接到当前库连接。在完成attach命令执行后，单个库连接有两个数据库连接，应用程序现在可以通过同一个库访问两个数据库中的所有表。）这是一个很好的连接。我假设MyDB数据库有一个Students（SID）表，MyDBExtn数据库有一个Courses（name，SID）表。应用程序执行一条SQL select语句，从两个数据库访问这两个表。

**库连接与数据库连接混淆：**到SQLite库的连接可以有多个数据库与其关联，如图2.7所示。在图中，单个库连接有三个数据库连接，每个连接到不同的数据库文件。应用程序可以通过同一个库连接访问所有数据库。虽然应用程序只看到一个库连接，但SQLite在内部打开了多个数据库连接，每个数据库一个。如果库连接只有一个数据库，我们简洁地称之为数据库连接。您已收到连接混乱的警告。

```c
#include <stdio.h>
#include "sqlite3.h"

int main(void)
{
    sqlite3* db = O;
    sqlite3_open("MyDB", &db); /* Open a database named MyDB */
    sqlite3_exec(db, "attach database MyDBExtn as DBl" , 0, 0, 0);
    sqlite3_exec(db, "select* from Students S, Courses C where S.sid = C.sid", callback , 0, 0);
    sqlite3_close( db);
    return O;
}
```

此处略过图2.7。

### 事务处理

如下所示，显示了一个处理事务的典型SQLite应用程序。应用程序通过执行begin命令打开一个事务，将一行插入到事务中的Students表中，另一行插入到Courses表中，最后通过执行commit命令提交事务。INSERT语句不需要回调函数，因此，我在示例应用程序的sqlite3_exec的回调参数传0。如果第二次插入失败，可以执行rollback命令而不是commit命令，第一次插入将被撤消。我将在第63页的第2.3节中详细介绍事务。

**注意：**SQLite允许在一个exec API调用中使用多个SQL语句；如下所示，同一批命令可以通过在单个exec调用中传递这一系列语句来执行`begin; insert into Students values(2000); insert into Courses values('SQLite Database', 2000); commit`。这种写法更好——如果第二次插入失败，系统将中止事务。如果批处理包含SELECT语句，则使用相同的回调函数来处理生成的行集。

```c
#include <stdio.h>
#include "sqlite3.h"

int main(void)
{
    sqlite3* db = 0;
    sqlite3_open("MyDB", &db); /* Open a database named MyDB */
    sqlite3_exec(db, "attach database MyDBExtn as DBl", 0, 0, 0);
    sqlite3_exec(db, "begin", 0, 0, 0);
    sqlite3_exec(db, "insert into Students values(2000)", 0, 0, 0);
    sqlite3_exec(db, "insert into Courses values('SQLite Database', 2000)", 0, 0, 0);
    sqlite3_exec(db, "commit", 0, 0, 0);
    sqlite3_close(db);
    return 0;
}
```

### catalog的使用

数据库系统还存储有关用户信息的（元）信息。元信息也被表示为catalog表或系统表，以区别于用户表。本质上，catalog是一个由SQLite本身创建和维护的表，它存储有关数据库的一些元信息。SQLite在每个数据库中维护一个名为SQLite_master的主目录。主目录存储有关表、索引、触发器和视图的模式信息。您可以查询主目录（例如，从sqlite_uu主目录中选择*），但不能手动删除或直接修改目录。还有其他可选的目录表。所有目录表名称都以sqlite前缀开头，这些名称由sqlite开发团队保留供内部使用。（您不能创建数据库对象，例如表、视图、索引、触发器，这些对象的名称是大写、小写或混合大写的。）我将在第67页的第2.4节中详细介绍目录。