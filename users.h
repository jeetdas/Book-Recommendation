#pragma once
#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include "Functions.h"

using namespace std;

class user : protected bookRecommend
{
public:
	user();
	user(int ID, string usrName);
	void setID(int ID) { userID = ID; }
	int getID() {return userID; }
	void setName(string theName) { name = theName; }
	string getName() { return name; }
	//read in the data from customers.txt
	template<class K, class T>
	void read_two_column_list(std::map<K, T> &m, std::string fileName);

	
	std::map <int, std::string> user_list;

private:
	int userID;
	string name;
	

};
