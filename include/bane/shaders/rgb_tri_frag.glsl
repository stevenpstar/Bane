std::string rgb_tri_frag = R"(
#version 460 core
out vec4 FragColour;

void main()
{
  //FragColour = texture(uitexture, texCoords);// * vec4(1.0, 1.0, 1.0, 1.0);
  FragColour = vec4(1.0, 1.0, 1.0, 1.0);
}
)";
