#include "book.h"

Book::Book()
{
	ISBN = -1;
	bookTitle = "";
}
Book::Book(int isbn, string title)
{
	ISBN = isbn;
	bookTitle = title;
}
template<class K, class T>
void Book::read_two_column_list(std::map<K, T> &m, std::string fileName)
{
	K key;
	T data;

	std::ifstream inputFile;
	inputFile.open(fileName);
	if (!inputFile)
	{
		std::cerr << "Could not open the file: " << fileName << std::endl;
		system("pause");
		exit(1);
	}
	else
	{
		getline(inputFile, data); // Skip the header
		while (!inputFile.eof())
		{
			inputFile >> key;
			getline(inputFile, data);
			data = data.substr(2); // Skip over a blank space and a comma
			m[key] = data;
		}
	}
	inputFile.close();
}
