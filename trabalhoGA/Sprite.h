
using namespace glm;
using namespace std;


class Sprite
{
public:
	Sprite();
	~Sprite();

    void setupSprite(int texID, vec3 position, vec3 dimensions, int nFrames, int nAnimations, GLuint shader);
	virtual void draw();
	virtual void move();
	
    vec3 getPMin(); // Retorna o ponto inferior esquerdo
    vec3 getPMax(); // Retorna o ponto superior direito

    GLuint VAO, texture;
	vec3 position, dimensions;
	float angle;

	// Para controle da animação
	int nAnimations, nFrames;
	int iAnimation, iFrame;
	float FPS;
	float lastTime;
	vec2 d;

	GLuint shader;
protected:

    
};