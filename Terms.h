//
// Created by Maximilian on 05.10.2021.
//

#ifndef MYSTRATEGY_CPP_TERMS_H
#define MYSTRATEGY_CPP_TERMS_H

#include "model/Action.hpp"
#include <iostream>

using namespace std;

enum {
	ORE = 0,
	SAND,
	ORGANICS,
	METAL,
	SILICON,
	PLASTIC,
	CHIP,
	ACCUMULATOR,

	STONE,
};

enum {
	MINES = 0,
	CAREER,
	FARM,
	FOUNDRY,
	FURNACE,
	BIOREACTOR,
	CHIP_FACTORY,
	ACCUMULATOR_FACTORY,
	REPLICATOR, // всего 9 позиций

	QUARRY,
};

model::Resource t2r(int t);
int r2t(model::Resource r);
model::BuildingType t2b(int t);
int b2t(model::BuildingType b);
int stoneCost(int t);

#endif //MYSTRATEGY_CPP_TERMS_H
