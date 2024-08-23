#include "Mode3.h"
#include<string>
#include<vector>
using namespace std;

int Mode3::size = 0;
vector<vector<int>> Mode3::right = vector<vector<int>>();
vector<vector<int>> Mode3::top = vector<vector<int>>();

Mode3::Mode3()
{
	isGood = Indefinite;
	m = new Block3 * [size];
	for (int i = 0; i < size; i++)
	{
		m[i] = new Block3[size];
		for (int j = 0; j < size; j++)
			m[i][j] = Indefinite;
	}
}

Mode3::Mode3(const Mode3& x)
{
	isGood = x.isGood;
	m = new Block3 * [size];
	for (int i = 0; i < size; i++)
	{
		m[i] = new Block3[size];
		for (int j = 0; j < size; j++)
			m[i][j] = x.m[i][j];
	}
}

Mode3::Mode3(Mode3&& x)
{
	isGood = x.isGood;
	m = x.m;
	x.m = nullptr;
}

Mode3::~Mode3()
{
	if (m != nullptr)
	{
		for (int i = 0; i < size; i++)
			delete[] m[i];
		delete[] m;
	}
}

const Mode3& Mode3::operator=(const Mode3& x)
{
	for (int i = 0; i < size; i++)
		delete[] m[i];
	delete[] m;

	isGood = x.isGood;
	m = new Block3 * [size];
	for (int i = 0; i < size; i++)
	{
		m[i] = new Block3[size];
		for (int j = 0; j < size; j++)
			m[i][j] = x.m[i][j];
	}

	return *this;
}

Block3* Mode3::operator[](const int i)
{
	isGood = Indefinite;
	return m[i];
}

bool Mode3::is_right()
{
	if (isGood == True)
		return true;
	if (isGood == False)
		return false;

	//for (int i = 0; i < size; i++)
	//{
	//	for (int j = 0; j < size; j++)
	//	{
	//		switch (m[i][j])
	//		{
	//		case 0:
	//			cout << "X ";
	//			break;
	//		case 1:
	//			cout << "# ";
	//			break;
	//		case 2:
	//			cout << "? ";
	//			break;
	//		default:
	//			cout << "\n\nERROR\n\n";
	//			system("pause");
	//		}
	//	}
	//	cout << endl;
	//}
	//cout << endl;

	for (int i = 0; i < size; i++)
		if (m[0][i] == Indefinite)
		{
			isGood = False;
			return false;
		}

	for (int i = 0; i < size; i++)
	{
		vector<int> h;
		int sum = 0;
		int j = 0;
		while (j < size)
		{
			while (j < size && m[i][j] == True)
			{
				sum++;
				j++;
			}
			if (sum != 0)
				h.push_back(sum);
			sum = 0;
			while (j < size && m[i][j] == False)
				j++;
		}
		if (h != right[i])
		{
			isGood == False;
			return false;
		}
	}
	isGood == True;
	return true;
}

bool Mode3::can_be_right()
{
	if (isGood == True)
		return true;

	for (int i = 0; i < size; i++)
	{
		vector<int> h;
		int sum = 0;
		int j = 0;
		while (j < size && m[i][j] != Indefinite)
		{
			while (j < size && m[i][j] == True)
			{
				sum++;
				j++;
			}
			if (sum != 0)
				h.push_back(sum);
			sum = 0;
			while (j < size && m[i][j] == False)
				j++;
		}
		
		if (h.size() > right[i].size())
			return false;

		if (h.size() > 0)
		{
			bool B = true;
			for (int j = 0; j < h.size() - 1 && B; j++)
				if (h[j] != right[i][j])
					B = false;
			if (h[h.size() - 1] > right[i][h.size() - 1])
				B = false;

			if (!B)
				return false;
		}
		else
		{
			int sum = 0; 
			for (int j = 0; j < right[i].size(); j++)
				sum += right[i][j];
			sum += right[i].size() - 1;
			
			int index = 0;
			for (; index < size && m[0][index] == False; index++);
			if (size - index < sum)
				return false;
		}
	}
	return true;
}