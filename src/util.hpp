#ifndef UTIL__H
#define UTIL__H

#include <string>

bool isSeparator(char c);
bool containsLetter(char * buffer);
bool equalsChar(char a, char b);
std::string getFilenameFromPath(std::string s);
unsigned int lengthPrinted(const std::string & s);

#endif
