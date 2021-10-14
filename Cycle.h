//
// Created by Maximilian on 09.10.2021.
//

#ifndef MYSTRATEGY_CPP_CYCLE_H
#define MYSTRATEGY_CPP_CYCLE_H

#define CYCLE_BUILD_NUM 9

#include <vector>
#include <set>

using namespace std;

class Cycle {
public:
	vector<int> buildingPlanet; // QUARRY -> REPLICATOR
	vector<bool> orderedPlanet; // QUARRY -> REPLICATOR
	set<int> usedPlanets;
	bool isBuilt;
	bool isPlanned;

	Cycle();
};


#endif //MYSTRATEGY_CPP_CYCLE_H
