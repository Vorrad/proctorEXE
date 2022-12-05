# 项目说明

[toc]

## proctorEXE数据库软件

此部分为proctorEXE系统的用户层数据库应用软件，为用户提供简单便捷的数据库创建和连接、增删改查命令。

### 运行环境

- 操作系统：Linux(Ubuntu 18.10 LTS)
- 数据库：MySQL 14.14 Distrib 5.7.26
  - 安装方法：
  - `sudo apt install mysql-server ` 安装MySQL服务器
  - `sudo apt install libmysqlclient-dev` 安装MySQL开发包

### 编译

在项目根目录下，用命令行运行：

1. `bash ./make.sh`
2. `sudo bin/start`

请确保在根目录下运行。

### 使用

1. 本软件需要以root身份运行
2. 输入help以获取帮助

## 文件目录

- doc/：存放文本文件，如设计书、设计报告等
- bin/：存放二进制可执行文件
- include/：存放依赖文件（头文件）
- src/：存放源码
  - fileaudit/：实例代码
  - app/：用户层应用源码
- make.sh：编译规则脚本，后续可能用Makefile或Cmake替代
- API.md：规定模块间接口、数据结构
- README.md：项目开发说明

## 开发规范

- 项目编译的中间文件不要上传到库，请在.gitignore中自行定义
- 每一次commit需要有明确主题，用**type: subject**的形式描述commit的内容
  - 零散的工作不要频繁commit，尽量使每一次commit都有一定意义，开发过程中可以在本地用`git add <files>`暂存更改
  - 参考文章：[如何规范你的Git commit？ - 知乎](https://zhuanlan.zhihu.com/p/182553920)

| type  | subject               |
| ----- | --------------------- |
| feat  | 新功能（feature）     |
| fix   | 修复bug               |
| docs  | 文档（documentation） |
| test  | 增加测试              |
| merge | 代码合并              |
|refactor|重构（即不是新增功能，也不是修改bug的代码变动）|
| ...   | ...                   |

- 分支管理
  - 主分支master：用于上传功能完整的版本
  - 副分支dev：用于功能、模块测试
  - 特性分支feature-XXX：开发新特性时的分支，用于单一功能开发