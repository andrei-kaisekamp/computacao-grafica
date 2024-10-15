#include "Background.h"

void Background::move() 
{
	this->position.x -= globalGameSpeed;
	if (this->position.x <= 0.0)
		resetPosition();
}

void Background::resetPosition()
{
	this->position = this->spawnPoint;
}