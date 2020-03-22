DROP DATABASE if exists `test17`;
CREATE DATABASE `test17`;
USE `test17`;
CREATE TABLE `words` (
    `id` int(11) NOT NULL AUTO_INCREMENT,
    `word` varchar(64) DEFAULT NULL,
    PRIMARY KEY (`id`)
) ENGINE=InnoDB;

drop procedure if exists `idata`;
delimiter ;;
create procedure `idata`()
begin
    declare i int;
    set i=0;
    while i<10000 do
        insert into words(word) values(concat(char(97+(i div 1000)), char(97+(i % 1000 div 100)), char(97+(i % 100 div 10)), char(97+(i % 10))));
        set i=i+1;
    end while;
end;;
delimiter ;