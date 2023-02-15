#version 430
//mtl data
struct MaterialInfo{
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
};
uniform MaterialInfo Material; 

//lighting color, a uniform
uniform vec4 ambientColor;
uniform vec4 diffuseColor;
uniform vec4 specularColor;
//for material specular, also called specular strength
uniform float Shininess;
uniform int EnvironmentMappingACK; //Whether to use environment mapping

//uniform samplerCube shadowMap;
layout(binding = 0) uniform sampler2D shadowMap;

layout(binding = 1) uniform samplerCube cubemap;

layout(binding = 2) uniform sampler2D texPlane;

layout(binding = 3) uniform sampler2D texLava1;

layout(binding = 4) uniform sampler2D texLava2;

layout(binding = 5) uniform sampler2D textureNew;

layout(binding = 6) uniform sampler2D textureNewNormal;

//light box coordinate
uniform vec3 vLightPosition;

//input output
out vec4 vFragColor;
in vec3 inverseTransposeNormal;
in vec2 UV;
in vec3 viewDir;
in vec3 halfwayDir;
in vec4 lightSpacePosition;
in vec3 fragPos;
in vec3 lightDir;
in vec3 reverseViewDir; 
in mat3 TBN;
uniform int toon_active;

uniform float currentTime;
uniform float mixVal;

float levels = 3;


float ShadowCalculation(vec4 lightSpacePosition, vec3 lightDir){
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

void main(void)
{ 

    if(EnvironmentMappingACK){
        if(EnvironmentMappingACK == 3){
            vFragColor = vec4(0.0,0.0,0.0,1.0);
        }else if(EnvironmentMappingACK == 4){
            float offsetX = currentTime * 0.001;            

            vec3 normalTexture = texture(textureNewNormal, vec2(UV.x + offsetX, UV.y)).rgb;
            normalTexture = normalize(normalTexture * 2.0 - 1.0);
            normalTexture = normalize(TBN * normalTexture);

            vec3 lightDirTangent = TBN * lightDir;
            vec3 viewDirTangent = TBN * viewDir;
            vec3 halfwayDirTangent = normalize(lightDirTangent + viewDirTangent);

            vec3 planeTexture = texture(textureNew, vec2(UV.x + offsetX, UV.y)).rgb;     
            
            vec3 ambient = 0.1 * planeTexture;

            float diff = max(dot(lightDirTangent, normalTexture), 0.0);
            vec3 newDiffuse = diff * planeTexture;

            vec3 reflectDir = reflect(-lightDirTangent, normalTexture);
            float spec = pow(max(dot(normalTexture, halfwayDirTangent), 0.0), 32.0);

            vec3 newSpecular = vec3(0.2) * spec;

            vFragColor = vec4(ambient + newDiffuse + newSpecular, 1.0);


        }else{
            float refractRatio = 1.0/1.52; //for now, it is regarded that it is through a glass
            //use the available glsl feature to count refraction coordinate

            vec3 refractCoordinate = vec3(0,0,0);

            if(EnvironmentMappingACK == 1){
                //reflect
                 refractCoordinate = reflect(reverseViewDir, inverseTransposeNormal);
            }else if(EnvironmentMappingACK == 2){
                refractCoordinate = refract(reverseViewDir, inverseTransposeNormal, refractRatio);
            }
           //vec3 refractCoordinate = reflect(reverseViewDir, inverseTransposeNormal);
            vFragColor = vec4(texture(cubemap, refractCoordinate).rgb, 1.0);
            //vFragColor = vec4(0,0,0, 1.0);
        }
    }else{
        vec4 planeTexture = texture(texPlane, UV);
        // planeTexture = vec4(1.0f, 1.0f, 1.0f, 1.0f);
        //attenuation for point lighting
        float lightDistance = length(lightDir);
        //float attenuation = 1.0 / (Kc + Kl * lightDistance + Kq * (lightDistance * lightDistance)); 


        //shadowing
        float shadow = ShadowCalculation(lightSpacePosition, lightDir);


        // diffuse intensity
        float diff = max(0.0, dot(normalize(inverseTransposeNormal), normalize(lightDir)));
        //-----------------------------------------------
        if(toon_active == 1)
        {
            float level = floor(diff * levels);
            diff = level / levels;
        }
        //-----------------------------------------------
        
        //diff*diffuse results in difuse color, and Kd is the objectColor
        //vFragColor = (1.0 - shadow)*attenuation*diffuseColor*(diff*vec4(Material.Kd,1.0));
        vFragColor = (1.0 - shadow) * diffuseColor*(diff*planeTexture);

        // Add in ambient light, note that shadow doesn't affect ambient
        //vFragColor += (1.0)*attenuation*ambientColor*vec4(Material.Ka,1);
        vFragColor += (1.0)*ambientColor*planeTexture;
   

        // Specular Light, for shininess
        float spec = max(0.0, dot(normalize(inverseTransposeNormal), halfwayDir));
        if(diff != 0) {
		    spec = pow(spec, Shininess);
            //-----------------------------------------------
            if(toon_active == 1)
            {
                float level = floor(spec * levels);
                spec = level / levels;
            }
            //-----------------------------------------------
            
		    //vFragColor += (1.0 - shadow)*attenuation*specularColor*(spec*vec4(Material.Ks,1));
            vFragColor += (1.0 - shadow)*specularColor*(spec*vec4(Material.Ks,1));
        }
    }    
}
	
    