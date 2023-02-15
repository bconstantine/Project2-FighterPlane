#version 410

layout (binding = 0) uniform sampler2D tex; 

uniform int shader_now; 
uniform float mosaic_val; 
uniform float width_size;
uniform float height_size;

out vec4 color; 
in VS_OUT
{
vec2 texcoord; 
} fs_in;

#define SORT_SIZE  49  
#define MASK_SIZE  7

precision highp float; 

vec2 TexSize = vec2(width_size, height_size); 
// vec2 TexSize = vec2(2000, 2000); 
vec2 mosaicSize = vec2(mosaic_val, mosaic_val); 

void main (void) 
{
    // color = texture(tex, fs_in.texcoord);
    switch (shader_now)
	{
		case(0):
		{
			color = texture(tex, fs_in.texcoord);
			break;
		}
		case(8):
		{
			// square mosaic
			vec2 intXY = vec2(fs_in.texcoord.x * TexSize.x, fs_in.texcoord.y * TexSize.y);
			vec2 XYMosaic = vec2(floor(intXY.x/mosaicSize.x) * mosaicSize.x, floor(intXY.y/mosaicSize.y) * mosaicSize.y);
			vec2 UVMosaic = vec2(XYMosaic.x/TexSize.x, XYMosaic.y/TexSize.y);
			vec4 colorRes = texture2D(tex, UVMosaic);
			color = colorRes;
            break;
		}
		case(9):
		{
			// triangle mosaic
			const float TR = 0.866025;
            const float PI6 = 0.523599;
    
            float x = fs_in.texcoord.x;
            float y = fs_in.texcoord.y;
    
            int wx = int(x/(1.5 * mosaicSize.x));
            int wy = int(y/(TR * mosaicSize.y));
    
            vec2 v1, v2, vn;
    
            if (wx/2 * 2 == wx) {
                if (wy/2 * 2 == wy) {
                    v1 = vec2(mosaicSize.x * 1.5 * float(wx), mosaicSize.y * TR * float(wy));
                    v2 = vec2(mosaicSize.x * 1.5 * float(wx + 1), mosaicSize.y * TR * float(wy + 1));
                } else {
                    v1 = vec2(mosaicSize.x * 1.5 * float(wx), mosaicSize.y * TR * float(wy + 1));
                    v2 = vec2(mosaicSize.x * 1.5 * float(wx + 1), mosaicSize.y * TR * float(wy));
                }
            } else {
                if (wy/2 * 2 == wy) {
                    v1 = vec2(mosaicSize.x * 1.5 * float(wx), mosaicSize.y * TR * float(wy + 1));
                    v2 = vec2(mosaicSize.x * 1.5 * float(wx+1), mosaicSize.y * TR * float(wy));
                } else {
                    v1 = vec2(mosaicSize.x * 1.5 * float(wx), mosaicSize.y * TR * float(wy));
                    v2 = vec2(mosaicSize.x * 1.5 * float(wx + 1), mosaicSize.y * TR * float(wy+1));
                }
            }
    
            float s1 = sqrt(pow(v1.x - x, 2.0) + pow(v1.y - y, 2.0));
            float s2 = sqrt(pow(v2.x - x, 2.0) + pow(v2.y - y, 2.0));
    
            if (s1 < s2) {
                vn = v1;
            } else {
                vn = v2;
            }
    
            vec4 mid = texture2D(tex, vn);
            float a = atan((x - vn.x)/(y - vn.y));
    
            vec2 area1 = vec2(vn.x, vn.y - mosaicSize.y * TR/2.0);
            vec2 area2 = vec2(vn.x + mosaicSize.x/2.0, vn.y - mosaicSize.y * TR/2.0);
            vec2 area3 = vec2(vn.x + mosaicSize.x/2.0, vn.y + mosaicSize.y * TR/2.0);
            vec2 area4 = vec2(vn.x, vn.y + mosaicSize.y * TR/2.0);
            vec2 area5 = vec2(vn.x - mosaicSize.x/2.0, vn.y + mosaicSize.y * TR/2.0);
            vec2 area6 = vec2(vn.x - mosaicSize.x/2.0, vn.y - mosaicSize.y * TR/2.0);
    
    
            if (a >= PI6 && a < PI6 * 3.0) {
                vn = area1;
            } else if (a >= PI6 * 3.0 && a < PI6 * 5.0) {
                vn = area2;
            } else if ((a >= PI6 * 5.0 && a <= PI6 * 6.0) || (a < -PI6 * 5.0 && a > -PI6 * 6.0)) {
                vn = area3;
            } else if (a < -PI6 * 3.0 && a >= -PI6 * 5.0) {
                vn = area4;
            } else if(a <= -PI6 && a> -PI6 * 3.0) {
                vn = area5;
            } else if (a > -PI6 && a < PI6) {
                vn = area6;
            }
            
            vec4 colorRes = texture2D(tex, vn);
			color = colorRes;
            break;
		}
        case(10):
        {
            float length = mosaicSize.x;
            float TR = 0.866025;
    
            float x = fs_in.texcoord.x;
            float y = fs_in.texcoord.y;
    
            int wx = int(x/1.5/length);
            int wy = int(y/TR/length);
            vec2 v1, v2, vn;
    
            if (wx/2 * 2 == wx) {
                if (wy/2 * 2 == wy) {
                   //(0,0),(1,1)
                    v1 = vec2(length * 1.5 * float(wx), length * TR * float(wy));
                    v2 = vec2(length * 1.5 * float(wx + 1), length * TR * float(wy + 1));
                } else {
                   //(0,1),(1,0)
                    v1 = vec2(length * 1.5 * float(wx), length * TR * float(wy + 1));
                    v2 = vec2(length * 1.5 * float(wx + 1), length * TR * float(wy));
                }
            }else {
                if (wy/2 * 2 == wy) {
                   //(0,1),(1,0)
                    v1 = vec2(length * 1.5 * float(wx), length * TR * float(wy + 1));
                    v2 = vec2(length * 1.5 * float(wx + 1), length * TR * float(wy));
                } else {
                   //(0,0),(1,1)
                    v1 = vec2(length * 1.5 * float(wx), length * TR * float(wy));
                    v2 = vec2(length * 1.5 * float(wx + 1), length * TR * float(wy + 1));
                }
            }
    
            float s1 = sqrt(pow(v1.x - x, 2.0) + pow(v1.y - y, 2.0));
            float s2 = sqrt(pow(v2.x - x, 2.0) + pow(v2.y - y, 2.0));
            if (s1 < s2) {
                vn = v1;
            } else {
                vn = v2;
            }
            vec4 colorRes = texture2D(tex, vn);
    
            color = colorRes;
        }
	}
}

