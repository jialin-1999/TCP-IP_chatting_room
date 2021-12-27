//
// Created by Jonny on 2021/12/27.
//

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <string>
#include <climits>
#include <algorithm>
#include <queue>
#include <vector>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
const int BACKLOG=2; //完成三次握手但没有accept的队列的长度
const int CONCURRENT_MAX=3; //应用层同时可以处理的连接
const int BUFFER_SIZE=1024;//将被加入缓存器的最大字节数
char input_msg[BUFFER_SIZE];
char recv_msg[BUFFER_SIZE];

class Socket_server{
public:
    Socket_server(){
        server_sock_fd=0;
        tv.tv_sec = 20;//秒
        tv.tv_usec = 0;//微秒
        max_fd=-1;
        memset(client_fds,0,sizeof(client_fds));
        memset(client_to,-1,sizeof(client_to));
        filename.clear(); path.clear();
    };
    ~Socket_server(){

    };
    bool init(const int port);
    bool start();
    bool sendmessage();//服务器发出信息
    bool sendIP(int index);//发送号码表
    void recv_and_forward();//接收信息，同时起到中转站的作用
    void recv_word(int index,int byte_num);
    void recv_file(int index,int byte_num);
    void recv_video(int index);
    void accept_new();
private:
    int server_sock_fd;//服务端套接字
    //fd_set
    fd_set server_fd_set;//用来存放所有的待检查的文件描述符
    int max_fd;
    timeval tv;//struct timeval结构用于描述一段时间长度，如果在这个时间内，需要监视的描述符没有事件发生则函数返回，返回值为0。
    int client_fds[CONCURRENT_MAX+1];//客户端套接字
    struct sockaddr_in client_addr[CONCURRENT_MAX+1];//客户端对应的地址信息
    int client_to[CONCURRENT_MAX+1];//客户端想要发送的对象
    std::string filename;
    std::string path;
};
bool Socket_server::init(const int port){
    struct sockaddr_in server_addr;//sockaddr_in: Socket address, internet style.表示网络地址的结构体(internet和socket通用)
    server_addr.sin_len = sizeof(struct sockaddr_in);
    server_addr.sin_family = AF_INET;//socket只能用这个
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
//    server_addr.sin_port = htons(0);//16位整数 主机字节序转网络字节序,此处为设置端口号(本机是小端存储，网络字节是大端)
//    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);//设置本机IP地址
    memset(&(server_addr.sin_zero),0,8);
    //创建socket
    server_sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_sock_fd == -1){//未能成功打开网络通讯端口
        perror("socket error");//输出字符串+errno对应的错误
        return false;
    }

    int on = 1;
    if(setsockopt(server_sock_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))==-1){//防止出现bind error的地址已被占用
        perror("setsockopt");
        return false;
    }

    //绑定socket
    int bind_result = bind(server_sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    if (bind_result == -1){
        perror("bind error");
        return false;
    }

    //listen
    if (listen(server_sock_fd, BACKLOG) == -1){
        perror("listen error");
        return false;
    }
    printf("欢迎使用SOCKET通讯系统，输入quit并确认可退出\n");

    return true;
}
bool Socket_server::start()
{
    FD_ZERO(&server_fd_set);
    //标准输入
    FD_SET(STDIN_FILENO, &server_fd_set);//用于在文件描述符集合中增加一个新的文件描述符。STDIN_FILENO是标准输入文件描述符

    if (max_fd < STDIN_FILENO)//STDIN_FILENO = 0
        max_fd = STDIN_FILENO;
    //服务器端socket
    FD_SET(server_sock_fd, &server_fd_set);//把要检测的套接字server_sock_fd加入到集合中
    if (max_fd < server_sock_fd){
        max_fd = server_sock_fd;
    }
    //客户端连接
    for (int i = 1; i <= CONCURRENT_MAX; i++){
        if (client_fds[i]!=0){
            FD_SET(client_fds[i], &server_fd_set);
            if (max_fd < client_fds[i])//找到文件描述符中的最大值并存储
                max_fd = client_fds[i];
        }
    }
    int ret = select(max_fd+1, &server_fd_set, NULL, NULL, &tv);//检测server_fd_set中的套接字中是否有可读信息
    if (ret < 0){
        perror("select 出错");
        return false;
    }
    else if(ret == 0){
        printf("timeout...\n");
        return false;
    }
    return true;
}
bool Socket_server::sendmessage()
{
    //ret为未状态发生变化的文件描述符的个数
    if (FD_ISSET(STDIN_FILENO, &server_fd_set)){//检测是否可写，可发送语句
        //标准输入
        memset(input_msg,0,sizeof(input_msg));
        fgets(input_msg, BUFFER_SIZE, stdin);
        char *find = strchr(input_msg, '\n');//查找换行符
        if(find) *find = '\0';//如果find不为空指针,就把一个空字符放在这里
        if (strcmp(input_msg, "\\quit") == 0){
            shutdown(server_sock_fd,SHUT_RDWR);
            return false;
        }
        if(input_msg[0]=='1' && input_msg[1]==' '){
            for (int i=1; i<=CONCURRENT_MAX; i++){//向每个连接的客户端发送信息
                if (client_fds[i]!=0){
                    std::string s; s.clear(); s="0";//表明这是谁发过来的
                    if(send(client_fds[i],s.c_str(),BUFFER_SIZE,0)==-1)
                        perror("发送消息出错!");
                    if(send(client_fds[i], input_msg, BUFFER_SIZE, 0)==-1)
                        perror("发送消息出错!");
                }
            }
        }
        else if(input_msg[0]=='2' && input_msg[1]==' '){
            for (int i=1; i<=CONCURRENT_MAX; i++){
                if(client_fds[i]!=0){
                    std::string s; s.clear();
                    s="0";//表明这是谁发过来的
                    if(send(client_fds[i],s.c_str(),BUFFER_SIZE,0)==-1)
                        perror("转发消息出错!");
                }
            }
            for (int i=1; i<=CONCURRENT_MAX; i++){
                if(client_fds[i]!=0){
                    if(send(client_fds[i],input_msg,BUFFER_SIZE,0)<0)
                        perror("发送消息出错！");
                }
            }
            path.clear();
            for (int i=2; input_msg[i]!='\n' && input_msg[i]!='\0'; i++){
                path+=input_msg[i];
            }
            FILE *fp=fopen(path.c_str(),"rb+");
            char chtemp[BUFFER_SIZE+1];
            memset(chtemp,'\0',sizeof(chtemp));
            int cnt=0;
            if(fp==nullptr){
                printf("文件无法打开\n");
            }
            else {
                while((cnt=fread(chtemp,sizeof(char),BUFFER_SIZE,fp))>0){
                    for (int i=1; i<=CONCURRENT_MAX; i++){
                        if(client_fds[i]!=0){
                            if(send(client_fds[i],chtemp,cnt,0)<0){
                                perror("发送消息出错!");
                            }
                        }
                    }
                    memset(chtemp,'\0',sizeof(chtemp));
                }
                fclose(fp);
                printf("Transfer Successful!\n");
            }
            path.clear(); filename.clear();
        }
        else if(input_msg[0]=='3' && input_msg[1]==' '){
            printf("敬请期待!\n");
        }

    }
    return true;
}
bool Socket_server::sendIP(int index)
{
    std::string s1="0";
    send(client_fds[index],s1.c_str(),BUFFER_SIZE,0);
    s1.clear();

    s1="1 您现在是"+std::to_string(index)+"号客户机, 当前聊天室内成员如下：\n";
    send(client_fds[index],s1.c_str(),BUFFER_SIZE,0);
    for (int i=1; i<=CONCURRENT_MAX; i++){
        if(client_fds[i]!=0){
            s1.clear();
            s1="0";
            send(client_fds[index],s1.c_str(),BUFFER_SIZE,0);
            s1.clear();
            s1="1 "+std::to_string(i)+"号客户机，IP地址为："+inet_ntoa(client_addr[i].sin_addr)+"\n";
            if(send(client_fds[index],s1.c_str(),BUFFER_SIZE,0)==-1) return false;
        }
        if(client_fds[i]!=0 && i!=index){
            s1.clear();
            s1="0";
            send(client_fds[i],s1.c_str(),BUFFER_SIZE,0);
            s1.clear();
            s1="1 新加入了"+std::to_string(index)+"号客户机，IP地址为："+inet_ntoa(client_addr[index].sin_addr)+"\n";
            if(send(client_fds[i],s1.c_str(),BUFFER_SIZE,0)==-1) return false;
        }
    }
    return true;
}
void Socket_server::recv_word(int index,int byte_num)
{
    if(client_to[index]==0 || client_to[index]==-2){//发送给服务器的或者群发
        printf("客户端(%d):",index);
        for (int j=2; j<byte_num; j++){
            if(recv_msg[j]=='\0') break;
            printf("%c",recv_msg[j]);
        }
        printf("\n");
    }
    if(client_to[index]==-2){//群发其余客户端
        for (int i=1; i<=CONCURRENT_MAX; i++){
            if(client_fds[i]!=0 && i!=index){
                //先给id发送这是谁发过来的
                std::string s; s.clear();
                s=std::to_string(index);//表明这是谁发过来的
                if(send(client_fds[i],s.c_str(),BUFFER_SIZE,0)==-1)
                    perror("转发消息出错!");
                if (send(client_fds[i], recv_msg, BUFFER_SIZE, 0) == -1){
                    perror("转发消息出错!");
                }
            }
        }
    }
    else if(client_to[index]!=0){//发送给个人的
        //先给id发送这是谁发过来的
        std::string s; s.clear();
        s=std::to_string(index);//表明这是谁发过来的
        if(send(client_fds[client_to[index]],s.c_str(),BUFFER_SIZE,0)==-1)
            perror("转发消息出错!");
        if (send(client_fds[client_to[index]], recv_msg, BUFFER_SIZE, 0) == -1){
            perror("转发消息出错!");
        }
    }
}
void Socket_server::recv_file(int index,int byte_num)
{
    std::string tempfilename; tempfilename.clear();
    if(client_to[index]==0 || client_to[index]==-2){//发送给服务器的或者群发
        int pos=0;
        for (int j=byte_num-1; j>0; j--){
            if(recv_msg[j]=='/' || recv_msg[j]=='\\'){
                pos=j; break;
            }
        }
        for (int j=pos+1; j<byte_num; j++){
            if(recv_msg[j]=='\0') break;
            filename+=recv_msg[j];
        }
        //取得文件名称，准备建立新的文件
        printf("客户端(%d)向你传送了一个文件，请输入您想要存放的绝对路径，例如：/Users/longzhengtian/Desktop/\n注意，最后要有一个'/'符号，如果没有，系统将自动填充，请输入：",index);
        std::cin>>path; getchar();
        if(path[path.size()-1]!='/'){//如果没有'/'的补救措施
            path+="/";
        }
        //filename="new_"+filename;
        path+=filename;
        FILE *fp=fopen(path.c_str(),"wb+");
        if(fp==nullptr)
            printf("接收文件出错!\n");
        else {
            memset(recv_msg,0,sizeof(recv_msg));
            int cnt=0;
            while((cnt=recv(client_fds[index],recv_msg,BUFFER_SIZE,0)) && cnt>0){
                if(fwrite(recv_msg,sizeof(char),cnt,fp)<cnt){
                    printf("接收文件出错!\n");
                    break;
                }
                memset(recv_msg,0,sizeof(recv_msg));
                if(cnt!=BUFFER_SIZE) break;
            }
            printf("Receive successful!\n");
            fclose(fp);
        }
        if(client_to[index]==0) path.clear(); filename.clear();
    }
    if(client_to[index]==-2){//群发
        for (int i=1; i<=CONCURRENT_MAX; i++){
            if(client_fds[i]!=0 && i!=index){
                std::string s; s.clear();
                s=std::to_string(index);//表明这是谁发过来的
                if(send(client_fds[i],s.c_str(),BUFFER_SIZE,0)==-1)
                    perror("转发消息出错!");
            }
        }
        filename="2 "+path;
        for (int i=1; i<=CONCURRENT_MAX; i++){
            if(i!=index && client_fds[i]!=0){
                if(send(client_fds[i],filename.c_str(),BUFFER_SIZE,0)<0)
                    perror("发送消息出错！");
            }
        }
        FILE *fp=fopen(path.c_str(),"rb+");
        char chtemp[BUFFER_SIZE+1];
        memset(chtemp,'\0',sizeof(chtemp));
        int cnt=0;
        if(fp==nullptr){
            printf("文件无法打开\n");
        }
        else {
            while((cnt=fread(chtemp,sizeof(char),BUFFER_SIZE,fp))>0){
                for (int i=1; i<=CONCURRENT_MAX; i++){
                    if(i!=index && client_fds[i]!=0){
                        if(send(client_fds[i],chtemp,cnt,0)<0){
                            perror("发送消息出错!");
                        }
                    }
                }
                memset(chtemp,'\0',sizeof(chtemp));
            }
            fclose(fp);
            printf("Transfer Successful!\n");
        }
        path.clear(); filename.clear();
    }
    else if(client_to[index]!=0){//发送给个人的
        std::string s; s.clear();
        s=std::to_string(index);//表明这是谁发过来的
        if(send(client_fds[client_to[index]],s.c_str(),BUFFER_SIZE,0)==-1)
            perror("转发文件出错!");
        if(send(client_fds[client_to[index]],recv_msg,BUFFER_SIZE,0)==-1)
            perror("转发文件出错!");

        memset(recv_msg,0,sizeof(recv_msg));
        int cnt=0;
        while((cnt=recv(client_fds[index],recv_msg,BUFFER_SIZE,0)) && cnt>0){
            if(send(client_fds[client_to[index]],recv_msg,cnt,0)==-1)
                perror("转发文件出错!");
            memset(recv_msg,0,sizeof(recv_msg));
            if(cnt!=BUFFER_SIZE) break;
        }
        printf("A file from Client%d to Client%d was successfully forwarded through this server\n",index,client_to[index]);
    }
}
void Socket_server::recv_video(int index)
{
    printf("敬请期待！\n");
}
void Socket_server::recv_and_forward()
{
    for (int i = 1; i <= CONCURRENT_MAX; i++){
        if (client_fds[i]!=0){
            if (FD_ISSET(client_fds[i], &server_fd_set)){
                //处理某个客户端过来的消息
                memset(recv_msg,0,sizeof(recv_msg));
                int byte_num = recv(client_fds[i],recv_msg,BUFFER_SIZE,0);
                if (byte_num > 0){
                    if (byte_num > BUFFER_SIZE) byte_num = BUFFER_SIZE;
                    recv_msg[byte_num] = '\0';
                    if(strcmp(recv_msg,"\\exit")==0){//客户端要求解除连接
                        client_to[i]=-1;
                    }
                    else if(client_to[i]==-1){//这条信息是对面发来指定发送对象的
                        if(recv_msg[0]=='-') client_to[i]=-2;
                        else client_to[i]=recv_msg[0]-'0';
                    }
                    else if(recv_msg[0]=='1'){//文字
                        recv_word(i,byte_num);
                    }
                    else if(recv_msg[0]=='2'){//文件
                        recv_file(i,byte_num);
                    }
                    else if(recv_msg[0]=='3'){//视频
                        recv_video(i);
                    }
                }
                else if(byte_num < 0)
                    printf("从客户端(%d)接受消息出错.\n",i);
                else{
                    FD_CLR(client_fds[i], &server_fd_set);//用于在文件描述符集合中删除一个文件描述符。
                    client_fds[i] = 0;
                    printf("客户端(%d)退出了\n",i);
                    std::string s1;
                    for (int j=1; j<=CONCURRENT_MAX; j++){
                        if(client_fds[j]!=0 && i!=j){
                            s1.clear();
                            s1=std::to_string(i)+"号客户机退出了!\n";
                            send(client_fds[j],s1.c_str(),BUFFER_SIZE,0);
                        }
                    }
                }
            }
        }
    }
}
void Socket_server::accept_new()
{
    if (FD_ISSET(server_sock_fd, &server_fd_set)){//检测server_sock_fd是否真的变成了可读，可接受语句
        //有新的连接请求
        struct sockaddr_in client_address;
        socklen_t address_len=sizeof(struct sockaddr_in);
        int client_socket_fd = accept(server_sock_fd, (struct sockaddr *)&client_address, &address_len);//对应客户端对connect

        if (client_socket_fd > 0){
            int index = -1;
            for (int i = 1; i <= CONCURRENT_MAX; i++){
                if (client_fds[i] == 0){
                    index = i;
                    client_fds[i] = client_socket_fd;
                    break;
                }
            }
            if (index >= 0){
                printf("新客户端(%d)加入成功 %s:%d \n",index,inet_ntoa(client_address.sin_addr),ntohs(client_address.sin_port));
                //inet_ntoa:接受一个in_addr结构体类型的参数并返回一个以点分十进制格式表示的IP地址字符串
                client_addr[index]=client_address;
                sendIP(index);
            }
            else{
                memset(input_msg,0,sizeof(input_msg));
                strcpy(input_msg, "服务器加入的客户端数达到最大值!\n");
                send(client_socket_fd, input_msg, BUFFER_SIZE, 0);
                printf("客户端连接数达到最大值，新客户端加入失败 %s:%d \n",inet_ntoa(client_address.sin_addr),ntohs(client_address.sin_port));
            }
        }
    }
}
int main (){
    //本地地址
    Socket_server tcpserver;
    tcpserver.init(11111);//初始化，创建并绑定套接字，开始监听

    while (true){
        if(!tcpserver.start()) {//建立连接失败
            continue;
        }
        else{
            tcpserver.accept_new();//是否有新的加入
            if(!tcpserver.sendmessage()) exit(0);//发送信息，包含退出操作
            tcpserver.recv_and_forward();//接受信息。
        }
    }
    return 0;
}
