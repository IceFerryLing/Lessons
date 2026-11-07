-- 第2章示例：MySQL 建库建表脚本
-- 用法：mysql -u root -p < create_mysql.sql
CREATE DATABASE IF NOT EXISTS shop DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;
USE shop;

DROP TABLE IF EXISTS sales;
CREATE TABLE sales (
    order_id    VARCHAR(20)  NOT NULL COMMENT '订单编号',
    order_time  DATETIME              COMMENT '下单时间',
    store       VARCHAR(10)           COMMENT '门店（A店/B店/C店）',
    category    VARCHAR(20)           COMMENT '商品类别',
    amount      DECIMAL(10,2)         COMMENT '销售额（元）',
    quantity    INT                   COMMENT '数量',
    PRIMARY KEY (order_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='销售流水记录表';

DROP TABLE IF EXISTS product;
CREATE TABLE product (
    product_id   VARCHAR(10) NOT NULL COMMENT '商品编号',
    product_name VARCHAR(50)          COMMENT '商品名称',
    category     VARCHAR(20)          COMMENT '商品类别',
    unit_price   DECIMAL(10,2)        COMMENT '单价（元）',
    supplier     VARCHAR(50)          COMMENT '供应商',
    PRIMARY KEY (product_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='商品信息表';

DROP TABLE IF EXISTS store;
CREATE TABLE store (
    store      VARCHAR(10) NOT NULL COMMENT '门店代码',
    store_name VARCHAR(50)          COMMENT '门店名称',
    city       VARCHAR(30)          COMMENT '所在城市',
    manager    VARCHAR(20)          COMMENT '店长',
    open_date  DATE                 COMMENT '开业日期',
    PRIMARY KEY (store)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='门店信息表';

DROP TABLE IF EXISTS order_detail;
CREATE TABLE order_detail (
    order_id   VARCHAR(20) NOT NULL COMMENT '订单编号',
    product_id VARCHAR(10) NOT NULL COMMENT '商品编号',
    quantity   INT                  COMMENT '数量',
    PRIMARY KEY (order_id, product_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='订单商品明细表';

-- 数据导入（在命令行执行，注意转义符与文件路径）
-- LOAD DATA LOCAL INFILE 'D:/sales_2026.csv' INTO TABLE sales
--   CHARACTER SET utf8mb4 FIELDS TERMINATED BY ',' ENCLOSED BY '"'
--   LINES TERMINATED BY '\n' IGNORE 1 LINES;

-- pandas 连接字符串示例（见 01_读取数据.py）
-- engine = create_engine("mysql+pymysql://root:123456@localhost:3306/shop?charset=utf8mb4")
