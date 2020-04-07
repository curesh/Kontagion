#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <iostream> // defines the overloads of the << operator
#include <sstream>  // defines the type std::ostringstream
#include <iomanip>  // defines the manipulator setw

#include "Actor.h"
using namespace std;



GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{


}

StudentWorld::~StudentWorld(){
    cleanUp();
}

int StudentWorld::init()
{
    //intialization stuff
    m_player = new Socrates(this);
    
    int numFoods = min(5*getLevel(), 25);
    int numDirts = max(180 - 20*getLevel(), 20);
    placeObjRand(getLevel(), obj_PIT);
    placeObjRand(numFoods, obj_FOOD);
    placeObjRand(numDirts, obj_DIRT);
    m_pitsDone = getLevel();
    m_bacteriaLeft = 0;
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    ostringstream oss;
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    
    //call doSomethings
    for(int i = 0; i < m_actors.size(); i++){
        if(m_actors[i]->getExistStatus()){
            m_actors[i]->doSomething();
        }
        if(getNumBacteriaLeft() == 0 && getPitsLeft() == 0){
            playSound(SOUND_FINISHED_LEVEL);
            return GWSTATUS_FINISHED_LEVEL;
        }
        if(!m_player->getExistStatus()){
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
    }
    
    //deletes dead objs
    for(int i = 0; i < m_actors.size(); i++){
        if(!m_actors[i]->getExistStatus()){
            delete m_actors[i];
            m_actors[i] = m_actors[m_actors.size()-1];
            m_actors.pop_back();
            i--;
        }
    }
    
    m_player->doSomething();
    //sets up goodie spawning probabilities
    int chanceFungus = max(510-getLevel()*10, 200);
    int chanceGoodie = max(510-getLevel()*10, 250);
    int randFungus = randInt(0, chanceFungus-1);
    int randAngle = randInt(0, 359);
    double randXGoodie = VIEW_WIDTH/2;
    double randYGoodie = VIEW_HEIGHT/2;
    int lifeTime = max(randInt(0,(300-10 * getLevel()-1)), 50);
    if(randFungus <= 0){
        getCoordsForLocalCircRelativeToOrigin(randXGoodie, randYGoodie, randAngle, VIEW_RADIUS);
        m_actors.push_back(new Fungus(randXGoodie, randYGoodie, lifeTime, this));
    }
    int randomGoodie = randInt(0, chanceGoodie-1);
    if(randomGoodie == 0){
        randAngle = randInt(0, 359);
        lifeTime = max(randInt(0, (300-10 * getLevel() - 1)), 50);
        int randGoodie = randInt(0, 9);
        getCoordsForLocalCircRelativeToOrigin(randXGoodie, randYGoodie, randAngle, VIEW_RADIUS);
        if(randGoodie <= 0)
            m_actors.push_back(new ExtraLife(randXGoodie, randYGoodie, lifeTime, this));
        else if(randGoodie <= 3)
            m_actors.push_back(new FlameThrowerGoodie(randXGoodie, randYGoodie, lifeTime, this));
        else
            m_actors.push_back(new RestoreHealth(randXGoodie, randYGoodie, lifeTime, this));
    }
    
    //sets up scoreboard
    oss.fill('0');
    oss << setw(6) << getScore();
    
    
    string boardText = "Score: " + oss.str() + "  Level: " + to_string(getLevel()) + "  Lives: " + to_string(getLives()) + "  Health: ";
    boardText += to_string(m_player->getExistStatus()) + "  Sprays: " + to_string(m_player->getNSprays()) + "  Flames: " + to_string(m_player->getNFlames());
    
    
    /* Score: 004500 Level: 4 Lives: 3 health: 82 Sprays: 16 Flames: 4 */
    setGameStatText(boardText);
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete m_player;
    m_player = nullptr;
    for(long i = m_actors.size()-1; i >= 0; i--){
        delete m_actors[i];
        m_actors.pop_back();
    }
    
}

int StudentWorld::getNumBacteriaLeft(){
    return m_bacteriaLeft;
}
//returns true if there is overlap
bool StudentWorld::checkOverlap(double x1, double x2, double y1, double y2, int radius){
    double dist = sqrt(pow(x2-x1,2) + pow(y2-y1,2));
    return dist <= radius;
}


//returns true if objects were damaged
bool StudentWorld::deleteDamagableObjects(double testX, double testY, int dmg){
    
    for(int i = 0; i < m_actors.size(); i++){
        if(checkOverlap(m_actors[i]->getX(), testX, m_actors[i]->getY(), testY, SPRITE_WIDTH)){
            if(m_actors[i]->getDamagedByFlame() && m_actors[i]->getExistStatus()){
                //deal 5 damage to everyone
                int curr = m_actors[i]->getExistStatus();
                m_actors[i]->setExistStatus(curr-dmg);
                return true;
            }
        }
    }
    return false;
}

void StudentWorld::createRingOfFire(){
    for(int i = 0; i < 16; i++){
        Direction startInt = m_player->getDirection() + i*22;
        double start = startInt % 360;
        double startX, startY;
        //startX = m_player->getX() + cos(2.0*M_PI*start/360.0) * 2*SPRITE_RADIUS;
        //startY = m_player->getY() + sin(2*M_PI*start/360.0) * 2*SPRITE_RADIUS;
        startX = m_player->getX();
        startY = m_player->getY();
        getCoordsForLocalCircRelativeToOrigin(startX, startY, start, SPRITE_WIDTH);
        m_actors.push_back(new Flame(startX, startY, start, this));
    }
        //cout << m_actors[m_actors.size()-1-i]->getDirection() << endl;
}

void StudentWorld::shootSpray(){
    double startX, startY;
    m_player->getPositionInThisDirection(m_player->getDirection(), SPRITE_WIDTH, startX, startY);
    m_actors.push_back(new Spray(startX, startY, m_player->getDirection(), this));
}

bool StudentWorld::checkOverlapWithSocrates(double x, double y){
    return checkOverlap(x, m_player->getX(), y, m_player->getY(), SPRITE_WIDTH);
}

void StudentWorld::restoreHealth(){
    m_player->setExistStatus(100);
}

void StudentWorld::damageSocrates(int amt){
    m_player->setExistStatus(m_player->getExistStatus() - amt);
}

void StudentWorld::addFlameThrowers(){
    m_player->addFlames();
}

void StudentWorld::setPitsLeft(int set){
    m_pitsDone = set;
}

int StudentWorld::getPitsLeft(){
    return m_pitsDone;
}

bool StudentWorld::eatFoodIfThere(double bactLocX, double bactLocY){
    for(int i = 0; i < m_actors.size(); i++){
        //food fullfills these categories
        if(m_actors[i]->attractsBacteria()){
            if(checkOverlap(bactLocX, m_actors[i]->getX(), bactLocY, m_actors[i]->getY(), SPRITE_WIDTH)){
                m_actors[i]->setExistStatus(false);
                return true;
            }
        }
    }
    return false;
}
bool StudentWorld::overlapWithDirt(double bactLocX, double bactLocY){
    for(int i = 0; i < m_actors.size(); i++){
        if(m_actors[i]->getDamagedByFlame() && !m_actors[i]->isAnimate()){
            //check overlap with dirt so different radius apparently
            if(checkOverlap(bactLocX, m_actors[i]->getX(), bactLocY, m_actors[i]->getY(), SPRITE_WIDTH/2)){
                return true;
            }
        }
    }
    return false;
}

bool StudentWorld::notWithinCircle(double newX, double newY, int rad){
    //adjusts the points so that it is centered around a circle of given radius at the center of the game and does necessary calculations based off of this
    double centeredX = newX - VIEW_WIDTH/2;
    double centeredY = newY - VIEW_HEIGHT/2;
    double r = centeredX*centeredX + centeredY*centeredY;
    r = sqrt(r);
    return r >= rad;
}

int StudentWorld::getDistBetween(double x1, double x2, double y1, double y2){
    return sqrt(pow(x2-x1,2) + pow(y2-y1,2));
}

int StudentWorld::getDirToClosestFood(double bactLocX, double bactLocY){
    
    int closestDir = -1;
    for(int i = 0; i < m_actors.size(); i++){
        if(m_actors[i]->attractsBacteria()){
            if(getDistBetween(bactLocX, m_actors[i]->getX(), bactLocY, m_actors[i]->getY()) > 128)
                continue;
            int tempDir = getDirBetween( bactLocX, m_actors[i]->getX(),  bactLocY, m_actors[i]->getY());
            if(closestDir == -1 || closestDir > tempDir)
                closestDir = tempDir;
        }
    }
    return closestDir;
}

bool StudentWorld::checkIfSocratesNearby(double x, double y, int& distMax){
    int temp = distMax;
    distMax = getDirBetween( x, m_player->getX(), y, m_player->getY());
    return getDistBetween(x, m_player->getX(), y, m_player->getY()) <= temp;
}

void StudentWorld::bacteriaDied(){
    m_bacteriaLeft--;
}

//Private functions

//uses trigonometry to find correct direction pointing to x2,y2.
int StudentWorld::getDirBetween(double x1, double x2, double y1, double y2){
    //relative to the player so x2,y2 is bacteria (player is reference point)
    int dir = 360.0*atan(abs((y2-y1)/(x2-x1)))/(2.0*M_PI);
    if(x2 - x1 < 0){
        //quad 3
        if(y2 - y1 < 0){
            return 180 + dir;
        }
        //quad 2
        else{
            return 180 - dir;
        }
    }
    else{
        //quad 4
        if(y2 - y1 < 0){
            return 360 - dir;
        }
        //quad 1
        //do nothing
    }
    return dir;
}

bool StudentWorld::overlapWithOtherObjects(double testX, double testY){
    if(checkOverlapWithSocrates(testX, testY))
        return true;
    for(int i = 0; i < m_actors.size(); i++){
        if(!m_actors[i]->getDamagedByFlame() || m_actors[i]->isAnimate()){
            if(checkOverlap(testX, m_actors[i]->getX(), testY, m_actors[i]->getY(), SPRITE_WIDTH)){
                return true;
            }
        }
    }
    return false;
}



void StudentWorld::placeObjRand(int numObj, int objType){
    double randX, randY;
    for(int i = 0; i < numObj; i++){
        randX = randInt(0, VIEW_WIDTH);
        randY = randInt(0, VIEW_HEIGHT);
        if(notWithinCircle(randX, randY, 120) || overlapWithOtherObjects(randX, randY)){
            i--;
            continue;
        }
        
        createObj(randX, randY, objType);
    }
}

//NOTE: This does not violate the spec since it isn't determining the type of any object using image ID. It is merely creating a new object of a desired type which is allowed.
void StudentWorld::createObj(int x, int y, int objType){
    switch (objType) {
        case obj_REGSAL:
            m_bacteriaLeft++;
            m_actors.push_back(new RegSalmonella(x, y, this));
            break;
        case obj_AGGSAL:
            m_bacteriaLeft++;
            m_actors.push_back(new AggSalmonella(x, y, this));
            break;
        case obj_ECOLI:
            m_bacteriaLeft++;
            m_actors.push_back(new EColi(x, y, this));
            break;
        case obj_PIT:
            m_actors.push_back(new Pit(x, y, this));
            break;
        case obj_DIRT:
            m_actors.push_back(new Dirt(x, y, this));
            break;
        case obj_FOOD:
            m_actors.push_back(new Food(x, y, this));
            break;
        default:
            cerr << "something wrong in createObj" << endl;
            break;
    }
}
//this function is used a lot since you often need to get information for the local points that are the radial distance away from another point. Basically it does some trigonemtry and returns points in a circle centered at any point you want (localX, localY)
void StudentWorld::getCoordsForLocalCircRelativeToOrigin(double& localX, double& localY, double localAngleDeg, double radius){
    localX = localX + cos(2.0*M_PI*localAngleDeg/360.0) *radius;
    localY = localY + sin(2.0*M_PI*localAngleDeg/360.0) *radius;
}
