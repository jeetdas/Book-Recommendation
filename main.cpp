#include <iostream>
#include <fstream>
#include <map>
#include <string>

template<class K, class T>
void show_map(std::map<K, T> &m);

template<class K, class T>
void read_two_column_list(std::map<K, T> &m, std::string fileName);

std::map<int, std::map<long, int> > read_ratings();
void show_ratings_map(std::map <int, std::map<long, int> > &m);

int main()
{
	std::map <long, std::string> book_list;
	std::map <int, std::string> user_list;
	std::map <int, std::map<long, int> > ratings;

	read_two_column_list(book_list, "books.txt");
	read_two_column_list(user_list, "customers.txt");
	ratings = read_ratings();

	//show_map(book_list);
	//show_ratings_map(ratings);

	return 0;
}

template<class K, class T>
void show_map(std::map <K, T> &m)
{
	for(typename std::map <K, T>::iterator it = m.begin(); it != m.end(); ++it)
	{
		std::cout << it->first << " --> " << it->second << "\n";
	}
}

void show_ratings_map(std::map <int, std::map<long, int> > &m)
{
	for(std::map <int, std::map<long, int> >::iterator it = m.begin(); it != m.end(); ++it)
	{
		std::cout << it->first << " --> ";
		show_map(it->second);
		std::cout << "\n";
	}
}

template<class K, class T>
void read_two_column_list(std::map<K, T> &m, std::string fileName)
{
	/*
	 * Key -> Value
	 */

	K key;
	T data;
	std::ifstream inputFile("./Sample Data/" + fileName);

	if (!inputFile)
		std::cerr << "Could not open the file: " << fileName << std::endl;
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

std::map<int, std::map<long, int> > read_ratings()
{
	/*
	 * User ID -> ISBN -> Rating
	 *            ISBN -> Rating
	 *            ISBN -> Rating
	 *
	 * User ID -> ISBN -> Rating
	 *            ISBN -> Rating
	 *            ISBN -> Rating
	 */

	std::map<int, std::map<long, int> > user_ratings;
	int id, rating;
	long isbn;
	std::string temp;

	std::ifstream inputFile("./Sample Data/ratings.txt");

	if (!inputFile)
		std::cerr << "Could not open the file: ratings.txt" << std::endl;
	else
	{
		getline(inputFile, temp); // Skip the header
		while (!inputFile.eof())
		{
			inputFile >> id >> temp >> rating >> temp >> isbn;
			user_ratings[id][isbn] = rating;
		}
	}

	inputFile.close();

	return user_ratings;
}