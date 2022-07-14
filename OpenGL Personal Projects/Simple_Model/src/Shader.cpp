#include "Shader.h"

std::string get_file_contents(const char* filepath)
{
    std::ifstream in(filepath, std::ios::binary);
    if(in)
    {
        std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
    }
    throw(errno);

}


Shader::Shader(const char* vertexShaderSource, const char* fragmentShaderSource)
{
    // Read vertexFile and fragmentFile and store the strings
	std::string vertexCode = get_file_contents(vertexShaderSource);
	std::string fragmentCode = get_file_contents(fragmentShaderSource);

	// Convert the shader source strings into character arrays
	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

    // Create Vertex Shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" <<
        infoLog << std::endl;
    };

	// Create Fragment Shader Object and get its reference
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" <<
        infoLog << std::endl;
    };

	// Create Shader Program 
	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" <<
        infoLog << std::endl;
    }

    // Delete the now useless Vertex and Fragment Shader
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::setVectorUniform(glm::vec3 &vec, const char* name)
{
    int uniformLocation = glGetUniformLocation(ID, name);
    glUniform3fv(uniformLocation, 1, glm::value_ptr(vec));
}

void Shader::setMatrixUniform(glm::mat4 &mat, const char* name)
{
    int uniformLocation = glGetUniformLocation(ID, name);
    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setFloatUniform(float &t, const char* name)
{
    int uniformLocation = glGetUniformLocation(ID, name);
    glUniform1f(uniformLocation, t);
}

void Shader::Activate()
{
	glUseProgram(ID);
}

void Shader::Delete()
{
	glDeleteProgram(ID);
}