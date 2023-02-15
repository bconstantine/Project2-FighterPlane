#version 430
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform int isReflection; 
uniform vec3 waterPlanePoint;
uniform vec3 waterPlaneNormal;

void main()
{
    TexCoords = aPos;
    if(isReflection){
        vec4 curPos = vec4(aPos, 1.0);
        vec3 A = curPos.xyz;
        vec3 Dvec = A - waterPlanePoint; //tmeporay vector from quad to point we want reflected
        vec3 Dnormal2Quad = waterPlaneNormal*dot(Dvec, waterPlaneNormal);
        vec3 Dparallel2Quad = Dvec-Dnormal2Quad;
        vec3 Dreflected = Dparallel2Quad - Dnormal2Quad;
        vec3 finalReflectedPos = waterPlanePoint+Dreflected;
        gl_Position = projection*view*vec4(finalReflectedPos,1.0);
    }else{
        vec4 pos = projection * view * vec4(aPos, 1.0);
        gl_Position = pos.xyww;
    }
}  