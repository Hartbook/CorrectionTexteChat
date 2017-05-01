#ifndef LAYOUT__H
#define LAYOUT__H

#include "File.hpp"

class Layout
{
	private :

	File * baseText;
	File * correctedText;

	public :

	Layout(File * baseText, File * correctedText);
	void transferLayout(File * target);
};

#endif

