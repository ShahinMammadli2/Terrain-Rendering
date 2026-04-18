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

void main() {
    vec3 N = normalize(fs_in.normal);
    float ndl = max(dot(N, normalize(uLightDir)), 0.0);

    //sample texture
    vec3 texColor = texture(uTerrainTex, fs_in.uv).rgb;

    vec3 color = uAlbedo * (0.15 + 0.85 * ndl);
    FragColor = vec4(texColor, 1.0);
}
