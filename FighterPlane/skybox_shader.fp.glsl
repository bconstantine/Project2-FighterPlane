#version 430
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

uniform int toon_active;

void main()
{    
    FragColor = texture(skybox, TexCoords);

    if(toon_active == 1)
    {
        bool rFound = false, gFound = false, bFound = false;
        float factor = 10.0f;
        for(float i = 0.0; i < factor; i++)
        {
            float currentVal = 1.0/factor * i;
            if(FragColor.x <= currentVal && !rFound && currentVal < 9.0/20.0)
            {
                FragColor.x = currentVal;
                rFound = true;
            }

            if(FragColor.y <= currentVal && !gFound && currentVal < 15.0/20.0)
            {
                FragColor.y = currentVal;
                gFound = true;
            }

            if(FragColor.z <= currentVal && !bFound)
            {
                FragColor.z = currentVal;
                bFound = true;
            }

            if(rFound && gFound && bFound)
            {
                break;
            }
        }   
    }
}