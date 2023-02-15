#version 430
precision mediump float; //set float accuracy a little bit higher

uniform vec4 baseColor;
uniform float fragmentTime;

in float vLifetime;
in vec2 vTexCoord;

layout(binding = 0) uniform sampler2D fireTex;

out vec4 vFragColor;
void main () {
  float time = mod(fragmentTime, vLifetime);
  float percentOfLife = time / vLifetime;
  percentOfLife = clamp(percentOfLife, 0.0, 1.0);

  //we use a tiled texture, so we should count the offset
  float offset = floor(16.0 * percentOfLife);
  float offsetX = floor(mod(offset, 4.0)) / 4.0;
  float offsetY = 0.75 - floor(offset / 4.0) / 4.0;

  vec4 texColor = texture2D(fireTex, vec2((vTexCoord.x / 4.0) + offsetX, (vTexCoord.y / 4.0) + offsetY));
  vFragColor = baseColor * texColor;
  //vFragColor = baseColor;

  vFragColor.a *= vLifetime; //gets dimmer as fragment older

  if(vFragColor.x == 0 &&vFragColor.y == 0 && vFragColor.z == 0){
	vFragColor.a =0;
  }
}