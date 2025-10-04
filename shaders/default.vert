#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 projection;

// Novos uniforms para controlar a animação
uniform int nFrames; // Total de frames na spritesheet (no seu caso, 8)
uniform int currentFrame; // Frame atual que queremos desenhar (de 0 a 7)

void main()
{
   gl_Position = projection * model * vec4(aPos, 1.0);

   // Calcula o deslocamento (offset) horizontal da textura
   float texOffset = float(currentFrame) / float(nFrames);

   // Aplica o deslocamento apenas na coordenada X da textura
   TexCoord = vec2(aTexCoord.x + texOffset, aTexCoord.y);
}