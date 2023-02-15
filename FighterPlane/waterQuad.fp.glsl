#version 430
in vec4 clipSpaceCoordinate;
in vec2 texCoord;
in vec3 viewVec;
in vec3 lightDir;
in vec3 halfwayDir;
in vec4 lightSpacePos;
out vec4 vFragColor;

const float dudvIntensity = 0.08;
const float Shininess = 20;
const float reflectiveIndex = 0.2;
const float shadowScaler = 0.5; //since on water, shadow is not that black

layout(binding=0) uniform sampler2D reflectTex;
layout(binding=1) uniform sampler2D refractTex;
layout(binding=2) uniform sampler2D dudvTex; 
layout(binding=3) uniform sampler2D normalTex;
layout(binding=4) uniform sampler2D shadowMap;

float ShadowCalculation(vec4 lightSpacePosition, vec3 lightDir, vec3 inverseTransposeNormal){
    //normalize with alpha, to [-1,1]
    vec3 normLSPosition = lightSpacePosition.xyz / lightSpacePosition.w;

    //depth map is in range[0,1], since this is the coordinate we use for texture
    normLSPosition = normLSPosition * 0.5 + 0.5; 

    //sample mapping from the texture we have made, call the .r to find the depth that is closest to light 
    //(in other words, displayed, not shadw)
    float closestDepth = texture(shadowMap, normLSPosition.xy).r;  
    
    //compare with current depth
    float currentDepth = normLSPosition.z; 

    //fix shadow acne,caused by light coming from different angle, which is not parallel with shadow texture
    //simply offset the depth of the surface (or the shadow map) by a small bias amount
    //such that the fragments are not incorrectly considered above the surface.
    //we will try fix exploding bias with peter panning
    float bias = max(0.05 * (1.0 - dot(inverseTransposeNormal, lightDir)), 0.005);

    //partially fix the jagged edges by PCF(partially close filtering)
    //produce softer shadows by sample more than once from the depth map, 3x3 size, and average the result based on the 9 coordinates
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, normLSPosition.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // this is to fix the oversampling, when the particle is more than the border capacity, just make it no shadow
    if(normLSPosition.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

uniform float randomSample; 
uniform vec3 lightColor;
void main(){

	vec2 normalizedDeviceSpaceCoordinate = (clipSpaceCoordinate.xy / clipSpaceCoordinate.w) / 2.0 + 0.5;
	vec2 rfrcTexCoord = vec2(normalizedDeviceSpaceCoordinate.x, normalizedDeviceSpaceCoordinate.y);
	vec2 rflcTexCoord = vec2(normalizedDeviceSpaceCoordinate.x, normalizedDeviceSpaceCoordinate.y);
	vec2 dudvTexCoord = texture(dudvTex, vec2(texCoord.x + randomSample, texCoord.y)).rg * 0.1;
	dudvTexCoord = texCoord + vec2(dudvTexCoord.x, dudvTexCoord.y + randomSample);
	vec2 dudvData = (texture(dudvTex, dudvTexCoord).rg * 2.0 - 1.0) *dudvIntensity; //scale the dudv with the desired amount
	//dudvData = vec2(0.0, 0.0);
	rfrcTexCoord += dudvData;
	//rfrcTexCoord = clamp(rfrcTexCoord, 0.001, 0.999);
	rflcTexCoord += dudvData;
	//rflcTexCoord = clamp(rflcTexCoord, 0.001, 0.999);
	
	//sample the normal map using the same dudvTexCoord
	//since normal map is entirely blue, we will use blue color as the main normal direction (y axis)
	vec4 normalMapData = texture(normalTex, dudvTexCoord);
	vec3 trueNormal = vec3(normalMapData.r * 2.0 - 1, normalMapData.b, normalMapData.g * 2.0 - 1);

	//count the specular of the water
	vec4 specRes = vec4(0.0, 0.0, 0.0, 1.0);
	vec3 inverseTransposeNormal = trueNormal;
    inverseTransposeNormal = normalize(inverseTransposeNormal);

	//count shadow
	float shadow = ShadowCalculation(lightSpacePos, lightDir, inverseTransposeNormal);


	float diff = max(0.0, dot(normalize(inverseTransposeNormal), normalize(lightDir)));
	float spec = max(0.0, dot(normalize(inverseTransposeNormal), halfwayDir));
    if(diff != 0) {
		spec = pow(spec, Shininess);
		specRes += vec4(lightColor,1.0)*(diff*vec4(reflectiveIndex, reflectiveIndex, reflectiveIndex,1));
		if(specRes.x < 0.3 && specRes.y < 0.3 && specRes.z < 0.3){
			//specRes = vec4(1.0,1.0,1.0,1.0);
		}
		
    }

	vec4 reflectColour = texture(reflectTex, rflcTexCoord);
	vec4 refractColour = texture(refractTex, rfrcTexCoord);

	vec3 normViewVec = normalize(viewVec);
	float rfrcFactor = dot(normViewVec, vec3(0,1,0));
	rfrcFactor = pow(rfrcFactor, 1);
	//vFragColor = reflectColour;
	vFragColor = mix(reflectColour, refractColour, rfrcFactor);
	vFragColor = mix(vFragColor, vec4(0.0,0.3,0.5,1.0), 0.2) + specRes;
	vFragColor *= (1-shadow * shadowScaler);
//	vFragColor = texture(dudvTex, vec2(texCoord));
//	if(textureQueryLevels(reflectTex)){
//		vFragColor = vec4(1.0,1.0,1.0,1.0);
//	}
	//vFragColor = texture(reflectTex, vec2(normalizedDeviceSpaceCoordinate));
	//vFragColor = vec4(dudvData, 0,1.0);
	//vFragColor = mix(reflectColour, refractColour, 0.5);
//	if(reflectColour.x == 0 && reflectColour.y == 0 && reflectColour.z == 0){
//		vFragColor = vec4(0,0,1.0,1.0);
//	}

	//vFragColor = vec4(0,0,1.0,1.0);
	//vFragColor = mix(reflectColour, refractColour, 0.5);
}