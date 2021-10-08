//
// Created by Maximilian on 05.10.2021.
//

#ifndef MYSTRATEGY_CPP_TERMS_H
#define MYSTRATEGY_CPP_TERMS_H

#include "model/Action.hpp"
#include <iostream>

using namespace std;

enum {
	STONE = 0,
	ORE,
	SAND,
	ORGANICS,
	METAL,
	SILICON,
	PLASTIC,
	CHIP,
	ACCUMULATOR,

	MINES,
	QUARRY,
	CAREER,
	FARM,
	FOUNDRY,
	FURNACE,
	BIOREACTOR,
	CHIP_FACTORY,
	ACCUMULATOR_FACTORY,
	REPLICATOR,
};

model::Resource t2r(int t);

#endif //MYSTRATEGY_CPP_TERMS_H
