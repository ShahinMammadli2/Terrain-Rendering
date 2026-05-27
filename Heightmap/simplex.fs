#version 450 core
in TES_OUT {
    vec3 worldPos;
    vec3 normal;
    vec2 uv;
    float height;
} fs_in;

out vec4 FragColor;

layout(binding = 0) uniform sampler2D uTerrainTex;

// quick directional light
layout(location=20) uniform vec3 uLightDir = normalize(vec3(0.6, 1.0, 0.4));
layout(location=21) uniform vec3 uAlbedo   = vec3(0.45, 0.65, 0.35);

uniform vec3 CameraWoldPos;
uniform float FogStart = 150.0;
uniform float FogEnd = 500.0;
uniform vec3 FogColor = vec3(0.7, 0.8, 0.9);

float CalcLinearFogFactor()
{
      float CameraToPixelDist = length(fs_in.worldPos - CameraWoldPos);
      float FogRange = FogEnd - FogStart;
      float FogDist = FogEnd - CameraToPixelDist;
      float FogFactor = FogDist / FogRange;
      FogFactor = clamp(FogFactor, 0.0, 1.0);
      return FogFactor;
}

void main() {
    vec3 N = normalize(fs_in.normal);
    float ndl = max(dot(N, normalize(uLightDir)), 0.0);

    //sample texture
    vec3 texColor = texture(uTerrainTex, fs_in.uv).rgb;

    //vec3 color = uAlbedo * (0.15 + 0.85 * ndl);

    if (FogColor != vec3(0.0))
{
    float FogFactor = CalcLinearFogFactor();
    texColor = mix(FogColor, texColor, FogFactor);
}
        
    FragColor = vec4(texColor, 1.0);
}
