#ifndef ACTOR_H_
#define ACTOR_H_


#define _USE_MATH_DEFINES

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
//#include "StudentWorld.h"
#include "GameConstants.h"
#include <cmath>
class StudentWorld;

//****//****//****//****//****//****//****//****//****//****//****//****//****//****//****//****//****
//****//****//****//****//****//****//****//****//****//****//****//****//****//****//****//****//****
//Constants
const int SOCRATES_STARTINGHP = 100;
const int MAX_SPRAY = 20;
const int START_FLAME = 5;
//To account for mistake in the spec... Now this matches the sample program
const int TRAVEL_DIST_FLAME = 40;
const int TRAVEL_DIST_SPRAY = 112;

const int obj_FOOD = 0;
const int obj_DIRT = 1;
const int obj_PIT = 2;
const int obj_REGSAL = 3;
const int obj_AGGSAL = 4;
const int obj_ECOLI = 5;
//Unusable Abstract classes

class Actor : public GraphObject{
public:
    Actor(int imageID, double startX, double startY, int exists, StudentWorld *sw, Direction dir, int depth);
    virtual ~Actor() = 0;
    virtual int getExistStatus();
    virtual void setExistStatus(int newExistStatus);
    virtual void doSomething() = 0;
    StudentWorld* getStudentWorld();
    virtual bool getDamagedByFlame();
    virtual bool isAnimate() = 0;
    virtual bool attractsBacteria();
    //add virtual functions here that determine the class of the derived classes (by determining whether it is animate or inanimate etc.
    
private:
    int m_exists;
    bool m_damagedByFlame;
    StudentWorld *m_myStudentWorld;
    
};

class Inanimate : public Actor{
public:
    Inanimate(int imageID, double startX, double startY, int exists, StudentWorld *sw, Direction dir = 0, int depth = 1);
    virtual ~Inanimate() = 0;
    virtual void doSomething();
    virtual bool getDamagedByFlame();
    virtual bool isAnimate();
    virtual bool attractsBacteria();
};

class Goodie : public Inanimate{ //and baddie (fungus)
public:
    Goodie(int imageID, double startX, double startY, StudentWorld *sw, int lifeTime);
    virtual ~Goodie();
    virtual void actionToTake() = 0;
    void doSomething();
    virtual void playGoodieSound();
private:
    int m_lifeTimeLeft;
};

class Projectile : public Inanimate{
public:
    Projectile(int imageID, double startX, double startY, int exists, int travelDist, StudentWorld *sw, int dmg, Direction dir);
    virtual ~Projectile() = 0;
    void doSomething();
    void decreaseTravelDist(int decr);
    bool getDamagedByFlame();
    
private:
    int m_travelDistLeft;
    int m_dmg;
};

class Animate : public Actor{
public:
    Animate(int imageID, double startX, double startY, int exists, StudentWorld *sw, int hp, Direction dir = 90, int depth = 0);
    virtual ~Animate() = 0;
    virtual void doSomething() = 0;
    int getExistStatus();
    virtual void setExistStatus(int newExistStatus);
    virtual bool getDamagedByFlame();
    virtual bool isAnimate();
    virtual void playHurtSound() = 0;
    virtual void playKilledSound() = 0;
    virtual bool isBacteria() = 0;
private:
    int m_healthPoints;
};

class Bacteria : public Animate{
public:
    Bacteria(int imageID, double startX, double startY, int hp, StudentWorld* sw, int attackPower);
    virtual ~Bacteria();
    virtual void doSomething();
    void damageSocrates();
    virtual bool differentiatedAttackSocrates();
    virtual void differentiatedMovement();
    virtual void mitosis(double x, double y) = 0;
    bool attempToMove(Direction dir, int distToMove, bool checkForBoundary);
    virtual void playHurtSound();
    virtual void playKilledSound();
    bool isBacteria();

private:
    int m_movementPlan;
    int m_attackPower;
    int m_foodEaten;
};



//****//****//****//****//****//****//****//****//****//****//****//****//****//****//****//****//****
//****//****//****//****//****//****//****//****//****//****//****//****//****//****//****//****//****
//Usable classes

class Dirt : public Inanimate{
public:
    Dirt(double startX, double startY, StudentWorld *sw);
    virtual ~Dirt();
    bool getDamagedByFlame();
    
};

class Food : public Inanimate{
public:
    Food(double startX, double startY, StudentWorld *sw);
    virtual ~Food();
    bool attractsBacteria();
};

class Flame : public Projectile{
public:
    Flame(double startX, double startY, Direction dir, StudentWorld* sw);
    virtual ~Flame();
};

class Pit : public Inanimate{
public:
    Pit(double startX, double startY, StudentWorld* sw);
    virtual ~Pit();
    void doSomething();
private:
    int m_nSalmonella;
    int m_nAggSalmonella;
    int m_nEColi;
};

class Spray : public Projectile{
public:
    Spray(double startX, double startY, Direction dir, StudentWorld* sw);
    virtual ~Spray();
};

class RestoreHealth : public Goodie{
public:
    RestoreHealth(double startX, double startY, int lifeTime, StudentWorld* sw);
    virtual ~RestoreHealth();
    void actionToTake();
};

class FlameThrowerGoodie : public Goodie{
public:
    FlameThrowerGoodie(double startX, double startY, int lifeTime, StudentWorld* sw);
    virtual ~FlameThrowerGoodie();
    void actionToTake();
};

class ExtraLife : public Goodie{
public:
    ExtraLife(double startX, double startY, int lifeTime, StudentWorld* sw);
    virtual ~ExtraLife();
    void actionToTake();
};

class Fungus : public Goodie{
public:
    Fungus(double startX, double startY, int lifeTime, StudentWorld* sw);
    virtual ~Fungus();
    void actionToTake();
    void playGoodieSound();
};

class RegSalmonella : public Bacteria{
public:
    RegSalmonella(double startX, double startY, StudentWorld* sw);
    void mitosis(double x, double y);
    virtual ~RegSalmonella();

};
class AggSalmonella : public Bacteria{
public:
    AggSalmonella(double startX, double startY, StudentWorld* sw);
    virtual ~AggSalmonella();
    void mitosis(double x, double y);
    bool differentiatedAttackSocrates();

};
class EColi : public Bacteria{
public:
    EColi(double startX, double startY, StudentWorld* sw);
    virtual ~EColi();
    void mitosis(double x, double y);
    void differentiatedMovement();
    void playHurtSound();
    void playKilledSound();
};


class Socrates : public Animate{
public:
    Socrates(StudentWorld *sw);
    virtual ~Socrates();
    void setExistStatus(int newExistStatus);
    void doSomething();
    void decrementSprayCharges();
    void decrementFlameThrowerCharges();
    void addFlames();
    int getNSprays();
    int getNFlames();
    bool getDamagedByFlame();
    void playHurtSound();
    void playKilledSound();
    bool isBacteria();
private:
    int m_nSprayCharges;
    int m_nFlameThrowerCharges;
    
};



#endif // ACTOR_H_
