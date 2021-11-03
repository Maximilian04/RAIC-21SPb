#ifndef _MY_STRATEGY_HPP_
#define _MY_STRATEGY_HPP_

#include "model/Game.hpp"
#include "model/Action.hpp"

#include "Cycle.h"
#include "Planet.h"
#include "Terms.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>
#include <chrono>
#include <map>
#include <set>

using namespace std;

class MyStrategy {
public:
    MyStrategy();
    model::Action getAction(const model::Game& game);

	int homePlanet;
	vector<int> enemyhomePlanets;
	vector<Planet> planetInf;
	vector<vector<int>> planetDists;
	void init(const model::Game& game);
	void separatePlanets(const model::Game& game); // generating list of planets

	bool onMySide(int id);
	//int buildeff(vector<int> candidates);
	//int planetType(const model::Game& game, int id);
	//bool compareBuilds(vector<int> v1, vector<int> v2);

	int resetTimer;
	Cycle prodCycle;
};

#endif