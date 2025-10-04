#version 330 core
out vec4 FragColor;

in vec2 TexCoord; // Recebe do vertex shader

// A textura que vamos usar
uniform sampler2D ourTexture;

void main()
{
   // Pega a cor da textura na coordenada específica e a usa como cor final
   FragColor = texture(ourTexture, TexCoord);
}