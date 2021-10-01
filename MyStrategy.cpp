#include "MyStrategy.hpp"
#include <exception>

#include <iostream>
#include <vector>

using namespace std;

#define NUM_TYPES 24 // number of types of means of production
#define NUM_RESES 14 // number of types of means of production

#define SQUAD_SIZE 10
#define BUILD_KOEF 0.5

#define PRICE_STEP 1

class Mean { // class for contain information about means of production like robots, resources, buildings
public:
	enum class Type; // type of mean of production
	//!!!undefined behaviour with incorrect t!!!
	static model::BuildingType t2b(Type t); // translate Mean::Type to model::BuildingType
	static model::Resource t2r(Type t); // translate Mean::Type to model::Resource
	static Type b2t(model::BuildingType t); // translate model::BuildingType to Mean::Type
	static Type r2t(model::Resource t); // translate model::Resource to Mean::Type
	static Type harvest_r2t(model::Resource t); // translate model::Resource to Mean::Type
	static Type eth2t(model::BuildingType t); // DANGEROUS translate model::BuildingType to Mean::Type
	static Type eth2t(model::Resource t); // DANGEROUS translate model::Resource to Mean::Type
	static Type eth2t(int t); // DANGEROUS translate int to Mean::Type !!!!!!USE ONLY WITH -1!!!!!!
	static Type getParents(Type t); //TODO ONLY ONE RES
	static Type getRightBuild(Type t); //
	static Type getRightRes(Type t); //
	static vector<Type> getChildren(Type t); //TODO реализовать
	static int type_table[NUM_TYPES][NUM_TYPES]; // таблица количеств необходимых ресурсов
	static bool isInitialised; //
	static void initialise(const model::Game& game);

	static const vector<Type> res_list; // list of resources
	static const vector<double> res_price_list;
private:
};

class PlSub;

class Advert { // advert about object of trade
public:
	int x, y;
	double p; // price or answered price
	Mean::Type type; // type of object

	bool isAnswered; // init with false

	PlSub& author; // subject of trade
	Advert(PlSub& author, double p = -1); // type as none
	Advert(PlSub& author, Mean::Type type, double p = -1);
}; //
class Supply : public Advert {
public:
	Supply(PlSub& author, double p = -1); // type as none
	Supply(PlSub& author, Mean::Type type, double p = -1);
}; //
class Demand : public Advert {
public:
	bool isInternal; // is process or transport

	Demand(PlSub& author, bool isInternal = false, double p = -1); // type as none
	Demand(PlSub& author, Mean::Type type, bool isInternal, double p = -1);
};

class PlSub { // planet - subject of trade
public:
	int x, y;
	int id;
	Mean::Type mean; // building

	///vector<Supply> supplies;
	///vector<Demand> demands;

	double prices[NUM_TYPES]; // only NUM_RESES makes sense

	PlSub(); // init with -1
	PlSub(int x, int y, int id, Mean::Type mean);

private:
};

class Deal {
public:
	Demand* demand = nullptr;
	Supply* supply = nullptr;
	//bool isConcluded = false;
};

vector<PlSub> plSubs; // all "our" planets
vector<bool> plZones;

model::Action MyStrategy::getAction(const model::Game& game) {
	if (!Mean::isInitialised) { // Initialising
		Mean::isInitialised = true;
		Mean::initialise(game);
		plZones = vector<bool>(game.planets.size(), false);
	}


	// ----- generating list of subjects -----
	for (int id = 0; id < game.planets.size(); ++id) {
		//TODO change to true zone separating
		if (game.planets[id].workerGroups.empty() || game.planets[id].workerGroups[0].playerIndex == game.myIndex) {
			if (!plZones[id]) {
				plZones[id] = true;
				if (game.planets[id].building.has_value()) {
					plSubs.push_back(PlSub(game.planets[id].x, game.planets[id].y, id,
										   Mean::b2t(game.planets[id].building.value().buildingType)));
				} else {
					plSubs.push_back(PlSub(game.planets[id].x, game.planets[id].y, id, Mean::eth2t(-1)));
				}
			}
		} else {
			if (plZones[id]) {
				plZones[id] = false;

				for (int i = plSubs.size() - 1; i >= 0; --i) {
					if (plSubs[i].id == id) {
						plSubs.erase(plSubs.begin() + i);
						break;
					}
				}
			}
		}
	}

	// ----- correcting prices -----
	for (PlSub& subj: plSubs) {
		unordered_map<model::Resource, int> resources = game.planets[subj.id].resources;
		Mean::Type profRes = Mean::getRightRes(subj.mean);
		bool hasProfRes = false;

		for (pair<model::Resource, int> resource: resources) {
			if (resource.first == model::Resource::STONE) {
				if (subj.mean == Mean::eth2t(-1)) {
					if (game.planets[subj.id].harvestableResource.has_value() /*!game.planets[subj.id].workerGroups.empty()*/) {
						subj.prices[(int) Mean::r2t(resource.first)] += PRICE_STEP;
					} else {
						subj.prices[(int) Mean::r2t(resource.first)] -= PRICE_STEP;
					}
				} else {
					subj.prices[(int) Mean::r2t(resource.first)] -= PRICE_STEP;
				}
			} else if (Mean::r2t(resource.first) == profRes) {
				hasProfRes = true;
				if (!game.planets[subj.id].workerGroups.empty() &&
					game.planets[subj.id].workerGroups[0].number >= 100) { // todo change to maxWorkers
					subj.prices[(int) Mean::r2t(resource.first)] -= PRICE_STEP;
				} else {
					subj.prices[(int) Mean::r2t(resource.first)] += PRICE_STEP;
				}
			} else {
				subj.prices[(int) Mean::r2t(resource.first)] -= PRICE_STEP;
			}
		}

		if (profRes != Mean::eth2t(-1) && !hasProfRes) {
			subj.prices[(int) profRes] += PRICE_STEP;
		}

		if (subj.mean == Mean::eth2t(-1) && !game.planets[subj.id].workerGroups.empty()) {

			subj.prices[0] += 0.5 * PRICE_STEP;
		}
	}

	// ----- simulating marketplace with robots -----
	vector<model::MoveAction> moveActionsR;
	vector<model::BuildingAction> buildActionsR;
	for (int id = 0; id < game.planets.size(); ++id) { // todo нужно как-то тасовать порядок планет мб?
//		cout << "plId " << id << endl;

		if (game.planets[id].workerGroups.empty() || game.planets[id].workerGroups[0].playerIndex != game.myIndex) {
			continue;
		}

		int x = game.planets[id].x;
		int y = game.planets[id].y;
		for (int robotNum = game.planets[id].workerGroups[0].number; robotNum >= SQUAD_SIZE; robotNum -= SQUAD_SIZE) {
			pair<PlSub*, Mean::Type> bestBegin, bestEnd;
			double bestDeltaPrice = -1;

			for (int beginPl = 0; beginPl < plSubs.size(); ++beginPl) {
				for (int endPl = 0; endPl < plSubs.size(); ++endPl) {
					double targetDist = (abs(game.planets[id].x - plSubs[beginPl].x) +
										 abs(game.planets[id].y - plSubs[beginPl].y));

					PlSub& beg = plSubs[beginPl];
					PlSub& eed = plSubs[endPl];
					if (endPl == beginPl) { // переработка
						if (game.planets[plSubs[endPl].id].building.has_value()) { // если есть постройка
							Mean::Type build = Mean::b2t(game.planets[plSubs[endPl].id].building.value().buildingType);
							for (Mean::Type res: Mean::res_list) {
								if (build == Mean::getRightBuild(res)) {

									double deltaPrice = plSubs[endPl].prices[(int) res] -
														plSubs[endPl].prices[(int) Mean::getParents(res)] -
														0.01 * targetDist;

									int temp = (int) res;
									int tempp = (int) Mean::getParents(res);
									double temp2 = plSubs[endPl].prices[(int) res];
									double temp3 = plSubs[endPl].prices[(int) Mean::getParents(res)];

									if (deltaPrice > bestDeltaPrice) {
										bestDeltaPrice = deltaPrice;

										bestBegin = {&plSubs[endPl], Mean::getParents(res)};
										bestEnd = {&plSubs[endPl], res};
									}
									break;
								}
							}
						} else { // если нет постройки
							for (Mean::Type res: Mean::res_list) {
								double deltaPrice = plSubs[endPl].prices[(int) res] -
													plSubs[endPl].prices[(int) Mean::getParents(res)] -
													0.01 * targetDist;

								if (deltaPrice > bestDeltaPrice) {
									bestDeltaPrice = deltaPrice;

									bestBegin = {&plSubs[endPl], Mean::getParents(res)};
									bestEnd = {&plSubs[endPl], res};
								}
							}
						}
					} else { // доставка
						for (Mean::Type res: Mean::res_list) {
							double deltaPrice = plSubs[endPl].prices[(int) res] -
												plSubs[beginPl].prices[(int) res] -
												targetDist;

							if (deltaPrice > bestDeltaPrice) {
								bestDeltaPrice = deltaPrice;

								bestBegin = {&plSubs[beginPl], res};
								bestEnd = {&plSubs[endPl], res};
							}
						}
					}
				}
			}

//			cout << bestBegin.first << " resSs " << bestEnd.first << endl;

			if (bestDeltaPrice == -1 || bestBegin.first == nullptr || bestEnd.first == nullptr) {
				cout << "KARAUL" << endl;
				continue;
			}

			if (bestBegin.first == bestEnd.first) { // переработка
				if (game.planets[bestEnd.first->id].building.has_value()) { // если есть постройка
					if (bestEnd.first->id == id) {

					} else {
						moveActionsR.push_back(model::MoveAction(id, bestEnd.first->id, SQUAD_SIZE,
																 optional<model::Resource>()));
					}
				} else { // если нет постройки
					if (bestEnd.first->id == id) {
						buildActionsR.push_back(model::BuildingAction(id, optional<model::BuildingType>(
								Mean::t2b(Mean::getRightBuild(bestEnd.second)))));
					} else {
						moveActionsR.push_back(model::MoveAction(id, bestEnd.first->id, SQUAD_SIZE,
																 optional<model::Resource>()));
					}
				}
			} else { // доставка
				if (bestBegin.first->id == id) {
					moveActionsR.push_back(model::MoveAction(id, bestEnd.first->id, SQUAD_SIZE,
															 optional<model::Resource>(Mean::t2r(bestEnd.second))));
				} else {
					moveActionsR.push_back(model::MoveAction(id, bestBegin.first->id, SQUAD_SIZE,
															 optional<model::Resource>()));
				}
			}
		}
	}

	vector<int> targetPlanets;
	vector<model::MoveAction> moveActions;
	vector<model::BuildingAction> buildingActions;

	for (int i = 0; i < game.planets.size(); ++i) {
		if (game.planets[i].harvestableResource.has_value()) {
			targetPlanets.push_back(i);

			buildingActions.push_back(model::BuildingAction(i, optional<model::BuildingType>(
					(model::BuildingType) (int) game.planets[i].harvestableResource.value())));
		}
	}

	for (int i = 0; i < game.planets.size(); ++i) {
		for (model::WorkerGroup workerGroup: game.planets[i].workerGroups) {
			if (workerGroup.playerIndex == game.myIndex) {
				for (int& targetPlanet: targetPlanets) {
					moveActions.push_back(model::MoveAction(i, targetPlanet, workerGroup.number / targetPlanets.size(),
															optional<model::Resource>(model::Resource::STONE)));
				}
			}
		}
	}

	return model::Action(moveActionsR, buildActionsR);
}

MyStrategy::MyStrategy() {}

enum class Mean::Type { // it's necessary, what STONE has 0 number!!!
	none = -1,              // it's not true value!!!
	STONE = 0,              //prt

	QUARRY,                 //bld

	MINES,                  //bld
	CAREER,                 //bld
	FARM,                   //bld
	ORE,                    //prt
	SAND,                   //prt
	ORGANICS,               //prt

	FOUNDRY,                //bld
	FURNACE,                //bld
	BIOREACTOR,             //bld
	METAL,                  //prt
	SILICON,                //prt
	PLASTIC,                //prt

	CHIP_FACTORY,           //bld
	ACCUMULATOR_FACTORY,    //bld
	CHIP,                   //prt
	ACCUMULATOR,            //prt

	REPLICATOR,             //bld
	ROBOT,                  //prt

	HARV_STONE,            //prt	it's not true value!!!
	HARV_ORE,              //prt	it's not true value!!!
	HARV_SAND,             //prt	it's not true value!!!
	HARV_ORGANICS,         //prt	it's not true value!!!
}; // it's necessary, what STONE has 0 number!!!
//!!!undefined behaviour with incorrect t!!!
model::BuildingType Mean::t2b(Mean::Type t) {
	switch (t) {
		case Mean::Type::QUARRY:
			return model::BuildingType::QUARRY;
			break;
		case Mean::Type::MINES:
			return model::BuildingType::MINES;
			break;
		case Mean::Type::CAREER:
			return model::BuildingType::CAREER;
			break;
		case Mean::Type::FARM:
			return model::BuildingType::FARM;
			break;
		case Mean::Type::FOUNDRY:
			return model::BuildingType::FOUNDRY;
			break;
		case Mean::Type::FURNACE:
			return model::BuildingType::FURNACE;
			break;
		case Mean::Type::BIOREACTOR:
			return model::BuildingType::BIOREACTOR;
			break;
		case Mean::Type::CHIP_FACTORY:
			return model::BuildingType::CHIP_FACTORY;
			break;
		case Mean::Type::ACCUMULATOR_FACTORY:
			return model::BuildingType::ACCUMULATOR_FACTORY;
			break;
		case Mean::Type::REPLICATOR:
			return model::BuildingType::REPLICATOR;
			break;
	}
} //
model::Resource Mean::t2r(Mean::Type t) {
	switch (t) {
		case Mean::Type::STONE:
			return model::Resource::STONE;
			break;
		case Mean::Type::ORE:
			return model::Resource::ORE;
			break;
		case Mean::Type::SAND:
			return model::Resource::SAND;
			break;
		case Mean::Type::ORGANICS:
			return model::Resource::ORGANICS;
			break;
		case Mean::Type::METAL:
			return model::Resource::METAL;
			break;
		case Mean::Type::SILICON:
			return model::Resource::SILICON;
			break;
		case Mean::Type::PLASTIC:
			return model::Resource::PLASTIC;
			break;
		case Mean::Type::CHIP:
			return model::Resource::CHIP;
			break;
		case Mean::Type::ACCUMULATOR:
			return model::Resource::ACCUMULATOR;
			break;
	}

	cout << "I've returned STONE, but I'm objecting" << endl;
	return model::Resource::STONE;
} //
Mean::Type Mean::b2t(model::BuildingType t) {
	switch (t) {
		case model::BuildingType::QUARRY:
			return Mean::Type::QUARRY;
			break;
		case model::BuildingType::MINES:
			return Mean::Type::MINES;
			break;
		case model::BuildingType::CAREER:
			return Mean::Type::CAREER;
			break;
		case model::BuildingType::FARM:
			return Mean::Type::FARM;
			break;
		case model::BuildingType::FOUNDRY:
			return Mean::Type::FOUNDRY;
			break;
		case model::BuildingType::FURNACE:
			return Mean::Type::FURNACE;
			break;
		case model::BuildingType::BIOREACTOR:
			return Mean::Type::BIOREACTOR;
			break;
		case model::BuildingType::CHIP_FACTORY:
			return Mean::Type::CHIP_FACTORY;
			break;
		case model::BuildingType::ACCUMULATOR_FACTORY:
			return Mean::Type::ACCUMULATOR_FACTORY;
			break;
		case model::BuildingType::REPLICATOR:
			return Mean::Type::REPLICATOR;
			break;
	}
} //
Mean::Type Mean::r2t(model::Resource t) {
	switch (t) {
		case model::Resource::STONE:
			return Mean::Type::STONE;
			break;
		case model::Resource::ORE:
			return Mean::Type::ORE;
			break;
		case model::Resource::SAND:
			return Mean::Type::SAND;
			break;
		case model::Resource::ORGANICS:
			return Mean::Type::ORGANICS;
			break;
		case model::Resource::METAL:
			return Mean::Type::METAL;
			break;
		case model::Resource::SILICON:
			return Mean::Type::SILICON;
			break;
		case model::Resource::PLASTIC:
			return Mean::Type::PLASTIC;
			break;
		case model::Resource::CHIP:
			return Mean::Type::CHIP;
			break;
		case model::Resource::ACCUMULATOR:
			return Mean::Type::ACCUMULATOR;
			break;
	}
} //
Mean::Type Mean::harvest_r2t(model::Resource t) {
	switch (t) {
		case model::Resource::STONE:
			return Mean::Type::HARV_STONE;
			break;
		case model::Resource::ORE:
			return Mean::Type::HARV_ORE;
			break;
		case model::Resource::SAND:
			return Mean::Type::HARV_SAND;
			break;
		case model::Resource::ORGANICS:
			return Mean::Type::HARV_ORGANICS;
			break;
	}
	return Mean::Type::none;
} //
Mean::Type Mean::eth2t(model::BuildingType t) {
	switch (t) {
		case model::BuildingType::QUARRY:
			return Mean::Type::QUARRY;
			break;
		case model::BuildingType::MINES:
			return Mean::Type::MINES;
			break;
		case model::BuildingType::CAREER:
			return Mean::Type::CAREER;
			break;
		case model::BuildingType::FARM:
			return Mean::Type::FARM;
			break;
		case model::BuildingType::FOUNDRY:
			return Mean::Type::FOUNDRY;
			break;
		case model::BuildingType::FURNACE:
			return Mean::Type::FURNACE;
			break;
		case model::BuildingType::BIOREACTOR:
			return Mean::Type::BIOREACTOR;
			break;
		case model::BuildingType::CHIP_FACTORY:
			return Mean::Type::CHIP_FACTORY;
			break;
		case model::BuildingType::ACCUMULATOR_FACTORY:
			return Mean::Type::ACCUMULATOR_FACTORY;
			break;
		case model::BuildingType::REPLICATOR:
			return Mean::Type::REPLICATOR;
			break;
	}
} //
Mean::Type Mean::eth2t(model::Resource t) {
	switch (t) {
		case model::Resource::STONE:
			return Mean::Type::STONE;
			break;
		case model::Resource::ORE:
			return Mean::Type::ORE;
			break;
		case model::Resource::SAND:
			return Mean::Type::SAND;
			break;
		case model::Resource::ORGANICS:
			return Mean::Type::ORGANICS;
			break;
		case model::Resource::METAL:
			return Mean::Type::METAL;
			break;
		case model::Resource::SILICON:
			return Mean::Type::SILICON;
			break;
		case model::Resource::PLASTIC:
			return Mean::Type::PLASTIC;
			break;
		case model::Resource::CHIP:
			return Mean::Type::CHIP;
			break;
		case model::Resource::ACCUMULATOR:
			return Mean::Type::ACCUMULATOR;
			break;
	}
} //
Mean::Type Mean::eth2t(int t) {
	if (t != -1) {
		cout << "ALL'S WRONG ACHTUNG" << endl;
	}
	return Mean::Type::none;
} //
Mean::Type Mean::getParents(Type t) {
	switch (t) {
		case Mean::Type::STONE:
			return Mean::Type::HARV_STONE;
			break;
		case Mean::Type::ORE:
			return Mean::Type::HARV_ORE;
			break;
		case Mean::Type::SAND:
			return Mean::Type::HARV_SAND;
			break;
		case Mean::Type::ORGANICS:
			return Mean::Type::HARV_ORGANICS;
			break;
		case Mean::Type::METAL:
			return Mean::Type::ORE;
			break;
		case Mean::Type::SILICON:
			return Mean::Type::SAND;
			break;
		case Mean::Type::PLASTIC:
			return Mean::Type::ORGANICS;
			break;
		case Mean::Type::CHIP:
			return Mean::Type::SILICON;
			break;
		case Mean::Type::ACCUMULATOR:
			return Mean::Type::PLASTIC;
			break;
		case Mean::Type::ROBOT:
			return Mean::Type::ACCUMULATOR;
			break;
		case Mean::Type::HARV_STONE:
			return Mean::Type::none;
			break;
		case Mean::Type::HARV_ORE:
			return Mean::Type::none;
			break;
		case Mean::Type::HARV_SAND:
			return Mean::Type::none;
			break;
		case Mean::Type::HARV_ORGANICS:
			return Mean::Type::none;
			break;
	}
} //TODO OBLY ONE RES
Mean::Type Mean::getRightBuild(Type t) {
	switch (t) {
		case Mean::Type::STONE:
			return Mean::Type::QUARRY;
			break;
		case Mean::Type::ORE:
			return Mean::Type::MINES;
			break;
		case Mean::Type::SAND:
			return Mean::Type::CAREER;
			break;
		case Mean::Type::ORGANICS:
			return Mean::Type::FARM;
			break;
		case Mean::Type::METAL:
			return Mean::Type::FOUNDRY;
			break;
		case Mean::Type::SILICON:
			return Mean::Type::FURNACE;
			break;
		case Mean::Type::PLASTIC:
			return Mean::Type::BIOREACTOR;
			break;
		case Mean::Type::CHIP:
			return Mean::Type::CHIP_FACTORY;
			break;
		case Mean::Type::ACCUMULATOR:
			return Mean::Type::ACCUMULATOR_FACTORY;
			break;
		case Mean::Type::ROBOT:
			return Mean::Type::REPLICATOR;
			break;
		case Mean::Type::HARV_STONE:
			return Mean::Type::none;
			break;
		case Mean::Type::HARV_ORE:
			return Mean::Type::none;
			break;
		case Mean::Type::HARV_SAND:
			return Mean::Type::none;
			break;
		case Mean::Type::HARV_ORGANICS:
			return Mean::Type::none;
			break;
	}
} //
Mean::Type Mean::getRightRes(Type t) {
	switch (t) {
		case Mean::Type::FOUNDRY:
			return Mean::Type::ORE;
			break;
		case Mean::Type::FURNACE:
			return Mean::Type::SAND;
			break;
		case Mean::Type::BIOREACTOR:
			return Mean::Type::ORGANICS;
			break;
		case Mean::Type::CHIP_FACTORY:
			return Mean::Type::SILICON;
			break;
		case Mean::Type::ACCUMULATOR_FACTORY:
			return Mean::Type::PLASTIC;
			break;
		case Mean::Type::REPLICATOR:
			return Mean::Type::ACCUMULATOR;
			break;
	}
	return Mean::Type::none;
} //
vector<Mean::Type> Mean::getChildren(Type t) {
	/*switch (t) {
		case Mean::Type::STONE:
			return {Mean::Type::};
			break;
		case Mean::Type::ORE:
			return {Mean::Type::};
			break;
		case Mean::Type::SAND:
			return {Mean::Type::};
			break;
		case Mean::Type::ORGANICS:
			return {Mean::Type::};
			break;
		case Mean::Type::METAL:
			return {Mean::Type::};
			break;
		case Mean::Type::SILICON:
			return {Mean::Type::};
			break;
		case Mean::Type::PLASTIC:
			return {Mean::Type::};
			break;
		case Mean::Type::CHIP:
			return {Mean::Type::};
			break;
		case Mean::Type::ACCUMULATOR:
			return {Mean::Type::};
			break;
		case Mean::Type::ROBOT:
			return {};
			break;
		case Mean::Type::HARV_STONE:
			return {};
			break;
		case Mean::Type::HARV_ORE:
			return {};
			break;
		case Mean::Type::HARV_SAND:
			return {};
			break;
		case Mean::Type::HARV_ORGANICS:
			return {};
			break;
	}*/
	return {};
} // todo realise
int Mean::type_table[NUM_TYPES][NUM_TYPES] = {}; //
bool Mean::isInitialised = false; //
void Mean::initialise(const model::Game& game) {
	for (int i = 0; i < NUM_TYPES; ++i) {
		for (int j = 0; j < NUM_TYPES; ++j) {
			Mean::type_table[i][j] = 0;
		}
	}
	for (pair<model::BuildingType, model::BuildingProperties> product: game.buildingProperties) {
		for (pair<model::Resource, int> condition: product.second.workResources) {
			Mean::type_table[(int) Mean::b2t(product.first)][(int) Mean::r2t(condition.first)] = condition.second;
		}
		for (pair<model::Resource, int> condition: product.second.buildResources) {
			Mean::type_table[(int) Mean::b2t(product.first)][(int) Mean::r2t(condition.first)] = condition.second;
		}
	}
	for (pair<model::BuildingType, model::BuildingProperties> producer: game.buildingProperties) {
		if (producer.second.produceResource.has_value()) {
			Mean::type_table[(int) Mean::r2t(producer.second.produceResource.value())][(int) Mean::b2t(producer.first)]
					= 1;
		}
		if (producer.second.produceWorker) {
			Mean::type_table[(int) Mean::Type::ROBOT][(int) Mean::b2t(producer.first)] = 1;
		}
	}
}

const vector<Mean::Type> Mean::res_list = {
		Mean::Type::STONE,
		Mean::Type::ORE,
		Mean::Type::SAND,
		Mean::Type::ORGANICS,
		Mean::Type::METAL,
		Mean::Type::SILICON,
		Mean::Type::PLASTIC,
		Mean::Type::CHIP,
		Mean::Type::ACCUMULATOR,
		Mean::Type::ROBOT,
}; //
const vector<double> Mean::res_price_list = {
		10, //1,      // STONE
		20, //2,      // ORE
		30, //4,      // SAND
		40, //8,      // ORGANICS
		50, //20,     // METAL
		60, //40,     // SILICON
		70, //80,     // PLASTIC
		80, //200,    // CHIP
		90, //400,    // ACCUMULATOR
		100, //800,    // ROBOT
};

Advert::Advert(PlSub& author, double p) : author(author), x(author.x), y(author.y), type(Mean::Type::none), p(p),
										  isAnswered(false) {
} //
Advert::Advert(PlSub& author, Mean::Type type, double p) : author(author), x(author.x), y(author.y), type(type), p(p),
														   isAnswered(false) {
} //
Supply::Supply(PlSub& author, double p) : Advert(author, p) {
}; //
Supply::Supply(PlSub& author, Mean::Type type, double p) : Advert(author, type, p) {
}; //
Demand::Demand(PlSub& author, bool isInternal, double p) : Advert(author, p), isInternal(isInternal) {
}; //
Demand::Demand(PlSub& author, Mean::Type type, bool isInternal, double p) : Advert(author, type, p),
																			isInternal(isInternal) {
}; //
PlSub::PlSub() : x(-1), y(-1), id(-1), mean(Mean::Type::none) {
	for (double& price: prices) {
		price = 0;
	}
	for (int i = 0; i < Mean::res_list.size(); ++i) {
		prices[(int) Mean::res_list[i]] = Mean::res_price_list[i];
	}
} //
PlSub::PlSub(int x, int y, int id, Mean::Type mean) : x(x), y(y), id(id), mean(mean) {
	for (double& price: prices) {
		price = 0;
	}
	for (int i = 0; i < Mean::res_list.size(); ++i) {
		prices[(int) Mean::res_list[i]] = Mean::res_price_list[i];
	}
} //
