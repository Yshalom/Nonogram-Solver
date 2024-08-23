#pragma once
#include<string>
#include<vector>
//#include<iostream>
using namespace std;
#define True 1
#define False 0
#define Indefinite 2
#define Block3 short

class Mode3
{
private:
	Block3** m;
	Block3 isGood;

public:
	static int size;
	static vector<vector<int>> right;
	static vector<vector<int>> top;

	Mode3();
	Mode3(const Mode3&);
	Mode3(Mode3&&);
	~Mode3();
	const Mode3& operator=(const Mode3&);
	Block3* operator[](const int);
	bool is_right();
	bool can_be_right();
};