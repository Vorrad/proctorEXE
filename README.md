# 项目开发说明

[toc]

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