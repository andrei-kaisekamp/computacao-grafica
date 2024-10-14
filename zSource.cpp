
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

#include "Sprite.cpp"
#include "GameEngine.cpp"

using namespace glm;

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 800, HEIGHT = 600;

float vel = 3;
float scenarioVel = 5;
int level = 0;
int points = 0;

float amplitude = 80.0f; // Amplitude do movimento (altura máxima em relação ao eixo Y)
float frequency = 3.0f;   // Frequência do movimento (quão rápido o pássaro sobe e desce)
float birdTime = 0.0f;        // Tempo que será incrementado a cada frame

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

	// Compilando e buildando o programa de shader
	GLuint backgroundShader = engine->setupShader();
	GLuint characterShader  = engine->setupShader();
	GLuint obstacleShader   = engine->setupShader();
	GLuint birdShader       = engine->setupShader();

	std::cout << "background: " << backgroundShader << std::endl;
	std::cout << "character: " << characterShader << std::endl;
	std::cout << "obstacle: " << obstacleShader << std::endl;
	std::cout << "bird: " << birdShader << std::endl;

	// Gerando um buffer simples, com a geometria de um triângulo
	// Sprite do fundo da cena
    Sprite *background = new Sprite;
    Sprite *character = new Sprite;
    Sprite *obstacle = new Sprite;
    Sprite *bird = new Sprite;

	int imgWidth, imgHeight, jumpImgWidth, jumpImgHeight;

	// inicializa imagem de pulo
	GLuint jumpTexID = engine->loadTexture("textures/character/warewolf/Jump.png", jumpImgWidth, jumpImgHeight);

	//glUseProgram(engine->shader);

	// Enviando a cor desejada (vec4) para o fragment shader
	// Utilizamos a variáveis do tipo uniform em GLSL para armazenar esse tipo de info
	// que não está nos buffers
	glUseProgram(backgroundShader);
	glUniform1i(glGetUniformLocation(backgroundShader, "texBuffer"), 0);

	glUseProgram(characterShader);
	glUniform1i(glGetUniformLocation(characterShader, "texBuffer"), 0);

	glUseProgram(birdShader);
	glUniform1i(glGetUniformLocation(birdShader, "texBuffer"), 0);

	glUseProgram(obstacleShader);
	glUniform1i(glGetUniformLocation(obstacleShader, "texBuffer"), 0);

	// Matriz de projeção ortográfica
	mat4 projection = ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);

	glUseProgram(backgroundShader);
	glUniformMatrix4fv(glGetUniformLocation(backgroundShader, "projection"), 1, GL_FALSE, value_ptr(projection));

	glUseProgram(characterShader);
	glUniformMatrix4fv(glGetUniformLocation(characterShader, "projection"), 1, GL_FALSE, value_ptr(projection));

	glUseProgram(birdShader);
	glUniformMatrix4fv(glGetUniformLocation(birdShader, "projection"), 1, GL_FALSE, value_ptr(projection));

	glUseProgram(obstacleShader);
	glUniformMatrix4fv(glGetUniformLocation(obstacleShader, "projection"), 1, GL_FALSE, value_ptr(projection));

	// Ativando o primeiro buffer de textura da OpenGL
	glActiveTexture(GL_TEXTURE0);

	// Habilitar a transparência
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Habilitar o teste de profundidade
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);

	bool isJumping = false;
	float jumpVelocity = 15.0f; // Velocidade inicial do pulo
	float gravity = 0.98f;      // Gravidade
	float groundLevel = 127.0f; // Nível do chão onde o personagem "fica de pé"

	float lastFrameTime = glfwGetTime();

	// Inicializando a sprite do background
	glUseProgram(backgroundShader);
	int texID = engine->loadTexture("textures/background/game_background.png", imgWidth, imgHeight);
	background->setupSprite(texID, vec3(800.0, 300.0, 0.0), vec3(imgWidth / 2.4, imgHeight / 1.8, 1.0), 1, 1, backgroundShader);
	objects.push_back(background); //objects[0]

	// Inicializando a sprite do obstaculo
	glUseProgram(obstacleShader);
	texID = engine->loadTexture("textures/obstacles/rock.png", imgWidth, imgHeight);
	obstacle->setupSprite(texID, vec3(900.0, 85.0, 0.0), vec3(imgWidth, imgHeight, 1.0), 1, 1, obstacleShader);
	objects.push_back(obstacle); //objects[1]

	// Inicializando a sprite do passaro
	glUseProgram(birdShader);
	texID = engine->loadTexture("textures/bird/fly.png", imgWidth, imgHeight);
	bird->setupSprite(texID, vec3(800.0, 100.0, 0.0), vec3(imgWidth / 6.0, imgHeight - 2.0, 1.0), 6, 1, birdShader);
	objects.push_back(bird); //objects[2]

	// Inicializando a sprite do personagem
	glUseProgram(characterShader);
	texID = engine->loadTexture("textures/character/warewolf/Run.png", imgWidth, imgHeight);
	character->setupSprite(texID, vec3(50.0, 127.0, 0.0), vec3(imgWidth / 9.0, imgHeight, 1.0), 9, 1, characterShader);
	objects.push_back(character); //objects[3]

	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//char animation
		vec2 charoffsetTex = vec2(0.0, 0.0);
		vec2 birdoffsetTex = vec2(0.0, 0.0);
		glUseProgram(objects[3]->shader);
		glUniform2f(glGetUniformLocation(objects[3]->shader, "offsetTex"), charoffsetTex.s, charoffsetTex.t);
		
		// move o background em loop:
		objects[0]->position.x -= scenarioVel;
		if (objects[0]->position.x <= 0.0)
			objects[0]->position.x = 800.0;

		objects[1]->position.x -= scenarioVel; 
		if (objects[1]->position.x <= 0.0) {
			objects[1]->position.x = 900.0;
			
			points += 100;

			int newLevel = points / 300; // A cada 300 pontos, o nível aumenta
        	if (newLevel > level) {
            	level = newLevel; // Atualiza o nível apenas se ele mudou
            	std::cout << "Level up! Now you are at level: " << level << std::endl;
				scenarioVel += (level / 2);
        	}
		}

		if (keys[GLFW_KEY_LEFT] || keys[GLFW_KEY_A])
			objects[3]->position.x -= vel;
		if (keys[GLFW_KEY_RIGHT] || keys[GLFW_KEY_D])
			objects[3]->position.x += vel;
		if (keys[GLFW_KEY_SPACE] && !isJumping) {
    		isJumping = true;           // Marca que o personagem está pulando
			glUseProgram(objects[3]->shader);
			objects[3]->setupSprite(jumpTexID, objects[3]->position, vec3(jumpImgWidth / 11.0, jumpImgHeight, 1.0), 11, 1, objects[3]->shader); // Configura o sprite de pulo
    		objects[3]->position.y += 1.0f; // Pequeno ajuste inicial para evitar que ele fique travado no chão
			objects[3]->FPS = 14.66;
		}

		if (isJumping) {
    		objects[3]->position.y += jumpVelocity; // Move o personagem com a velocidade de pulo
    		jumpVelocity -= gravity;              // Aplica gravidade reduzindo a velocidade de pulo

    		// Quando o personagem atinge o chão novamente
    		if (objects[3]->position.y <= groundLevel) {
        		objects[3]->position.y = groundLevel; // Garante que o personagem não vá abaixo do chão
        		isJumping = false;                  // Reseta o estado de pulo
        		jumpVelocity = 15.0f;               // Reseta a velocidade para o próximo pulo
				glUseProgram(objects[3]->shader);
				objects[3]->setupSprite(texID, objects[3]->position, vec3(imgWidth / 9.0, imgHeight, 1.0), 9, 1, objects[3]->shader);
				objects[3]->FPS = 12.0;
    		}
		}

		// Incremento circular (em loop) do índice do frame DO CHAR
		float now = glfwGetTime();
		float chardt = now - objects[3]->lastTime;
		if (chardt >= 1.0 / objects[3]->FPS)
		{
			objects[3]->iFrame = (objects[3]->iFrame + 1) % objects[3]->nFrames;
			objects[3]->lastTime = now;
		}	

		charoffsetTex.s = objects[3]->iFrame * objects[3]->d.s;
		charoffsetTex.t = 0.0;

		glUseProgram(objects[3]->shader);
		glUniform2f(glGetUniformLocation(objects[3]->shader, "offsetTex"), charoffsetTex.s, charoffsetTex.t);

		float currentFrameTime = glfwGetTime(); // Obtém o tempo atual
		float deltaTime = currentFrameTime - lastFrameTime; // Calcula o tempo entre frames
		lastFrameTime = currentFrameTime; // Atualiza o tempo do último frame
		birdTime += deltaTime; // deltaTime é o tempo entre frames
		objects[2]->position.y = 300.0f + amplitude * sin(frequency * birdTime); // Ajuste 300.0f para a altura média desejada
		
		objects[2]->position.x -= scenarioVel + 1; 
		if (objects[2]->position.x <= 0.0)
			objects[2]->position.x = 900.0;

		// Incremento circular (em loop) do índice do frame DO PASSARO
		float nowBird = glfwGetTime();
		float birddt = nowBird - objects[2]->lastTime;
		if (birddt >= 1.0 / objects[2]->FPS)
		{
			objects[2]->iFrame = (objects[2]->iFrame + 1) % objects[2]->nFrames;
			objects[2]->lastTime = now;
		}	

		birdoffsetTex.s = objects[2]->iFrame * objects[2]->d.s;
		birdoffsetTex.t = 0.0;

		glUseProgram(objects[2]->shader);
		glUniform2f(glGetUniformLocation(objects[2]->shader, "offsetTex"), birdoffsetTex.s, birdoffsetTex.t);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->draw();
		}

		// Verifica se houve uma colisão entre character e obstacle
   		if (engine->checkCollisionWithMargin(objects[3], objects[1], -20.0)) {
        	// Lógica a ser executada quando ocorre uma colisão
        	std::cout << "Colisão com pedra detectada!" << std::endl;

			glfwTerminate();
			return 0;
    	}

		if (engine->checkCollisionWithMargin(objects[3], objects[2], -20.0)) {
        	// Lógica a ser executada quando ocorre uma colisão
        	std::cout << "Colisão com pássaro detectada!" << std::endl;

			glfwTerminate();
			return 0;
    	}

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}

	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}