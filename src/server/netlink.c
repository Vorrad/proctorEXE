#include "netlink.h"
#include "../../include/general.h"
#include "../../include/app/mysql.h"
#include "../../include/app/operation.h"
#include "../../include/server/queryDB.h"

static int netlink_socket = -1;
static struct sockaddr_nl  *user_addr = NULL;      // self address
static struct sockaddr_nl  *kernel_addr = NULL;    // target address
static struct prm_nlmsg    *msg = NULL;            // message buffer


/**
 * @brief 创建netlink socket，填写地址信息
 * 
 * @return If this function succeeds, return PRM_SUCCESS, else return PRM_ERROR
 */
int socket_init()
{
    // Create netlink socket
    netlink_socket = socket(AF_NETLINK, SOCK_RAW, NETLINK_PRM);
    if (netlink_socket == -1)
    {
        perror("Socket create failed!\n");
        return PRM_ERROR;
    }
    
    // debug
    // if(sizeof(struct prm_nlmsg) != NLMSG_SPACE(1024+4)) perror("nlmsg space error\n");
    
    // fill msg head info
    msg = (struct prm_nlmsg *)malloc(sizeof(struct prm_nlmsg));
    if(msg == NULL)
    {
        perror("msg malloc failed!\n");
        return PRM_ERROR;
    }
    memset(msg, 0, sizeof(struct prm_nlmsg));
    msg->nlh.nlmsg_len = sizeof(struct prm_nlmsg);
    msg->nlh.nlmsg_pid = getpid();  // sender's pid
    msg->nlh.nlmsg_flags = 0;   // no special flag

    // fill sender's addr
    user_addr = (struct sockaddr_nl *)malloc(sizeof(struct sockaddr_nl));
    if(user_addr == NULL)
    {
        perror("user_addr malloc failed!\n");
        return PRM_ERROR;
    }
    memset(user_addr, 0, sizeof(struct sockaddr_nl));
    user_addr->nl_family = AF_NETLINK;   // Netlink socket
    user_addr->nl_pid = getpid();        // user space process pid
    user_addr->nl_groups = 0;            // no multicast

    // fill receive's addr
    kernel_addr = (struct sockaddr_nl *)malloc(sizeof(struct sockaddr_nl *));
    if(kernel_addr == NULL)
    {
        perror("kernel_addr malloc failed!\n");
        return PRM_ERROR;
    }
    memset(kernel_addr, 0, sizeof(struct sockaddr_nl));
    kernel_addr->nl_family = AF_NETLINK;
    kernel_addr->nl_pid = 0;     // kernel process pid
    kernel_addr->nl_groups = 0;

    // bind socket
    int ret = bind(netlink_socket, (struct sockaddr *) user_addr, sizeof(struct sockaddr_nl));
    if (ret == -1)
    {
        perror("bind error\n");
        return PRM_ERROR;
    }
    return PRM_SUCCESS;
}

/**
 * @brief 关闭socket，释放空间
 * 
 * @return PRM_SUCCESS
 */
int socket_release()
{
    close(netlink_socket);
    free(msg);
    free(user_addr);
    free(kernel_addr);

    netlink_socket = -1;
    msg = NULL;
    user_addr = NULL;
    kernel_addr = NULL;

    return PRM_SUCCESS;
}


/**
 * @brief send len bytes in buf to kernel module
 * 
 * @param buf 
 * @param len the num of bytes to be sent
 * @return PRM_SUCCESS for success, PRM_ERROR for error.
 */
int send(char *buf, size_t len)
{
    if(buf == NULL)
    {
        perror("u2k send buf is NULL!\n");
        return PRM_ERROR;
    }
    if(len > PAYLOAD_MAX_SIZE)
    {
        perror("u2k send too long msg!\n");
        return PRM_ERROR;
    }

    // printf("try to send 1\n");
    memset(&(msg->msg_len), 0, PAYLOAD_MAX_SIZE+4);
    memcpy(msg->msg_data, buf, len);
    msg->msg_len = len;
    // printf("try to send 2\n")
    
    ssize_t send_len = sendto(netlink_socket, msg, msg->nlh.nlmsg_len, 0, (struct sockaddr *)kernel_addr, sizeof(struct sockaddr_nl));
    // printf("=========\n");
    if(send_len == -1)
    {
        perror("u2k send failed!\n");
        return PRM_ERROR;
    }
    return PRM_SUCCESS;
}

/**
 * @brief receive msg from kernel module, save data in buf
 * 
 * @param buf 
 * @param buflen length of buf
 * @return If this function succeeds, return num of received bytes, else return PRM_ERROR
 */
ssize_t recv(char *buf, size_t buflen)
{
    if(buf == NULL)
    {
        perror("u2k recv buf is NULL!\n");
        return PRM_ERROR;
    }
    ssize_t len = -1;
    struct prm_nlmsg recv_msg;
    // recv msg
    socklen_t kernel_addrlen = sizeof(struct sockaddr_nl);
    len = recvfrom(netlink_socket, &recv_msg, sizeof(struct prm_nlmsg), 0, (struct sockaddr *)kernel_addr, &kernel_addrlen);
    if(len == -1)
    {
        perror("u2k recv failed!\n");
        return PRM_ERROR;
    }
    if(buflen < recv_msg.msg_len)
    {
        perror("u2k recv failed, buf is too short!\n");
        return PRM_ERROR;
    }
    memcpy(buf, recv_msg.msg_data, recv_msg.msg_len);
    return recv_msg.msg_len;
}


/**
 * @brief 发送连接建立消息，建立连接
 * 
 * @return int PRM_SUCCESS 成功，PRM_ERROR 失败
 */
int connect()
{
    char buf[1024];
    struct prm_msg msg;
    int send_ret;
    ssize_t recv_ret;
    struct prm_msg * recv_msg_ptr;

    // 填充消息内容
    msg.index = 0;
    msg.type = PRM_MSG_TYPE_CONNECT;
    // 发送连接请求
    send_ret = send((char*)&msg, sizeof(struct prm_msg));
    if(send_ret != PRM_SUCCESS)
    {
        return PRM_ERROR;
    }
    // 等待确认消息
    recv_ret = recv(buf, 1024);
    recv_msg_ptr = (struct prm_msg *)buf;
    if(recv_msg_ptr->type == PRM_MSG_TYPE_CONNECT_CONFIRM)
    {
        return PRM_SUCCESS;
    }
    return PRM_ERROR;
}

/**
 * @brief 用户态断开与核心态的连接
 * 
 * @return int 
 */
int disconnect()
{
    struct prm_msg mmm;
    memset((void *)&mmm, 0, sizeof(mmm));
    mmm.type = PRM_MSG_TYPE_DISCONNECT;
    send((char *)&mmm, sizeof(struct prm_msg));
}


/**
 * @brief 重新与内核态建立连接
 * 
 * @return int PRM_SUCCESS成功，PRM_ERROR失败
 */
int reconnect()
{
    socket_release();
    if(socket_init() != PRM_SUCCESS)
    {
        return PRM_ERROR;
    }
    if(connect() != PRM_SUCCESS)
    {
        return PRM_ERROR;
    }
    return PRM_SUCCESS;
}

/**
 * @brief 处理来自内核态的消息
 * 
 * @param msg 来自内核态的消息指针
 * @return int 
 */
int msg_handle(struct prm_msg *msg)
{
    struct prm_msg send_msg;
    memset(&send_msg, 0, sizeof(struct prm_msg));
    
    // 判断收到的消息类型
    if(msg->type == PRM_MSG_TYPE_CHECK)
    {
        int result = 0;
        // 权限检查
        printf("Handle privilege check: \n");
        if (msg->p_type == P_DEMESG)
        {
            printf("Check rights: dmesg\n");
            //result = ret_auth(msg->pid,msg->action);
            result = 1;
            printf("查询结果%d\n", result);
            if (result == 0)
            {
                // 0 代表没通过
                send_msg.result_type = CHECK_RESULT_NOTPASS;
            }
            else
            {
                send_msg.result_type = CHECK_RESULT_PASS;
            }
        }
        else if (msg->p_type == P_NET)
        {
            // 禁止1001对于net的访问
            printf("Check rights: net %u\n",msg->uid);
            //result = ret_auth(msg->pid,msg->action);
            if(msg->uid==1001) result = 0;
            else result = 1;
            printf("查询结果%d\n", result);
            if (result == 0)
            {
                // 0 代表没通过
                send_msg.result_type = CHECK_RESULT_NOTPASS;
            }
            else
            {
                send_msg.result_type = CHECK_RESULT_PASS;
            }
        }
        else if (msg->p_type == P_REBOOT)
        {
            // 禁止root重启
            printf("Check rights: reboot\n");
            //result = ret_auth(msg->pid,msg->action);
            if(msg->uid==1001) result = 0;
            else result = 1;
            printf("查询结果%d\n", result);
            if (result == 0)
            {
                // 0 代表没通过
                send_msg.result_type = CHECK_RESULT_NOTPASS;
            }
            else
            {
                send_msg.result_type = CHECK_RESULT_PASS;
            }
            // send_msg.result_type = CHECK_RESULT_NOTPASS;
        }
        else if (msg->p_type == P_REG)
        {
            printf("Check rights: REG %u\n", msg->uid);
            //result = ret_auth(msg->pid,msg->action);
            if(msg->uid==1001) result = 0;
            else result = 1;
            printf("查询结果%d\n", result);
            if (result == 0)
            {
                // 0 代表没通过
                send_msg.result_type = CHECK_RESULT_NOTPASS;
            }
            else
            {
                send_msg.result_type = CHECK_RESULT_PASS;
            }
        }
        else if (msg->p_type == P_DIR)
        {
            printf("Check rights: DIR %u\n", msg->uid);
            //result = ret_auth(msg->pid,msg->action);
            result = 1;
            printf("查询结果%d\n", result);
            if (result == 0)
            {
                // 0 代表没通过
                send_msg.result_type = CHECK_RESULT_NOTPASS;
            }
            else
            {
                send_msg.result_type = CHECK_RESULT_PASS;
            }
        }
        else
        {
            send_msg.result_type = CHECK_RESULT_PASS;
        }

        // result = ret_auth(msg->ino, msg->uid, msg->p_type);

        // 构建返回消息
        send_msg.type = PRM_MSG_TYPE_RESULT;
        printf("CHECK_RESULT %d\n", send_msg.result_type);
        // send_msg.result_type = CHECK_RESULT_PASS;
        send_msg.sem_msg_ptr = msg->sem_msg_ptr;
        // 返回消息
        send((char *)&send_msg, sizeof(struct prm_msg));
    }
    return PRM_SUCCESS;
}

int main ()
{
if(fork()){
    char buf[1024];
    char msg[1024];
    
    // scanf("%s", msg);
    socket_init();
    printf("init succees\n");

    // scanf("%s", msg);
    connect();
    printf("connect!\n");

    while(1)
    {   
        recv(buf, 1024);
        msg_handle((struct prm_msg *)buf);
        printf("handel finish\n");
    }

    scanf("%s", msg);
    disconnect();
    printf("disconnect");

    // scanf("%s", msg);
    socket_release();
    printf("Release!");
}
    return 0;
}
