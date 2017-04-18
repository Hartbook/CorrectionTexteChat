#ifndef UTIL__H
#define UTIL__H

#include <string>
#include "File.hpp"

bool isSeparator(char c);
bool containsLetter(char * buffer);
bool equalsChar(char a, char b);
std::string getFilenameFromPath(std::string s);
unsigned int lengthPrinted(const std::string & s);
bool isNum(const std::string & s);
bool isUpper(char c);
bool endSentence(char c);
File * cleanCorpus(File * corpus, std::string path);
bool ignoreSeparators(File & corpus);
unsigned int readWord(File & corpus, std::string & word, bool sentenceBegin);

#endif
