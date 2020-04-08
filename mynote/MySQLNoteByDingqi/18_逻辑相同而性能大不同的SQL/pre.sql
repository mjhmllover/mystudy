DROP DATABASE if exists `test18`;
CREATE DATABASE `test18`;
use `test18`;
CREATE TABLE `tradelog` (
    `id` int(11) NOT NULL,
    `tradeid` varchar(32) DEFAULT NULL,
    `operator` int(11) DEFAULT NULL,
    `t_modified` datetime DEFAULT NULL,
    PRIMARY KEY (`id`),
    KEY `tradeid` (`tradeid`),
    KEY `t_modified` (`t_modified`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

DROP PROCEDURE if exists `logdata`;
delimiter //
CREATE PROCEDURE `logdata` (IN init INT, IN loop_time INT)
BEGIN
    DECLARE count INT;
    DECLARE ID INT;
    SET count = 0;
    SET ID = init;
    WHILE count < loop_time DO
        insert into `tradelog`(id,tradeid,operator,t_modified) values (ID, '11112222', 0, FROM_UNIXTIME(FLOOR(1451577600 + RAND() * 94694399)));
        SET ID = ID + 1;
        SET count = count + 1;
    END WHILE;
END;
//
delimiter ;

DROP TABLE if exists `test18`.`trade_detail`;
CREATE TABLE `test18`.`trade_detail` (
    `id` int(11) NOT NULL,
    `tradeid` varchar(32) DEFAULT NULL,
    `trade_step` int(11) DEFAULT NULL, 
    `step_info` varchar(32) DEFAULT NULL,
    PRIMARY KEY (`id`),
    KEY `tradeid` (`tradeid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;