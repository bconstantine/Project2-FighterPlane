#version 430

layout(location=0) in float curLifetime;
layout(location=1) in vec2 texCoord;
layout(location=2) in vec2 curCorner;
layout(location=3) in vec3 centerOffset;
layout(location=4) in vec3 curVelocity;

uniform float totalTime;
uniform vec3 centralFirePos;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out float vLifetime;
out vec2 vTexCoord;

void main(){
	//count current time with respect to how much it has existed, restart to beginning when time expired
	float time = mod(totalTime, curLifetime);
	
	//count particle position based on center position, offset, and the displacement
	vec4 position = vec4(centralFirePos + centerOffset + (time*curVelocity), 1.0);

	//vLifetime shrink as particle ages, use this since the top of fire is getting smaller
	vLifetime = 1.3 - (time/curLifetime);
	vLifetime = clamp(vLifetime, 0.0, 1.0);
	float size = (vLifetime*vLifetime)*0.05; //0.05 is just some scaling constant

	//use billboarding to count rotation of texture
	vec3 camRightVec = vec3(viewMatrix[0].x, viewMatrix[1].x,viewMatrix[2].x);
	vec3 camUpVec = vec3(viewMatrix[0].y, viewMatrix[1].y,viewMatrix[2].y);
	position.xyz += (camRightVec * curCorner.x * size) + (camUpVec * curCorner.y * size);

	//apply regular position transformation to the result of the billboarding
	gl_Position = projMatrix * viewMatrix * position;
	//pas output to fragment shader
	vTexCoord = texCoord;
	vLifetime = curLifetime;
}