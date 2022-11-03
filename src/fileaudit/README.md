- 样例代码运行步骤：
  1. 配置环境
  2. 编译configure以及kernel_module模块
     - 编译configure：`gcc –o auditdemo auditdemo.c`
     - 编译内核：`make`
  3. 加载内核模块：
     - 在kernel_module目录下：`insmod AuditModule.ko`

