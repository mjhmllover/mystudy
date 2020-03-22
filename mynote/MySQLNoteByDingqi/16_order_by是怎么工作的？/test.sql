# pro(起始值, city, 步长)
CALL pro(1, 'beijing', 2000);
CALL pro(2001, 'hangzhou', 2000);
CALL pro(4001, 'nanjing', 2000);

# 查询城市为“hangzhou”的所有人名字，并且按姓名排序返回前1000个人的姓名、年龄


## 全字段排序

select city,name,age from test.t where city='hangzhou' order by name limit 1000;

desc select city,name,age from test.t where city='hangzhou' order by name limit 1000;
+----+-------------+-------+------------+------+---------------+------+---------+-------+------+----------+---------------------------------------+
| id | select_type | table | partitions | type | possible_keys | key  | key_len | ref   | rows | filtered | Extra                                 |
+----+-------------+-------+------------+------+---------------+------+---------+-------+------+----------+---------------------------------------+
|  1 | SIMPLE      | t     | NULL       | ref  | city          | city | 18      | const | 2000 |   100.00 | Using index condition; Using filesort |
+----+-------------+-------+------------+------+---------------+------+---------+-------+------+----------+---------------------------------------+
1 row in set, 1 warning (0.00 sec)

Using filesort: 需要排序，排序内存称为sort_buffer，sort_buffer中放入city、name和age三个字段

# 确定排序语句是否使用了临时文件

# 打开optimizer_trace，只对本线程有效
SET optimizer_trace='enabled=on';

# @a保存Innodb_rows_read的初始值
select VARIABLE_VALUE into @a from performance_schema.session_status where variable_name = 'Innodb_rows_read';

# 执行排序语句
select city,name,age from test.t where city='hangzhou' order by name limit 1000;

# 查看OPTIMIZER_TRACE输出
select * from `information_schema`.`OPTIMIZER_TRACE`\G;
*************************** 1. row ***************************
                            QUERY: select city,name,age from t where city='hangzhou' order by name limit 1000
                            TRACE: {
  ......
            "filesort_execution": [
            ],
            "filesort_summary": {
              "rows": 1001,
              "examined_rows": 2000,
              "number_of_tmp_files": 0,
              "sort_buffer_size": 66072,
              "sort_mode": "<sort_key, additional_fields>"
            }
}
MISSING_BYTES_BEYOND_MAX_MEM_SIZE: 0
          INSUFFICIENT_PRIVILEGES: 0
1 row in set (0.00 sec)

# @b保存Innodb_rows_read的当前值
select VARIABLE_VALUE into @b from performance_schema.session_status where variable_name = 'Innodb_rows_read';

# 计算Innodb_rows_read差值
select @b-@a;
+-------+
| @b-@a |
+-------+
|  2001 |
+-------+
1 row in set (0.00 sec)


## rowid排序

# 控制用于排序的行数据的长度，单行的长度大于该参数值时则选择rowid排序
SET max_length_for_sort_data = 16;

sort_buffer中只放入name和id

explain select city,name,age from test.t where city='hangzhou' order by name limit 1000;
-------------------------------+
| id | select_type | table | partitions | type | possible_keys | key  | key_len | ref   | rows | filtered | Extra                                 |
+----+-------------+-------+------------+------+---------------+------+---------+-------+------+----------+---------------------------------------+
|  1 | SIMPLE      | t     | NULL       | ref  | city          | city | 18      | const | 2000 |   100.00 | Using index condition; Using filesort |
+----+-------------+-------+------------+------+---------------+------+---------+-------+------+----------+---------------------------------------+
1 row in set, 1 warning (0.00 sec)

# 计算访问行数
select VARIABLE_VALUE into @a from performance_schema.session_status where variable_name = 'Innodb_rows_read';
select city,name,age from t where city='hangzhou' order by name limit 1000;
select * from `information_schema`.`OPTIMIZER_TRACE`\G;
"filesort_execution": [
            ],
            "filesort_summary": {
              "rows": 1001,
              "examined_rows": 2000,
              "number_of_tmp_files": 0,
              "sort_buffer_size": 28032,
              "sort_mode": "<sort_key, rowid>"
            }
select VARIABLE_VALUE into @b from performance_schema.session_status where variable_name = 'Innodb_rows_read';
select @b-@a;
+-------+
| @b-@a |
+-------+
|  3001 |
+-------+
1 row in set (0.00 sec)

# rowid排序多访问了一次表t的主键索引，排序完成后还需根据id去原表取值，因此2000+1000+1


## 构建不需要排序的“order by”

# 创建city和name的联合索引
alter table test.t add index city_user(city, name);

explain select city,name,age from test.t where city='hangzhou' order by name limit 1000;
+----+-------------+-------+------------+------+----------------+-----------+---------+-------+------+----------+-----------------------+
| id | select_type | table | partitions | type | possible_keys  | key       | key_len | ref   | rows | filtered | Extra                 |
+----+-------------+-------+------------+------+----------------+-----------+---------+-------+------+----------+-----------------------+
|  1 | SIMPLE      | t     | NULL       | ref  | city,city_user | city_user | 18      | const | 2000 |   100.00 | Using index condition |
+----+-------------+-------+------------+------+----------------+-----------+---------+-------+------+----------+-----------------------+
1 row in set, 1 warning (0.00 sec)

# 没有了“Using filesort”，即不需要排序了

# 再次进行测试
select VARIABLE_VALUE into @a from performance_schema.session_status where variable_name = 'Innodb_rows_read';
select city,name,age from t where city='hangzhou' order by name limit 1000;
select VARIABLE_VALUE into @b from performance_schema.session_status where variable_name = 'Innodb_rows_read';
select @b-@a;
-------+
| @b-@a |
+-------+
|  1000 |
+-------+
1 row in set (0.00 sec)

# 进一步优化，使用覆盖索引：索引上的信息足够满足查询要求，不需要再回到主键索引上去取数据

# 创建city、name和age的联合索引
alter table test.t add index city_user_age(city, name, age);

explain select city,name,age from test.t where city='hangzhou' order by name limit 1000;
+----+-------------+-------+------------+------+------------------------------+---------------+---------+-------+------+----------+--------------------------+
| id | select_type | table | partitions | type | possible_keys                | key           | key_len | ref   | rows | filtered | Extra                    |
+----+-------------+-------+------------+------+------------------------------+---------------+---------+-------+------+----------+--------------------------+
|  1 | SIMPLE      | t     | NULL       | ref  | city,city_user,city_user_age | city_user_age | 18      | const | 2000 |   100.00 | Using where; Using index |
+----+-------------+-------+------------+------+------------------------------+---------------+---------+-------+------+----------+--------------------------+
1 row in set, 1 warning (0.00 sec)

# Using index：使用了覆盖索引，性能好很多

# 获取访问主键上数据行数
select VARIABLE_VALUE into @a from performance_schema.session_status where variable_name = 'Innodb_rows_read';
select city,name,age from t where city='hangzhou' order by name limit 1000;
select VARIABLE_VALUE into @b from performance_schema.session_status where variable_name = 'Innodb_rows_read';
select @b-@a;
+-------+
| @b-@a |
+-------+
|  1000 |
+-------+
1 row in set (0.00 sec)

# 疑问之一：这里也需要扫描1000行？？？


## 课后题：假设已存在联合索引city_name(city,name)，查询hangzhou和nanjing两个城市中所有市民的姓名，按姓名排序，取前100条记录
# 问：若SQL语句写成：
select * from test.t where city in ('hangzhou','nanjing') order by name limit 100;
# 则该语句执行时会有排序过程吗？
# 如何实现不需排序的方案？
# 增加分页需求，要求显示第101页，即“limit 100”改为“limit 10000,100”，如何实现？

# 初步思考之后，结论是会有排序过程。不同城市之间的市民的姓名是无序的。

## 不需排序的方案：
# 把一条SQL语句拆成两条：
select * from test.t where city = 'hangzhou' order by name limit 100; # 结果存进数组A
select * from test.t where city = 'nanjing' order by name limit 100; # 结果存进数组B
# 采用归并排序的思想，对有序数组A、B进行排序，取name最小的前100个值，即为所求

# 实现分页：
select * from test.t where city = 'hangzhou' order by name limit 100100; 
select * from test.t where city = 'nanjing' order by name limit 100100; 