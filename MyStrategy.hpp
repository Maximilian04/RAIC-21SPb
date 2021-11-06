#ifndef _MY_STRATEGY_HPP_
#define _MY_STRATEGY_HPP_

#include "model/Game.hpp"
#include "model/Action.hpp"

#include "Observer.h"
#include "FlyingController.h"
#include "Cycle.h"
#include "Planet.h"
#include "Terms.h"
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

class MyStrategy {
public:
	MyStrategy();
	model::Action getAction(const model::Game& game);

	int population;
	int homePlanet;
	vector<int> enemyHomePlanets;
	vector<Planet> planetInf;
	vector<vector<int>> planetDists;
	vector<vector<int>> logDists; //distances between planets for logists
	void init(const model::Game& game);
	void separatePlanets(const model::Game& game);

	int resetTimer;
	Cycle prodCycle;

	FlyingController fc;
	Observer observer;
};

#endif
