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
const int BUFFER_SIZE=1024;
char recv_msg[BUFFER_SIZE];
char input_msg[BUFFER_SIZE];

class Socket_client
{
public:
    Socket_client(){
        server_sock_fd=0;
        tv.tv_sec = 20;
        tv.tv_usec = 0;
        toID=-1;
        fromID=0;
    }
    ~Socket_client(){

    }
    bool start();
    bool init();
    bool connect_s();
    bool sendmessage();
    void receive();
    void recvmessage();
private:
    int server_sock_fd;
    struct sockaddr_in server_addr;
    fd_set client_fd_set;
    struct timeval tv;
    int toID;//-1是初始化，-2是群发，0以上是私聊
    int fromID;
    std::string filename;
    std::string path;
};
bool Socket_client::init()
{
    server_addr.sin_len = sizeof(struct sockaddr_in);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(11111);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(&(server_addr.sin_zero),0,8);
    server_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock_fd == -1){
        perror("socket error");
        return false;
    }
    return true;
}
bool Socket_client::connect_s()
{
    if(connect(server_sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in))==0) {
        printf("服务器加入成功！\n");
        return true;
    }
    else return false;
}
bool Socket_client::start()
{
    FD_ZERO(&client_fd_set);
    FD_SET(STDIN_FILENO, &client_fd_set);
    FD_SET(server_sock_fd, &client_fd_set);

    int ret = select(server_sock_fd + 1, &client_fd_set, NULL, NULL, &tv);
    if (ret < 0 ){
        printf("select 出错!\n");
        return false;
    }
    else if(ret ==0){
        printf("timeout...\n");
        return false;
    }
    return true;
}
bool Socket_client::sendmessage()
{
    if (FD_ISSET(STDIN_FILENO, &client_fd_set)){

        if(toID==-1){//没有制定发送对象，0号表示服务器
            int a=-1,b=-1;
            scanf("%d",&a);
            if(a==1) {
                scanf("%d",&b); toID=b;
            }
            else toID=-2;
            std::string s=std::to_string(toID);
            if(send(server_sock_fd,s.c_str(),BUFFER_SIZE,0)==-1){
                perror("制定发送对象出错！");
            }
            getchar();
        }
        else {
            memset(input_msg,0,sizeof(input_msg));
            fgets(input_msg, BUFFER_SIZE, stdin);

            char *find = strchr(input_msg, '\n');//查找换行符
            if(find)//如果find不为空指针
                *find = '\0';//就把一个空字符放在这里

            if (strcmp(input_msg, "\\quit") == 0){
                shutdown(server_sock_fd,SHUT_RDWR);
                return false;
            }
            if (strcmp(input_msg, "\\exit") == 0){
                toID=-1;
                send(server_sock_fd,input_msg,5,0);
                return true;
            }
            if(input_msg[0]=='1'){//文字传送
                if (send(server_sock_fd, input_msg, BUFFER_SIZE, 0) == -1){
                    perror("发送消息出错!");
                }
            }
            else if(input_msg[0]=='2'){//文件传输
                if (send(server_sock_fd, input_msg, BUFFER_SIZE, 0) == -1){
                    perror("文件传输出错!");
                }
                std::string _path;
                for (int i=2; input_msg[i]!='\n' && input_msg[i]!='\0'; i++){
                    _path+=input_msg[i];
                }
                FILE *fp=fopen(_path.c_str(),"rb+");
                char chtemp[BUFFER_SIZE+1];
                memset(chtemp,'\0',sizeof(chtemp));
                int cnt=0;
                if(fp==nullptr){
                    printf("文件无法打开\n");
                }
                else {
                    while((cnt=fread(chtemp,sizeof(char),BUFFER_SIZE,fp))>0){
                        if(send(server_sock_fd,chtemp,cnt,0)<0){
                            perror("发送消息出错!");
                        }
                        memset(chtemp,'\0',sizeof(chtemp));
                    }
                    fclose(fp);
                    printf("Transfer Successful!\n");
                }
            }
            else if(input_msg[0]=='3'){//视频聊天
                printf("您选择了视频聊天，功能尚未完成\n");
            }
            else {
                printf("传输形式不合法，请重新输入。\n");
            }
        }
    }
    return true;
}
void Socket_client::recvmessage()
{
    fromID=0;
    //先接收到的是发送方ID；
    for (int i=0; recv_msg[i]!='\0'; i++){
        fromID*=10;
        fromID+=recv_msg[i]-'0';
    }
    memset(recv_msg,0,sizeof(recv_msg));
    memset(recv_msg,0,sizeof(recv_msg));
    int byte_num=recv(server_sock_fd,recv_msg,BUFFER_SIZE,0);
    if(byte_num>0){
        if (byte_num > BUFFER_SIZE) byte_num = BUFFER_SIZE;
        recv_msg[byte_num] = '\0';
        if(recv_msg[0]=='1' && recv_msg[1]==' '){//文字传输
            if(fromID==0) printf("服务器：");
            else printf("客户端(%d):",fromID);
            for (int j=2; j<byte_num; j++){
                if(recv_msg[j]=='\0' || recv_msg[j]=='\n') break;
                printf("%c",recv_msg[j]);
            }
            printf("\n");
        }
        else if(recv_msg[0]=='2' && recv_msg[1]==' '){
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
            if(fromID==0) printf("服务器");
            else printf("客户端(%d)",fromID);
            printf("向你传送了一个文件，请输入您想要存放的绝对路径，例如：/Users/longzhengtian/Desktop/\n注意，最后要有一个'/'符号，如果没有，系统将自动填充，请输入：");
            std::cin>>path; getchar();
            if(path[path.size()-1]!='/'){//如果没有'/'的补救措施
                path+="/";
            }
            //filename="new_"+filename;
            path+=filename;
            FILE *fp=fopen(path.c_str(),"wb+");
            if(fp==nullptr)
                printf("接收文件出错!(R216)\n");
            else {
                memset(recv_msg,0,sizeof(recv_msg));
                int cnt=0;
                while((cnt=recv(server_sock_fd,recv_msg,BUFFER_SIZE,0)) && cnt>0){
                    if(fwrite(recv_msg,sizeof(char),cnt,fp)<cnt){
                        printf("接收文件出错!(R223)\n");
                        break;
                    }
                    memset(recv_msg,0,sizeof(recv_msg));
                    if(cnt!=BUFFER_SIZE) break;
                }
                printf("Receive successful!\n");
                fclose(fp);
            }
            path.clear(); filename.clear();
        }
        else if(recv_msg[0]=='3' && recv_msg[1]==' '){
            //视频传输尚未完成
        }
    }
    else if(byte_num < 0){
        printf("接受消息出错!\n");
    }
    else{
        printf("服务器端退出!\n");
        shutdown(server_sock_fd,SHUT_RDWR);
        exit(0);
    }
}
void Socket_client::receive()
{
    if (FD_ISSET(server_sock_fd, &client_fd_set)){
        memset(recv_msg,0,sizeof(recv_msg));
        long byte_num = recv(server_sock_fd,recv_msg,BUFFER_SIZE,0);
        if (byte_num > 0){
            if (byte_num > BUFFER_SIZE){
                byte_num = BUFFER_SIZE;
            }
            recv_msg[byte_num] = '\0';

            recvmessage();

            if(strcmp(recv_msg,"服务器加入的客户端数达到最大值!\n")==0){
                exit(0);
            }
        }
        else if(byte_num < 0){
            printf("接受消息出错!\n");
        }
        else{
            printf("服务器端退出!\n");
            shutdown(server_sock_fd,SHUT_RDWR);
            exit(0);
        }
    }
}
int main (){
    Socket_client tcpclient;
    tcpclient.init();

    if (tcpclient.connect_s()){
        while (true){
            if(!tcpclient.start()){
                continue;
            }
            else{
                if(!tcpclient.sendmessage()) exit(0);
                tcpclient.receive();
            }
        }
    }
    return 0;
}
