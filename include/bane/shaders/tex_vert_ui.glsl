std::string tex_vert_ui = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColour;
layout (location = 2) in vec2 aTexCoords;

out vec3 colour;
out vec2 texCoords;

uniform mat4 model;
uniform mat4 projection;

void main()
{
  gl_Position = model * projection * vec4(aPos, 1.0);
  texCoords = aTexCoords;
  colour = aColour;
}

)";
