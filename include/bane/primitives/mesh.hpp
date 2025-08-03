#ifndef BANE_MESH
#define BANE_MESH
#include <vector>
#include <bane/primitives/vertex.hpp>
#include <bane/primitives/texture.hpp>
class Shader;
class Camera;
struct aiScene;
struct aiMesh;
class Mesh
{
  public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    Mesh(
        std::vector<Vertex> verts,
        std::vector<unsigned int> ind,
        std::vector<Texture> tex);

    void Render(glm::vec3 pos, glm::vec3 rotation, Shader* shader, Camera* camera);
    void Render(glm::mat4 transform, Shader* shader, Camera* camera, unsigned int shadowTex);
    void RenderBasic();
    void setupMesh();
    unsigned int VAO, VBO, EBO;
  private:
};
#endif
