# 随机生成2016-01-01 00:00:00 ~ 2018-12-31 23:59:59
CALL logdata(1, 100000);

# 认真审视逻辑相同，性能却差之千里的SQL语句

## 案例一：条件字段函数操作

# 统计所有年份中7月份的交易记录总数
select count(*) from test18.tradelog where month(t_modified) = 7;

# 对字段做了函数计算，就无法使用索引

# B+树能够快速定位，靠的是同一层兄弟节点的有序性

# 对索引字段做函数操作，可能会破坏索引值的有序性，因此优化器会放弃走树搜索功能
# t_modified长度比主键小，优化器选择遍历索引t_modified

explain select count(*) from test18.tradelog where month(t_modified) = 7;
+----+-------------+----------+------------+-------+---------------+------------+---------+------+--------+----------+--------------------------+
| id | select_type | table    | partitions | type  | possible_keys | key        | key_len | ref  | rows   | filtered | Extra                    |
+----+-------------+----------+------------+-------+---------------+------------+---------+------+--------+----------+--------------------------+
|  1 | SIMPLE      | tradelog | NULL       | index | NULL          | t_modified | 6       | NULL | 100010 |   100.00 | Using where; Using index |
+----+-------------+----------+------------+-------+---------------+------------+---------+------+--------+----------+--------------------------+
1 row in set, 1 warning (0.01 sec)

# rows：100010 >> 进行了全索引扫描
# Using index >> 使用了覆盖索引

# 变换SQL语句，避免全索引扫描
select count(*) from test18.tradelog where (t_modified >= '2016-7-1' and t_modified < '2016-8-1') or
                                           (t_modified >= '2017-7-1' and t_modified < '2017-8-1') or 
                                           (t_modified >= '2018-7-1' and t_modified < '2018-8-1');
+----------+
| count(*) |
+----------+
|     8384 |
+----------+
1 row in set (0.00 sec)

explain select count(*) from test18.tradelog where (t_modified >= '2016-7-1' and t_modified < '2016-8-1') or
                                                   (t_modified >= '2017-7-1' and t_modified < '2017-8-1') or 
                                                   (t_modified >= '2018-7-1' and t_modified < '2018-8-1');
+----+-------------+----------+------------+-------+---------------+------------+---------+------+------+----------+--------------------------+
| id | select_type | table    | partitions | type  | possible_keys | key        | key_len | ref  | rows | filtered | Extra                    |
+----+-------------+----------+------------+-------+---------------+------------+---------+------+------+----------+--------------------------+
|  1 | SIMPLE      | tradelog | NULL       | range | t_modified    | t_modified | 6       | NULL | 8384 |   100.00 | Using where; Using index |
+----+-------------+----------+------------+-------+---------------+------------+---------+------+------+----------+--------------------------+
1 row in set, 1 warning (0.00 sec)

# 对于不改变索引有序性的函数，优化器也不会考虑使用索引，如
select * from test18.tradelog where id + 1 = 10000;
# 优化器无法使用id索引快速定位到9999这一行


## 隐式类型转换

# 让人掉坑里的一个SQL
select * from test18.tradelog where tradeid=11112222;

# 数据类型转换规则：字符串与数字作比较时，会将字符串转换成数字
# 验证规则：
select "10" > 9;
# 结果为1 >> 字符串转成数字
# 结果为0 >> 数字转字符串
+----------+
| "10" > 9 |
+----------+
|        1 |
+----------+
1 row in set (0.00 sec)

# 掉坑里的SQL相当于
select * from test18.tradelog where CAST(tradeid AS signed int) = 11112222;
# 对索引字段做函数操作，优化器放弃走树搜索功能

explain select * from test18.tradelog where CAST(tradeid AS signed int) = 11112222;
+----+-------------+----------+------------+------+---------------+------+---------+------+--------+----------+-------------+
| id | select_type | table    | partitions | type | possible_keys | key  | key_len | ref  | rows   | filtered | Extra       |
+----+-------------+----------+------------+------+---------------+------+---------+------+--------+----------+-------------+
|  1 | SIMPLE      | tradelog | NULL       | ALL  | NULL          | NULL | NULL    | NULL | 100010 |   100.00 | Using where |
+----+-------------+----------+------------+------+---------------+------+---------+------+--------+----------+-------------+
1 row in set, 1 warning (0.00 sec)
# 显然，全表扫描，没有使用索引

# 反过来，执行
select * from test18.tradelog where id='8888';
+------+----------+----------+---------------------+
| id   | tradeid  | operator | t_modified          |
+------+----------+----------+---------------------+
| 8888 | 11112222 |        0 | 2017-02-12 12:30:22 |
+------+----------+----------+---------------------+
1 row in set (0.00 sec)
# 相当于 ？
select * from test18.tradelog where id=(int)'8888';

explain select * from test18.tradelog where id='8888';
+----+-------------+----------+------------+-------+---------------+---------+---------+-------+------+----------+-------+
| id | select_type | table    | partitions | type  | possible_keys | key     | key_len | ref   | rows | filtered | Extra |
+----+-------------+----------+------------+-------+---------------+---------+---------+-------+------+----------+-------+
|  1 | SIMPLE      | tradelog | NULL       | const | PRIMARY       | PRIMARY | 4       | const |    1 |   100.00 | NULL  |
+----+-------------+----------+------------+-------+---------------+---------+---------+-------+------+----------+-------+
1 row in set, 1 warning (0.00 sec)
# 使用了主键索引


## 隐式字符编码转换

# 插入数据
# `id` int(11) NOT NULL,`tradeid` varchar(32) DEFAULT NULL,`operator` int(11) DEFAULT NULL,`t_modified` datetime DEFAULT NULL
# `id` int(11) NOT NULL,`tradeid` varchar(32) DEFAULT NULL,`trade_step` int(11) DEFAULT NULL,`step_info` varchar(32) DEFAULT NULL
use test18;
truncate table tradelog;
insert into tradelog values(1, 'aaaaaaaa', 1000, now());
insert into tradelog values(2, 'aaaaaaab', 1000, now());
insert into tradelog values(3, 'aaaaaaac', 1000, now());

use test18;
truncate table trade_detail;
insert into trade_detail values(1, 'aaaaaaaa', 1, 'add');
insert into trade_detail values(2, 'aaaaaaaa', 2, 'update');
insert into trade_detail values(3, 'aaaaaaaa', 3, 'commit');
insert into trade_detail values(4, 'aaaaaaab', 1, 'add');
insert into trade_detail values(5, 'aaaaaaab', 2, 'update');
insert into trade_detail values(6, 'aaaaaaab', 3, 'update again');
insert into trade_detail values(7, 'aaaaaaab', 4, 'commit');
insert into trade_detail values(8, 'aaaaaaac', 1, 'add');
insert into trade_detail values(9, 'aaaaaaac', 2, 'update');
insert into trade_detail values(10, 'aaaaaaac', 3, 'update again');
insert into trade_detail values(11, 'aaaaaaac', 4, 'commit');

# 查询表tradelog中id=2的交易所有操作步骤信息
select d.* from test18.tradelog l, test18.trade_detail d where d.tradeid=l.tradeid and l.id=2;
# 相当于
select d.* from test18.tradelog l, test18.trade_detail d where CONVERT(d.tradeid USING utf8mb4)=l.tradeid and l.id=2;
+----+----------+------------+--------------+
| id | tradeid  | trade_step | step_info    |
+----+----------+------------+--------------+
|  4 | aaaaaaab |          1 | add          |
|  5 | aaaaaaab |          2 | update       |
|  6 | aaaaaaab |          3 | update again |
|  7 | aaaaaaab |          4 | commit       |
+----+----------+------------+--------------+
4 rows in set (0.00 sec)

explain select d.* from test18.tradelog l, test18.trade_detail d where d.tradeid=l.tradeid and l.id=2;
+----+-------------+-------+------------+-------+-----------------+---------+---------+-------+------+----------+-------------+
| id | select_type | table | partitions | type  | possible_keys   | key     | key_len | ref   | rows | filtered | Extra       |
+----+-------------+-------+------------+-------+-----------------+---------+---------+-------+------+----------+-------------+
|  1 | SIMPLE      | l     | NULL       | const | PRIMARY,tradeid | PRIMARY | 4       | const |    1 |   100.00 | NULL        |
|  1 | SIMPLE      | d     | NULL       | ALL   | NULL            | NULL    | NULL    | NULL  |   11 |   100.00 | Using where |
+----+-------------+-------+------------+-------+-----------------+---------+---------+-------+------+----------+-------------+
2 rows in set, 1 warning (0.17 sec)
# tradelog(utf8mb4)：驱动表，trade_detail(utf8)：被驱动表，tradeid：关联字段

# 优化方法一
# 把trade_detail表上的tradeid字段的字符集改成utf8mb4
alter table test18.trade_detail modify tradeid varchar(32) CHARACTER SET utf8mb4 default null;
explain select d.* from test18.tradelog l, test18.trade_detail d where d.tradeid=l.tradeid and l.id=2;
+----+-------------+-------+------------+-------+-----------------+---------+---------+-------+------+----------+-------+
| id | select_type | table | partitions | type  | possible_keys   | key     | key_len | ref   | rows | filtered | Extra |
+----+-------------+-------+------------+-------+-----------------+---------+---------+-------+------+----------+-------+
|  1 | SIMPLE      | l     | NULL       | const | PRIMARY,tradeid | PRIMARY | 4       | const |    1 |   100.00 | NULL  |
|  1 | SIMPLE      | d     | NULL       | ref   | tradeid         | tradeid | 131     | const |    4 |   100.00 | NULL  |
+----+-------------+-------+------------+-------+-----------------+---------+---------+-------+------+----------+-------+
2 rows in set, 1 warning (0.00 sec)
# 缺点：数据量比较大，或者业务上暂时不能做该DDL时不适合此方法

# 优化方法二
# 修改SQL语句
select d.* from test18.tradelog l, test18.trade_detail d where d.tradeid=CONVERT(l.tradeid USING utf8) and l.id=2;
explain select d.* from test18.tradelog l, test18.trade_detail d where d.tradeid=CONVERT(l.tradeid USING utf8) and l.id=2;
+----+-------------+-------+------------+-------+---------------+---------+---------+-------+------+----------+-------+
| id | select_type | table | partitions | type  | possible_keys | key     | key_len | ref   | rows | filtered | Extra |
+----+-------------+-------+------------+-------+---------------+---------+---------+-------+------+----------+-------+
|  1 | SIMPLE      | l     | NULL       | const | PRIMARY       | PRIMARY | 4       | const |    1 |   100.00 | NULL  |
|  1 | SIMPLE      | d     | NULL       | ref   | tradeid       | tradeid | 99      | const |    4 |   100.00 | NULL  |
+----+-------------+-------+------------+-------+---------------+---------+---------+-------+------+----------+-------+
2 rows in set, 1 warning (0.00 sec)

# 查询表trade_detail中id=4的操作
select l.operator from test18.tradelog l, test18.trade_detail d where d.tradeid=l.tradeid and d.id=4;
# 相当于 
select l.operator from test18.tradelog l, test18.trade_detail d where l.tradeid=CONVERT(d.tradeid USING utf8mb4) and d.id=4;
+----------+
| operator |
+----------+
|     1000 |
+----------+
1 row in set (0.00 sec)

explain select l.operator from test18.tradelog l, test18.trade_detail d where d.tradeid=l.tradeid and d.id=4;
+----+-------------+-------+------------+-------+---------------+---------+---------+-------+------+----------+-----------------------+
| id | select_type | table | partitions | type  | possible_keys | key     | key_len | ref   | rows | filtered | Extra                 |
+----+-------------+-------+------------+-------+---------------+---------+---------+-------+------+----------+-----------------------+
|  1 | SIMPLE      | d     | NULL       | const | PRIMARY       | PRIMARY | 4       | const |    1 |   100.00 | NULL                  |
|  1 | SIMPLE      | l     | NULL       | range | tradeid       | tradeid | 131     | NULL  |    1 |   100.00 | Using index condition |
+----+-------------+-------+------------+-------+---------------+---------+---------+-------+------+----------+-----------------------+
2 rows in set, 1 warning (0.00 sec)
# trade_detail(utf8)：驱动表，tradelog(utf8mb4)：被驱动表，tradeid：关联字段