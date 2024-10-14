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

// STB_IMAGE
#include <C:\Users\andre\git\computacao-grafica\Dependencies\stb_image\stb_image.cpp>

using namespace glm;

struct SpriteStruct
{
	GLuint VAO;
	GLuint texID;
	vec3 position;
	vec3 dimensions;
	float angle;
	// Para controle da animação
	int nAnimations, nFrames;
	int iAnimation, iFrame;
	vec2 d;
	float FPS;
	float lastTime;

	// Função de inicialização
	void setupSprite(int texID, vec3 position, vec3 dimensions, int nFrames, int nAnimations);

	// Declaração das funções getPMax() e getPMin()
    vec3 getPMin(); // Retorna o ponto inferior esquerdo
    vec3 getPMax(); // Retorna o ponto superior direito
};

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

// Protótipos das funções
int setupShader();
int loadTexture(string filePath, int &imgWidth, int &imgHeight);
void drawSprite(SpriteStruct spr, GLuint shaderID);

bool checkCollisionWithMargin(SpriteStruct &one, SpriteStruct &two, float margin);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 800, HEIGHT = 600;

// Código fonte do Vertex Shader (em GLSL): ainda hardcoded
const GLchar *vertexShaderSource = 
	"#version 400\n"
	"layout (location = 0) in vec3 position;\n"
	"layout (location = 1) in vec2 texc;\n"
	"uniform mat4 projection;\n"
	"uniform mat4 model;\n"
	"out vec2 texCoord;\n"
	"void main()\n"
	"{\n"
	"gl_Position = projection * model * vec4(position.x, position.y, position.z, 1.0);\n"
	"texCoord = vec2(texc.s, 1.0 - texc.t);\n"
	"}\0";

// Códifo fonte do Fragment Shader (em GLSL): ainda hardcoded
const GLchar *fragmentShaderSource = 
	"#version 400\n"
	"in vec2 texCoord;\n"
	"uniform sampler2D texBuffer;\n"
	"uniform vec2 offsetTex;\n"
	"out vec4 color;\n"
	"void main()\n"
	"{\n"
	"color = texture(texBuffer, texCoord + offsetTex);\n"
	"}\n\0";

float vel = 3;
float scenarioVel = 5;
int level = 0;
bool keys[1024] = {false};
int points = 0;

float amplitude = 80.0f; // Amplitude do movimento (altura máxima em relação ao eixo Y)
float frequency = 3.0f;   // Frequência do movimento (quão rápido o pássaro sobe e desce)
float birdTime = 0.0f;        // Tempo que será incrementado a cada frame

int main() 
{
	// Inicialização da GLFW
	glfwInit();

	// Criação da janela GLFW
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "TRABALHO GA", nullptr, nullptr);
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

	// Compilando e buildando o programa de shader
	GLuint shaderID = setupShader();

	// Gerando um buffer simples, com a geometria de um triângulo
	// Sprite do fundo da cena
	SpriteStruct background, character, obstacle, bird;
	int imgWidth, imgHeight, jumpImgWidth, jumpImgHeight;

	SpriteStruct button;
	int ButtontexID = loadTexture("textures/button/play.png", imgWidth, imgHeight);
	button.setupSprite(ButtontexID, vec3(400.0, 300.0, 0.0), vec3(imgWidth, imgHeight, 1.0), 1, 1);
	// Carregando uma textura (recebendo seu ID)

	// Inicializando a sprite do background
	int texID = loadTexture("textures/background/game_background.png", imgWidth, imgHeight);
	background.setupSprite(texID, vec3(800.0, 300.0, 0.0), vec3(imgWidth / 2.4, imgHeight / 1.8, 1.0), 1, 1);

	// Inicializando a sprite do obstaculo
	texID = loadTexture("textures/obstacles/rock.png", imgWidth, imgHeight);
	obstacle.setupSprite(texID, vec3(900.0, 85.0, 0.0), vec3(imgWidth, imgHeight, 1.0), 1, 1);

	// Inicializando a sprite do passaro
	texID = loadTexture("textures/bird/fly.png", imgWidth, imgHeight);
	bird.setupSprite(texID, vec3(800.0, 100.0, 0.0), vec3(imgWidth / 6.0, imgHeight - 2.0, 1.0), 6, 1);

	// Inicializando a sprite do personagem
	texID = loadTexture("textures/character/warewolf/Run.png", imgWidth, imgHeight);
	character.setupSprite(texID, vec3(50.0, 127.0, 0.0), vec3(imgWidth / 9.0, imgHeight, 1.0), 9, 1);

	// inicializa imagem de pulo
	GLuint jumpTexID = loadTexture("textures/character/warewolf/Jump.png", jumpImgWidth, jumpImgHeight);

	glUseProgram(shaderID);

	// Enviando a cor desejada (vec4) para o fragment shader
	// Utilizamos a variáveis do tipo uniform em GLSL para armazenar esse tipo de info
	// que não está nos buffers
	glUniform1i(glGetUniformLocation(shaderID, "texBuffer"), 0);

	// Matriz de projeção ortográfica
	mat4 projection = ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);

	glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, value_ptr(projection));

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
		glUniform2f(glGetUniformLocation(shaderID, "offsetTex"), charoffsetTex.s, charoffsetTex.t);
		
		// move o background em loop:
		background.position.x -= scenarioVel;
		if (background.position.x <= 0.0)
			background.position.x = 800.0;
		drawSprite(background, shaderID);

		obstacle.position.x -= scenarioVel; 
		if (obstacle.position.x <= 0.0) {
			obstacle.position.x = 900.0;
			
			points += 100;

			int newLevel = points / 300; // A cada 300 pontos, o nível aumenta
        	if (newLevel > level) {
            	level = newLevel; // Atualiza o nível apenas se ele mudou
            	std::cout << "Level up! Now you are at level: " << level << std::endl;
				scenarioVel += (level / 2);
        	}
		}
		drawSprite(obstacle, shaderID);

		if (keys[GLFW_KEY_LEFT] || keys[GLFW_KEY_A])
			character.position.x -= vel;
		if (keys[GLFW_KEY_RIGHT] || keys[GLFW_KEY_D])
			character.position.x += vel;
		if (keys[GLFW_KEY_SPACE] && !isJumping) {
    		isJumping = true;           // Marca que o personagem está pulando
			character.setupSprite(jumpTexID, character.position, vec3(jumpImgWidth / 11.0, jumpImgHeight, 1.0), 11, 1); // Configura o sprite de pulo
    		character.position.y += 1.0f; // Pequeno ajuste inicial para evitar que ele fique travado no chão
			character.FPS = 14.66;
		}

		if (isJumping) {
    		character.position.y += jumpVelocity; // Move o personagem com a velocidade de pulo
    		jumpVelocity -= gravity;              // Aplica gravidade reduzindo a velocidade de pulo

    		// Quando o personagem atinge o chão novamente
    		if (character.position.y <= groundLevel) {
        		character.position.y = groundLevel; // Garante que o personagem não vá abaixo do chão
        		isJumping = false;                  // Reseta o estado de pulo
        		jumpVelocity = 15.0f;               // Reseta a velocidade para o próximo pulo
				character.setupSprite(texID, character.position, vec3(imgWidth / 9.0, imgHeight, 1.0), 9, 1);
				character.FPS = 12.0;
    		}
		}

		// Incremento circular (em loop) do índice do frame DO CHAR
		float now = glfwGetTime();
		float chardt = now - character.lastTime;
		if (chardt >= 1.0 / character.FPS)
		{
			character.iFrame = (character.iFrame + 1) % character.nFrames;
			character.lastTime = now;
		}	

		charoffsetTex.s = character.iFrame * character.d.s;
		charoffsetTex.t = 0.0;

		glUniform2f(glGetUniformLocation(shaderID, "offsetTex"), charoffsetTex.s, charoffsetTex.t);
		drawSprite(character, shaderID);

		float currentFrameTime = glfwGetTime(); // Obtém o tempo atual
		float deltaTime = currentFrameTime - lastFrameTime; // Calcula o tempo entre frames
		lastFrameTime = currentFrameTime; // Atualiza o tempo do último frame
		birdTime += deltaTime; // deltaTime é o tempo entre frames
		bird.position.y = 300.0f + amplitude * sin(frequency * birdTime); // Ajuste 300.0f para a altura média desejada
		
		bird.position.x -= scenarioVel + 1; 
		if (bird.position.x <= 0.0)
			bird.position.x = 900.0;

		// Incremento circular (em loop) do índice do frame DO PASSARO
		float nowBird = glfwGetTime();
		float birddt = nowBird - bird.lastTime;
		if (birddt >= 1.0 / bird.FPS)
		{
			bird.iFrame = (bird.iFrame + 1) % bird.nFrames;
			bird.lastTime = now;
		}	

		birdoffsetTex.s = bird.iFrame * bird.d.s;
		birdoffsetTex.t = 0.0;

		glUniform2f(glGetUniformLocation(shaderID, "offsetTex"), birdoffsetTex.s, birdoffsetTex.t);
		drawSprite(bird, shaderID);

		// Verifica se houve uma colisão entre character e obstacle
   		if (checkCollisionWithMargin(character, obstacle, -20.0)) {
        	// Lógica a ser executada quando ocorre uma colisão
        	std::cout << "Colisão com pedra detectada!" << std::endl;

			glfwTerminate();
			return 0;
    	}

		if (checkCollisionWithMargin(character, bird, -20.0)) {
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

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
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

// Esta função está basntante hardcoded - objetivo é compilar e "buildar" um programa de
//  shader simples e único neste exemplo de código
//  O código fonte do vertex e fragment shader está nos arrays vertexShaderSource e
//  fragmentShader source no iniçio deste arquivo
//  A função retorna o identificador do programa de shader

int setupShader() {
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

// Esta função está bastante harcoded - objetivo é criar os buffers que armazenam a
// geometria de um triângulo
// Apenas atributo coordenada nos vértices
// 1 VBO com as coordenadas, VAO com apenas 1 ponteiro para atributo
// A função retorna o identificador do VAO
void SpriteStruct::setupSprite(int texID, vec3 position, vec3 dimensions, int nFrames, int nAnimations)
{
	this->texID = texID;
	this->dimensions = dimensions;
	this->position = position;
	this->nAnimations = nAnimations;
	this->nFrames = nFrames;
	iAnimation = 0;
	iFrame = 0;

	d.s = 1.0 / (float)nFrames;
	d.t = 1.0 / (float)nAnimations;
	// Aqui setamos as coordenadas x, y e z do triângulo e as armazenamos de forma
	// sequencial, já visando mandar para o VBO (Vertex Buffer Objects)
	// Cada atributo do vértice (coordenada, cores, coordenadas de textura, normal, etc)
	// Pode ser arazenado em um VBO único ou em VBOs separados
	GLfloat vertices[] = {
		// x   y     z    s     		t
		// T0
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
	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos
	glBindVertexArray(VAO);
	// Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando:
	//  Localização no shader * (a localização dos atributos devem ser correspondentes no layout especificado no vertex shader)
	//  Numero de valores que o atributo tem (por ex, 3 coordenadas xyz)
	//  Tipo do dado
	//  Se está normalizado (entre zero e um)
	//  Tamanho em bytes
	//  Deslocamento a partir do byte zero

	// Atributo 0 - Posição - x, y, z
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
	FPS = 12.0;
	lastTime = 0.0;
}

vec3 SpriteStruct::getPMin() {
	return vec3(position.x - (dimensions.x / 2), position.y - (dimensions.y / 2), position.z);
}

vec3 SpriteStruct::getPMax() {
    return vec3(position.x + (dimensions.x / 2), position.y + (dimensions.y / 2), position.z);
}

int loadTexture(string filePath, int &imgWidth, int &imgHeight)
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

void drawSprite(SpriteStruct spr, GLuint shaderID)
{
	glBindVertexArray(spr.VAO); // Conectando ao buffer de geometria

	glBindTexture(GL_TEXTURE_2D, spr.texID); // conectando o buffer de textura

	// Matriz de modelo - Tranformações na geometria, nos objetos
	mat4 model = mat4(1); // matriz identidade
	// Translação
	model = translate(model, spr.position);
	// Rotação
	model = rotate(model, radians(spr.angle), vec3(0.0, 0.0, 1.0));
	// Escala
	model = scale(model, spr.dimensions);
	// Enviar para o shader
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));

	// Chamada de desenho - drawcall
	// Poligono Preenchido - GL_TRIANGLES
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0); // Desconectando o buffer de geometria
}

bool checkCollisionWithMargin(SpriteStruct &one, SpriteStruct &two, float margin = 0.0f) {
    // Obtenha os pontos mínimo e máximo com a margem
    vec3 oneMin = one.getPMin() - vec3(margin, margin, 0);
    vec3 oneMax = one.getPMax() + vec3(margin, margin, 0);
    vec3 twoMin = two.getPMin() - vec3(margin, margin, 0);
    vec3 twoMax = two.getPMax() + vec3(margin, margin, 0);

    // Verificação de colisão no eixo x
    bool collisionX = oneMax.x >= twoMin.x && twoMax.x >= oneMin.x;

    // Verificação de colisão no eixo y
    bool collisionY = oneMax.y >= twoMin.y && twoMax.y >= oneMin.y;

    // Retorna verdadeiro se houver colisão em ambos os eixos
    return collisionX && collisionY;
}