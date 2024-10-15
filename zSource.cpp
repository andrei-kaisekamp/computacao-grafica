
#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

// STB_IMAGE
#include <C:\Users\andre\git\computacao-grafica\Dependencies\stb_image\stb_image.cpp>

static bool keys[1024] = {false};

#include "globals.cpp"
#include "Sprite.cpp"
#include "Background.cpp"
#include "Obstacle.cpp"
#include "Bird.cpp"
#include "Character.cpp"
#include "GameEngine.cpp"

using namespace glm;

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 800, HEIGHT = 600;

float vel = 3;
int level = 0;
int points = 0;

float birdTime = 0.0f;
float frequency = 3.0f;
float amplitude = 80.0f;

//float amplitude = 80.0f; // Amplitude do movimento (altura máxima em relação ao eixo Y)
//float frequency = 3.0f;   // Frequência do movimento (quão rápido o pássaro sobe e desce)
//float birdTime = 0.0f;        // Tempo que será incrementado a cada frame

vector <Sprite*> objects;

int main() 
{
	// Inicialização da GLFW
	glfwInit();

    GameEngine *engine = new GameEngine;

	// Criação da janela GLFW
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "TRABALHO GA", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, engine->key_callback);

	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		std::cout << "Failed to initialize GLAD" << std::endl;

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Ativando o primeiro buffer de textura da OpenGL
	glActiveTexture(GL_TEXTURE0);

	// Habilitar a transparência
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Habilitar o teste de profundidade
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);

	engine->createAllObjects();
	objects = engine->sprites;

	float lastFrameTime = glfwGetTime();
	vec2 birdoffsetTex = vec2(0.0, 0.0);

	// Limpa o buffer de cor
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // cor de fundo
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->move();
			objects[i]->draw();
		}

		if(engine->detectColisions())
			return 0;

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}