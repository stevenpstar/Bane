#ifndef BANE_MODEL
#define BANE_MODEL
#include "glm/fwd.hpp"
#include <string>
#include <vector>
#include <bane/primitives/mesh.hpp>
#include <bane/primitives/texture.hpp>
class Shader;
class Camera;
struct aiMesh;
struct aiNode;
struct aiScene;
struct aiMaterial;
enum aiTextureType;
class Model
{
  public:
    Model(const char *path);
    glm::vec3 position;
    void Render(Shader &shader, Camera* cam);
  private:
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> textures_loaded;

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene *scene);
    Mesh processMesh(aiMesh* mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(
        aiMaterial* mat, 
        aiTextureType type,
        std::string typeName);
};
#endif
