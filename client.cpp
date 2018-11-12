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
//#include "utility.h"
//#include "thread_functions.h"

using namespace std;
//move to utility
void error(const char *msg)
{
    perror(msg);
    exit(0);
}
//move to utility
void get_split_strings(string input,vector<string>& output)
{
    string x;
    stringstream iss(input);
     while(iss >> x)
    {
        output.push_back(x);
    }
}
//move to utility
void write_log( const std::string &text )
{
    std::ofstream log;
    log.open("log.txt", std::ios_base::out | std::ios_base::app );
    log<<text<<"\n";
}
//move to thread_fun
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
//move to thread_fun
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
//move to utility
string getFileName1(string filePath, bool withExtension=true, char seperator = '/')
{
	size_t dotPos = filePath.rfind('.');
	size_t sepPos = filePath.rfind(seperator);
	if(sepPos != std::string::npos)
	{
		return filePath.substr(sepPos + 1, filePath.size() - (withExtension || dotPos != std::string::npos ? 1 : dotPos) );
	}
	return "";
}
//move to thread_fun
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
//move to utility
string remove_extension(const string& filename)
{
    size_t lastdot=filename.find_last_of(".");
    if (lastdot==string::npos) return filename;
    return filename.substr(0, lastdot); 
}
//move to utility
string getFileName(std::string filePath, bool withExtension = false, char seperator = '/')
{
	// Get last dot position
	std::size_t dotPos = filePath.rfind('.');
	std::size_t sepPos = filePath.rfind(seperator);
 
	if(sepPos != std::string::npos)
	{
		return filePath.substr(sepPos + 1, filePath.size() - (withExtension || dotPos != std::string::npos ? 1 : dotPos) );
	}
	return "";
}
//move to utility
string generate_mtorrent(string data_file_path,string tracker_ip,string port,string file_hash,string tor_name)
{
    fstream make_file;
    string t_name=getFileName(data_file_path);
    string raw_name=remove_extension(t_name);
    //raw_name=raw_name+".mtorrent";
    make_file.open(tor_name,std::ios_base::out);
    make_file<<tracker_ip<<"\n";
    make_file<<port<<"\n";
    make_file<<data_file_path<<"\n";
    struct stat stat_buf;
    stat(data_file_path.c_str(), &stat_buf);
    make_file<<stat_buf.st_size<<"\n";
    make_file<<file_hash<<"\n";
    cout<<"mtorrent file generated"<<endl;
    return tor_name;
}   
//move to utility
string calculate_hash(string file_path)
{
   // cout<<"In calculate hash function"<<endl;
    string file_hash="";
    struct stat stat_buf;
    long long buf_size=512*1024;
    stat(file_path.c_str(), &stat_buf);
    size_t file_size=stat_buf.st_size;
   // cout<<"file size:"<<file_size<<endl;
    size_t total_chunks=(file_size/buf_size);
    std::ifstream fin(file_path, std::ifstream::binary);
    size_t last_chunk_size = file_size % buf_size;
    if(last_chunk_size != 0) 
    {
       ++total_chunks; 
    }
    else 
    {
       last_chunk_size=buf_size;
    }
    //cout<<"total chunks:"<<total_chunks<<endl;
    //cout<<"last chunk:"<<last_chunk_size<<endl;

    for (size_t chunk=0;chunk<total_chunks;++chunk)
    {
      size_t curr_chunk_size= (chunk == total_chunks-1)? last_chunk_size:buf_size;
      char chunk_data[curr_chunk_size];
      fin.read(chunk_data,curr_chunk_size);   
      unsigned char md[SHA_DIGEST_LENGTH];
      char obuf[(SHA_DIGEST_LENGTH*2)+1];
      SHA1((const unsigned char *)chunk_data,curr_chunk_size,md);
      for(int i=0,j=0;i<20;i++,j+=2)
      {
        sprintf( &obuf[j], "%02x",md[i]);
      }
        string tmp(obuf);
        file_hash=file_hash+tmp;
    }
    //cout<<file_hash<<endl;      
    return file_hash;
}
//move to utility
bool is_reg_file(string path) 
{
    struct stat buf;
    stat(path.c_str(), &buf);
    return S_ISREG(buf.st_mode);
}

int main(int argc, char *argv[])
{
    int sockfd_2,sockfd_3, port_tracker,n;
    struct sockaddr_in track_addr,self_addr,peer_addr;
    struct hostent *tracker,*peer;
    char buffer[1024];

    if(argc<4) 
    {
       fprintf(stderr,"usage %s client_ip client_port tracker_ip tracker_port \n", argv[0]);
       write_log("usage %s client_ip client_port tracker_ip tracker_port");
       exit(0);
    }
    /* starting thread to communicate with peers */

    std::thread t(peer_listening,argv[1],argv[2]);

    /* communication with tracker  */
    while(true)
    { 
        cout<<"Enter command:"<<endl;
        string com;
        getline(cin,com);
        string path,x;
        vector<string> command;
        get_split_strings(com,command);
        if(command[0]=="share" && command.size()>=3)
        {
            cout<<command[1]<<endl;
            if(is_reg_file(command[1]))
                      {
                          //calulate_sha1(path);
                            string h=calculate_hash(command[1]);
                            string f_name=generate_mtorrent(command[1],argv[3],argv[4],h,command[2]);
                            port_tracker= atoi(argv[4]);
                            sockfd_2=socket(AF_INET, SOCK_STREAM, 0);
                            if(sockfd_2<0) 
                            {
                                error("failed to open socket");
                            }
                            tracker=gethostbyname(argv[3]);
                            if(tracker==NULL) 
                            {
                                cout<<"tracker is not working"<<endl;
                                exit(0);
                            }
                            /* initiate track_addr zeroes by \0 */
                            bzero((char *) &track_addr, sizeof(track_addr));
                            track_addr.sin_family = AF_INET;

                            /* copy server->h_addr(found by getbyhostname)  into serv_addr */
                            bcopy((char *)tracker->h_addr,(char *)&track_addr.sin_addr.s_addr,tracker->h_length);
                            track_addr.sin_port = htons(port_tracker);

                            /* connect call to server */
                            if (connect(sockfd_2,(struct sockaddr *) &track_addr,sizeof(track_addr)) < 0) 
                                error("Failed to connect");
                            /* reading input string from client */
                            cout<<"Successfully connected to tracker"<<endl;
                            bzero(buffer,1024);
                            string client_ip=argv[1];
                            string client_port=argv[2];
                            string s=command[0]+" "+client_ip+" "+client_port+" "+command[1]+" "+h;
                            for(int i=0;s[i];i++)
                            {
                                buffer[i]=s[i];
                            }
                            n=write(sockfd_2,buffer,strlen(buffer));
                            if(n<0) 
                                error("error wrinting to socket");
                            bzero(buffer,1024);
                            n=read(sockfd_2,buffer,1024);
                            if(n<0)
                            { 
                               error("No meesage from server");
                            }
                            cout<<buffer<<endl;
                            close(sockfd_2);
                        }
                      else
                      {
                          cout<<"Not a file"<<endl;
                      }
        }
        else if(command[0]=="get" && command.size()==2)
        {
                     /* ping peers using UDP  */
                    ifstream f(command[1]),f1(command[1]),f2(command[1]);
                    string hash_from_file,file_path,file_size;
                    for(int i=1;i<=5; i++)
                    {
                        getline(f,hash_from_file);  
                    }
                    for(int i=1;i<=3;i++)
                    {
                        getline(f1,file_path);
                    }
                    for(int i=1;i<=4;i++)
                    {
                        getline(f2,file_size);
                    }
                    size_t f_size=atoi(file_size.c_str());
                    int sockfd,n; 
                    char buffer[2048]; 
                    struct sockaddr_in tracker_addr; 
                    port_tracker= atoi(argv[4]);
                    sockfd_2=socket(AF_INET, SOCK_STREAM, 0);
                    if(sockfd_2<0) 
                    {
                        error("failed to open socket");
                    }
                    tracker=gethostbyname(argv[3]);
                    if(tracker==NULL) 
                    {
                        cout<<"tracker is not working"<<endl;
                        exit(0);
                    }
                    /* initiate track_addr zeroes by \0 */
                    bzero((char *) &track_addr, sizeof(track_addr));
                    track_addr.sin_family = AF_INET;

                    /* copy server->h_addr(found by getbyhostname)  into serv_addr */
                    bcopy((char *)tracker->h_addr,(char *)&track_addr.sin_addr.s_addr,tracker->h_length);
                    track_addr.sin_port = htons(port_tracker);

                    /* connect call to tracker */
                    if (connect(sockfd_2,(struct sockaddr *) &track_addr,sizeof(track_addr)) < 0) 
                        error("Failed to connect");
                    /* reading input string from client */
                    bzero(buffer,2048);
                    string client_ip=argv[1];
                    string client_port=argv[2];
                    string s=command[0]+" "+client_ip+" "+client_port+" "+hash_from_file;
                    for(int i=0;s[i];i++)
                    {
                        buffer[i]=s[i];
                    }
                    n=write(sockfd_2,buffer,strlen(buffer));
                    if(n<0) 
                        error("error writing to socket");
                    bzero(buffer,1024);
                    n=read(sockfd_2,buffer,1024);
                    if(n<0)
                    { 
                        error("No meesage from server");
                    }                    
                    string fromTracker(buffer);
                    cout<<"from tracker:"<<fromTracker<<endl;
                    stringstream iss(fromTracker);
                    string response;
                    vector<string> ip_port;
                    while(iss >> response)
                    {
                        ip_port.push_back(response);
                        cout<<response<< endl;
                    }
                    cout<<fromTracker<<endl;
                    close(sockfd_2);

                    /* Make request to thread peer listening of peer for whichi ip is receiveed from server */ 
                
                    sockfd_3=socket(AF_INET, SOCK_STREAM, 0);
                    if(sockfd_3<0) 
                    {
                        error("failed to open socket");
                    }
                   // cout<<1<<endl;
                   // cout<<ip_port[0]<<endl;
                    peer=gethostbyname(ip_port[0].c_str());
                    if(peer==NULL) 
                    {
                        cout<<"peer is not working"<<endl;
                        exit(0);
                    }
                   // cout<<2<<endl;
                            /* initiate track_addr zeroes by \0 */
                    bzero((char *) &peer_addr, sizeof(peer_addr));
                    peer_addr.sin_family = AF_INET;
                    //cout<<3<<endl;
                    /* copy server->h_addr(found by getbyhostname)  into serv_addr */
                    bcopy((char *)peer->h_addr,(char *)&peer_addr.sin_addr.s_addr,peer->h_length);
                    peer_addr.sin_port = htons(atoi(ip_port[1].c_str()));
                    
                    /* connect call to server */
                    if (connect(sockfd_3,(struct sockaddr *) &peer_addr,sizeof(peer_addr)) < 0) 
                        error("Failed to connect");
                    //cout<<4<<endl;
                    
                    /* reading input string from client */
                    bzero(buffer,1024);
                    string peer_ip=ip_port[0];
                    string peer_port=ip_port[1];
                    //string str1=file_path;
                    cout<<"data to be sent:"<<file_path<<endl;
                    for(int i=0;file_path[i];i++)
                    {
                        buffer[i]=file_path[i];
                    }
                    std::thread file_receiver(file_receive,sockfd_3,file_path,f_size);
                    file_receiver.detach(); 
                    n=write(sockfd_3,buffer,strlen(buffer));
                    if(n<0) 
                        error("error wrinting to socket");
        }
        else if(command[0]=="seeders")
        {
            cout<<"this is seeders list"<<endl;
        }
        else if(command[0]=="remove")
        {

            cout<<"Invalid command"<<endl;
        }
    }
    return 0;
}
