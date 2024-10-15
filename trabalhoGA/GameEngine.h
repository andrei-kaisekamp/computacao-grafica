
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

class GameEngine
{
    public:
        static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
		void initialize();
		void run();

    private:
		vector <Sprite*> sprites;
		mat4 projection = ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);

		int setupShader();
		int loadTexture(string filePath, int &imgWidth, int &imgHeight);

		void createBackground();
		void createCharacter();
		void createObstacle();
		void createBird();
		void createAllObjects();

		bool detectColisions();
		bool checkCollisionWithMargin(Sprite* one, Sprite* two, float margin);

		bool levelCompleted();
		void levelUp();
};