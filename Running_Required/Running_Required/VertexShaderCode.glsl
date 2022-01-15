#version 430


in layout(location=0) vec3 position;
in layout(location=1) vec2 uv;
in layout(location=2) vec3 normal;
in layout(location=3) vec3 tangent;
in layout(location=4) vec3 bitangent;


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
