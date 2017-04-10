#ifndef TRIE__H
#define TRIE__H

#include <memory>
#include <vector>

class Node
{
	public :

	char label;
	bool finishWord;
	int wordCode;
	Node * leftChild;
	Node * rightChild;

	public :

	Node(char label, bool finishWord, int wordCode);
	void print(bool endLine);
};

class Trie
{
	private :
	
	static constexpr int maxNbBackups = 128;
	bool printUnknownWords;

	std::vector< std::unique_ptr<Node> > nodes;
	Node * root;
	Node * current;
	char currentLetter;
	int codeMin;
	int codeMax;

	char buffer[maxNbBackups];
	int bufferIndex;
	char charToPutBack[maxNbBackups];
	int nbCharToPutBack;

	Node * backups[maxNbBackups];
	int backupsIndexes[maxNbBackups];
	char backupsCurrentLetters[maxNbBackups];
	int nbBackups;

	public :

	Trie(bool printUnknownWords);
	Node * addNode(char label, bool finishWord, int wordCode);
	void addWord(const char * word, int code);
	int processChar(char letter);
	void resetState();
	bool atRoot();
	int findWord(const char * word);
	int getCurrentCode();
	int getCodeMax();
	int getCodeMin();
	void emptyBuffer();
	char * getBuffer();
	void regressState();
	void printCurrent();
	void addToBuffer(char letter);
	void printNodes();
	void printUnknownWord();
	void printWordFound();
	void addBackup(Node * node);
	int getNbBackups();
	void emptyBackups();
	int getNbCharToPutBack();
	char * getCharToPutBack();
	bool bufferContainsLetter();
};

#endif

