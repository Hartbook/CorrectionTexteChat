#ifndef UTIL__H
#define UTIL__H

#include <string>

bool isSeparator(char c);
bool containsLetter(char * buffer);
bool equalsChar(char a, char b);
FILE * readFile(const char * filename);
FILE * writeFile(const char * filename);
FILE * readFile(const std::string & filename);
FILE * writeFile(const std::string & filename);

#endif
