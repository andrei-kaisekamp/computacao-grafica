
//Class implementations:
#include "globals.cpp"
#include "Sprite.cpp"
#include "Background.cpp"
#include "Obstacle.cpp"
#include "Bird.cpp"
#include "Character.cpp"
#include "GameEngine.cpp"

using namespace std;
using namespace glm;


int main() 
{
	GameEngine *engine = new GameEngine;
	engine->initialize();
	engine->run();
	return 0;
}