# 预置10000行记录
call idata();

# 随机选择3个单词，如何实现？

## 内存临时表，使用memory引擎

# 使用order by rand()实现，执行流程比较复杂
select word from test17.words order by rand() limit 3;

explain select word from test17.words order by rand() limit 3;
+----+-------------+-------+------------+------+---------------+------+---------+------+------+----------+---------------------------------+
| id | select_type | table | partitions | type | possible_keys | key  | key_len | ref  | rows | filtered | Extra                           |
+----+-------------+-------+------------+------+---------------+------+---------+------+------+----------+---------------------------------+
|  1 | SIMPLE      | words | NULL       | ALL  | NULL          | NULL | NULL    | NULL | 9980 |   100.00 | Using temporary; Using filesort |
+----+-------------+-------+------------+------+---------------+------+---------+------+------+----------+---------------------------------+
1 row in set, 1 warning (0.00 sec)

# Using temporary：需要使用临时表；Using filesort：需要执行排序

# 16节结论：对于InnoDB表来说，执行全字段排序会减少磁盘访问，因此会被优先选择；
# 对于内存表，回表过程只是简单的根据数据行的位置，直接访问内存得到数据，不会导致多访问磁盘。此时，优先考虑的是用于排序的行越小越好，因此优先选择rowid排序。

# 涉及到表操作，才会增加扫描行数

# 获取扫描行数
select VARIABLE_VALUE into @a from performance_schema.session_status where variable_name = 'Innodb_rows_read';
select word from test17.words order by rand() limit 3;
select VARIABLE_VALUE into @b from performance_schema.session_status where variable_name = 'Innodb_rows_read';
select @b-@a;
+-------+
| @b-@a |
+-------+
| 10000 |
+-------+
1 row in set (0.00 sec)
# 这种方式获取的不是总的扫描行数，无法获取到内存表的扫描行数，可以通过查询慢查询日志查看

# 查看慢查询是否打开
show variables like 'slow_query_log';
# 查看写慢查询日志的查询时间阈值
show variables like 'long_query_time';
# 打开慢查询
set @@global.slow_query_log='ON';
set global slow_query_log='ON';

# 疑问之一：打开慢查询的两个写法有什么不同？

# 慢查询日志：
# Time: 2020-03-20T16:46:34.207066Z
# User@Host: root[root] @ localhost []  Id:     6
# Query_time: 0.002739  Lock_time: 0.000074 Rows_sent: 3  Rows_examined: 20003
SET timestamp=1584722794;
select word from test17.words order by rand() limit 3;

# 如果表没有主键，或主键被删除，那么，InnoDB会生成一个长度为6字节的rowid来作为主键。
# rowid：唯一标识数据行的信息

# 查看排序方法
SET optimizer_trace='enabled=on';
select word from test17.words order by rand() limit 3;
select * from `information_schema`.`OPTIMIZER_TRACE`\G;
*************************** 1. row ***************************
                            QUERY: select word from test17.words order by rand() limit 3
"filesort_summary": {
              "rows": 4,
              "examined_rows": 10000,
              "number_of_tmp_files": 0,
              "sort_buffer_size": 96,
              "sort_mode": "<sort_key, rowid>"
            }
# sort_mode显示使用的是rowid排序

# 内存临时表有两个字段：第一个为大于0小于1的随机小数，double类型，记为字段R；第二个为原表输出相关字段，即word，varchar(64)类型，记为字段W
# sort_buffer中有两个字段：第一个为字段R(8字节)，第二个为rowid(6字节)

# order by rand()实现随机选择三个单词使用了内存临时表；
# 需要扫描20003行；
# 使用了rowid排序方法进行排序；


## 磁盘临时表

# 参数tmp_table_size限制了内存临时表的大小，默认为16M
# 临时表大小超过了tmp_table_size时，内存临时表将转成磁盘临时表

# 磁盘临时表默认引擎为InnoDB，由参数internal_tmp_disk_storage_engine控制

# tmp_table_size：决定了是否需由内存临时表转成磁盘临时表，默认为16M
# sort_buffer_size：决定了是否需要临时文件进行排序
# max_length_for_sort_data：决定了是否采用rowid排序方法进行排序

set tmp_table_size=1024;
set sort_buffer_size=32768;
set max_length_for_sort_data=16;
set optimizer_trace='enabled=on';
select word from test17.words order by rand() limit 3;
select * from `information_schema`.`OPTIMIZER_TRACE`\G;
*************************** 1. row ***************************
                            QUERY: select word from test17.words order by rand() limit 3
"filesort_priority_queue_optimization": {
              "limit": 3,
              "rows_estimate": 1213,
              "row_size": 14,
              "memory_available": 32768,
              "chosen": true  # 表示使用了优先队列排序算法
            },
            "filesort_execution": [
            ],
"filesort_summary": {
              "rows": 4,
              "examined_rows": 10000,
              "number_of_tmp_files": 0,
              "sort_buffer_size": 88,
              "sort_mode": "<sort_key, rowid>"
            }

# 参与排序的字段是R字段和rowid字段组成的行，使用的是rowid排序
# 总排序大小(8+6)*10000=140000 > sort_buffer_size=32768，然而此处并不使用归并排序
# 采用优先队列排序算法

## 结论：无论采用哪种临时表，order by rand()都会使计算过程非常复杂，需要大量的扫描行数，资源消耗很大

## 随机排序方法

# 随机取1个word值，算法1：
# 1. 取得表主键id的最大值M和最小值N；
# 2. 随机生成一个最小值和最大值之间的数X=(M-N)*rand()+N；
# 3. 取第一个id不小于X的行；
select max(id),min(id) into @M,@N from test17.words;
set @X = floor((@M-@N+1)*rand() + @N);
select * from test17.words where id >= @X limit 1;

# 若id中间有空洞，算法1并不能做到真正的随机

# 随机算法2：
# 1. 取得整个表的行数，记为C；
# 2. 取Y = floor(C*rand())；
# 3. 使用limit Y,1取得一行；
select count(*) into @C from test17.words;
set @Y = floor(@C * rand());
set @sql = concat("select * from test17.words limit ", @Y, ",1");
prepare stmt from @sql;
execute stmt;
DEALLOCATE prepare stmt;
# limit后面不能直接跟变量，可采用prepare+execute的方法实现
# limit Y,1：按顺序读取Y+1个，丢掉前Y个，把下一个记录作为返回结果

## 总结
# 使用order by rand()进行随机排序，查询的执行代价往往比较大，应尽量避免
# 尽量将业务逻辑写在业务代码中，让数据库只做“读写数据”的事情

## 课后题：采用随机算法2完成随机选择3个单词，扫描总行数为C+(Y1+1)+(Y2+1)+(Y3+1)，如何进一步减少扫描行数？
select count(*) into @C from test17.words;
+-------+
| @C    |
+-------+
| 10000 |
+-------+
1 row in set (0.00 sec)

set @Y1 = floor(@C * rand());
+------+
| @Y1  |
+------+
| 5676 |
+------+
1 row in set (0.00 sec)

set @Y2 = floor(@C * rand());
+------+
| @Y2  |
+------+
| 3517 |
+------+
1 row in set (0.00 sec)

set @Y3 = floor(@C * rand());
+------+
| @Y3  |
+------+
|  557 |
+------+
1 row in set (0.00 sec)

select LEAST(@Y1, @Y2, @Y3) into @L;
mysql> select @L;
+------+
| @L   |
+------+
|  557 |
+------+
1 row in set (0.00 sec)

select GREATEST(@Y1, @Y2, @Y3) into @G;
+------+
| @G   |
+------+
| 5676 |
+------+
1 row in set (0.00 sec)

select @Y1 + @Y2 + @Y3 - @L - @G into @M;
+------+
| @M   |
+------+
| 3517 |
+------+
1 row in set (0.00 sec)

set @l_sql = concat("select id into @l_id from test17.words limit ", @L, ",1");
prepare stmt from @l_sql;
execute stmt;
DEALLOCATE prepare stmt;
+-------+
| @l_id |
+-------+
|   558 |
+-------+
1 row in set (0.00 sec)

set @m_sql = concat("select id into @m_id from test17.words where id > @l_id limit ", @M - @L, ",1");
prepare stmt from @m_sql;
execute stmt;
DEALLOCATE prepare stmt;
+-------+
| @m_id |
+-------+
|  3519 |
+-------+
1 row in set (0.00 sec)

set @g_sql = concat("select id into @g_id from test17.words where id > @m_id limit ", @G - @M, ",1");
prepare stmt from @g_sql;
execute stmt;
DEALLOCATE prepare stmt;
+-------+
| @g_id |
+-------+
|  5679 |
+-------+
1 row in set (0.00 sec)

# 随机选择三个单词
select * from test17.words where id in (@l_id, @m_id, @g_id);
+------+------+
| id   | word |
+------+------+
|  558 | affh |
| 3519 | dfbi |
| 5679 | fghi |
+------+------+
3 rows in set (0.00 sec)