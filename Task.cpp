//
// Created by Maximilian on 06.10.2021.
//

#include "Task.h"

model::MoveAction Task::getMoveAction() const {
	if (startPlanet == finishPlanet) {
		cout << "You're trying to get incorrect MoveAction, I'm objecting" << endl;
	}

	if (resource >= STONE && resource <= ACCUMULATOR) {
		return {startPlanet, finishPlanet, number, optional<model::Resource>(t2r(resource))};
	} else {
		return {startPlanet, finishPlanet, number, optional<model::Resource>()};
	}
}
