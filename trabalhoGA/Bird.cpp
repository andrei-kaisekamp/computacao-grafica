#include "Bird.h"

void Bird::move() 
{
	float now = glfwGetTime();
	float chardt = now - this->lastTime;

	birdTime += chardt;
	this->position.y = 300.0f + amplitude * sin(frequency * birdTime);
	
	this->position.x -= globalGameSpeed + 0.5; 
	if (this->position.x <= 0.0)
		this->position.x = 900.0;

	// Incremento circular (em loop) do índice do frame
	if (chardt >= 1.0 / this->FPS)
	{
		this->iFrame = (this->iFrame + 1) % this->nFrames;
		this->lastTime = now;
	}	

	offset.s = this->iFrame * this->d.s;
	offset.t = 0.0;

	glUseProgram(this->shader);
	glUniform2f(glGetUniformLocation(this->shader, "offsetTex"), offset.s, offset.t);
}

void Bird::resetPosition()
{
	this->position.x = this->xSpawnPoint;
}