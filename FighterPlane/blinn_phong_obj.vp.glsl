#version 430

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vTangents;
layout(location = 4) in vec3 vBitangents;

uniform MatVP{
	mat4 View;
	mat4 Projection;
};
uniform mat4 Model;
uniform vec3 vLightPosition;
uniform vec3 varyingViewPos; //possibly seen from water reflection
uniform vec3 permanentViewPos; //real eye
uniform mat4 lightSpaceMatrix;
uniform vec4 clipPlane; //clipping for water reflection and refraction
uniform int isReflection; 
uniform vec3 waterPlanePoint;
uniform vec3 waterPlaneNormal;
// to fragment shader
out vec3 inverseTransposeNormal;
out vec2 UV;
out vec3 viewDir;
out vec3 halfwayDir;
out vec4 lightSpacePosition;
out vec3 fragPos;
out vec3 lightDir;
out vec3 reverseViewDir; 
out mat3 TBN;
//out vec3 tangents;
//out vec3 bitangents;
void main(void) 
{ 
    //place the coordinate according to the transformation matrix
    
    if(isReflection){
        vec4 curPos = Model* vec4(vPosition, 1.0);
        vec3 A = curPos.xyz;
        vec3 Dvec = A - waterPlanePoint; //tmeporay vector from quad to point we want reflected
        vec3 Dnormal2Quad = waterPlaneNormal*dot(Dvec, waterPlaneNormal);
        vec3 Dparallel2Quad = Dvec-Dnormal2Quad;
        vec3 Dreflected = Dparallel2Quad - Dnormal2Quad;
        vec3 finalReflectedPos = waterPlanePoint+Dreflected;
        gl_Position = Projection*View*vec4(finalReflectedPos,1.0);
        //gl_Position = Projection * View * Model * vec4(vPosition,1.0);
    }else{
        gl_Position = Projection * View * Model * vec4(vPosition,1.0);
    }

    gl_ClipDistance[0] = dot(gl_Position, clipPlane);

    //take care the uv and normal from obj file
    inverseTransposeNormal = transpose(inverse(mat3(Model))) * vertexNormal;
    inverseTransposeNormal = normalize(inverseTransposeNormal);
	UV = vertexUV;
    
    //for shadow
    fragPos = vec3(Model*vec4(vPosition,1.0)); //purely on world coordinate, not based on camera coordinate or screen coordinate
    lightSpacePosition = lightSpaceMatrix * (vec4(fragPos,1.0)); //now, find fragment position based on the light, to map for shadow mapping

    //light direction vector with respect to each fragment position
    lightDir = normalize(vLightPosition - fragPos);
    //count viewDir, viewDirection compared to each vertices
    viewDir = normalize(varyingViewPos - fragPos);
    //count reverseViewDir, for enviroment mapping
    reverseViewDir = normalize(fragPos - varyingViewPos);
    //count halfwayDir, for Blinn-Phong
    halfwayDir = normalize(lightDir + viewDir);

    mat3 MV3x3 = mat3(Model);

//    vec3 T = normalize(vec3(Model * vec4(vTangents, 0.0)));
//    vec3 B = normalize(vec3(Model * vec4(vBitangents, 0.0)));
//    vec3 N = normalize(vec3(Model * vec4(vertexNormal, 0.0)));
    mat3 normalMatrix = transpose(inverse(mat3(Model)));
    vec3 T = normalize(normalMatrix * vTangents);
    vec3 N = normalize(normalMatrix * vertexNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    TBN = transpose(mat3(T, B, N));

//    tangents = vTangents;
//    bitangents = vBitangents;
}
