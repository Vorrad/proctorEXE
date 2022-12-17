#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/syscalls.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/unistd.h>
#include <asm/pgtable.h>
#include <asm/uaccess.h>
#include <asm/ptrace.h>
#include <asm/unistd_64.h>


#include "hook.h"
#include "netlink.h"

/*
** module macros
*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("ZOU");
MODULE_DESCRIPTION("hook sys_call_table");

typedef void (* sys_call_ptr_t)(void);
sys_call_ptr_t *get_sys_call_table(void);
sys_call_ptr_t * sys_call_table = NULL;

unsigned int level;
pte_t *pte;

typedef asmlinkage long (*sys_call_t)(struct pt_regs*);

// 原始的系统调用函数
sys_call_t orig_openat;
sys_call_t orig_read;
sys_call_t orig_write;
//sys_call_t orig_reboot;
//sys_call_t orig_socket;
//sys_call_t orig_execve;

int get_info_from_fd(unsigned int fd, unsigned long * ino, int *type)
{
    struct file *file_p = NULL;
    struct inode * f_inode = NULL;
    umode_t imode = 0;      // unsigned short
    struct fd f;

    // 获取 uid
    //*uid = current_uid().val;   // unsigned int
    // 获取fd对应的file struct
    // file_p = fget_raw(fd);
    f = fdget(fd);
    file_p = f.file;
    if (!file_p)
    {
        // 获取失败
        return PRM_ERROR;
    }
    // 获取 ino
    f_inode = file_p->f_inode;
    *ino = f_inode->i_ino;

    // 获标准取文件类型
    *type = FILE_U;
    imode = f_inode->i_mode;
    if(S_ISLNK(imode)){
        *type = FILE_LNK;
    }else if(S_ISREG(imode)){
        *type = FILE_REG;
    }else if(S_ISDIR(imode)){
        *type = FILE_DIR;
    }else if(S_ISCHR(imode)){
        *type = FILE_CHR;
    }else if(S_ISBLK(imode)){
        *type = FILE_BLK;
    }else if(S_ISFIFO(imode)){
        *type = FILE_FIFO;
    }else if(S_ISSOCK(imode)){
        *type = FILE_SOCK;
    }
    return PRM_SUCCESS;
}

asmlinkage long hacked_openat(struct pt_regs *regs)
{
    long ret =  orig_openat(regs);

    // unsigned int fd = 0;
    unsigned long ino = 0;
    pid_t pid = 0;
    int f_type = 0;
    int p_result = 0;
    int p_type;
    int action = OPEN;

    pid = current->pid;
    if (ret < 0)
    {
        // linux 本身权限控制不通过，不处理
        p_result = CHECK_RESULT_PASS;
    }
    else
    {
        if(get_info_from_fd(ret, &ino , &f_type) == PRM_ERROR)
        {
            
            // 获取inode失败，默认通过
            p_result = CHECK_RESULT_PASS;
        }
        else
        {
            p_type = P_U;
            if (f_type == FILE_REG || f_type == FILE_LNK){
                p_type = P_REG;
            } else if (f_type == FILE_DIR){
                p_type = P_DIR;
            }

            //printk("p_type = %d",p_type);

            if(p_type == P_U)
            {
                // 未定义的文件类型，调用原函数
                p_result = CHECK_RESULT_PASS;
            }
            else
            {
                int check_ret = PRM_ERROR;
                check_ret = admissionReq(action, pid, p_type, &p_result);
                if(check_ret != PRM_SUCCESS)
                {
                    // 权限查询出错，默认通过
                    p_result = CHECK_RESULT_PASS;
                }
            }
        }
    }

    if(p_result != CHECK_RESULT_NOTPASS)
    {
        // ret =  real_openat(regs);
    }
    else
    {
        ret = -1;
        if (p_type == P_REG) printk("Block: open REG file inode=%ld\n", ino);
        if (p_type == P_DIR) printk("Block: open DIR inode=%ld\n", ino);
    }

    return ret;
}

asmlinkage long hacked_read(struct pt_regs * regs)
{
    long ret = -1;

    unsigned int fd = 0;
    unsigned long ino = 0;
    pid_t pid = 0;
    int f_type = 0;
    int p_result = 0;
    int p_type;
    int action = READ;

    pid = current->pid;
    fd = regs->di;
    if(get_info_from_fd(fd, &ino, &f_type) == PRM_ERROR)
    {
        // 文件标识符无法解析，直接调用原函数
        p_result = CHECK_RESULT_PASS;
    }
    else if (f_type == FILE_U)
    {
        // 文件类型无法判断，调用原函数
        p_result = CHECK_RESULT_PASS;
    }
    else
    {
        // 判断权限类型
        p_type = P_U;
        if (f_type == FILE_REG || f_type == FILE_LNK){
            p_type = P_REG;         // 标准文件
        }else if (f_type == FILE_DIR){
            p_type = P_DIR;
        }
        
        

        if(p_type == P_U)
        {
            // 未定义的文件类型，调用原函数
            p_result = CHECK_RESULT_PASS;
        }
        else
        {
            int check_ret = PRM_ERROR;
            check_ret = admissionReq(action, pid, p_type, &p_result);
            if(check_ret != PRM_SUCCESS)
            {
                // 权限查询出错，默认通过
                p_result = CHECK_RESULT_PASS;
            }
        }
    }

    // debug
    // p_result = CHECK_RESULT_PASS;

    // 判断权限检查结果，是否不允许执行
    if(p_result != CHECK_RESULT_NOTPASS)
    {
        ret = orig_read(regs);
    }
    else
    {
        if (p_type == P_REG) printk("Block: read REG file inode=%ld\n", ino);
        if (p_type == P_DIR) printk("Block: read DIR inode=%ld\n", ino);
    }   
    
    return ret;
}

/**
 * @brief 对sys_write重载
 * asmlinkage long sys_write(unsigned int fd, const char __user *buf, size_t count);
 * 
 * 
 * @param regs 
 * @return asmlinkage 
 */
asmlinkage long hacked_write(struct pt_regs * regs)
{
    long ret = -1;

    unsigned int fd = 0;
    unsigned long ino = 0;
    pid_t pid = current->pid;
    int f_type = 0;
    int p_result = 0;
    int p_type;
    int action = WRITE;

    fd = regs->di;
    if(get_info_from_fd(fd, &ino, &f_type) == PRM_ERROR)
    {
        // 文件标识符无法解析，直接调用原函数
        p_result = CHECK_RESULT_PASS;
    }
    else if (f_type == FILE_U)
    {
        // 文件类型无法判断，调用原函数
        p_result = CHECK_RESULT_PASS;
    }
    else
    {
        // 判断权限类型
        p_type = P_U;
        if (f_type == FILE_REG || f_type == FILE_LNK){
            p_type = P_REG;         // 标准文件
        }else if (f_type == FILE_DIR){
            p_type = P_DIR;
        }

        if(p_type == P_U)
        {
            // 未定义的文件类型，调用原函数
            p_result = CHECK_RESULT_PASS;
        }
        else
        {
            int check_ret = PRM_ERROR;
            check_ret = admissionReq(action, pid, p_type, &p_result);
            if(check_ret != PRM_SUCCESS)
            {
                // 权限查询出错，默认通过
                p_result = CHECK_RESULT_PASS;
            }
        }
    }

    // debug
    // p_result = CHECK_RESULT_PASS;

    // 判断权限检查结果，是否不允许执行
    if(p_result != CHECK_RESULT_NOTPASS)
    {
        ret = orig_write(regs);
    }
    else
    {
        if (p_type == P_REG) printk("Block: write REG file inode=%ld\n", ino);
        if (p_type == P_DIR) printk("Block: write DIR inode=%ld\n", ino);
    }
    
    return ret;
}















static int __init mod_init(void)
{

	sys_call_table = get_sys_call_table();
	printk("Info: sys_call_table found at %lx\n",(unsigned long)sys_call_table) ;

    //Hook Sys Call Openat
	orig_openat = (void *) sys_call_table[__NR_openat];
    orig_read = (void *) sys_call_table[__NR_read];
    orig_write = (void *) sys_call_table[__NR_write];
	
	pte = lookup_address((unsigned long) sys_call_table, &level);
    // Change PTE to allow writing
    set_pte_atomic(pte, pte_mkwrite(*pte));
    printk("Info: Disable write-protection of page with sys_call_table\n");

    sys_call_table[__NR_openat] = (sys_call_ptr_t) hacked_openat;
    sys_call_table[__NR_read] = (sys_call_ptr_t) hacked_read;
    sys_call_table[__NR_write] = (sys_call_ptr_t) hacked_write;

    set_pte_atomic(pte, pte_clear_flags(*pte, _PAGE_RW));
    printk("Info: sys_call_table hooked!\n");

    netlink_init();
    return 0;
}


static void __exit mod_exit(void)
{

    pte = lookup_address((unsigned long) sys_call_table, &level);
    set_pte_atomic(pte, pte_mkwrite(*pte));

	sys_call_table[__NR_openat] = (sys_call_ptr_t)orig_openat;
    sys_call_table[__NR_read] = (sys_call_ptr_t)orig_read;
    sys_call_table[__NR_write] = (sys_call_ptr_t)orig_write;

	set_pte_atomic(pte, pte_clear_flags(*pte, _PAGE_RW));

    netlink_release();
}

module_init(mod_init);
module_exit(mod_exit);
