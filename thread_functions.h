#ifndef THREAD_FUNCTIONS_H
#define THREAD_FUNCTIONS_H
#include<string>
void file_send(int,std::string);
void peer_listening(char *,char *);
void file_receive(int,std::string ,size_t);



#endif