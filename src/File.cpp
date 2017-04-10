#include "File.hpp"

char File::peek()
{
	if (!buffer.empty())
		return buffer.top();

	char c = fgetc(file);
	ungetc(c, file);

	return c;
}

File::File(std::string & filename, std::string mode)
{
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

