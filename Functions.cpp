#include "Functions.h"
#include <iostream>
using namespace std;

bookRecommend::bookRecommend()
{
	//
}
void bookRecommend::menu()
{
	int option = 0;

	std::map <int, std::string> book_list;
	std::map <int, std::string> user_list;
	std::map <int, std::map<int, int> > ratings;

	read_two_column_list(book_list, "books.txt");
	read_two_column_list(user_list, "customers.txt");
	ratings = read_ratings();

	//show_map(book_list);
	//show_ratings_map(ratings);

	while (!(option >= 1 && option <= 4))
	{
		std::cout << "1. Search book" << std::endl;
		std::cout << "2. Rate book" << std::endl;
		std::cout << "3. Recommend a book" << std::endl;
		std::cout << "4. Quit (Please choose this option to protect your data" << std::endl;

		std::cin >> option;

		switch (option)
		{
			int searchedBookISBN, recISBN;
		case 1:
			searchedBookISBN = searchBook(book_list);
			break;
		case 2:
			if (searchedBookISBN != -1)
			{
				updateBook(book_list, searchedBookISBN, 6);
			}
			break;
		case 3:
			recISBN = recommendBook(ratings, 6);
			std::cout << "Recommended Book -> " << book_list[recISBN] << std::endl;
			break;
		case 4:
			//exit();
			break;
		}
	}
}
template<class K, class T>
void bookRecommend::show_map(std::map<K, T> &m)
{
	for (typename std::map <K, T>::iterator it = m.begin(); it != m.end(); ++it)
	{
		std::cout << it->first << " --> " << it->second << "\n";
	}
}
template<class K, class T>
void bookRecommend::read_two_column_list(std::map<K, T> &m, std::string fileName)
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
std::map<int, std::map<int, int>> bookRecommend::read_ratings()
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

	std::map<int, std::map<int, int> > user_ratings;
	int id, rating;
	int isbn;
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

void bookRecommend::show_ratings_map(std::map <int, std::map<int, int> > &m)
{
	for (std::map <int, std::map<int, int> >::iterator it = m.begin(); it != m.end(); ++it)
	{
		std::cout << it->first << " --> ";
		show_map(it->second);
		std::cout << "\n";
	}
}
template<typename TK, typename TV>
void bookRecommend::extract_keys(std::map<TK, TV> & m, std::set<TK> &keys)
{
	for (typename std::map <TK, TV>::iterator it = m.begin(); it != m.end(); ++it)
	{
		keys.insert((*it).first);
	}
}
int bookRecommend::jaccard_index_similarity(std::map <int, int> &user1, std::map <int, int> &user2)
{
	/*
	* Algorithm:
	*
	* 1. Get all ISBNs for both users and put into a set (only unique elements that way).
	* 2. Cycle through both users for those ISBN ratings.
	* 3. If found with both users, add minimum score to numerator.
	*    Else, add minimum (zero) to numerator.
	* 4. If found with both users, add maximum score to denominator.
	*    Else, add maximum (five) to denominator.
	*/

	std::set<int> isbn_list;
	std::map<int, int>::iterator it1, it2;

	double numerator = 0, denominator = 0, score;

	//it = m.find(searchItem);
	//return (it != m.end());

	extract_keys(user1, isbn_list);
	extract_keys(user2, isbn_list);

	for (std::set<int>::iterator it = isbn_list.begin(); it != isbn_list.end(); ++it)
	{
		if (user1.find(*it) != user1.end() && user2.find(*it) != user2.end())
		{
			// If found with both users
			numerator += std::min(user1[*it], user2[*it]);
			denominator += std::max(user1[*it], user2[*it]);

			//std::cout << " -> " << numerator << " / " << denominator << std::endl;
		}
	}

	score = (numerator / denominator) * 100.0;

	//std::cout << score << std::endl;

	return score;
}
int bookRecommend::LevenshteinDistance(std::string s, int len_s, std::string t, int len_t)
{
	// From Wikipedia: https://en.wikipedia.org/wiki/Levenshtein_distance

	int cost;

	/* base case: empty strings */
	if (len_s == 0) return len_t;
	if (len_t == 0) return len_s;

	/* test if last characters of the strings match */
	if (s[len_s - 1] == t[len_t - 1])
		cost = 0;
	else
		cost = 1;

	/* return minimum of delete char from s, delete char from t, and delete char from both */
	return std::min(std::min(LevenshteinDistance(s, len_s - 1, t, len_t) + 1, LevenshteinDistance(s, len_s, t, len_t - 1) + 1), LevenshteinDistance(s, len_s - 1, t, len_t - 1) + cost);
}
template<class TA, class KA>
bool bookRecommend::findItem(std::map<TA, KA> &m, TA searchItem)
{
	typename std::map<TA, KA>::iterator it;

	it = m.find(searchItem);

	return (it != m.end());
}
int bookRecommend::searchBook(std::map <int, std::string> &book_list)
{
	std::string searchItem;
	std::cout << "Enter ISBN/book title to search database: ";
	getline(std::cin, searchItem);

	// remove spaces
	searchItem.erase(std::remove(searchItem.begin(), searchItem.end(), ' '), searchItem.end());

	if (searchItem.find_first_not_of("0123456789") == std::string::npos)
	{
		// it's a number, so search using ISBN
		int t = std::stoi(searchItem, nullptr, 10);
		if (findItem(book_list, t))
		{
			std::cout << book_list[t] << std::endl;
			return t;
		}
		else
		{
			std::cout << "Book not found" << std::endl;
			return -1;
		}
	}
	else
	{
		//converts the searchItem string to lower case
		std::transform(searchItem.begin(), searchItem.end(), searchItem.begin(), ::tolower);
		// Search through book titles
		if (findItem(book_list, searchItem))
		{

		}
		else
			return -1;

	}
}
bool bookRecommend::updateBook(std::map <int, std::string>& b_list, int isbn, int userID)
{
	// To update:
	int new_rating;
	// save to respective files
	if (findItem(b_list, isbn))// if ISBN is found
	{
		std::ofstream outputFile("./Sample Data/ratings.txt"); //outstream for file
		std::cout << "Enter a rating between 1-5";
		std::cin >> new_rating;
		b_list[isbn] = new_rating; //updates the map

		while (outputFile.good())
		{
			outputFile << userID << ", " << new_rating << ", " << isbn; //updates the file for new rated book
		}
		outputFile.close();
		return true; //everything worked
	}
	else
	{
		std::cout << "ISBN you entered could not be found" << std::endl;
		return false; //book could not be rated
	}
}
int bookRecommend::recommendBook(std::map <int, std::map<int, int>> ratings, int userId)
{
	double maxVal = 0, val;
	int u;
	for (int i = 0; i < ratings.size(); ++i)
	{
		if (i != userId)
		{
			val = jaccard_index_similarity(ratings[userId], ratings[i]);
			if (maxVal < val)
			{
				maxVal = val;
				u = i;
			}
		}
	}

	// check if that user has more books
	// find first unrated book
	// recommend the book

	std::cout << " u = " << u << std::endl;
	return u;
}
