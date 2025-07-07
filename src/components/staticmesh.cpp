#include <bane/primitives/model.hpp>
#include <bane/utility/shader.hpp>
#include <bane/components/staticmesh.hpp>

StaticMesh::StaticMesh(Model* m,
    Shader* shdr)
{
  shader = shdr;
  model = m;
}

void StaticMesh::Render()
{}
