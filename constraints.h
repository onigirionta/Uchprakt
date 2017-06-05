#pragma once
#include "climits"
#include "vector"
#include <math.h>
#include "json.hpp"
#include <fstream>
#include <map>
#include <string>
#include "iostream"

using namespace std;

class AbstractConstraint
{
public:
	//Этот метод проверяет что значение удовлетворяет ограничению
	virtual bool check(double v) = 0;

	//Этот метод подгоняет значение под ограничение
	//Использовать при проверке выходного сигнала
	virtual double fit(double v) = 0;

	//Этот метод описывает допустимые значения
	virtual string errorMsg() = 0;

	AbstractConstraint() {};
	virtual ~AbstractConstraint() {};
};

//Ограничение вида: набор целочисленных значений
class DiscreteConstraint : public AbstractConstraint
{
private:
	vector<int> values;
public:
	DiscreteConstraint();
	~DiscreteConstraint();
	DiscreteConstraint(vector<int> v);
	void add(int v);
	bool check(double v);
	double fit(double v);
	string errorMsg();
};

//Ограничение вида: целочисленный диапазон допустимых значений
class DiscreteRangeConstraint : public AbstractConstraint
{
private:
	int minval;
	int maxval;

public:
	DiscreteRangeConstraint();
	~DiscreteRangeConstraint();
	DiscreteRangeConstraint(int a, int b);

	bool check(double v);

	double fit(double v);

	string errorMsg();
};

//Ограничение вида: диапазон допустимых значений с плавающей запятой
class FloatingConstraint : public AbstractConstraint
{
private:
	double minval;
	double maxval;
public:
	FloatingConstraint();
	~FloatingConstraint();
	FloatingConstraint(double a, double b);

	bool check(double v);

	double fit(double v);

	string errorMsg();
};

//Тип: "Множество пар (ключ,ограничение)""
using ConstrMap = map<string, AbstractConstraint*>;
ConstrMap readConstraints(string filename);
//Функция, запрашивающая значение из консоли 
//Пока оно не будет удовлетворять ограничению
//Т -- шаблонный тип. В нашем случае либо int, либо double
template <typename T>
T readWhileConstraint(string key, ConstrMap* cm) {
	T res;
	cout << "Enter value for " << key << endl;
	cin >> res;
	while (!cm->at(key)->check(res)) {
		cout << "Incorrect value for " << key << "." << endl;
		cout << cm->at(key)->errorMsg() << endl;
		cin >> res;
	}
	return res;
}