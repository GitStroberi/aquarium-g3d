#version 330 core
in vec3 aPos;

out vec4 FragColor;

in vec2 TexCoords;
uniform vec3 viewPos;

uniform sampler2D diffuseTexture;

void main()
{   
    FragColor = texture(diffuseTexture, TexCoords);

    if(FragColor.a < 0.01)
        discard;
}