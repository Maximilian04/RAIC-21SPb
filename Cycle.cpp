//
// Created by Maximilian on 09.10.2021.
//

#include "Cycle.h"

Cycle::Cycle() : buildingPlanet(CYCLE_BUILD_NUM, -1), isBuilt(false),
				 orderedPlanet(CYCLE_BUILD_NUM, false), isPlanned(false),
				 stackedPlanet(CYCLE_BUILD_NUM, false), prodFactor(1) {
}