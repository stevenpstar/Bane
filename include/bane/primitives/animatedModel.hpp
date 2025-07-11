#ifndef BANE_ANIMATED_MODEL
#define BANE_ANIMATED_MODEL
#include "assimp/mesh.h"
#include <array>
#include <glm/fwd.hpp>
#include <glm/gtc/quaternion.hpp>
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

struct RotationKeyFrame
{
  glm::quat rot;
  float timeStamp;
};

struct PositionKeyFrame
{
  glm::mat4 pos;
  float timeStamp;
};

struct ScaleKeyFrame
{
  glm::mat4 scale;
  float timeStamp;
};

struct Bone2
{
  int ID;
  std::string Name;
  glm::mat4 Offset;
  glm::mat4 AnimTransform;
  glm::mat4 LocalTransform;
  glm::mat4 InvTransform;
  std::vector<Bone2> Children;
};

struct Bone
{
  int ID;
  std::string Name;
  glm::mat4 offset;
  std::vector<RotationKeyFrame> rotKeyFrames;
  std::vector<PositionKeyFrame> posKeyFrames;
  std::vector<ScaleKeyFrame> scaleKeyFrames;
};

struct Animation
{
  std::string name;
  std::vector<Bone> bones;
};


class AnimatedModel
{
  public:
    AnimatedModel(const char *path);
    glm::vec3 position;
    void Render(Shader &shader, Camera* cam);
    void SetBoneMatricesUnif(glm::mat4 matrix, Shader* shader, int index);
    void getRotFromFrame(glm::mat4& rot, unsigned int frameIndex, unsigned int bone);
    void getPosFromFrame(glm::mat4& pos, unsigned int frameIndex, unsigned int bone);
    void getScaleFromFrame(glm::mat4& scale, unsigned int frameIndex, unsigned int bone);
    aiNode* GetParentBone(std::string name);
    glm::mat4 GetTransformAtFrame(Bone* bone, unsigned int frameIndex);
    void calcInverseTransform(Bone2* bone, glm::mat4 parentTransform);
    std::array<glm::mat4, 2> boneMatrices;
    std::vector<Animation> anim;
    std::vector<glm::mat4> getTransformsAtTime(std::string animationName, float animTime); 
  private:
    const aiScene* ModelScene;
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> textures_loaded;
    void loadAnimatedModel(std::string path);

    void processNode(aiNode* node, const aiScene *scene);
    Mesh processMesh(aiMesh* mesh, const aiScene *scene);

    void processBones(std::vector<Vertex>& vertices,
        aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(
        aiMaterial* mat, 
        aiTextureType type,
        std::string typeName);
};
#endif
