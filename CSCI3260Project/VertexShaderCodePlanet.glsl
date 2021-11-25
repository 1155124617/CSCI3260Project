// #version 430
#version 330
layout(location=0) in vec3 position;
layout(location=1) in vec2 uv;
layout(location=2) in vec3 normal;
/*
in layout(location=0) vec3 position;
in layout(location=1) vec2 uv;
in layout(location=2) vec3 normal;*/

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform sampler2D textureSampler1;


out vec2 uvCoord;
out vec3 normalWorld;
out vec3 vertexWorld;

void main()
{	
	gl_Position=projection*view*model*vec4(position,1.0);
    normalWorld=mat3(model)*normalize(texture(textureSampler1, uv).rgb * 2.0 - 1.0);
	vertexWorld=vec3(model*vec4(position,1.0f));
	uvCoord=uv;
}
