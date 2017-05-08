/// \file 
/// \author Franck Dary
/// \author Cindy Aloui
#ifndef FILE__H
#define FILE__H

#include <cstdio>
#include <stack>
#include <string>
#include <functional>

/////////////////////////////////////////////////////////////////////////////
/// \brief Represents a file on the system.
///
/// This is an enhancement of the c-style `FILE` file descriptor.
/// The main target is to allow for consecutives `ungetChar` using a stack.
///
/// Usage example :
/// \code
/// File file("relative/path/to/file", "w"); // Create a new empty file on system
/// fprintf(file.getDescriptor(), "This file is %s\n", file.getName().c_str());
///
/// file.rewind(); // Same as opening in "r" read mode
/// file.ungetChar('b'); //
/// file.ungetChar('a'); // legit
///
/// char c = file.getChar(); // c == 'a'
/// c = file.getChar(); // c == 'b'
/// c = file.getChar(); // c == 'T'
///
/// c = file.readUntil(' '); // c == ' '
/// c = file.getChar(); // c == 'f'
/// c = file.readUntil([](char c){return !std::isalpha(c);}); // c == ' '
/// c = file.getChar(); // c == 'i'
///
/// // How to read the entire file
/// while (!file.isFinished())
/// {
///		char c = file.getChar();
///		if (file.peek() == 'Z')
///			doStuff(...);
/// }
/// \endcode
///
////////////////////////////////////////////////////////////////////////////
class File
{
	private :

	FILE * file;
	std::stack<char> buffer;
	bool endHasBeenReached;
	std::string filename;

	public :

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Constructor.
	///
	/// \param filename path to the file to be created or opened.
	/// \param mode "r" or "w" for reading an existing file or creating a new file.
	///
	////////////////////////////////////////////////////////////////////////////
	File(const std::string & filename, const std::string & mode);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Destructor, taking care of closing the file descriptor.
	///
	////////////////////////////////////////////////////////////////////////////
	~File();

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Peek the next `char` to be read from the file.
	///
	/// This is better than doing `ungetChar(getChar())` since it does not fill
	/// the stack.
	///
	/// \return The `char` that getChar() will return if called.
	///
	////////////////////////////////////////////////////////////////////////////
	char peek();

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Is there nothing more to read from this file ?
	///
	/// \return Return true if the next call to getChar() will return EOF.
	///
	////////////////////////////////////////////////////////////////////////////
	bool isFinished();

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Read a `char` from the file.
	///
	/// \return The `char` that has been read, is EOF if the file has been read
	/// entirely.
	///
	////////////////////////////////////////////////////////////////////////////
	char getChar();

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Put back a `char` in the file.
	///
	/// This does not write into the file. The next call to getChar() will 
	///	return c.
	///
	/// \param c `char` to be put inside the file.
	///
	////////////////////////////////////////////////////////////////////////////
	void ungetChar(char c);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Return the file file descirptor.
	///
	/// Can be used to read / write from the file using `fprintf` family functions.
	///
	/// \return File descirptor.
	///
	////////////////////////////////////////////////////////////////////////////
	FILE * getDescriptor();

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Get the name of the file.
	///
	/// \return  Return a reference to the path of the file.
	///
	////////////////////////////////////////////////////////////////////////////
	const std::string & getName();

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Read the file until c is encountered.
	///
	/// \param c The function will stop reading if this `char` is read.
	///
	/// \return c
	///
	////////////////////////////////////////////////////////////////////////////
	char readUntil(char c);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Read the file until a `char` that satisfies condition is encountered.
	///
	/// \param condition A `boolean` function that takes a `char` as parameter.
	///
	/// \return The `char` that met the condition.
	///
	////////////////////////////////////////////////////////////////////////////
	char readUntil(const std::function<bool(char)> & condition);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Rewind the File as if it was freshly constructed in "r" mode
	///
	////////////////////////////////////////////////////////////////////////////
	void rewind();
};

#endif
