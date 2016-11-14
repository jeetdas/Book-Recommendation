#include "RecommendBook.h"

bookRecommend::bookRecommend()
{
	menu();
}
//Reads in the files for the map, displays options for the user. 
void bookRecommend::menu()
{
	std::map <int, std::string> book_list;
	std::map <int, std::string> user_list;
	std::map <int, std::map<int, int> > ratings;

	//parallel sections for reading in the files and storing them
	#pragma omp sections
	{
		#pragma omp section
		{
			read_two_column_list(book_list, "books.txt");
		}
		#pragma omp section
		{
			read_two_column_list(user_list, "customers.txt");
		}
		#pragma omp section
		{
			ratings = read_ratings();
		}
	}

	int searchedBookISBN, inputUserID, option;
	std::vector <int> recISBN;
	searchedBookISBN = -1;
	option = 0;
	bool valid = false;

	while (!valid)
	{
		std::cout << "Input your user ID: ";
		std::cin >> inputUserID;
		if (inputUserID >= 0 && inputUserID <= 9)
		{
			valid = !valid;
		}
		else if (std::cin.fail() || !valid)
		{
			std::cout << "Invalid user ID, please enter again" << std::endl;
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
	}

	while (true)
	{
		std::cout << "Hello, " << user_list[inputUserID] << "\n\n";
		std::cout << "1. Search a book" << std::endl;
		std::cout << "2. Rate a book" << std::endl;
		std::cout << "3. Recommend me a book" << std::endl;
		std::cout << "4. Quit" << std::endl;

		std::cin >> option;
		if (std::cin.fail() || (option > 4 || option < 1))
		{
			std::cout << "Invalid option, try again." << std::endl;
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			option = 0;
		}

		switch (option)
		{
		case 1:
			searchedBookISBN = searchBook(book_list);
			if (searchedBookISBN != -1)
			{
				std::cout << "Book found" << std::endl;
			}
			break;
		case 2:
			if (searchedBookISBN == -1)
			{
				searchedBookISBN = searchBook(book_list);
				if (searchedBookISBN != -1)
				{
					std::cout << "Book found" << std::endl;
					updateBook(ratings, searchedBookISBN, inputUserID);
				}
				else
				{
					std::cout << "Book not found" << std::endl;
				}
			}
			else
			{
				updateBook(ratings, searchedBookISBN, inputUserID);
			}
			break;
		case 3:
			recISBN = recommendBook(ratings, inputUserID);
			for (int i = 0; i < recISBN.size(); ++i)
			{
				std::cout << "Recommended Book -> " << book_list[recISBN[i]] << std::endl;
			}
			break;
		case 4:
			exit(0);
			break;
		}
	}
}
//reads in data for the map
template<class K, class T>
void bookRecommend::read_two_column_list(std::map<K, T> &m, std::string fileName)
{
	/*
	 * Key -> Value
	 */

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

std::map<int, std::map<int, int> > bookRecommend::read_ratings()
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

	std::ifstream inputFile("ratings.txt");

	if (!inputFile)
	{
		std::cerr << "Could not open the file: ratings.txt" << std::endl;
		system("pause");
		exit(1);
	}
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

//extracts all the keys from the map
template<typename TK, typename TV>
void bookRecommend::extract_keys(std::map<TK, TV> & m, std::set<TK> &keys)
{
	for (typename std::map <TK, TV>::iterator it = m.begin(); it != m.end(); ++it)
	{
		keys.insert((*it).first);
	}
}

//equation to compare two users and find the most similar using Jaccard similarity coefficient. 
int bookRecommend::jaccard_index_similarity(std::map <int, int> &user1, std::map <int, int> &user2)
{
	/*
	* Algorithm:
	*
	* 1. Get all ISBNs for both users and put into a set (only unique elements that way).
	* 2. Cycle through both users for those ISBN ratings.
	* 3. If found with both users, add minimum score to numerator.
	* 4. If found with both users, add maximum score to denominator.
	*/

	std::set<int> isbn_list;
	std::map<int, int>::iterator it1, it2;

	int count;
	double numerator, denominator, score;

	count = 0;
	numerator = 0;
	denominator = 0;

	extract_keys(user1, isbn_list);
	extract_keys(user2, isbn_list);

	for (std::set<int>::iterator it = isbn_list.begin(); it != isbn_list.end(); ++it)
	{
		if (user1.find(*it) != user1.end() && user2.find(*it) != user2.end())
		{
			count ++;
			numerator += std::min(user1[*it], user2[*it]);
			denominator += std::max(user1[*it], user2[*it]);
		}
	}

	score = (numerator / denominator) * 100.0;

	if (count == user2.size())
		return 0;
	else
		return score;
}

/*calculates and compares the number of changes an input book title goes through with every book title in the books file, 
	the book that has the least changes is most likely the correct book to be used*/
int bookRecommend::LevenshteinDistance(std::string s, std::string t)
{
	// From Wikipedia: https://en.wikipedia.org/wiki/Levenshtein_distance

	// degenerate cases
	int cost;
    if (s == t) return 0;

    if (s.length() == 0) return t.length();
    if (t.length() == 0) return s.length();

    // create two work vectors of integer distances
	std::vector <int> v0(t.length() + 1);
	std::vector <int> v1(t.length() + 1);

    // initialize v0 (the previous row of distances)
    // this row is A[0][i]: edit distance for an empty s
    // the distance is just the number of characters to delete from t
	#pragma omp parallel for
    for (int i = 0; i < t.length() + 1; i++)
        v0[i] = i;

    for (int i = 0; i < s.length(); i++)
    {
        // calculate v1 (current row distances) from the previous row v0

        // first element of v1 is A[i+1][0]
        //   edit distance is delete (i+1) chars from s to match empty t
        v1[0] = i + 1;

        // use formula to fill in the rest of the row
		for (int j = 0; j < t.length(); j++)
        {
            cost = (s[i] == t[j]) ? 0 : 1;
			v1[j + 1] = std::min(std::min(v1[j] + 1, v0[j + 1] + 1), v0[j] + cost);
        }

        // copy v1 (current row) to v0 (previous row) for next iteration
		#pragma omp parallel for
        for (int j = 0; j < t.length() + 1; j++)
            v0[j] = v1[j];
    }

    return v1[t.length()];
}

//looks for the item that need to be found in the three files (i.e. books, ISBN, or user)
template<class TA, class KA>
bool bookRecommend::findItem(std::map<TA, KA> &m, TA searchItem)
{
	typename std::map<TA, KA>::iterator it = m.find(searchItem);
	return (it != m.end());
}

//looks for a book in the book file by ISBN or book title that the user puts in
int bookRecommend::searchBook(std::map <int, std::string> &book_list)
{
	std::string searchItem;
	std::cout << "Enter ISBN/book title to search database: ";
	getline(std::cin, searchItem);
	getline(std::cin, searchItem);

	// Remove spaces
	searchItem.erase(std::remove(searchItem.begin(), searchItem.end(), ' '), searchItem.end());

	if (searchItem.find_first_not_of("0123456789") == std::string::npos)
	{
		int t;
		// It's a number, so search using ISBN
		try
		{
			t = std::stoi(searchItem, nullptr, 10);
		}
		catch (const std::out_of_range)
		{
			std::cout << "Invalid entry" << std::endl;
			t = -1;
			return -1;
		}
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
		// Converts the searchItem string to lower case
		std::transform(searchItem.begin(), searchItem.end(), searchItem.begin(), ::tolower);
		// Search through book titles
		std::string b_title = "-1", alternativeBook;
		int alternativeBookIsbn, score = std::numeric_limits<int>::max(), tempScore;
		
		for (std::map<int, std::string>::iterator m_it = book_list.begin(); m_it != book_list.end(); ++m_it)
		{
			b_title = (*m_it).second;

			std::transform(b_title.begin(), b_title.end(), b_title.begin(), ::tolower);
			b_title.erase(std::remove(b_title.begin(), b_title.end(), ' '), b_title.end());
			b_title.erase(std::remove(b_title.begin(), b_title.end(), '\n'), b_title.end());

			if (b_title == searchItem)
			{
				return (*m_it).first;
			}
			else
			{
				tempScore = LevenshteinDistance(b_title, searchItem);
				//std::cout << "TEMP SCORE = " << tempScore << std::endl;
				if (tempScore < score)
				{
					score = tempScore;
					alternativeBook = (*m_it).second;
					alternativeBookIsbn = (*m_it).first;
				}
			}
		}

		char opt;
		std::cout << "Book not found, did you mean this book? " << alternativeBook << std::endl;
		std::cout << "Y/N?" << std::endl;
		std::cin >> opt;
		if (opt == 'y' || opt == 'Y')
		{
			return alternativeBookIsbn;
		}

		return -1;
	}
}
//looks for the book the user is looking for and wanting to rate and updates the rating files to reflect the added rating
bool bookRecommend::updateBook(std::map <int, std::map<int, int> > ratings, int isbn, int userID)
{
	// To update:
	// save to respective files
	if (ratings[userID].find(isbn) != ratings[userID].end())// if ISBN is found
	{
		int new_rating;
		bool valid = false;
		std::ofstream outputFile("ratings.txt", std::fstream::app); //outstream for file
		while (!valid)
		{
			std::cout << "Enter a rating between 1-5: ";
			std::cin >> new_rating;
			if (new_rating >= 1 && new_rating <= 5)
			{
				valid = !valid;
			}
			else
			{
				std::cout << "Please enter a valid rating between 1 and 5" << std::endl;
			}
		}
	
		ratings[userID][isbn] = new_rating;

		if (outputFile.good())
		{
			outputFile <<  "\n" << userID << ", " << new_rating << ", " << isbn ; //updates the file for new rated book
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

//the user a book to read based from the jaccard function, it outputs a book to be read
std::vector<int> bookRecommend::recommendBook(std::map <int, std::map<int, int> > ratings, int userId)
{
	double maxVal = 0, val;
	int u, maxRating, recomBook;
	#pragma parallel for
	for (int i = 0; i < ratings.size(); ++i)
	{
		if (i != userId)
		{
			val = jaccard_index_similarity(ratings[userId], ratings[i]);
			#pragma omp critical
			{
				if (maxVal < val)
				{
					maxVal = val;
					u = i;
				}
			}
		}
	}
	
	// [✓] check if that user has more books
	
	// [✓] find first unrated book
	std::vector <int> recommendBookList;
	//#pragma omp parallel for shared (recommendBookList)
	for (int i = 0; i < 10; ++i)
	{
		maxRating = 0;
		for (std::map<int, int>::iterator it = ratings[u].begin(); it != ratings[u].end(); ++it)
		{
			if ((*it).second > maxRating && ratings[userId].count((*it).first) == 0 && std::find(std::begin(recommendBookList), std::end(recommendBookList), (*it).first) == std::end(recommendBookList))
			{
				maxRating = (*it).second;
				recomBook = (*it).first;
			}
		}
		recommendBookList.push_back(recomBook);
	}

	// [✓] recommend the book
	return recommendBookList;
}

int main()
{
	bookRecommend bi;
	return 0;
}