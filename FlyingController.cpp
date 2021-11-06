#include "FlyingController.h"

FlyingController::FlyingController() {
}

void FlyingController::setup(vector<vector<int>> distances, vector<vector<int>> adjacent) {
	d = distances;
	adj = adjacent;
}

vector<int> FlyingController::findPath(FlyingGroup group) {
	int to = group.to;
	int fr = group.fr;

	vector<int> path = {to};

	int prev = to;
	while (prev != fr) {
		for (int v: adj[prev]) {
			if (d[fr][v] + d[v][prev] == d[fr][prev] and v != prev) {
				path.push_back(v);
				prev = v;
			}
		}
	}

	reverse(path.begin(), path.end());

	return path;
}

int FlyingController::onFlightAt(int planet) {
	int res = 0;
	for (FlyingGroup group: groups)
		if (!group.isFinished && group.path.size() != 1 && (group.path[0] == planet && group.timeToNext == 1))
			res += group.num;

	return res;
}

void FlyingController::send(int fr, int to, int num, optional<model::Resource> res) {
	FlyingGroup group(fr, to, num, res);
	group.setPath(findPath(group), d);

	groups.push_back(group);
}

vector<model::MoveAction> FlyingController::update() {
	vector<model::MoveAction> moves;

	for (FlyingGroup& group: groups) {
		optional<model::MoveAction> move = group.step(d);
		if (move.has_value())
			moves.push_back(move.value());
	}

	return moves;
}
