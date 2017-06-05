#pragma once
#include "json.hpp"
#include "iostream"
#include "constraints.h"
#include <fstream>
#include <string>

struct Config
{
	int m1;
	int m2;
	int l;
	int j;
	int n;
	double b[6];
	double targetY;
};

Config readConfigFromCommandLine(string constraintsFile);
Config readConfigJson(string configFileName, string constraintsFileName);