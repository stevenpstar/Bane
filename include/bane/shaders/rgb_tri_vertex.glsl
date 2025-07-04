std::string rgb_tri_vert_shader_src = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColour;

out vec3 colour;

void main()
{
  gl_Position = vec4(aPos, 1.0);
  colour = aColour;
}
)";
