#include "Util.h"
#include <fstream>

char* Util::ReadFile(const char* filename)
{
	std::ifstream f;
	f.open(filename, std::ios_base::binary); // Open
	f.seekg(0, std::ios_base::end);      // Position at end
	int length = static_cast<int>(f.tellg());              //   to get the length

	char* content = new char[length + 1]; // Create buffer of needed length
	f.seekg(0, std::ios_base::beg);     // Position at beginning
	f.read(content, length);            //   to read complete file
	f.close();                           // Close

	content[length] = char(0);           // Finish with a NULL
	return content;
}
