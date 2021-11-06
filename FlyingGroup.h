#ifndef MYSTRATEGY_CPP_FLYING_GROUP
#define MYSTRATEGY_CPP_FLYING_GROUP

#include "model/Game.hpp"
#include "model/Action.hpp"
#include <vector>
#include <utility>
#include <set>
#include <random>
#include <algorithm>
#include <chrono>
#include <map>
//#include <functional>

using namespace std;

class FlyingGroup
{
public:
    int timeToNext;
    int timeFromPrev;

    // Departure and destination planets
    int fr, to;
    // Number of wrokers in group
    int num;
    // Resource to carry
    optional<model::Resource> res;
    // Is trip for group finished
    bool isFinished;
    // Array of the planets along the way
    vector<int> path;

    FlyingGroup(int fr, int to, int num, optional<model::Resource> res)
    {
        this->fr = fr;
        this->to = to;
        this->num = num;
        this->res = res;
        isFinished = false;
    }

    void setPath(vector<int> path, vector<vector<int>> &d)
    {
        this->path = path;
        timeToNext = 1;
        timeFromPrev = 0;
    }

    optional<model::MoveAction> step(vector<vector<int>> &d)
    {
        if (isFinished)
            return nullopt;

        if (timeToNext == 1)
        {
            if (path.size() != 1)
            {
                timeToNext = d[path[0]][path[1]];
                timeFromPrev = 0;
                return model::MoveAction(path[0], path[1], num, res);
            }
            else
                isFinished = true;
        }

        if (timeToNext == 2)
            path.erase(path.begin());
        
        timeToNext -= 1;
        timeFromPrev += 1;

        return nullopt;
    }
};

#endif //MYSTRATEGY_CPP_FLYING_GROUP