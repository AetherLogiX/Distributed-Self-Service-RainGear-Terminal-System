-- RainHub 数据库初始化脚本
-- 在MySQL中执行此脚本来创建数据库和测试数据

-- 创建数据库
CREATE DATABASE IF NOT EXISTS rain_gear_db CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
USE rain_gear_db;

-- 创建用户表
CREATE TABLE IF NOT EXISTS t_user (
    user_id VARCHAR(20) PRIMARY KEY COMMENT '学号/工号',
    password VARCHAR(64) NOT NULL COMMENT '密码SHA256',
    real_name VARCHAR(20) NOT NULL COMMENT '真实姓名',
    role INT NOT NULL DEFAULT 0 COMMENT '角色: 0-学生, 1-教职工, 9-管理员',
    balance DECIMAL(10,2) NOT NULL DEFAULT 0.00 COMMENT '账户余额',
    avatar_id INT DEFAULT 1 COMMENT '头像ID',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) COMMENT='用户信息表';

-- 插入测试数据
-- 密码 "123456" 的SHA256值: e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
-- 密码 "password" 的SHA256值: 5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8
INSERT INTO t_user (user_id, password, real_name, role, balance) VALUES
('20241001', SHA2('123456', 256), '张三', 0, 100.00),
('20241002', SHA2('password', 256), '李四', 0, 150.50),
('20241003', SHA2('admin123', 256), '王五', 1, 200.00),
('T001', SHA2('teacher123', 256), '陈老师', 1, 300.00),
('admin', SHA2('admin', 256), '系统管理员', 9, 999.99)
ON DUPLICATE KEY UPDATE
    password = VALUES(password),
    real_name = VALUES(real_name),
    balance = VALUES(balance);

-- 验证数据
SELECT user_id, real_name, role, balance FROM t_user;

-- 创建站点表
CREATE TABLE IF NOT EXISTS t_station (
    station_id INT PRIMARY KEY AUTO_INCREMENT COMMENT '站点ID',
    name VARCHAR(50) NOT NULL COMMENT '站点名称',
    pos_x FLOAT NOT NULL COMMENT 'X坐标',
    pos_y FLOAT NOT NULL COMMENT 'Y坐标', 
    status INT DEFAULT 1 COMMENT '状态: 1-在线, 0-离线, 2-故障'
) COMMENT='站点信息表';

-- 插入测试站点数据
INSERT INTO t_station (name, pos_x, pos_y, status) VALUES
('文德楼西门', 0.2, 0.3, 1),
('明德楼东门', 0.7, 0.4, 1),
('图书馆南门', 0.5, 0.6, 1)
ON DUPLICATE KEY UPDATE
    pos_x = VALUES(pos_x),
    pos_y = VALUES(pos_y);

-- 创建雨具表
CREATE TABLE IF NOT EXISTS t_rain_gear (
    gear_id VARCHAR(20) PRIMARY KEY COMMENT '雨具ID',
    type_id INT NOT NULL COMMENT '类型: 1-普通伞, 2-抗风伞, 3-遮阳伞, 4-雨衣',
    station_id INT COMMENT '所在站点ID',
    slot_id INT COMMENT '卡槽编号 1-12',
    status INT DEFAULT 1 COMMENT '状态: 1-可借, 2-已借, 3-损坏',
    FOREIGN KEY (station_id) REFERENCES t_station(station_id)
) COMMENT='雨具库存表';

-- 显示创建完成信息
SELECT '=== 数据库初始化完成 ===' AS message;
SELECT '测试账号信息:' AS info;
SELECT CONCAT('账号: ', user_id, ' | 姓名: ', real_name, ' | 密码: 根据role选择') AS login_info 
FROM t_user WHERE user_id IN ('20241001', 'T001', 'admin');

SELECT '' AS separator;
SELECT '密码对照表:' AS password_info;
SELECT '学生张三(20241001) - 密码: 123456' AS student_login;
SELECT '教师陈老师(T001) - 密码: teacher123' AS teacher_login;  
SELECT '管理员(admin) - 密码: admin' AS admin_login;
