#include "users.h"

user::user()
{
	userID = -1;
	name = "";
}
user::user(int ID, string usrName)
{
	userID = ID;
	name = usrName;
}

template<class K, class T>
void user::read_two_column_list(std::map<K, T> &m, std::string fileName)
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
