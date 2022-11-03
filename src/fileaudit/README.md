- 样例代码运行步骤：
  1. 配置环境
  2. 编译configure以及kernel_module模块
     - 编译configure：`gcc –o auditdemo auditdemo.c`
     - 编译内核：`make`
  3. 加载内核模块
     - 在kernel_module目录下：`insmod AuditModule.ko`
  4. 检测内核模块是否正确加载
     - 命令行输入`lsmod`可以查看已加载的模块，如果有“AuditModule”模块说明加载成功
  5. 运行应用程序：在configure目录下`./auditdemo`
  6. 检测内核模块是否被使用：`lsmod`查看“AuditModule”后面数字，若为1则说明正确

