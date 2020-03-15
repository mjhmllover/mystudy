
CALL pro(1, 'beijing', 2000);
CALL pro(2001, 'hangzhou', 2000);
CALL pro(4001, 'nanjing', 2000);

# 查询城市为“杭州”的所有人名字，并且按姓名排序返回前1000个人的姓名、年龄

# 全字段排序
select city,name,age from t where city='hangzhou' order by name limit 1000;

desc select city,name,age from t where city='hangzhou' order by name limit 1000;
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
select city,name,age from t where city='hangzhou' order by name limit 1000;

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


# rowid排序

# 控制用于排序的行数据的长度，单行的长度大于该参数值时则选择rowid排序
SET max_length_for_sort_data = 16;

sort_buffer中只放入name和id

explain select city,name,age from t where city='hangzhou' order by name limit 1000;
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