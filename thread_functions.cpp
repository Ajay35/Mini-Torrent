#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <openssl/sha.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include <bits/stdc++.h>
#include <bits/stdc++.h>
#include <pthread.h>
#include "utility.h"
using namespace std;
void file_send(int new_socket_fd,string file_path)
{
    write_log("launched file_send thread");
    cout<<file_path<<endl;
    size_t BUFFER=512*1024;
    cout<<1<<endl;
     FILE *fp=fopen(file_path.c_str(),"rb");
        if(fp==NULL)
        {
            printf("File open error");
            return;
        }
        for (;;)
        {
            unsigned char buff[BUFFER]={0};
            int nread=fread(buff,1,BUFFER,fp);
            //cout<<"Bytes read:"<<nread;
            if(nread>0)
            {
                write(new_socket_fd,buff,nread);
            }
            write_log("Uploading ..");
            if (nread<(BUFFER))
            {
                if(feof(fp))
                {
                    write_log("End of file");
                }
                if(ferror(fp))
                {
                    write_log("Error reading");
                }
                fclose(fp);
                break;
            }
        }
        write_log("File Upload complete");
        cout<<"File upload finished"<<endl;
        close(new_socket_fd);
}
void peer_listening(char *ip,char *port)
{
    write_log("peer listening thread created");
    int sockfd,new_socket_fd; 
    socklen_t clilen;
    char buffer[1024]; 
    struct sockaddr_in self_addr, peer_addr; 
    if((sockfd=socket(AF_INET, SOCK_STREAM,0))<0)
    { 
        error("socket creation failed"); 
        write_log("TCP peer 1 socket creation failed"); 
        exit(0);
    } 

     memset(&self_addr, 0, sizeof(self_addr)); 
    self_addr.sin_family=AF_INET;
    self_addr.sin_addr.s_addr = inet_addr(ip);
    int PORT=atoi(port); 
    self_addr.sin_port = htons(PORT); 
    if(bind(sockfd,(const struct sockaddr *)&self_addr,sizeof(self_addr))<0) 
    { 
        error("TCP socket bind failed");
        write_log("TCP socket bind failed"); 
    }
    while(true)
    {
        write_log("Waiting for peer requests:");
        listen(sockfd,5);
        int new_socket_fd= accept(sockfd,(struct sockaddr *) &peer_addr,&clilen);
        bzero(buffer,1024);
        int n=read(new_socket_fd,buffer,1024);
        write_log("received from peer:"+n);
        if(n < 0) error("failed to read from socket");
        write_log(buffer);
        string file_path(buffer);
        write_log("new connection thread");
        std::thread t(file_send,new_socket_fd,file_path);
        t.detach();
    }
}
void file_receive(int sockfd,string file_path,size_t f_size)
{
    long BUFFER=512*1024;
    write_log("File receive thread started");
    size_t completeBytes=0;
    string file_name=getFileName1(file_path);
    //std::ofstream ofs(""+file_name, std::ios::binary | std::ios::out);
    string str2="/home/ajay/Desktop/"+file_name;
    FILE* file=fopen(str2.c_str(),"w+");
    fseek(file,f_size-1,SEEK_SET);
    fputc('\0',file);
    fclose(file);
    FILE *fp = fopen(str2.c_str(),"ab");
    int bytesReceived=0;
    char buff[BUFFER];
    memset(buff,'0',sizeof(buff));
    while((bytesReceived=read(sockfd,buff,BUFFER)) > 0)
    {
        //cout<<"Bytes received"<<bytesReceived;
        fwrite(buff, 1,bytesReceived,fp);
        completeBytes+=bytesReceived;
        write_log("dwonloading..");
        cout<<"downloaded:"<<((double)completeBytes/f_size)*100<<"%"<<endl;
    }

    if(bytesReceived<0)
    {
        printf("\n Read Error \n");
    }

    fclose(fp);
    close(sockfd);
}
