#include "Actor.h"
#include "StudentWorld.h"
// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp


//TODO
//When you decided to use int exist instead of bool exist some class still had a bool exist parameter so fix this if you need to
//Make sure the construction direction parameters are all right
//change rand() to randInt() provided in gameconstants
//make sure you arent using getdirection when you should be finding the inverse tan between two points, anywhere
//fix the scoreboard so that it uses stringstreams to get the right amount of spaces in between stuff



using namespace std;

//Actor Abstraction Implementation
 
Actor::Actor(int imageID, double startX, double startY, int exists, StudentWorld* sw, Direction dir, int depth)
    :GraphObject(imageID, startX, startY, dir, depth){
        m_myStudentWorld = sw;
        m_exists = exists;
}

bool Actor::getDamagedByFlame(){
    return m_damagedByFlame;
}

int Actor:: getExistStatus(){
    return (bool)(m_exists);
}

void Actor:: setExistStatus(int newExistStatus){
    //to allow boolean logic in studentwrold functions
    if(newExistStatus < 0)
        newExistStatus = 0;
    m_exists = (bool)(newExistStatus);
}

StudentWorld* Actor::getStudentWorld(){
    return m_myStudentWorld;
}

bool Actor::attractsBacteria(){
    return false;
}

Actor::~Actor(){}

//Inanimate Abstraction Implementation

Inanimate::Inanimate(int imageID, double startX, double startY, int exists, StudentWorld* sw, Direction dir, int depth)
    :Actor(imageID, startX, startY, exists, sw, dir, depth){
    
}

void Inanimate::doSomething(){
    //do nothing
}

bool Inanimate::getDamagedByFlame(){
    return false;
}

bool Inanimate::isAnimate(){
    return false;
}

bool Inanimate::attractsBacteria(){
    return false;
}

Inanimate::~Inanimate(){}

//Dirt Class Implementation


Pit::Pit(double startX, double startY, StudentWorld* sw)
:Inanimate(IID_PIT, startX, startY, true, sw){
    m_nSalmonella = 5;
    m_nAggSalmonella = 3;
    m_nEColi = 2;
}

Pit:: ~Pit(){}


void Pit::doSomething(){
    if(getExistStatus() == 0)
        return;
    int totBacteria = m_nEColi + m_nAggSalmonella + m_nSalmonella;
    if(totBacteria <= 0){
        getStudentWorld()->setPitsLeft(getStudentWorld()->getPitsLeft() - 1);
        setExistStatus(false);
        return;
    }
    //does bacteria releasing probability
    int releaseChance = randInt(0, 49);
    //std::cout << releaseChance << std::endl;
    if(releaseChance == 0){
        bool released = false;
        while(!released){
            int bactChance = randInt(1, 3);
            //std::cout << "Bact release" << std::endl;
            if(bactChance == 1){
                if(m_nSalmonella>0){
                    //create bacterias here
                    getStudentWorld()->createObj(getX(), getY(), obj_REGSAL);
                    m_nSalmonella--;
                    released = true;
                }
            }
            else if(bactChance == 2){
                if(m_nAggSalmonella > 0){
                    getStudentWorld()->createObj(getX(), getY(), obj_AGGSAL);
                    m_nAggSalmonella--;
                    released = true;
                }
            }
            else{
                if(m_nEColi > 0){
                    getStudentWorld()->createObj(getX(), getY(), obj_ECOLI);
                    m_nEColi--;
                    released = true;
                }
            }
        }
    }
}


//Projectile Abstraction Implementation

Projectile::Projectile(int imageID, double startX, double startY, int exists, int travelDist, StudentWorld* sw, int dmg, Direction dir)
    :Inanimate(imageID, startX, startY, exists, sw, dir){
        m_travelDistLeft = travelDist;
        m_dmg = dmg;
}

void Projectile::decreaseTravelDist(int decr){
    m_travelDistLeft -= decr;
    if(m_travelDistLeft<=0)
        this->setExistStatus(false);
}

void Projectile::doSomething(){
    if(!getExistStatus()){
        return;
    }
    if(getStudentWorld()->deleteDamagableObjects(getX(), getY(), m_dmg)){
        setExistStatus(0);
        return;
    }
    else{
        //cout << "flame doing something" << endl;
        moveAngle(getDirection(), SPRITE_WIDTH);
        //exist is set to false inside this
        decreaseTravelDist(SPRITE_WIDTH);
    }
}

bool Projectile::getDamagedByFlame(){
    return false;
}

Projectile::~Projectile(){}

//Flame Class Implementation

Flame::Flame(double startX, double startY, Direction dir, StudentWorld *sw)
    :Projectile(IID_FLAME, startX, startY, true, TRAVEL_DIST_FLAME, sw, 5, dir){
    
}

Flame::~Flame(){
    
}

//Spray Class Implementation

Spray::Spray(double startX, double startY, Direction dir, StudentWorld* sw)
    :Projectile(IID_SPRAY, startX, startY, true, TRAVEL_DIST_SPRAY, sw, 2, dir){
    
}
Spray:: ~Spray(){
    
}

//Animate Abstraction Implementation

Animate::Animate(int imageID, double startX, double startY, int exists, StudentWorld* sw, int hp, Direction dir, int depth)
    :Actor(imageID, startX, startY, exists, sw, dir, depth){
        m_healthPoints = hp;
}

Animate:: ~Animate(){
    
}

int Animate::getExistStatus(){
    return m_healthPoints;
}

void Animate::setExistStatus(int newExistStatus){
    if(newExistStatus < m_healthPoints && newExistStatus > 0)
        playHurtSound();
    else if(newExistStatus <= 0)
        playKilledSound();
    m_healthPoints = newExistStatus;
    //So that you can use boolean logic when calling this function
    if(m_healthPoints <= 0){
        m_healthPoints = 0;
        if(isBacteria()){
            getStudentWorld()->bacteriaDied();
            getStudentWorld()->increaseScore(100);
            int random = randInt(0, 1);
            if(random)
                getStudentWorld()->createObj(getX(), getY(), obj_FOOD);
        }
    }
}

bool Animate::getDamagedByFlame(){
    return true;
}

bool Animate::isAnimate(){
    return true;
}

//Bacteria Abstraction Implementation

Bacteria::Bacteria(int imageID, double startX, double startY, int hp, StudentWorld* sw, int attackPower)
    :Animate(imageID, startX, startY, true, sw, hp){
        m_movementPlan = 0;
        m_attackPower = attackPower;
        m_foodEaten = 0;
}

Bacteria:: ~Bacteria(){}

bool Bacteria::isBacteria(){
    return true;
}


void Bacteria::doSomething(){
    if(!getExistStatus()){
        //cout << getExistStatus() << endl;
        return;
    }
    //damage socrates if overlap
    bool dontDoMvmt = differentiatedAttackSocrates();
    bool skipToMvmt = false;
    if(getStudentWorld()->checkOverlapWithSocrates(getX(), getY())){
        getStudentWorld()->damageSocrates(m_attackPower);
        skipToMvmt = true;
    }
    
    //if its agg sal then move sal toward socrates 3 pixels before checking overlap (will need differentiating var/func)
    if(!skipToMvmt){
        if(m_foodEaten >= 3){
            cout << " Eaten 3 foods" << endl;
            double newX = getX();
            double newY = getY();
            if(newX > VIEW_WIDTH/2)
                newX -= SPRITE_WIDTH/2;
            else if(newX < VIEW_WIDTH/2)
                newX += SPRITE_WIDTH;
            if(newY > VIEW_HEIGHT/2)
                newY -= SPRITE_WIDTH/2;
            else if(newY < VIEW_HEIGHT/2)
                newY += SPRITE_WIDTH/2;
            mitosis(newX, newY);
            m_foodEaten = 0;
            skipToMvmt = true;
        }
    }
    if(!skipToMvmt){
        bool ate = getStudentWorld()->eatFoodIfThere(getX(), getY());
        if(ate)
            m_foodEaten++;
    }
    if(!dontDoMvmt){
        differentiatedMovement();
    }
}
bool Bacteria::attempToMove(Direction dir, int distToMove, bool checkForBoundary){
    double potentialX, potentialY;
    getPositionInThisDirection(dir, distToMove, potentialX, potentialY);
    setDirection(dir);
    //because some child classes of bacteria dont require you to check for boundary
    if(checkForBoundary){
        if(!getStudentWorld()->overlapWithDirt(potentialX, potentialY) && !getStudentWorld()->notWithinCircle(potentialX, potentialY, VIEW_RADIUS)){
            moveAngle(getDirection(), distToMove);
            return true;
        }
    }
    else{
        if(!getStudentWorld()->overlapWithDirt(potentialX, potentialY)){
            
            moveAngle(getDirection(), distToMove);
            return true;
        }
    }
    return false;
}

void Bacteria::differentiatedMovement(){
    //step 1
    if(m_movementPlan > 0){
        m_movementPlan--;
        if(attempToMove(getDirection(), 3, true)){
            return;
        }
        //else{goes to the bottom and does the randomize and return thing}
    }
    //step 2
    else{
        int closestDir = getStudentWorld()->getDirToClosestFood(getX(), getY());
        if(closestDir != -1){
            if(attempToMove(closestDir, 3, false))
                return;
            //else goes to the bottom and does the randomize and return things
        }
    }
    setDirection(randInt(0, 359));
    m_movementPlan = 10;
}

void Bacteria::damageSocrates(){

}



bool Bacteria::differentiatedAttackSocrates(){
    return false;
}

void Bacteria::playHurtSound(){
    getStudentWorld()->playSound(SOUND_SALMONELLA_HURT);
}



void Bacteria::playKilledSound(){
    getStudentWorld()->playSound(SOUND_SALMONELLA_DIE);
}

//REgSal
RegSalmonella::RegSalmonella(double startX, double startY, StudentWorld* sw)
:Bacteria(IID_SALMONELLA, startX, startY, 4, sw, 1){
    
}

void RegSalmonella::mitosis(double x, double y){
    getStudentWorld()->createObj(x, y, obj_REGSAL);
}

RegSalmonella:: ~RegSalmonella(){
    
}



//AggSal
AggSalmonella::AggSalmonella(double startX, double startY, StudentWorld* sw)
:Bacteria(IID_SALMONELLA, startX, startY, 10, sw, 2){
    
}

void AggSalmonella::mitosis(double x, double y){
    getStudentWorld()->createObj(x, y, obj_AGGSAL);
}

bool AggSalmonella::differentiatedAttackSocrates(){
    int turnDir = 72;
    if(!getStudentWorld()->checkIfSocratesNearby(getX(), getY(), turnDir))
        return false;
    attempToMove(turnDir, 3, false);
    
    return true;
    
}

AggSalmonella:: ~AggSalmonella(){
    
}

//EColi

EColi::EColi(double startX, double startY, StudentWorld* sw)
:Bacteria(IID_ECOLI, startX, startY, 5, sw, 4){
    
}
void EColi::mitosis(double x, double y){
    getStudentWorld()->createObj(x, y, obj_ECOLI);
}

void EColi::differentiatedMovement(){
    int distMax = 256;
    if(!getStudentWorld()->checkIfSocratesNearby(getX(), getY(), distMax))
        std::cout << "Something is very wrong: check differentiatedMovement func in ecoli" << std::endl;
    for(int i = 0; i < 10; i++){
        if(attempToMove(distMax, 2, false))
            return;
        else{
            distMax = (distMax + 10) % 359;
        }
    }
}

void EColi::playHurtSound(){
    getStudentWorld()->playSound(SOUND_ECOLI_HURT);
}

void EColi::playKilledSound(){
    getStudentWorld()->playSound(SOUND_ECOLI_DIE);
}

EColi:: ~EColi(){
}

//Dirt Class Implementation

Dirt::Dirt(double startX, double startY, StudentWorld* sw)
    :Inanimate(IID_DIRT, startX, startY, true, sw){
        
}

bool Dirt::getDamagedByFlame(){
    return true;
}

Dirt:: ~Dirt(){
    
}

//Food Class Implementation
Food::Food(double startX, double startY, StudentWorld* sw)
    :Inanimate(IID_FOOD, startX, startY, true, sw){
    
}

Food:: ~Food(){
    
}


bool Food::attractsBacteria(){
    return true;
}

//Goodie Abstraction Implementation

Goodie::Goodie(int imageId, double startX, double startY, StudentWorld *sw, int lifeTime)
    : Inanimate(imageId, startX, startY, true, sw){
        m_lifeTimeLeft = lifeTime;
        
}

void Goodie::doSomething(){
    if(!getExistStatus())
        return;
    if(getStudentWorld()->checkOverlapWithSocrates(getX(), getY())){
        playGoodieSound();
        actionToTake();
        setExistStatus(0);
        return;
    }
    if(m_lifeTimeLeft < 0)
        setExistStatus(0);
    m_lifeTimeLeft--;
}

void Goodie::playGoodieSound(){
    getStudentWorld()->playSound(SOUND_GOT_GOODIE);
}

Goodie:: ~Goodie(){}

//Restore Health Class Implementation

RestoreHealth::RestoreHealth(double startX, double startY, int lifeTime, StudentWorld* sw)
    :Goodie(IID_RESTORE_HEALTH_GOODIE, startX, startY, sw, lifeTime){
    
}

RestoreHealth::~RestoreHealth(){}

void RestoreHealth::actionToTake(){
    getStudentWorld()->increaseScore(250);
    getStudentWorld()->restoreHealth();
}

//FlameThrowerGoodie Class Implementation

FlameThrowerGoodie::FlameThrowerGoodie(double startX, double startY, int lifeTime, StudentWorld* sw)
:Goodie(IID_FLAME_THROWER_GOODIE, startX, startY, sw, lifeTime){
    
}
FlameThrowerGoodie:: ~FlameThrowerGoodie(){}

void FlameThrowerGoodie::actionToTake(){
    getStudentWorld()->increaseScore(300);
    getStudentWorld()->addFlameThrowers();
}

//Extra Life class implementation

ExtraLife::ExtraLife(double startX, double startY, int lifeTime, StudentWorld* sw)
    :Goodie(IID_EXTRA_LIFE_GOODIE, startX, startY, sw, lifeTime){
    
}

ExtraLife:: ~ExtraLife(){}

void ExtraLife::actionToTake(){
    getStudentWorld()->increaseScore(500);
    getStudentWorld()->incLives();
}

//Fungus Class implementation

Fungus::Fungus(double startX, double startY, int lifeTime, StudentWorld* sw)
    :Goodie(IID_FUNGUS, startX, startY, sw, lifeTime){
    
}

void Fungus::actionToTake(){
    getStudentWorld()->increaseScore(-50);
    getStudentWorld()->damageSocrates(20);
}

//We dont want to play a sound for fungus
void Fungus::playGoodieSound(){}

Fungus::~Fungus(){}


//Socrates class Implementation

Socrates::Socrates(StudentWorld* sw)
    :Animate(IID_PLAYER, 0, 128, true, sw, SOCRATES_STARTINGHP, 0, 0){
    m_nSprayCharges = MAX_SPRAY;
    m_nFlameThrowerCharges = START_FLAME;
}

Socrates:: ~Socrates(){
    
}
void Socrates::decrementSprayCharges(){
    if(m_nSprayCharges>0)
        m_nSprayCharges--;
}
void Socrates::decrementFlameThrowerCharges(){
    if(m_nFlameThrowerCharges)
        m_nFlameThrowerCharges--;
}

bool Socrates::getDamagedByFlame(){
    return false;
}

void Socrates::addFlames(){
    m_nFlameThrowerCharges += 5;
}

int Socrates::getNFlames(){
    return m_nFlameThrowerCharges;
}

int Socrates::getNSprays(){
    return m_nSprayCharges;
}

void Socrates::playHurtSound(){
    getStudentWorld()->playSound(SOUND_PLAYER_HURT);
}

void Socrates::playKilledSound(){
    getStudentWorld()->playSound(SOUND_PLAYER_DIE);
}

bool Socrates::isBacteria(){
    return false;
}

void Socrates::setExistStatus(int newExistStatus){
    if(newExistStatus <= 0)
        playKilledSound();
    else if(newExistStatus < getExistStatus())
        playHurtSound();
    Animate::setExistStatus(newExistStatus);
    
}

/*KEY_PRESS_LEFT
KEY_PRESS_RIGHT
KEY_PRESS_UP
KEY_PRESS_DOWN
KEY_PRESS_SPACE
KEY_PRESS_TAB
KEY_PRESS_ENTER*/

void Socrates::doSomething(){
    int keyPressed;
    if(getStudentWorld()->getKey(keyPressed)){
        int directionMultiplier = 1;
        switch (keyPressed) {
            case KEY_PRESS_SPACE:
                if(m_nSprayCharges>0){
                    getStudentWorld()->shootSpray();
                    decrementSprayCharges();
                    getStudentWorld()->playSound(SOUND_PLAYER_SPRAY);
                }
                //add a spray object in front of him somehow
                break;
            case KEY_PRESS_ENTER:{
                //CHANGE TO CORRECT VALUE
                if(m_nFlameThrowerCharges>0){
                    getStudentWorld()->createRingOfFire();
                    //do the 16 flame thing
                    decrementFlameThrowerCharges();
                    getStudentWorld()->playSound(SOUND_PLAYER_FIRE);
                }
            }
                break;
            //clockwise
            case KEY_PRESS_RIGHT:
                directionMultiplier = -1;
            //counterclockwise
            case KEY_PRESS_LEFT:{
                //check if the r constant is right by looping it 18 times and seeing if it matches the top of the circle coords
                setDirection(getDirection()+5*directionMultiplier);
                double newX = (VIEW_WIDTH/2) + (cos((2*M_PI*getDirection()/360.0)+M_PI)*VIEW_RADIUS);
                double newY = (VIEW_HEIGHT/2) + (sin((2*M_PI*getDirection()/360.0)+M_PI)*VIEW_RADIUS);
                moveTo(newX, newY);
                
            }
                break;
                
            default:
                break;
        }
    }
    else{
        if(m_nSprayCharges < MAX_SPRAY)
            m_nSprayCharges++;
    }
    
}
