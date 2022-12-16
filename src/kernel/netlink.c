#include "netlink.h"

extern char* module_name;  // kernel module name

static char *name = "Netlink";
static struct sock *nl_sk = NULL;  // netlink socket
static pid_t pid = -1;      // user space server pid (pid_t is int)，同时是连接的状态
static atomic_t index;      // prm_msg 消息 index
static int fail_count = 0;

int netlink_sendmsg(char *buf, size_t len)
{
    struct sk_buff *skb_out = nlmsg_new(PAYLOAD_MAX_SIZE+4, GFP_KERNEL);
    struct nlmsghdr *nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, PAYLOAD_MAX_SIZE+4, 0);
    NETLINK_CB(skb_out).dst_group = 0;
    // fill data
    *(u32 *)NLMSG_DATA(nlh) = len;
    memcpy((char *)NLMSG_DATA(nlh)+4, buf, len);

    return nlmsg_unicast(nl_sk, skb_out, pid);
}

static void nl_data_ready(struct sk_buff *__skb)
 {
    u8 *buf = NULL;
    int len;
    struct prm_nlmsg *msg;
    struct prm_msg *ptr;

    buf = kmalloc(PAYLOAD_MAX_SIZE, GFP_KERNEL);
    memset(buf, 0, PAYLOAD_MAX_SIZE);

    msg = (struct prm_nlmsg *) __skb->data;
    len = msg->msg_len;

    memcpy(buf, (char *)msg->msg_data, (size_t)msg->msg_len);

    ptr = (struct prm_msg *)buf;
    if(ptr->type == PRM_MSG_TYPE_CONNECT)
    {
        // 收到用户态进程注册消息
        // 设置通信的用户态进程pid
        pid = msg->nlh.nlmsg_pid;
        // 返回确认消息
        ptr->index = atomic_inc_return(&index);
        ptr->type = PRM_MSG_TYPE_CONNECT_CONFIRM;
        printk("Connection message received from pid=%d\n",pid);
        printk("Send connection confirm message to pid=%d\n",pid);
        netlink_sendmsg((char *)ptr, sizeof(struct prm_msg));
        fail_count = 0;

    }
    else if(ptr->type == PRM_MSG_TYPE_DISCONNECT)
    {
        // 用户态断开连接
        // 这里没有检查是否确实为对应的程序要求断开连接，直接断开
        printk("Disconnection message received from pid=%d, current connection pid=%d\n", msg->nlh.nlmsg_pid, pid);
        // 重置连接状态
        pid = -1;
        fail_count = 0;
        printk("Connection was closed\n");

    }
    else if(ptr->type == PRM_MSG_TYPE_RESULT)
    {
        // 收到权限查询结果
        struct sem_msg* sem_msg_ptr = (struct sem_msg*)(ptr->sem_msg_ptr);
        if(sem_msg_ptr->status == SEM_STATUS_READY)
        {
            // 正在等待结果
            sem_msg_ptr->status = SEM_STATUS_LOADED;
            sem_msg_ptr->data = ptr->result_type;
            // 释放信号量
            up(&(sem_msg_ptr->sem));
        }
        else{ }     
    }
    else
    {
        printk("Unknown message type\n");
    }
    kfree(buf);
}

int admissionReq(unsigned long ino, int action, pid_t pid_c, int p_type, int *result)
{
    struct prm_msg msg;
    struct sem_msg *ptr = NULL;
    int down_ret = -1;

    // 检查用户态服务器是否已经连接
    if(pid==-1)
    {
        // 未连接
        return PRM_ERROR_SERVEROFFLINE;
    }

    // 设置共享内存，初始化对应的信息量，设置共享内存状态为ready
    ptr = kmalloc(sizeof(struct sem_msg), GFP_KERNEL);
    memset(ptr, 0, sizeof(struct sem_msg));
    ptr->status = SEM_STATUS_READY;
    sema_init(&(ptr->sem), 0);

    // 构建消息
    msg.index = atomic_inc_return(&index);
    msg.type = PRM_MSG_TYPE_CHECK;
    msg.ino = (u32)ino;
    msg.pid = (u32)pid_c;
    msg.action = (s32)action;
    msg.p_type = (s32)p_type;
    msg.sem_msg_ptr = (u64)ptr;

    // 向应用态程序发送查询消息
    netlink_sendmsg((char *)&msg, sizeof(struct sem_msg));
    // 等待返回消息
    // down(&(ptr->sem));
    down_ret = down_timeout(&(ptr->sem), SEM_WAIT_CYCLE); // 在SEM_WAIT_CYCLE个时钟周期内等待信号量
    if(down_ret != 0)
    {
        fail_count += 1;
        printk("Cannot get response from pid=%d\n", pid);
        if(fail_count >= 3)
        {
            // 多次失败取消连接
            printk("Disconnection caused by failure, pid=%d\n", pid);
            // 重置连接状态
            pid = -1;
            fail_count = 0;
            printk("Connection was closed\n");
        }
    }

    kfree(ptr);
    if(ptr->status == SEM_STATUS_LOADED)
    {
        *result = ptr->data;
        return PRM_SUCCESS;
    }
    return PRM_ERROR;
}

static int socket_creat(void)
{
    struct netlink_kernel_cfg cfg = {
        .input = nl_data_ready,
    };

    nl_sk=(struct sock *)netlink_kernel_create(&init_net,NETLINK_PRM, &cfg);

    if (!nl_sk)
    {
		printk(KERN_ERR "net_link: Cannot create netlink socket.\n");
		if (nl_sk != NULL)
    		sock_release(nl_sk->sk_socket);
        return PRM_ERROR;
    }
    else  printk("net_link: create socket ok.\n");

    atomic_set(&index, 0);

    return PRM_SUCCESS;
}

static int socket_release(void)
{
    if (nl_sk != NULL){
        netlink_kernel_release(nl_sk);
        nl_sk = NULL;
    }
    printk("net_link: release socket ok.\n");
 	return PRM_SUCCESS;
}

int netlink_init(void) {
    return socket_creat();
}


int netlink_release(void) {
    return socket_release();
}