#version 430
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 aTexCoord;

out vec4 clipSpaceCoordinate;
out vec2 texCoord;
out vec3 viewVec;
out vec3 lightDir;
out vec3 halfwayDir; 
out vec4 lightSpacePos;
uniform mat4 Projection;
uniform mat4 View;
uniform vec3 permanentViewPos;
uniform vec3 lightPos;
uniform mat4 lightSpaceMatrix;
void main(){
	gl_Position = Projection * View * vec4(pos, 1.0);
	clipSpaceCoordinate = gl_Position;
	texCoord = aTexCoord;
	viewVec = permanentViewPos - pos;
	lightDir = normalize(lightPos - pos);
	halfwayDir = normalize(lightDir + normalize(viewVec));
	lightSpacePos = lightSpaceMatrix * vec4(pos, 1.0);
}