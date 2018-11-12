#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <bits/stdc++.h>
#include "utility.h"
#include "thread_functions.h"
using namespace std;
void error(const char *msg)
{
    perror(msg);
    exit(1);
}
struct seeders
{
    string ip;
    string port;
    string file_name;
    string file_hash;
};
int main(int argc, char *argv[])
{
    vector<seeders> seeders_info;
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[1024];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     if (argc < 3) {
         cout<<"Provide IP port number"<<endl;
         exit(1);
     }

     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     
     if (sockfd < 0) 
        error("ERROR opening socket");
     
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[2]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
     serv_addr.sin_port = htons(portno);
     if(bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
     {
        error("binding error");
     }
     
     /* listen for client */
     
     listen(sockfd,5);
     cout<<"Waiting for connection..."<<endl;
     clilen = sizeof(cli_addr);
    while(true)
    {
        /* generate new socket for new connection for writing */
        newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr,&clilen);
        cout<<"waiting for new connection"<<endl;
        if (newsockfd<0) 
        {
            error("failed to accept new connection");
            break;
        }
        bzero(buffer,1024);

        /* read meesage from client */
        n = read(newsockfd,buffer,1024);
        if (n < 0) error("failed to read from socket");
        string fromClient(buffer),s;
        //cout<<"received message:"<<fromClient<<" ";
        stringstream iss(fromClient);
        vector<string> seed_i;
        struct seeders s1;
        while(iss>>s)
        {
            seed_i.push_back(s);
            cout<<s<< endl;
        }
        if(seed_i[0]=="share")
        {
            /* saving info in vetor */
            s1.ip=seed_i[1];
            s1.port=seed_i[2];
            s1.file_name=seed_i[3];
            s1.file_hash=seed_i[4];
            seeders_info.push_back(s1);
            cout<<n<<" Bytes"<<endl;
            string str="From server:Tracker Updated with details: "+fromClient;
            n = write(newsockfd,str.c_str(),1024);
            if (n < 0) error("error writing to socket");
        }
        else if(seed_i[0]=="get")
        {
           // cout<<"in get block"<<endl;
            //cout<<"looking for:"<<seed_i[3]<<endl;
            string name="godfather";
            cout<<n<<" Bytes"<<endl;
            //string str="From server:Thanks for the request: "+name;
            string response="";
            for(int i=0;i<seeders_info.size();i++)
            {
                if(seeders_info[i].file_hash==seed_i[3])
                {
                    response=seeders_info[i].ip+" "+seeders_info[i].port;
                    cout<<"seeder:"<<seeders_info[i].ip<<" "<<seeders_info[i].port<<endl;
                    break;
                }
            }
            n = write(newsockfd,response.c_str(),1024);
            cout<<"bytes written in response:"<<n<<endl;
        }
        close(newsockfd);
    }
     close(sockfd);
     return 0; 
}
