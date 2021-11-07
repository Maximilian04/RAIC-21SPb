#include "FlyingGroup.h"

FlyingGroup::FlyingGroup(int fr, int to, int num, optional<model::Resource> res, int safety) {
	this->fr = fr;
	this->to = to;
	this->num = num;
	this->res = res;
	this->safety = safety;
	isFinished = false;
}

void FlyingGroup::setPath(vector<int> path, vector<vector<int>>& d) {
	this->path = path;
	timeToNext = 1;
	timeFromPrev = 0;
}

optional<model::MoveAction> FlyingGroup::step(vector<vector<int>>& d) {
	if (isFinished)
		return nullopt;

	if (timeToNext == 1) {
		if (path.size() != 1) {
			timeToNext = d[path[0]][path[1]];
			timeFromPrev = 0;
			return model::MoveAction(path[0], path[1], num, res);
		} else
			isFinished = true;
	}

	if (timeToNext == 2)
		path.erase(path.begin());

	timeToNext--;
	timeFromPrev++;
  
    return nullopt;
}