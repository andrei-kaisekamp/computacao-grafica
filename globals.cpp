#include "globals.h"
static bool keys[1024] = {false};
const GLuint WIDTH = 800, HEIGHT = 600;

float globalGameSpeed   = 5.0;
int globalLevel         = 1;
int globalPoints        = 0;
float globalGravity     = 0.98f;    
float globalGroundLevel = 127.0f; 
float globalTolerance   = -20.0f;
