### 查询长时间不返回

## 等MDL锁

# sessino A
lock table test19.t write;
                                        # session B
                                        select * from test19.t where id=1;
# session A 持有MDL写锁；session B需要获取MDL读锁，进入等待状态

show processlist;
+----+------+-----------+--------+---------+------+---------------------------------+-----------------------------------+
| Id | User | Host      | db     | Command | Time | State                           | Info                              |
+----+------+-----------+--------+---------+------+---------------------------------+-----------------------------------+
|  8 | root | localhost | test19 | Query   |    0 | starting                        | show processlist                  |
| 11 | root | localhost | NULL   | Query   |   13 | Waiting for table metadata lock | select * from test19.t where id=1 |
+----+------+-----------+--------+---------+------+---------------------------------+-----------------------------------+
2 rows in set (0.00 sec)

# 如何解决？找到持有MDL写锁的pid，然后kill掉

# 启动时，设置performance_schema=on才可以查看一些系统状态，但是相比设置为off会有10%左右的性能损失

# 查表找出造成阻塞的process id，要另开一个会话执行
select blocking_pid from sys.schema_table_lock_waits;
Empty set (0.00 sec)

# 上述查询为空的原因：没有启用MDL等待相关instruments
# 启用MDL等待相关instruments
call sys.ps_setup_enable_instrument('wait/lock/metadata/sql/mdl');

# 查询是否启用
select * from performance_schema.setup_instruments where name like '%/mdl';
+----------------------------+---------+-------+
| NAME                       | ENABLED | TIMED |
+----------------------------+---------+-------+
| wait/lock/metadata/sql/mdl | YES     | YES   |
+----------------------------+---------+-------+
1 row in set (0.00 sec)

# 再查表找阻塞的process id
select * from sys.schema_table_lock_waits;
*************************** 1. row ***************************
               object_schema: test19
                 object_name: t
           waiting_thread_id: 33
                 waiting_pid: 7
             waiting_account: root@localhost
           waiting_lock_type: SHARED_READ
       waiting_lock_duration: TRANSACTION
               waiting_query: select * from test19.t where id=1
          waiting_query_secs: 29
 waiting_query_rows_affected: 0
 waiting_query_rows_examined: 0
          blocking_thread_id: 32
                blocking_pid: 6
            blocking_account: root@localhost
          blocking_lock_type: SHARED_NO_READ_WRITE
      blocking_lock_duration: TRANSACTION
     sql_kill_blocking_query: KILL QUERY 6
sql_kill_blocking_connection: KILL 6
1 row in set (0.00 sec)

ERROR: 
No query specified

mysql> kill 6;
Query OK, 0 rows affected (0.00 sec)

# 杀掉阻塞的进程
kill 6;


## 等flush

# session A
select sleep(1) from test19.t;
                                # session B
                                flush table test19.t;
                                                        # session C
                                                        select * from test19.t where id=1;

# session D 执行
show processlist;
+----+------+-----------+------+---------+------+-------------------------+-----------------------------------+
| Id | User | Host      | db   | Command | Time | State                   | Info                              |
+----+------+-----------+------+---------+------+-------------------------+-----------------------------------+
|  7 | root | localhost | NULL | Query   |   51 | Waiting for table flush | flush table test19.t              |
|  8 | root | localhost | NULL | Query   |   66 | User sleep              | select sleep(1) from test19.t     |
|  9 | root | localhost | NULL | Query   |   38 | Waiting for table flush | select * from test19.t where id=1 |
| 10 | root | localhost | NULL | Query   |    0 | starting                | show processlist                  |
+----+------+-----------+------+---------+------+-------------------------+-----------------------------------+
4 rows in set (0.00 sec)

# 消除阻塞
kill query 8;


## 等行锁

# session A：占用写锁
begin;
update test19.t set c=c+1 where id=1; 
                                      # session B：加了共享锁，请求读锁
                                      select * from test19.t where id=1 lock in share mode;

# session C
show processlist;
+----+------+-----------+------+---------+------+------------+------------------------------------------------------+
| Id | User | Host      | db   | Command | Time | State      | Info                                                 |
+----+------+-----------+------+---------+------+------------+------------------------------------------------------+
|  7 | root | localhost | NULL | Query   |   12 | statistics | select * from test19.t where id=1 lock in share mode |
|  8 | root | localhost | NULL | Sleep   |   34 |            | NULL                                                 |
|  9 | root | localhost | NULL | Query   |    0 | starting   | show processlist                                     |
| 10 | root | localhost | NULL | Sleep   |  219 |            | NULL                                                 |
+----+------+-----------+------+---------+------+------------+------------------------------------------------------+
4 rows in set (0.00 sec)

# 查出谁在占用这个写锁
select * from sys.innodb_lock_waits where locked_table='`test19`.`t`'\G;
*************************** 1. row ***************************
                wait_started: 2020-04-11 00:38:06
                    wait_age: 00:00:05
               wait_age_secs: 5
                locked_table: `test19`.`t`
                locked_index: PRIMARY
                 locked_type: RECORD
              waiting_trx_id: 421949405547232
         waiting_trx_started: 2020-04-11 00:38:06
             waiting_trx_age: 00:00:05
     waiting_trx_rows_locked: 1
   waiting_trx_rows_modified: 0
                 waiting_pid: 7
               waiting_query: select * from test19.t where id=1 lock in share mode
             waiting_lock_id: 421949405547232:44:4:2
           waiting_lock_mode: S
             blocking_trx_id: 923149
                blocking_pid: 8
              blocking_query: NULL
            blocking_lock_id: 923149:44:4:2
          blocking_lock_mode: X
        blocking_trx_started: 2020-04-11 00:38:00
            blocking_trx_age: 00:00:11
    blocking_trx_rows_locked: 1
  blocking_trx_rows_modified: 1
     sql_kill_blocking_query: KILL QUERY 8
sql_kill_blocking_connection: KILL 8
1 row in set, 3 warnings (0.01 sec)

# KILL QUERY 8：停止8号线程当前正在执行的语句
# KILL 8：断开连接（自动回滚这个连接里正在执行的线程，即释放了行锁）

# 消除阻塞
kill 8;


### 查询慢