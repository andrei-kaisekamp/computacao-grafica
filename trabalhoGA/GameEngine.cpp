#include "GameEngine.h"

//static controllers for mouse and keyboard
static bool resized;
static GLuint width, height;

void GameEngine::initialize() 
{
	// Inicialização da GLFW
	glfwInit();

	// Criação da janela GLFW
	window = glfwCreateWindow(WIDTH, HEIGHT, "TRABALHO GA", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);

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

	// Limpa o buffer de cor
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // cor de fundo
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	createAllObjects();
	std::cout << "INICIANDO JOGO!  Level: 0    Speed: " << globalGameSpeed << std::endl;
}

void GameEngine::run()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		for (int i = 0; i < sprites.size(); i++)
		{
			sprites[i]->move();
			sprites[i]->draw();
		}

		if(detectColisions())
			return;

		if(levelCompleted())
			levelUp();

		glfwSwapBuffers(window);
	}
	glfwTerminate();
}

int GameEngine::loadTexture(string filePath, int &imgWidth, int &imgHeight)
{
	GLuint texID;

	// Gera o identificador da textura na memória
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	// Configurando o wrapping da textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Configurando o filtering de minificação e magnificação da textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Carregamento da imagem da textura
	int nrChannels;
	unsigned char *data = stbi_load(filePath.c_str(), &imgWidth, &imgHeight, &nrChannels, 0);

	if (data)
	{
		if (nrChannels == 3) // jpg, bmp
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else // png
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << filePath << std::endl;
	}

	return texID;
}

int GameEngine::setupShader() {
	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// Checando erros de compilação (exibição via log no terminal)
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
				  << infoLog << std::endl;
	}
	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// Checando erros de compilação (exibição via log no terminal)
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
				  << infoLog << std::endl;
	}
	// Linkando os shaders e criando o identificador do programa de shader
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Checando por erros de linkagem
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
				  << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

void GameEngine::key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (action == GLFW_PRESS)
	{
		keys[key] = true;
	}
	if (action == GLFW_RELEASE)
	{
		keys[key] = false;
	}
}

bool GameEngine::checkCollisionWithMargin(Sprite* one, Sprite* two, float margin = 0.0f) {
    // Obtenha os pontos mínimo e máximo com a margem
    vec3 oneMin = one->getPMin() - vec3(margin, margin, 0);
    vec3 oneMax = one->getPMax() + vec3(margin, margin, 0);
    vec3 twoMin = two->getPMin() - vec3(margin, margin, 0);
    vec3 twoMax = two->getPMax() + vec3(margin, margin, 0);

    // Verificação de colisão no eixo x
    bool collisionX = oneMax.x >= twoMin.x && twoMax.x >= oneMin.x;

    // Verificação de colisão no eixo y
    bool collisionY = oneMax.y >= twoMin.y && twoMax.y >= oneMin.y;

    // Retorna verdadeiro se houver colisão em ambos os eixos
    return collisionX && collisionY;
}

void GameEngine::createBackground()
{
	int imgWidth, imgHeight;
	GLuint backgroundShader = setupShader();
	Background *background = new Background;

	glUseProgram(backgroundShader);
	glUniform1i(glGetUniformLocation(backgroundShader, "texBuffer"), 0);
	glUniformMatrix4fv(glGetUniformLocation(backgroundShader, "projection"), 1, GL_FALSE, value_ptr(projection));

	int texture = loadTexture("textures/background/game_background.png", imgWidth, imgHeight);
	background->setupSprite(texture, vec3(800.0, 300.0, 0.0), vec3(imgWidth / 2.4, imgHeight / 1.8, 1.0), 1, 1, backgroundShader);
	this->sprites.push_back(background); //sprites[0]
}

void GameEngine::createCharacter()
{
	int imgWidth, imgHeight;
	GLuint characterShader = setupShader();
	Character *spr = new Character;

	glUseProgram(characterShader);
	glUniform1i(glGetUniformLocation(characterShader, "texBuffer"), 0);
	glUniformMatrix4fv(glGetUniformLocation(characterShader, "projection"), 1, GL_FALSE, value_ptr(projection));

	int texture = loadTexture("textures/character/warewolf/Jump.png", imgWidth, imgHeight);
	spr->jumpTexture = texture;
	spr->jumpimgWidth = imgWidth;
	spr->jumpimgHeight = imgHeight;

	texture = loadTexture("textures/character/warewolf/Run.png", imgWidth, imgHeight);
	spr->runTexture = texture;
	spr->imgWidth = imgWidth;
	spr->imgHeight = imgHeight;

	spr->setupSprite(texture, vec3(50.0, 127.0, 0.0), vec3(imgWidth / 9.0, imgHeight, 1.0), 9, 1, characterShader);
	this->sprites.push_back(spr); //sprites[0]
}

void GameEngine::createBird()
{
	int imgWidth, imgHeight;
	GLuint birdShader = setupShader();
	Bird *bird = new Bird;

	glUseProgram(birdShader);
	glUniform1i(glGetUniformLocation(birdShader, "texBuffer"), 0);
	glUniformMatrix4fv(glGetUniformLocation(birdShader, "projection"), 1, GL_FALSE, value_ptr(projection));

	int texture = loadTexture("textures/bird/fly.png", imgWidth, imgHeight);
	bird->setupSprite(texture, vec3(800.0, 100.0, 0.0), vec3(imgWidth / 6.0, imgHeight - 2.0, 1.0), 6, 1, birdShader);
	this->sprites.push_back(bird); //sprites[2]
}

void GameEngine::createObstacle()
{
	int imgWidth, imgHeight;
	GLuint obstacleShader = setupShader();
	Obstacle *obstacle = new Obstacle;

	glUseProgram(obstacleShader);
	glUniform1i(glGetUniformLocation(obstacleShader, "texBuffer"), 0);
	glUniformMatrix4fv(glGetUniformLocation(obstacleShader, "projection"), 1, GL_FALSE, value_ptr(projection));

	int texture = loadTexture("textures/obstacles/rock.png", imgWidth, imgHeight);
	obstacle->setupSprite(texture, vec3(900.0, 85.0, 0.0), vec3(imgWidth, imgHeight, 1.0), 1, 1, obstacleShader);
	this->sprites.push_back(obstacle); //sprites[2]
}

void GameEngine::createAllObjects()
{
	createBackground();
	createObstacle();
	createBird();
	createCharacter();
}

bool GameEngine::detectColisions()
{
   	if (checkCollisionWithMargin(this->sprites[3], this->sprites[1], globalTolerance) 
		|| checkCollisionWithMargin(this->sprites[3], this->sprites[2], globalTolerance)) {
    	
		std::cout << "GAME OVER! Colisão detectada!" << std::endl;
		std::cout << "FINAL SCORE: " << globalPoints << " FINAL LEVEL: " << globalLevel << std::endl;
		glfwTerminate();
		return true;
    }
	return false;
}

bool GameEngine::levelCompleted()
{
	int newLevel = globalPoints / 300;
	if(newLevel > globalLevel) {
		return true;
	}
	return false;
}

void GameEngine::levelUp()
{
	globalLevel += 1;
	globalGameSpeed += 1;
	std::cout << "LEVEL UP!         Level: " << globalLevel << std::endl;
	std::cout << "SPEED UP!         Speed: " << globalGameSpeed << std::endl;
};