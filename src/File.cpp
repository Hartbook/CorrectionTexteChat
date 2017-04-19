#include "File.hpp"

char File::peek()
{
	if (!buffer.empty())
		return buffer.top();

	if (endHasBeenReached)
		return EOF;

	char c = fgetc(file);

	if (c == EOF)
		endHasBeenReached = true;
	else
		ungetc(c, file);

	return c;
}

File::File(const std::string & filename, const std::string & mode)
{
	this->filename = filename;
	endHasBeenReached = false;

	if (mode != "r" && mode != "w")
	{
		printf("\"%s\" is an invalid mode when opening a file\n", mode.c_str());

		exit(1);
	}

	file = fopen(filename.c_str(), mode.c_str());

	if (!file)
	{
		printf("Cannot open file %s\n", filename.c_str());

		exit(1);
	}
}

File::~File()
{
	fclose(file);
}

bool File::isFinished()
{
	return buffer.empty() && peek() == EOF;
}

char File::getChar()
{
	if (buffer.empty())
		return getc(file);

	char result = buffer.top();
	buffer.pop();

	return result;
}

void File::ungetChar(char c)
{
	buffer.push(c);
}

FILE * File::getDescriptor()
{
	return file;
}

const std::string & File::getName()
{
	return filename;
}

void File::readUntil(char c)
{
	while (!isFinished() && getChar() != c);
}

void File::rewind()
{
	fclose(file);

	endHasBeenReached = false;
	while (!buffer.empty())
		buffer.pop();

	file = fopen(filename.c_str(), "r");

	if (!file)
	{
		printf("Cannot open file %s\n", filename.c_str());

		exit(1);
	}
}

