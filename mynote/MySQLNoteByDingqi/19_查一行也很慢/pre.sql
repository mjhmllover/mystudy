DROP DATABASE if exists `test19`;
CREATE DATABASE `test19`;
use `test19`;
CREATE TABLE `t`(
    `id` int(11) NOT NULL,
    `c` int(11) DEFAULT NULL,
    PRIMARY KEY (`id`)
) ENGINE=InnoDB;

drop procedure if exists `idata`;
delimiter ;;
create procedure idata()
begin
    declare i int;
    set i=1;
    while(i<=100000) do
        insert into t values(i, i);
        set i=i+1;
    end while;
end;;
delimiter ;

call idata();