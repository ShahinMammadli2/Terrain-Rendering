#version 410 core

//in float Height;
in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D uTexture0;

void main()
{
    //float h = (Height + 16)/64.0f;
     //FragColor = vec4(h, h, h, 1.0);
     FragColor = texture(utexture0, TexCoords);
}