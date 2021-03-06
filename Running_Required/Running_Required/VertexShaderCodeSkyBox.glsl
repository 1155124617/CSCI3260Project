#version 430
in layout (location = 0) vec3 aPos;

out vec3 TexCoords;

uniform mat4 view;
uniform mat4 projection;


void main()
{	
    TexCoords = aPos;
    gl_Position = projection * view * vec4(aPos, 1.0);
}