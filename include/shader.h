#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    // O ID do programa
    unsigned int ID;

    // Construtor lê e constrói o shader
    Shader(const char* vertexPath, const char* fragmentPath);

    // Ativa o shader
    void use();

private:
    // Função para checar erros de compilação/linkagem
    void checkCompileErrors(unsigned int shader, std::string type);
};

#endif