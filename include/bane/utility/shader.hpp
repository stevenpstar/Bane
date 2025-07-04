#ifndef BANE_SHADER
#define BANE_SHADER

#include <glm/fwd.hpp>
#include <string>

class Shader
{
  public:
    unsigned int ID;
    Shader(const char* vertexPath, const char* fragmentPath);
    Shader(bool hey, std::string vertexString, std::string fragmentString);

    void use();
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setMat4(const std::string &name, glm::mat4 value) const;
    void setVec3(const std::string &name, glm::vec3 value) const;

};

#endif
