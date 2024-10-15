#include "Obstacle.h"

void Obstacle::move()
{
    this->position.x -= globalGameSpeed; 
	if (this->position.x <= 0.0)
		resetPosition();
        increasePoints();
}

void Obstacle::resetPosition()
{
    this->position = this->spawnPoint;
}

void Obstacle::increasePoints()
{
    //A cada obstáculo desviado com sucesso, ganha 100 pontos:
    globalPoints += 100;
}