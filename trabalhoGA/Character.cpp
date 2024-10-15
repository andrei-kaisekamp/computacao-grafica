#include "Character.h"

void Character::move() 
{
    if (keys[GLFW_KEY_LEFT] || keys[GLFW_KEY_A])
		this->position.x -= speed;
	if (keys[GLFW_KEY_RIGHT] || keys[GLFW_KEY_D])
		this->position.x += speed;
	if (keys[GLFW_KEY_SPACE] && !this->isJumping) {
    	this->isJumping = true;
		setupJump();
	}

	if (isJumping) 
    	jump();

	// Incremento circular (em loop) do índice do frame DO CHAR
	float now = glfwGetTime();
	float chardt = now - this->lastTime;
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

void Character::setupJump()
{
    this->position.y += 1.0f;
	this->FPS = 14.66;

    glUseProgram(this->shader);
	this->setupSprite(this->jumpTexture, this->position, vec3(this->jumpimgWidth / 11.0, this->jumpimgHeight, 1.0), 11, 1, this->shader); // Configura o sprite de pulo
	glUniform2f(glGetUniformLocation(this->shader, "offsetTex"), offset.s, offset.t);
}

void Character::jump()
{
    this->position.y += jumpVelocity; 
    jumpVelocity -= globalGravity;

    // Quando o personagem atinge o chão novamente
    if (this->position.y <= globalGroundLevel) {
        this->position.y = globalGroundLevel;
        isJumping = false;                  
        jumpVelocity = 15.0f;               
		glUseProgram(this->shader);
		this->setupSprite(this->runTexture, this->position, vec3(this->imgWidth / 9.0, this->imgHeight, 1.0), 9, 1, this->shader);
		this->FPS = 12.0;
    }
}