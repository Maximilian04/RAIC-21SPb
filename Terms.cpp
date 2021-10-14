//
// Created by Maximilian on 06.10.2021.
//

#include "Terms.h"

model::Resource t2r(int t) {
	switch (t) {
		case STONE:
			return model::Resource::STONE;
			break;
		case ORE:
			return model::Resource::ORE;
			break;
		case SAND:
			return model::Resource::SAND;
			break;
		case ORGANICS:
			return model::Resource::ORGANICS;
			break;
		case METAL:
			return model::Resource::METAL;
			break;
		case SILICON:
			return model::Resource::SILICON;
			break;
		case PLASTIC:
			return model::Resource::PLASTIC;
			break;
		case CHIP:
			return model::Resource::CHIP;
			break;
		case ACCUMULATOR:
			return model::Resource::ACCUMULATOR;
			break;
		default:
			cout << "I've returned Resource::STONE, but I'm objecting" << endl;
			return model::Resource::STONE;
	}
}

int r2t(model::Resource r) {
	switch (r) {
		case model::Resource::STONE:
			return STONE;
			break;
		case model::Resource::ORE:
			return ORE;
			break;
		case model::Resource::SAND:
			return SAND;
			break;
		case model::Resource::ORGANICS:
			return ORGANICS;
			break;
		case model::Resource::METAL:
			return METAL;
			break;
		case model::Resource::SILICON:
			return SILICON;
			break;
		case model::Resource::PLASTIC:
			return PLASTIC;
			break;
		case model::Resource::CHIP:
			return CHIP;
			break;
		case model::Resource::ACCUMULATOR:
			return ACCUMULATOR;
			break;
		default:
			cout << "I've returned STONE, but I'm objecting" << endl;
			return STONE;
	}
}

model::BuildingType t2b(int t) {
	switch (t) {
		case QUARRY:
			return model::BuildingType::QUARRY;
			break;
		case CAREER:
			return model::BuildingType::CAREER;
			break;
		case FARM:
			return model::BuildingType::FARM;
			break;
		case FOUNDRY:
			return model::BuildingType::FOUNDRY;
			break;
		case FURNACE:
			return model::BuildingType::FURNACE;
			break;
		case BIOREACTOR:
			return model::BuildingType::BIOREACTOR;
			break;
		case CHIP_FACTORY:
			return model::BuildingType::CHIP_FACTORY;
			break;
		case ACCUMULATOR_FACTORY:
			return model::BuildingType::ACCUMULATOR_FACTORY;
			break;
		case REPLICATOR:
			return model::BuildingType::REPLICATOR;
			break;
		case MINES:
			return model::BuildingType::MINES;
			break;
		default:
			cout << "I've returned BuildingType::MINES, but I'm objecting" << endl;
			return model::BuildingType::MINES;
	}
}

int b2t(model::BuildingType b) {
	switch (b) {
		case model::BuildingType::QUARRY:
			return QUARRY;
			break;
		case model::BuildingType::CAREER:
			return CAREER;
			break;
		case model::BuildingType::FARM:
			return FARM;
			break;
		case model::BuildingType::FOUNDRY:
			return FOUNDRY;
			break;
		case model::BuildingType::FURNACE:
			return FURNACE;
			break;
		case model::BuildingType::BIOREACTOR:
			return BIOREACTOR;
			break;
		case model::BuildingType::CHIP_FACTORY:
			return CHIP_FACTORY;
			break;
		case model::BuildingType::ACCUMULATOR_FACTORY:
			return ACCUMULATOR_FACTORY;
			break;
		case model::BuildingType::REPLICATOR:
			return REPLICATOR;
			break;
		case model::BuildingType::MINES:
			return MINES;
			break;
		default:
			cout << "I've returned MINES, but I'm objecting" << endl;
			return MINES;
	}
}

int stoneCost(int t) {
	switch (t) {
		case QUARRY:
			return 50;
			break;
		case CAREER:
			return 50;
			break;
		case FARM:
			return 50;
			break;
		case FOUNDRY:
			return 100;
			break;
		case FURNACE:
			return 100;
			break;
		case BIOREACTOR:
			return 100;
			break;
		case CHIP_FACTORY:
			return 100;
			break;
		case ACCUMULATOR_FACTORY:
			return 100;
			break;
		case REPLICATOR:
			return 100;
			break;
		case MINES:
			return 50;
			break;
		default:
			cout << "I've returned 0, but I'm objecting" << endl;
			return 0;
	}
}