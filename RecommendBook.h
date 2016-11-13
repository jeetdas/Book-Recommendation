#pragma once
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <vector>
#include <limits.h>
#include <omp.h>

class bookRecommend
{
public:
	bookRecommend();

private:
	void menu();
	template<class K, class T>
	void read_two_column_list(std::map<K, T> &m, std::string fileName);
	std::map<int, std::map<int, int> > read_ratings();
	template<typename TK, typename TV>
	void extract_keys(std::map<TK, TV> & m, std::set<TK> &keys);
	int jaccard_index_similarity(std::map <int, int> &user1, std::map <int, int> &user2);
	int LevenshteinDistance(std::string s, std::string t);
	template<class TA, class KA>
	bool findItem(std::map<TA, KA> &m, TA searchItem);
	int searchBook(std::map <int, std::string> &book_list);
	bool updateBook(std::map <int, std::map<int, int> > ratings, int isbn, int userID);
	int recommendBook(std::map <int, std::map<int, int> > ratings, int userId);
};



