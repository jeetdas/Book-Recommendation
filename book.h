#pragma once
#include <iostream>
#include <string>
#include <map>
#include "Functions.h"
using namespace std;

class Book : protected bookRecommend
{
public:
	Book();
	Book(int isbn, string title);
	void setISBN(int bookNum) { ISBN = bookNum; }
	int getISBN() { return ISBN; }
	void setTitle(string bTitle) { bookTitle = bTitle; }
	string getTitle() { return bookTitle; }
	//Read in the data from books.txt
	template<class K, class T>
	void read_two_column_list(std::map<K, T> &m, std::string fileName);

	std::map <int, std::string> book_list;
private:
	int ISBN;
	string bookTitle;
	
};
