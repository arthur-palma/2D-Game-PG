#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 projection;

uniform int nFrames;
uniform int currentFrame;

void main()
{
   gl_Position = projection * model * vec4(aPos, 1.0);

   // Calcula a largura de um único frame no espaço da textura (ex: 1.0 / 10.0 = 0.1)
   float frameWidth = 1.0 / float(nFrames);

   // Calcula o deslocamento para o frame atual (ex: frame 3 * 0.1 = 0.3)
   float texOffset = float(currentFrame) * frameWidth;

   // Aplica a escala e o deslocamento à coordenada de textura
   // aTexCoord.x agora é 0 ou 1, representando o início ou fim do frame
   TexCoord.x = (aTexCoord.x * frameWidth) + texOffset;
   TexCoord.y = aTexCoord.y;
}