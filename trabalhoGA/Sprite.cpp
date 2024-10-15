#include "Sprite.h"

Sprite::Sprite()
{
}

void Sprite::setupSprite(int texID, vec3 position, vec3 dimensions, int nFrames, int nAnimations, GLuint shader)
{
    this->texture = texID;
	this->dimensions = dimensions;
	this->position = position;
	this->nAnimations = nAnimations;
	this->nFrames = nFrames;
	this->shader = shader;
	iAnimation = 0;
	iFrame = 0;

	glUseProgram(this->shader);

	d.s = 1.0 / (float)nFrames;
	d.t = 1.0 / (float)nAnimations;
	
	GLfloat vertices[] = {
		-0.5, -0.5, 0.0, 0.0, 0.0, // V0
		-0.5, 0.5, 0.0, 0.0, d.t,  // V1
		0.5, -0.5, 0.0, d.s, 0.0,  // V2
		0.5, 0.5, 0.0, d.s, d.t	   // V3
	};

	GLuint VBO, VAO;
	// Geração do identificador do VBO
	glGenBuffers(1, &VBO);
	// Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	// Atributo 1 - Coordenadas de textura - s, t
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);
	this->VAO = VAO;
	this->FPS = 12.0;
	this->lastTime = 0.0;
}

vec3 Sprite::getPMin() {
	return vec3(position.x - (dimensions.x / 2), position.y - (dimensions.y / 2), position.z);
}

vec3 Sprite::getPMax() {
    return vec3(position.x + (dimensions.x / 2), position.y + (dimensions.y / 2), position.z);
}

void Sprite::draw()
{	
	glUseProgram(this->shader);
	glBindVertexArray(this->VAO); // Conectando ao buffer de geometria
	glBindTexture(GL_TEXTURE_2D, this->texture); // conectando o buffer de textura

	// Matriz de modelo - Tranformações na geometria, nos objetos
	mat4 model = mat4(1); // matriz identidade
	// Translação
	model = translate(model, this->position);
	// Rotação
	model = rotate(model, radians(this->angle), vec3(0.0, 0.0, 1.0));
	// Escala
	model = scale(model, this->dimensions);
	// Enviar para o shader
	glUniformMatrix4fv(glGetUniformLocation(this->shader, "model"), 1, GL_FALSE, value_ptr(model));

	// Chamada de desenho - drawcall
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0); // Desconectando o buffer de geometria
}

void Sprite::move()
{
	
}