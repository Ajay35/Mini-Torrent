#ifndef UTILITY_H
#define UTILITY_H

void error(const char *);
void get_split_strings(std::string input,std::vector<std::string>& output);
void write_log( const std::string &);
bool is_reg_file(std::string);
std::string getFileName(std::string, bool withExtension=true, char seperator = '/');
std::string remove_extension(const std::string&);
std::string generate_mtorrent(std::string,std::string,std::string,std::string,std::string);
std::string calculate_hash(std::string);
std::string getFileName1(std::string, bool withExtension=true, char seperator = '/');

#endif