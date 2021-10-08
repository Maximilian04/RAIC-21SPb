//
// Created by Maximilian on 06.10.2021.
//

#ifndef MYSTRATEGY_CPP_TASK_H
#define MYSTRATEGY_CPP_TASK_H

#include "model/Action.hpp"
#include "Terms.h"
#include <iostream>

using namespace std;

class Task {
public:
	int number;
	int startPlanet;
	int finishPlanet;
	int resource;

	int currentPlanet;

	model::MoveAction getMoveAction() const;
private:
};


#endif //MYSTRATEGY_CPP_TASK_H
