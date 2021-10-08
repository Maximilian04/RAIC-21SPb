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
			cout << "I've returned STONE, but I'm objecting" << endl;
			return model::Resource::STONE;
	}
}