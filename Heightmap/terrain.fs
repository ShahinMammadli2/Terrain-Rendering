#version 410 core

in vec3 worldpos;
in vec2 uv;

out vec4 FragColor;

uniform vec3 CameraWoldPos;
uniform float FogStart = 150.0;
uniform float FogEnd = 500.0;
uniform vec3 FogColor = vec3(0.7, 0.8, 0.9);

uniform sampler2D terrainTex;

float CalcLinearFogFactor()
{
      float CameraToPixelDist = length(worldpos - CameraWoldPos);
      float FogRange = FogEnd - FogStart;
      float FogDist = FogEnd - CameraToPixelDist;
      float FogFactor = FogDist / FogRange;
      FogFactor = clamp(FogFactor, 0.0, 1.0);
      return FogFactor;
}

void main()
{
    vec3 texColor = texture(terrainTex, uv).rgb;

    if (FogColor != vec3(0.0))
{
    float FogFactor = CalcLinearFogFactor();
    texColor = mix(FogColor, texColor, FogFactor);
}

    FragColor = vec4(texColor, 1.0);
}