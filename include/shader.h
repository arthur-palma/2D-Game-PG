// include/Shader.h (VERSÃO CORRIGIDA)

#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp> // Necessário para glm::mat4
#include <string>

class Shader
{
public:
    // O ID do programa
    unsigned int ID;

    // Construtor
    Shader(const char* vertexPath, const char* fragmentPath);

    // Métodos
    void use();
    // DECLARAÇÕES ADICIONADAS
    void setMat4(const std::string &name, const glm::mat4 &mat) const;
    void setInt(const std::string &name, int value) const;

private:
    // Função de checagem de erros
    void checkCompileErrors(unsigned int shader, std::string type);
};

#endif