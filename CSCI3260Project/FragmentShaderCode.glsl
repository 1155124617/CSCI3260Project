#version 330
// #version 430

struct DirLight{
	vec3 direction;
	float intensity;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

};


struct SpotLight{
	vec3 position;
	vec3 direction;
	float cutAngle;
	float intensity;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;

};
struct PointLight{
	vec3 position;
	float intensity;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

out vec4 finalColor;

in vec3 normalWorld;
in vec3 vertexWorld;
in vec2 uvCoord;
in mat3 TBN;


uniform PointLight pointLight;
uniform DirLight dirLight;
uniform SpotLight spotLight;
uniform sampler2D textureSampler0;
uniform sampler2D textureSampler1;
uniform vec3 eyePositionWorld;
uniform int normalMapping_flag;

vec3 CalDircLight(DirLight light,vec3 normalWorld, vec3 eyeVectorWorld);
vec3 CalPointLight(PointLight pointLight, vec3 normalWorld,vec3 eyeVectorWorld,vec3 vertexWorld);
vec3 CalSpotLight(SpotLight spotLight, vec3 normalWorld, vec3 eyeVectorWorld, vec3 vertexWorld);
void main()
{
    vec3 normal;
    if (normalMapping_flag == 1) {
        normal = texture(textureSampler1, uvCoord).rgb;
        normal = normal * 2.0 - 1.0;
        normal = normalize(TBN * normal);
    }
    else {
        normal=normalize(normalWorld);
    }
	vec3 eyeVectorWorld=normalize(eyePositionWorld-vertexWorld);
	/*vec3 resultColor=CalDircLight(dirLight,normal,eyeVectorWorld)+
					CalPointLight(pointLight,normal,eyeVectorWorld,vertexWorld)+
					CalSpotLight(spotLight,normal,eyeVectorWorld,vertexWorld);*/
	vec3 resultColor=CalDircLight(dirLight,normal,eyeVectorWorld)+CalPointLight(pointLight,normal,eyeVectorWorld,vertexWorld);
	finalColor=vec4(resultColor,1.0f);
}




vec3 CalDircLight(DirLight light,vec3 normal, vec3 eyeVectorWorld){
	vec3 lightDirection=normalize(-light.direction);
	//
	float dif=max(dot(normal,lightDirection),0.0f);
	vec3 reflectedLight=reflect(-lightDirection,normal);
	float s=max(dot(reflectedLight,eyeVectorWorld),0.0f);
	s=pow(s,50);
	vec3 ambient=light.ambient*vec3(texture(textureSampler0,uvCoord));
	vec3 diffuse=light.diffuse*dif*vec3(texture(textureSampler0,uvCoord));
	vec3 specular=light.specular*s*vec3(texture(textureSampler0,uvCoord));
	vec3 result=light.intensity*(ambient+diffuse+specular);

	return result;
}


vec3 CalPointLight(PointLight pointLight, vec3 normal,vec3 eyeVectorWorld,vec3 vertexWorld){
	vec3 lightDirection=normalize(pointLight.position-vertexWorld);
	//Diffusion:
	float dif=max(dot(normal,lightDirection),0.0f);
	vec3 reflectedLight=reflect(-lightDirection,normal);
	float s=max(dot(reflectedLight,eyeVectorWorld),0.0f);
	s=pow(s,50);
	//Distance:
	float dis=length(pointLight.position-vertexWorld);
	float attenuation=1.0f/(pointLight.constant+pointLight.linear*dis+pointLight.quadratic*dis*dis);


	vec3 ambient=pointLight.ambient*vec3(texture(textureSampler0,uvCoord))*attenuation;
	vec3 diffuse=pointLight.diffuse*dif*vec3(texture(textureSampler0,uvCoord))*attenuation;
	vec3 specular=pointLight.specular*s*vec3(texture(textureSampler0,uvCoord))*attenuation;
	vec3 result=pointLight.intensity*(ambient+diffuse+specular);



	return result;
}


vec3 CalSpotLight(SpotLight spotLight, vec3 normal, vec3 eyeVectorWorld, vec3 vertexWorld){
     vec3 lightDirection=normalize(spotLight.position-vertexWorld);
	 //Diffusiong:
	 float dif=max(dot(normal,lightDirection),0.0f);
	 //Specular:
	 vec3 reflectedLight=reflect(-lightDirection,normal);
	 float s=max(dot(reflectedLight,eyeVectorWorld),0.0f);
	 s=pow(s,50);
     //Distance:
	 float dis=length(spotLight.position-vertexWorld);
	 float attenuation=1.0f/(spotLight.constant+spotLight.linear*dis+spotLight.quadratic*dis*dis);

	 float theta=max(dot(lightDirection,normalize(-spotLight.direction)),0.0f);
	 vec3 result=vec3(0.0f,0.0f,0.0f);
	 if(theta>spotLight.cutAngle){
	  vec3 ambient=spotLight.ambient*vec3(texture(textureSampler0,uvCoord))*attenuation;
	  vec3 diffuse=spotLight.diffuse*dif*vec3(texture(textureSampler0,uvCoord))*attenuation;
	  vec3 specular=spotLight.specular*s*vec3(texture(textureSampler0,uvCoord))*attenuation;
	  result=spotLight.intensity*(ambient+diffuse+specular);
	 }
	 else
		result=vec3(0.0f,0.0f,0.0f);
	 return result;
}
