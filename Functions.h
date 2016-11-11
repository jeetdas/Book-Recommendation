#pragma once
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <algorithm>


class bookRecommend
{
public:
	bookRecommend();

private:
	void menu();
	template<class K, class T>
	void show_map(std::map<K, T> &m);
	template<class K, class T>
	void read_two_column_list(std::map<K, T> &m, std::string fileName);
	std::map<int, std::map<int, int> > read_ratings();
	void show_ratings_map(std::map <int, std::map<int, int> > &m);
	template<typename TK, typename TV>
	void extract_keys(std::map<TK, TV> & m, std::set<TK> &keys);
	int jaccard_index_similarity(std::map <int, int> &user1, std::map <int, int> &user2);
	int LevenshteinDistance(std::string s, int len_s, std::string t, int len_t);
	template<class TA, class KA>
	bool findItem(std::map<TA, KA> &m, TA searchItem);
	int searchBook(std::map <int, std::string> &book_list);
	bool updateBook(std::map <int, std::string>& b_list, int isbn, int userID);
	int recommendBook(std::map <int, std::map<int, int> > ratings, int userId);
};



