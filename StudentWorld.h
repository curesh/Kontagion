#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
#include <vector>

class Actor;
class Socrates;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    bool deleteDamagableObjects(double testX, double testY, int dmg);
    bool checkOverlap(double x1, double x2, double y1, double y2, int radius);
    void createRingOfFire();
    void shootSpray();
    bool checkOverlapWithSocrates(double x, double y);
    void restoreHealth();
    void damageSocrates(int amt);
    void addFlameThrowers();
    void setPitsLeft(int set);
    int getPitsLeft();
    bool overlapWithDirt(double bactLocX, double bactLocY);
    //return true if ate food
    bool eatFoodIfThere(double bactLocX, double bactLocY);
    bool notWithinCircle(double newX, double newY, int rad);
    //return -1 if no food can be found
    int getDirToClosestFood(double bactLocX, double bactLocY);
    //distMax gets updated with direction pointing towards Socrates
    bool checkIfSocratesNearby(double x, double y, int& distMax);
    //Note: You can probably replace half of these functions by using this one
    int getDistBetween(double x1, double x2, double y1, double y2);
    void createObj(int x, int y, int objType);
    int getDirBetween(double x1, double x2, double y1, double y2);
    void bacteriaDied();
    int getNumBacteriaLeft();

private:
    bool overlapWithOtherObjects(double testX, double testY);
    Socrates* m_player;
    std::vector<Actor*> m_actors;
    int m_pitsDone;
    int m_bacteriaLeft;
    void placeObjRand(int numObj, int objType);
    //absolute coords will be in localX, localY
    void getCoordsForLocalCircRelativeToOrigin(double& localX, double& localY, double localAngleDeg, double radius);

};

#endif // STUDENTWORLD_H_
