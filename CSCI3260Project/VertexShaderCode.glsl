// #version 430
#version 330
layout(location=0) in vec3 position;
layout(location=1) in vec2 uv;
layout(location=2) in vec3 normal;
layout(location=3) in vec3 tangent;
layout(location=4) in vec3 bitangent;
/*
in layout(location=0) vec3 position;
in layout(location=1) vec2 uv;
in layout(location=2) vec3 normal;*/

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int normalMapping_flag;


out vec2 uvCoord;
out vec3 normalWorld;
out vec3 vertexWorld;
out mat3 TBN;

void main()
{	
	gl_Position=projection*view*model*vec4(position,1.0);
    if (normalMapping_flag == 1) {
        vec3 T = normalize(vec3(model * vec4(tangent, 1.0)));
        vec3 B = normalize(vec3(model * vec4(bitangent, 1.0)));
        vec3 N = normalize(vec3(model * vec4(normal, 1.0)));
        TBN = mat3(T, B, N);
    }
	normalWorld=mat3(model)*normal;
	vertexWorld=vec3(model*vec4(position,1.0f));
	uvCoord=uv;
}
