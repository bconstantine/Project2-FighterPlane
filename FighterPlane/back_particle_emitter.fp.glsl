#version 430
precision mediump float; //set float accuracy a little bit higher

uniform vec4 baseColor;
uniform float fragmentTime;

in float vLifetime;
in vec2 vTexCoord;
in vec4 fragPos;

out vec4 vFragColor;


void main () {
      float time = mod(fragmentTime, vLifetime);
      float percentOfLife = time / vLifetime;
      percentOfLife = clamp(percentOfLife, 0.0, 1.0);

      //we use a tiled texture, so we should count the offset
      vFragColor = baseColor;
      vFragColor = mix(vFragColor, vec4(0.415, 0.051, 0.6784, 1.0), (1-percentOfLife));
      vFragColor.a *= (1-percentOfLife);
  
}