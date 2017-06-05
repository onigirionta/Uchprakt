#ifndef CONSTRAINTS_CPP
#define CONSTRAINTS_CPP

#include "climits"
#include "vector"
#include <math.h>
#include "json.hpp"
#include <fstream>
#include <map>
#include <string>
#include "iostream"
#include "constraints.h"

using json = nlohmann::json;
using namespace std;

//Тип: "Множество пар (ключ,ограничение)""
using ConstrMap = map<string, AbstractConstraint*>;

DiscreteConstraint::DiscreteConstraint() { values = vector<int>(); }
DiscreteConstraint::~DiscreteConstraint() {};
DiscreteConstraint::DiscreteConstraint(vector<int> v){
	values = v;
};

void DiscreteConstraint::add(int v){
	values.push_back(v);
}

bool DiscreteConstraint::check(double v){
	int iv = round(v);
	for (int i = 0; i < values.size(); i++)
	{
		if (values[i] == iv) return true;
	}
    return false;
}

double DiscreteConstraint::fit(double v){
	int res = round(v);
    int dist = INT_MAX;
    for (int i = 0; i < values.size(); ++i)
    {
		double tdist = abs(values[i] - v); 
        if(tdist < dist)
        {
          dist = tdist;
          res = values[i];
        }
    }
		return static_cast<double>(res);
    }

string DiscreteConstraint::errorMsg(){
	string str = "Value should be in: {";
	for (int i : values)
    {
      str = str + to_string(i) + ",";
    }
    if(str.back() == ',') str.pop_back();
	str+="}";
    return str;
}

DiscreteRangeConstraint::DiscreteRangeConstraint(){};
DiscreteRangeConstraint::~DiscreteRangeConstraint() {};
DiscreteRangeConstraint::DiscreteRangeConstraint(int a, int b){
	minval = a;
    maxval = b;
}

bool DiscreteRangeConstraint::check(double v){
	int check_int = round(v);
    return (minval <= check_int && check_int <= maxval);
}

double DiscreteRangeConstraint::fit(double v){
	if(v < minval) return minval;
    if(v > maxval) return maxval;
    return v;
}

string DiscreteRangeConstraint::errorMsg(){
	return "Value should be in [" + to_string(minval) + ", " + to_string(maxval) + "]";
}

FloatingConstraint::FloatingConstraint()
{
}

FloatingConstraint::~FloatingConstraint() {};
FloatingConstraint::FloatingConstraint(double a, double b){
	minval = a;
    maxval = b;
}

bool FloatingConstraint::check(double v){
    return (minval <= v && v <= maxval);
}

double FloatingConstraint::fit(double v){
	if(v < minval) return minval;
    if(v > maxval) return maxval;
    return v;
}

string FloatingConstraint::errorMsg(){
	return "Value should be in [" + to_string(minval) + ", " + to_string(maxval) + "]";
}

//Вспомогательная функция
vector<int> jsonToVec(json j){
  vector<int> v;
  if(j.is_array()){
    for (int i = 0; i < j.size(); ++i)
    {
      if(j[i].is_number()){
        v.push_back(j[i]);
      }
    }
  }
  return v;
};


//Функция, читающая ограничения из json-файла
ConstrMap readConstraints(string filename){
  map<string, AbstractConstraint*> res;
  json j;
  ifstream constraints_file(filename, ifstream::binary);
  constraints_file >> j;
  if(j.is_array()){
    for (int i = 0; i < j.size(); i++){
      if(j[i]["type"].is_string() && j[i]["name"].is_string())
      {
        string type = j[i]["type"].dump();
        type = type.substr(1, type.size() - 2);
        string name = j[i]["name"].dump();
        name = name.substr(1, name.size() - 2);
        if(type == "discrete"){
          DiscreteConstraint* dc = new DiscreteConstraint(jsonToVec(j[i]["values"]));
          res.insert(pair<string,AbstractConstraint*>(name,dc));
        }
        if(type == "floating"){
          if(j[i]["minval"].is_number() && j[i]["maxval"].is_number()){
            FloatingConstraint* fc = new FloatingConstraint(j[i]["minval"],j[i]["maxval"]);
            res.insert(pair<string,AbstractConstraint*>(name,fc));
          }
        }
        if(type == "discrete_range"){
          if(j[i]["minval"].is_number() && j[i]["maxval"].is_number()){
            DiscreteRangeConstraint* drc = new DiscreteRangeConstraint(j[i]["minval"],j[i]["maxval"]);
            res.insert(pair<string,AbstractConstraint*>(name,drc));
          } 
        }
      }
    }
  }
  return res;
}

#endif