#version 330
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 vertex_color;

uniform mat4 projection;
out vec4 finalColor;

void main()
{
	//...pode ter mais linhas de código aqui!
	gl_Position = projection * vec4(position, 1.0);
	finalColor = vec4(vertex_color,1.0);
}