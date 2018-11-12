#include<bits/stdc++.h>
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
void error(const char *msg)
{
    perror(msg);
    exit(0);
}
void get_split_strings(string input,vector<string>& output)
{
    string x;
    stringstream iss(input);
     while(iss >> x)
    {
        output.push_back(x);
    }
}
bool is_reg_file(string path) 
{
    struct stat buf;
    stat(path.c_str(), &buf);
    return S_ISREG(buf.st_mode);
}
void write_log( const std::string &text )
{
    std::ofstream log;
    log.open("log.txt", std::ios_base::out | std::ios_base::app );
    log<<text<<"\n";
}
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
string remove_extension(const string& filename)
{
    size_t lastdot=filename.find_last_of(".");
    if (lastdot==string::npos) return filename;
    return filename.substr(0, lastdot); 
}
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
string generate_mtorrent(string data_file_path,string tracker_ip,string port,string file_hash,string tor_name)
{
    fstream make_file;
    string t_name=getFileName(data_file_path);
    string raw_name=remove_extension(t_name);
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