#include "glm/common.hpp"
#include "glm/matrix.hpp"
#include <glm/fwd.hpp>
#include <bane/primitives/assimpglmhelpers.hpp>
#include <string>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>
#include <bane/components/camera.hpp>
#include <cstring>
#include <glad/glad.h>
#include <assimp/material.h>
#include <assimp/mesh.h>
#include <assimp/types.h>
#include <stbimage/stb_image.h>
#include <bane/primitives/animatedModel.hpp>
#include <bane/primitives/mesh.hpp>
#include <bane/primitives/vertex.hpp>
#include <bane/primitives/texture.hpp>
#include <bane/utility/shader.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <vector>

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

AnimatedModel::AnimatedModel(const char* path)
{
  for (int i = 0; i < 100; ++i)
  {
    boneMatrices[i] = glm::mat4(1.f);
  }
  this->loadAnimatedModel(path);
}

void AnimatedModel::loadAnimatedModel(std::string path)
{
  Assimp::Importer importer;
  const aiScene *scene = importer.ReadFile(path,
      aiProcess_Triangulate | aiProcess_FlipUVs);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
  {
    std::cout << "Error importing model: " << importer.GetErrorString() << std::endl;
    return;
  }

  glm::mat4 rootTransform = AssimpGLMHelpers::ConvertMatrixToGLMFormat(scene->mRootNode->mTransformation);
  globalInverseMat4 = glm::inverse(rootTransform);
  directory = path.substr(0, path.find_last_of('/'));
  std::cout << "AnimatedModel found in directory: " << directory << std::endl;
  processNode(scene->mRootNode, scene);
  addAnimationData(scene);
  meshes[meshes.size()-1].setupMesh();
}

void AnimatedModel::processNode(aiNode* node, const aiScene* scene)
{
  for (unsigned int i = 0; i < node->mNumMeshes; i++)
  {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    meshes.push_back(processMesh(mesh, scene));
    processBones(meshes[meshes.size()-1].vertices, mesh, scene, node);
    for (auto vert : meshes[meshes.size()-1].vertices)
    {
      if (vert.BoneIds[0] == -1) {
        std::cout << "Vertex found with unassigned bone\n";
      }
    }
    createHierarchy(mesh, scene->mRootNode, scene);
    calcInverseTransform(rootBone, glm::mat4(1.f));
    auto vertices = meshes[meshes.size()-1].vertices;
  }
  for (unsigned int i = 0; i < node->mNumChildren; i++)
  {
    processNode(node->mChildren[i], scene);
  }
}

void AnimatedModel::createHierarchy(aiMesh* mesh, aiNode* node, const aiScene* scene)
{
  const char* nodeChars = node->mName.C_Str();
  std::string nodeName = nodeChars;
  if (namedBoneMap.count(nodeName))
  {
    // we have found a bone. Get parent id (if exists)
    unsigned int parentId = -1;
    aiNode* parentNode = node->mParent;
    if (parentNode)
    {
      const char* parentNodeChars = parentNode->mName.C_Str();
      std::string parentNodeName = parentNodeChars;

      if (namedBoneMap.count(parentNodeName))
      {
        // This is not the root bone, add it to its parents children vector
        boneMap[namedBoneMap[parentNodeName]].Children.push_back(&boneMap[namedBoneMap[nodeName]]);

      } else {
        if (rootBone == nullptr) {
          rootBone = &boneMap[namedBoneMap[nodeName]];
        }
      }
      boneMap[namedBoneMap[nodeName]].LocalTransform = AssimpGLMHelpers::ConvertMatrixToGLMFormat(node->mTransformation);
    }
  }

  for (unsigned int i = 0; i < node->mNumChildren; ++i)
  {
    createHierarchy(mesh, node->mChildren[i], scene);
  }
}

Mesh AnimatedModel::processMesh(aiMesh* mesh, const aiScene* scene)
{
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;
  for (unsigned int i = 0; i < mesh->mNumVertices; i++)
  {
    Vertex vertex;
    glm::vec3 vector;
    vector.x = mesh->mVertices[i].x;
    vector.y = mesh->mVertices[i].y;
    vector.z = mesh->mVertices[i].z;
    vertex.Position = vector;

    vector.x = mesh->mNormals[i].x;
    vector.y = mesh->mNormals[i].y;
    vector.z = mesh->mNormals[i].z;
    vertex.Normal = vector;

    if (mesh->mTextureCoords[0])
    {
      glm::vec2 vec;
      vec.x = mesh->mTextureCoords[0][i].x;
      vec.y = mesh->mTextureCoords[0][i].y;
      vertex.TexCoords = vec;
    } else {
      vertex.TexCoords = glm::vec2(0.f, 0.f);
    }

    for (int j = 0; j < 4; j++)
    {
      vertex.BoneIds[j] = -1;
      vertex.Weights[j] = 0;
    }
    vertices.push_back(vertex);
  }

  for (unsigned int i = 0; i < mesh->mNumFaces; i++)
  {
    aiFace face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++)
      indices.push_back(face.mIndices[j]);
  }

  if (mesh->mMaterialIndex >= 0)
  {
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<Texture> diffuseMaps = loadMaterialTextures(material,
        aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

    std::vector<Texture> specularMaps = loadMaterialTextures(material,
        aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
  }

  return Mesh(vertices, indices, textures);
}

void AnimatedModel::processBones(std::vector<Vertex>& vertices,
    aiMesh* mesh, const aiScene* scene, aiNode* node)
{

  for (unsigned int bi = 0; bi < mesh->mNumBones; ++bi)
  {
    // find node for transform????
    const char* boneName = mesh->mBones[bi]->mName.C_Str();
    if (!boneMap.count(bi))
    {
      Bone bone;
      boneMap[bi] = bone;
      boneMap[bi].ID = bi;
      boneMap[bi].Name = boneName;
      boneMap[bi].Offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[bi]->mOffsetMatrix);
    }
 
    if (!namedBoneMap.count(boneMap[bi].Name))
    {
      namedBoneMap[boneMap[bi].Name] = bi;
    }

    int weightSum = 0;

    for (unsigned int w = 0; w < mesh->mBones[bi]->mNumWeights; ++w)
    {
      weightSum += mesh->mBones[bi]->mWeights[w].mWeight;
      int vertId = mesh->mBones[bi]->mWeights[w].mVertexId;
      float weight = mesh->mBones[bi]->mWeights[w].mWeight;
      if (vertId >= vertices.size())
      {
      }
      for (int i = 0; i < 4; ++i) {
        if (vertices[vertId].BoneIds[i] < 0) {
          vertices[vertId].BoneIds[i] = bi;
          vertices[vertId].Weights[i] = weight;
          break;
        }
      }
      glm::mat4 offsetMatrix = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[bi]->mOffsetMatrix);
      if (bi < 100)
      {
        boneMatrices[bi] = offsetMatrix;
      }
    }
  }
}

void AnimatedModel::addAnimationData(const aiScene* scene)
{
  for (int a = 0; a < scene->mNumAnimations; ++a)
  {
    int animID = a;
    std::string animName = scene->mAnimations[a]->mName.C_Str();
    if (!animationMap.count(animID))
    {
      AnimationMetaData animMD;
      animMD.AnimDuration = scene->mAnimations[a]->mDuration;
      animMD.ticksPerSecond = scene->mAnimations[a]->mTicksPerSecond;
      animationMap[animID] = animMD;
      if (animID == 0)
      {
      }
    } 
    // Channels are bones, basically.
    for (int b = 0; b < scene->mAnimations[a]->mNumChannels; ++b)
    {
      // find our bone, to add an animation object
      std::string boneName = scene->mAnimations[a]->mChannels[b]->mNodeName.C_Str();
      if (!namedBoneMap.count(boneName))
      {
        //std::cout << "Bone not found for animation channel?\n";
        continue;
      }
      Animation anim;
      anim.AnimID = animID;
      anim.AnimName = animName;
      anim.ticksPerSecond = scene->mAnimations[a]->mTicksPerSecond;
      anim.AnimDuration = scene->mAnimations[a]->mDuration;

      if (!boneMap.count(namedBoneMap[boneName]))
      {
        //std::cout << "Bone not found in boneMap (id -> bone)\n";
        return;
      } 
      boneMap[namedBoneMap[boneName]].AnimationData.push_back(anim);

      for (unsigned int rk = 0; rk < scene->mAnimations[a]->mChannels[b]->mNumRotationKeys; ++rk)
      {
        RotationKeyFrame rotKey;
        rotKey.timeStamp = scene->mAnimations[a]->mChannels[b]->mRotationKeys[rk].mTime;
        rotKey.rot = AssimpGLMHelpers::GetGLMQuat(scene->mAnimations[a]->mChannels[b]->mRotationKeys[rk].mValue);
        boneMap[namedBoneMap[boneName]].AnimationData[a].rotKeyFrames.push_back(rotKey);
      }

      for (unsigned int pk = 0; pk < scene->mAnimations[a]->mChannels[b]->mNumPositionKeys; ++pk)
      {
        PositionKeyFrame posKey;
        posKey.timeStamp = scene->mAnimations[a]->mChannels[b]->mPositionKeys[pk].mTime;
        glm::vec3 posVec3 = AssimpGLMHelpers::GetGLMVec(scene->mAnimations[a]->mChannels[b]->mPositionKeys[pk].mValue);
        posKey.pos = posVec3;
        boneMap[namedBoneMap[boneName]].AnimationData[a].posKeyFrames.push_back(posKey);
      }

      for (unsigned int sk = 0; sk < scene->mAnimations[a]->mChannels[b]->mNumScalingKeys; ++sk)
      {
        ScaleKeyFrame scaleKey;
        scaleKey.timeStamp = scene->mAnimations[a]->mChannels[b]->mScalingKeys[sk].mTime;
        glm::vec3 scaleVec3 = AssimpGLMHelpers::GetGLMVec(scene->mAnimations[a]->mChannels[b]->mScalingKeys[sk].mValue);
        scaleKey.scale = scaleVec3;
        boneMap[namedBoneMap[boneName]].AnimationData[a].scaleKeyFrames.push_back(scaleKey);
      }

    }
  }
}

std::vector<Texture> AnimatedModel::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
  std::vector<Texture> textures;
  for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
  {
    aiString str;
    mat->GetTexture(type, i, &str);
    bool skip = false;
    for (unsigned int j = 0; j < textures_loaded.size(); j++)
    {
      if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
      {
        textures.push_back(textures_loaded[j]);
        skip = true;
        break;
      }
    }
    if (!skip)
    {
      Texture texture;
      texture.id = TextureFromFile(str.C_Str(), directory);
      texture.type = typeName;
      texture.path = str.C_Str();
      textures.push_back(texture);
      textures_loaded.push_back(texture);
    }
  }
  return textures;
}

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma)
{
  std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void AnimatedModel::Render(Shader &shader, Camera* camera)
{
  for (unsigned int i = 0; i < meshes.size(); i++)
  {
    meshes[i].Render(shader, camera);
  }
}

void AnimatedModel::PlayAnimation(int animIndex)
{
  //Reset animation time
  animTime = 0.f;
  currentAnimationIndex = animIndex;
}

void AnimatedModel::StopAnimation()
{
  animTime = 0.f;
  currentAnimationIndex = -1;
}

bool AnimatedModel::IsPlayingAnimation()
{
  return currentAnimationIndex > -1;
}

void AnimatedModel::UpdateAnimation(float dt)
{
  if (currentAnimationIndex == -1)
  {
    // No animation is playing, early return
    return;
  }

  this->animTime += animationMap[currentAnimationIndex].ticksPerSecond * dt;
  //this->animTime += 12 * dt;
  if (!animationMap.count(currentAnimationIndex))
  {
    std::cout << "Animation meta data not found for index: " << currentAnimationIndex << "\n";
    return;
  }
  animTime = fmod(animTime, animationMap[currentAnimationIndex].AnimDuration);
  calcAnimTransform(rootBone, glm::mat4(1.f));
}

void AnimatedModel::calcAnimTransform(Bone* bone, glm::mat4 transform)
{
  // maybe need to take into account transforms other than bones? Don't think so. Just a note.

  glm::mat4 translation = interpPos(bone);
  glm::mat4 rotation = interpRot(bone);
  glm::mat4 scale = interpScale(bone);
  glm::mat4 interpedTransform = translation * rotation * scale;
  glm::mat4 globalTransform = transform * interpedTransform;
  if (bone->ID >= boneMatrices.size())
  {
    std::cout<< "Bone id out of range of bone matrices: " << bone->ID << ", " << "matrices count: " << boneMatrices.size() << "\n";
    return;
  }
 
  boneMatrices[bone->ID] = globalInverseMat4 * globalTransform * bone->Offset;
  for (int i = 0; i < bone->Children.size(); ++i)
  {
    calcAnimTransform(bone->Children[i], globalTransform);
  }
}

void AnimatedModel::SetBoneMatricesUnif(Shader* shader)
{
  for (int index = 0; index < boneMatrices.size(); ++index)
  {
    shader->setMat4("finalBonesMatrices[" + std::to_string(index) + "]", boneMatrices[index], 1);
  }
}

void AnimatedModel::calcInverseTransform(Bone* bone, glm::mat4 parentTransform)
{
  glm::mat4 transform = parentTransform * bone->LocalTransform;
  bone->InvTransform = transform;
  for (Bone* child : bone->Children)
  {
    calcInverseTransform(child, transform);
  }
}

// Probably move this out of the animatedModel class eventually, lets just get things working for now
float AnimatedModel::getInterpDelta(float prevFrameTime, float nextFrameTime)
{
  float halfPoint = animTime - prevFrameTime;
  float diff = nextFrameTime - prevFrameTime;
  float delta = halfPoint / diff;
  return delta;
}

int AnimatedModel::getCurrentPosFrameIndex(Bone* bone)
{
  if (currentAnimationIndex < 0)
  {
    return 0;
  }
  for (int i = 0; i < bone->AnimationData[currentAnimationIndex].posKeyFrames.size() - 1; ++i)
  {
    if (animTime < bone->AnimationData[currentAnimationIndex].posKeyFrames[i + 1].timeStamp)
    {
      return i;
    }
  }
  // We should not get here, but worst case we just return the first frame
  return 0;
}

int AnimatedModel::getCurrentScaleFrameIndex(Bone* bone)
{
  if (currentAnimationIndex < 0)
  {
    return 0;
  }
 
  for (int i = 0; i < bone->AnimationData[currentAnimationIndex].scaleKeyFrames.size() - 1; ++i)
  {
    if (animTime < bone->AnimationData[currentAnimationIndex].scaleKeyFrames[i + 1].timeStamp)
    {
      return i;
    }
  }
  // We should not get here, but worst case we just return the first frame
  return 0;
}

int AnimatedModel::getCurrentRotFrameIndex(Bone* bone)
{
  if (currentAnimationIndex < 0)
  {
    return 0;
  }

  for (int i = 0; i < bone->AnimationData[currentAnimationIndex].rotKeyFrames.size() - 1; ++i)
  {
    if (animTime < bone->AnimationData[currentAnimationIndex].rotKeyFrames[i + 1].timeStamp)
    {
      return i;
    }
  }
  // We should not get here, but worst case we just return the first frame
  return 0;
}

glm::mat4 AnimatedModel::interpPos(Bone* bone)
{
  if (bone->AnimationData[currentAnimationIndex].posKeyFrames.size() == 1)
  {
    return glm::translate(glm::mat4(1.f), bone->AnimationData[currentAnimationIndex].posKeyFrames[0].pos);
  }

  int animIndex = getCurrentPosFrameIndex(bone);
  if (animIndex == bone->AnimationData[currentAnimationIndex].posKeyFrames.size() - 1)
  {
    // I don't think this can actually happen? But just in case.
    return glm::translate(glm::mat4(1.f), bone->AnimationData[currentAnimationIndex].posKeyFrames[animIndex].pos);
  }

  float interpDelta = getInterpDelta(bone->AnimationData[currentAnimationIndex].posKeyFrames[animIndex].timeStamp, 
      bone->AnimationData[currentAnimationIndex].posKeyFrames[animIndex + 1].timeStamp);
  glm::vec3 interpedPos = glm::mix(
      bone->AnimationData[currentAnimationIndex].posKeyFrames[animIndex].pos,
      bone->AnimationData[currentAnimationIndex].posKeyFrames[animIndex + 1].pos,
      interpDelta);
  return glm::translate(glm::mat4(1.f), interpedPos);
}

glm::mat4 AnimatedModel::interpScale(Bone* bone)
{
  if (bone->AnimationData[currentAnimationIndex].scaleKeyFrames.size() == 1)
  {
    return glm::translate(glm::mat4(1.f), bone->AnimationData[currentAnimationIndex].scaleKeyFrames[0].scale);
  }

  int animIndex = getCurrentScaleFrameIndex(bone);
  if (animIndex == bone->AnimationData[currentAnimationIndex].scaleKeyFrames.size() - 1)
  {
    // I don't think this can actually happen? But just in case.
    return glm::translate(glm::mat4(1.f), bone->AnimationData[currentAnimationIndex].scaleKeyFrames[animIndex].scale);
  }

  float interpDelta = getInterpDelta(bone->AnimationData[currentAnimationIndex].scaleKeyFrames[animIndex].timeStamp, 
      bone->AnimationData[currentAnimationIndex].scaleKeyFrames[animIndex + 1].timeStamp);

  glm::vec3 interpedScale = glm::mix(
      bone->AnimationData[currentAnimationIndex].scaleKeyFrames[animIndex].scale,
      bone->AnimationData[currentAnimationIndex].scaleKeyFrames[animIndex + 1].scale,
      interpDelta);
  return glm::scale(glm::mat4(1.f), interpedScale);
}

glm::mat4 AnimatedModel::interpRot(Bone* bone)
{
  if (bone->AnimationData[currentAnimationIndex].rotKeyFrames.size() == 1)
  {
    return glm::toMat4(glm::normalize(bone->AnimationData[currentAnimationIndex].rotKeyFrames[0].rot));
  }

  int animIndex = getCurrentRotFrameIndex(bone);
  if (animIndex == bone->AnimationData[currentAnimationIndex].rotKeyFrames.size() - 1)
  {
    // I don't think this can actually happen? But just in case.
    return glm::toMat4(glm::normalize(bone->AnimationData[currentAnimationIndex].rotKeyFrames[animIndex].rot));
  }

 float interpDelta = getInterpDelta(bone->AnimationData[currentAnimationIndex].rotKeyFrames[animIndex].timeStamp, 
    bone->AnimationData[currentAnimationIndex].rotKeyFrames[animIndex + 1].timeStamp);
  glm::quat slerpedQuat = glm::slerp(
      bone->AnimationData[currentAnimationIndex].rotKeyFrames[animIndex].rot,
      bone->AnimationData[currentAnimationIndex].rotKeyFrames[animIndex + 1].rot,
      interpDelta);
  slerpedQuat = glm::normalize(slerpedQuat);
  return glm::toMat4(slerpedQuat);
}
