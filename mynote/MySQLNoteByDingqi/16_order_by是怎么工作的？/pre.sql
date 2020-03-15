DROP DATABASE if exists `test`;
CREATE DATABASE `test`;
use `test`;
CREATE TABLE `t` (
    `id` int(11) NOT NULL,
    `city` varchar(16) NOT NULL,
    `name` varchar(16) NOT NULL,
    `age` int(11) NOT NULL,
    `addr` varchar(128) DEFAULT NULL,
    PRIMARY KEY (`id`),
    KEY `city` (`city`)
) ENGINE=InnoDB;

DROP PROCEDURE if exists `pro`;
delimiter //
CREATE PROCEDURE `pro` (IN init INT, IN city varchar(16), IN loop_time INT)
BEGIN
    DECLARE count INT;
    DECLARE ID INT;
    SET count = 0;
    SET ID = init;
    WHILE count < loop_time DO
        insert into `t`(id, city, name, age, addr) values (ID, city, 'ali', 30, 'jiangning');
        SET ID = ID + 1;
        SET count = count + 1;
    END WHILE;
END;
//
delimiter ;
