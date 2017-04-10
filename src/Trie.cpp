#include "Trie.hpp"
#include "util.hpp"
#include "colors.hpp"
#include <cassert>

Node::Node(char label, bool finishWord, int wordCode)
{
	this->label = label;
	this->finishWord = finishWord;
	this->wordCode = wordCode;
	this->leftChild = nullptr;
	this->rightChild = nullptr;
}

void Node::print(bool endLine = false)
{
	printf("[%s%c%s:%s] ", COLORGREEN, label, COLORBASE, finishWord ? "FIN" : "");

	if (endLine)
		printf("\n");
}

Trie::Trie(bool printUnknownWords)
{
	this->printUnknownWords = printUnknownWords;
	codeMin = -1;
	codeMax = -1;
	root = addNode('\0', false, 0);
	current = root;
	bufferIndex = 0;

	emptyBackups();
}

Node * Trie::addNode(char label, bool finishWord, int wordCode)
{
	if (codeMin == -1 || wordCode < codeMin)
		codeMin = wordCode;

	if (codeMax == -1 || wordCode > codeMax)
		codeMax = wordCode;

	nodes.emplace_back(new Node(label, finishWord, wordCode));
	return nodes.back().get();
}

void Trie::addWord(const char * word, int code)
{
	Node * current = root;
	bool endWord = false;

	while (*word != '\0')
	{
		char letter = *word;
		endWord = *(word+1) == '\0';

		if (!current->rightChild)
			current->rightChild = addNode(letter, endWord, code);

		current = current->rightChild;
	
		while (current->leftChild && !equalsChar(current->label, letter))
			current = current->leftChild;

		if (!equalsChar(current->label, letter))
		{
			current->leftChild = addNode(letter, endWord, code);
			current = current->leftChild;
		}

		word++;
	}
}

void Trie::addToBuffer(char letter)
{
	buffer[bufferIndex++] = letter;
	buffer[bufferIndex] = '\0';
}

int Trie::processChar(char letter)
{
	if (isSeparator(letter) && bufferIndex)
		addBackup(current);

	currentLetter = letter;
	addToBuffer(letter);

	if (!current->rightChild)
		return -1;

	current = current->rightChild;

	while (current->leftChild && !equalsChar(current->label, letter))
		current = current->leftChild;

	if (!equalsChar(current->label, letter))
		return -1;

	if (isSeparator(letter))
		addBackup(current);

	return current->wordCode;
}

void Trie::resetState()
{
	current = root;
	nbBackups = 0;
	nbCharToPutBack = 0;
	emptyBuffer();
}

bool Trie::atRoot()
{
	return current == root;
}

int Trie::findWord(const char * word)
{
	resetState();

	while (*word != '\0')
	{
		char letter = *word;
		
		if(processChar(letter) < 0)
		{	
			resetState();
			return 0;
		}
		
		word++;
	}
	
	int code = getCurrentCode();
	resetState();
	return code;
}

int Trie::getCurrentCode()
{
	if (!current->finishWord || current->label != currentLetter)
		return 0;

	return current->wordCode;
}

int Trie::getCodeMax()
{
	return codeMax;
}

int Trie::getCodeMin()
{
	return codeMin;
}

void Trie::regressState()
{
	assert(nbBackups > 0);

	int oldBufferIndex = bufferIndex;
	int newBufferIndex = backupsIndexes[nbBackups-1];
	nbCharToPutBack = oldBufferIndex - newBufferIndex;
	for (int i = 0; i < nbCharToPutBack; i++)
		charToPutBack[i] = buffer[newBufferIndex + i];

	current = backups[--nbBackups];
	bufferIndex = backupsIndexes[nbBackups];
	buffer[bufferIndex] = '\0';
	currentLetter = backupsCurrentLetters[nbBackups];
}

void Trie::printCurrent()
{
	current->print(true);
}

void Trie::printNodes()
{
	for (unsigned int i = 0; i < nodes.size(); i++)
		nodes[i].get()->print(true);
}
	
void Trie::addBackup(Node * node)
{
	backups[nbBackups++] = node;
	backupsIndexes[nbBackups-1] = bufferIndex;
	backupsCurrentLetters[nbBackups-1] = currentLetter;
}

void Trie::emptyBackups()
{
	nbBackups = 0;
}

int Trie::getNbBackups()
{
	return nbBackups;
}

void Trie::emptyBuffer()
{
	buffer[bufferIndex = 0] = '\0';
}

char * Trie::getBuffer()
{
	return buffer;
}

void Trie::printUnknownWord()
{
	if (printUnknownWords)
		printf("Unknown word <%s%s%s>\n", COLORRED, buffer, COLORBASE);
}

void Trie::printWordFound()
{
	if (printUnknownWords)
		printf("Found word <%s%s%s>\n", COLORGREEN, buffer, COLORBASE);
}

int Trie::getNbCharToPutBack()
{
	return nbCharToPutBack;
}

char * Trie::getCharToPutBack()
{
	return charToPutBack;
}

bool Trie::bufferContainsLetter()
{
	for (int i = 0; i < bufferIndex; i++)
		if (!isSeparator(buffer[i]))
			return true;

	return false;
}

