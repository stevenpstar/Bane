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
  for (int i = 0; i < 2; ++i)
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

  directory = path.substr(0, path.find_last_of('/'));
  std::cout << "AnimatedModel found in directory: " << directory << std::endl;
  processNode(scene->mRootNode, scene);
}

void AnimatedModel::processNode(aiNode* node, const aiScene* scene)
{
  for (unsigned int i = 0; i < node->mNumMeshes; i++)
  {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    meshes.push_back(processMesh(mesh, scene));
    std::cout<<"Do we finish processing mesh?\n";
    processBones(meshes[meshes.size()-1].vertices, mesh, scene);
    std::cout<<"Do we finish processing bones?\n";
    for (int i = 0; i < anim[0].bones.size(); ++i)
    {
      std::cout << "Anim 0 bone : " << i << " " << anim[0].bones[i].Name << "\n";
    }
    meshes[meshes.size()-1].setupMesh();
    auto vertices = meshes[meshes.size()-1].vertices;
  }
  for (unsigned int i = 0; i < node->mNumChildren; i++)
  {
    processNode(node->mChildren[i], scene);
  }
}

Mesh AnimatedModel::processMesh(aiMesh* mesh, const aiScene* scene)
{
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;
  ModelScene = scene;
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
    aiMesh* mesh, const aiScene* scene)
{

  std::string rootName = "Bone";
  for (unsigned int bi = 0; bi < mesh->mNumBones; ++bi)
  {
    for (unsigned int w = 0; w < mesh->mBones[bi]->mNumWeights; ++w)
    {
      int vertId = mesh->mBones[bi]->mWeights[w].mVertexId;
      float weight = mesh->mBones[bi]->mWeights[w].mWeight;
      if (vertId >= vertices.size())
      {
        std::cout << "VertId larger than vertices vector\n";
      }
      vertices[vertId].BoneIds[bi] = bi;
      vertices[vertId].Weights[bi] = weight;
      glm::mat4 offsetMatrix = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[bi]->mOffsetMatrix);
      if (bi < 2)
      {
        boneMatrices[bi] = offsetMatrix;
      }
    }
  }

  for (unsigned int ai = 0; ai < scene->mNumAnimations; ++ai)
  {
    Animation animations;
    for (unsigned int ci = 0; ci < scene->mAnimations[ai]->mNumChannels; ++ci)
    {
      std::cout << " Number of channels : " << scene->mAnimations[ai]->mNumChannels << "\n";
      Bone bone;
      bone.ID = - 1;
      for (unsigned int bi = 0; bi < mesh->mNumBones; ++bi)
      {
        const char* boneNameChar = mesh->mBones[bi]->mName.C_Str();
        std::string boneName = boneNameChar;

        const char* nodeNameChar = scene->mAnimations[ai]->mChannels[ci]->mNodeName.C_Str();
        std::string nodeName = nodeNameChar;

        if (boneName == nodeName)
        {
          std::cout << "We found it!\n";
          bone.ID = bi;
          bone.Name = scene->mAnimations[ai]->mChannels[ci]->mNodeName.C_Str();
        }
      }
      if (bone.ID == -1)
      {
        continue;
      }
      animations.name = scene->mAnimations[ai]->mName.C_Str();
      //bone.Name = mesh->mBones[bi]->mName.C_Str();
     // if (mesh->mBones[bi]->mNode->mParent)
     // {
     //   bone.parentBone = mesh->mBones[bi]->mNode->mParent;
     // }
      std::cout <<"Random are we setting bone offset (we about to: " << bone.ID << "\n";
      bone.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[bone.ID]->mOffsetMatrix);
      std::cout << "Anim: " << scene->mAnimations[ai]->mName.C_Str() << "\n";
      for (unsigned int rk = 0; rk < scene->mAnimations[ai]->mChannels[ci]->mNumRotationKeys; ++rk)
      {
        RotationKeyFrame rotKey;
        rotKey.timeStamp = scene->mAnimations[ai]->mChannels[ci]->mRotationKeys[rk].mTime;
        rotKey.rot = AssimpGLMHelpers::GetGLMQuat(scene->mAnimations[ai]->mChannels[ci]->mRotationKeys[rk].mValue);
        bone.rotKeyFrames.push_back(rotKey);
      }

      for (unsigned int pk = 0; pk < scene->mAnimations[ai]->mChannels[ci]->mNumPositionKeys; ++pk)
      {
        PositionKeyFrame posKey;
        posKey.timeStamp = scene->mAnimations[ai]->mChannels[ci]->mPositionKeys[pk].mTime;
        glm::vec3 posVec3 = AssimpGLMHelpers::GetGLMVec(scene->mAnimations[ai]->mChannels[ci]->mPositionKeys[pk].mValue);
        posKey.pos = glm::translate(glm::mat4(1.f), posVec3);
        bone.posKeyFrames.push_back(posKey);
      }

      for (unsigned int sk = 0; sk < scene->mAnimations[ai]->mChannels[ci]->mNumPositionKeys; ++sk)
      {
        ScaleKeyFrame scaleKey;
        scaleKey.timeStamp = scene->mAnimations[ai]->mChannels[ci]->mScalingKeys[sk].mTime;
        glm::vec3 scaleVec3 = AssimpGLMHelpers::GetGLMVec(scene->mAnimations[ai]->mChannels[ci]->mScalingKeys[sk].mValue);
        scaleKey.scale = glm::scale(glm::mat4(1.f), scaleVec3);
        bone.scaleKeyFrames.push_back(scaleKey);
      }

      animations.bones.push_back(bone);
      /// temp just to see what key frames we have
//        for (auto rotFrame : bone.rotKeyFrames)
//        {
//          std::cout << "Bone rot: " << glm::to_string(rotFrame.rot) << "\n";
//        }
//
//        for (auto posFrame : bone.posKeyFrames)
//        {
//          std::cout << "Bone pos: " << glm::to_string(posFrame.pos) << "\n\n";
//          std::cout << "Bone pos timeStamp: " << posFrame.timeStamp << "\n\n";
//        }
//
//        for (auto scaleFrame : bone.scaleKeyFrames)
//        {
//          std::cout << "Bone scale: " << glm::to_string(scaleFrame.scale) << "\n\n";
//          std::cout << "Bone scale timeStamp: " << scaleFrame.timeStamp << "\n\n";
//        }
//
    }
    anim.push_back(animations);
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

void AnimatedModel::SetBoneMatricesUnif(glm::mat4 matrix, Shader* shader, int index)
{
  shader->setMat4("finalBonesMatrices[" + std::to_string(index) + "]", matrix, 100);
}

void AnimatedModel::getRotFromFrame(glm::mat4& rot, unsigned int frameIndex, unsigned int bone)
{
  if (anim.size() == 0)
  {
    std::cout << "No Animation found!\n";
    return;
  }
  if (anim[0].bones.size() == 0)
  {
    std::cout << "No bones found!\n";
    return;
  }
  if (frameIndex > anim[0].bones[bone].rotKeyFrames.size() - 1)
  {
    frameIndex = 0;
  }

  glm::mat4 parentTransform = glm::mat4(1.f);
  for (Bone b : anim[0].bones)
  {
    if (b.Name == "Bone")
    {
      if (frameIndex < b.rotKeyFrames.size() && frameIndex != 0)
      {
        parentTransform = glm::toMat4(b.rotKeyFrames[frameIndex].rot);
        glm::mat4 parentOffset = b.offset;
        parentTransform *= parentOffset;
      }
    }
  }
 // rot = glm::toMat4(anim[0].bones[bone].rotKeyFrames[frameIndex].rot) * parentTransform * anim[0].bones[bone].offset;
  if (bone == 1)
  {
    rot = glm::toMat4(anim[0].bones[bone].rotKeyFrames[frameIndex].rot) * glm::inverse(parentTransform) * anim[0].bones[bone].offset;
  } else {
    rot = glm::toMat4(anim[0].bones[bone].rotKeyFrames[frameIndex].rot);
  }
  //std::cout<<"Bone: " << anim[0].bones[bone].Name << " Transform: " << glm::to_string(rot) << "\n";
}

void AnimatedModel::getPosFromFrame(glm::mat4& pos, unsigned int frameIndex, unsigned int bone)
{
  if (anim.size() == 0)
  {
    std::cout << "No Animation found!\n";
    return;
  }
  if (anim[0].bones.size() == 0)
  {
    std::cout << "No bones found!\n";
    return;
  }
  if (frameIndex > anim[0].bones[bone].posKeyFrames.size() - 1)
  {
    frameIndex = 0;
  }

  pos = anim[0].bones[bone].posKeyFrames[frameIndex].pos;
}

void AnimatedModel::getScaleFromFrame(glm::mat4& scale, unsigned int frameIndex, unsigned int bone)
{
  if (anim.size() == 0)
  {
    std::cout << "No Animation found!\n";
    return;
  }
  if (anim[0].bones.size() == 0)
  {
    std::cout << "No bones found!\n";
    return;
  }
  if (frameIndex > anim[0].bones[bone].scaleKeyFrames.size() - 1)
  {
    frameIndex = 0;
  }

  scale = anim[0].bones[bone].scaleKeyFrames[frameIndex].scale;
}

glm::mat4 AnimatedModel::GetTransformAtFrame(Bone* bone, unsigned int frameIndex)
{
  glm::mat4 pos = glm::mat4(1.f);
  glm::mat4 rot = glm::mat4(1.f);
  glm::mat4 scl = glm::mat4(1.f);

  if (frameIndex < bone->posKeyFrames.size())
  {
    pos = bone->posKeyFrames[frameIndex].pos;
  }

  if (frameIndex < bone->rotKeyFrames.size())
  {
    rot = glm::toMat4(bone->rotKeyFrames[frameIndex].rot);
    glm::mat4 parentTransform = glm::mat4(1.f);
  }

  if (frameIndex < bone->scaleKeyFrames.size())
  {
    scl = bone->scaleKeyFrames[frameIndex].scale;
  }


  return pos * rot * scl;
}

void AnimatedModel::calcInverseTransform(Bone2* bone, glm::mat4 parentTransform)
{
  glm::mat4 transform = parentTransform * bone->LocalTransform;
  bone->InvTransform = glm::inverse(transform);
  for (Bone2 child : bone->Children)
  {
    calcInverseTransform(&child, transform);
  }
}

