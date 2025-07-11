std::string tex_vert_nolit_shader_src = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in ivec4 boneIds;
layout (location = 4) in vec4 weights;

out vec3 position;
out vec2 texCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const int MAX_BONES = 2;
const int MAX_BONE_INFLUENCE = 4;

uniform mat4 finalBonesMatrices[2];

void main()
{
  mat4 totalPosition = finalBonesMatrices[boneIds[0]] * weights[0];
  totalPosition += finalBonesMatrices[boneIds[1]] * weights[1];
  totalPosition += finalBonesMatrices[boneIds[2]] * weights[2];
  totalPosition += finalBonesMatrices[boneIds[3]] * weights[3];
  vec4 posL = totalPosition * vec4(aPos, 1.0);
  gl_Position = projection * view * model * posL;//vec4(aPos, 1.0);
  //gl_Position = projection * view * model * vec4(aPos, 1.0);
 // position = gl_Position.xyz;
  texCoords = aTexCoords;
}

)";
