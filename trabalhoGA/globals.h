//Dependencies:
#include <iostream>
#include <string>
#include <vector>
#include <assert.h>

#include <glad/glad.h>                   											  // GLAD
#include <GLFW/glfw3.h>                  											  // GLFW
#include <glm/glm.hpp>                                                                // GLM
#include <glm/gtc/matrix_transform.hpp>                                               // GLM
#include <glm/gtc/type_ptr.hpp>                                                       // GLM
#include <C:\Users\andre\git\computacao-grafica\Dependencies\stb_image\stb_image.cpp> // STB_IMAGE

#ifndef GLOBALS_H
#define GLOBALS_H

extern float globalGameSpeed;
extern int globalLevel;
extern int globalPoints;

extern float globalGravity;      
extern float globalGroundLevel;

extern float globalTolerance;
extern bool gameShouldRun;
GLFWwindow *window;
#endif
