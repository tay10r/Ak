#include <Ak/ObjMeshModel.h>

#include <glm/glm.hpp>

#include <tiny_obj_loader.h>

#include <map>

#include <cstddef>

namespace Ak {

namespace {

struct Vertex final
{
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texCoords;
};

/// Contains the supported (not all) material attributes from OBJ models.
struct Material final
{
  glm::vec3 albedo;

  glm::vec3 emission;
};

struct Shape final
{
  Material material;

  std::vector<Vertex> vertices;
};

bool
isGoodIndex(const tinyobj::index_t& idx)
{
  return (idx.vertex_index != -1) && (idx.normal_index != -1) && (idx.texcoord_index != -1);
}

} // namespace

class ObjMeshModelImpl final
{
  friend ObjMeshModel;

  std::vector<Shape> m_shapes;
};

ObjMeshModel::ObjMeshModel()
  : m_impl(new ObjMeshModelImpl())
{}

ObjMeshModel::ObjMeshModel(ObjMeshModel&& other)
  : m_impl(other.m_impl)
{
  other.m_impl = nullptr;
}

ObjMeshModel::~ObjMeshModel()
{
  delete m_impl;
}

bool
ObjMeshModel::loadFile(const char* path)
{
  if (!m_impl)
    m_impl = new ObjMeshModelImpl();

  tinyobj::ObjReader objReader;

  if (!objReader.ParseFromFile(path))
    return false;

  const tinyobj::attrib_t& attrib = objReader.GetAttrib();

  const float* vertices = attrib.vertices.data();

  const float* normals = attrib.normals.data();

  const float* texCoords = attrib.texcoords.data();

  auto getPos = [vertices](const tinyobj::index_t& index) -> glm::vec3 {
    const int a = (index.vertex_index * 3) + 0;
    const int b = (index.vertex_index * 3) + 1;
    const int c = (index.vertex_index * 3) + 2;
    return glm::vec3(vertices[a], vertices[b], vertices[c]);
  };

  auto getNormal = [normals](const tinyobj::index_t& index) -> glm::vec3 {
    const int a = (index.normal_index * 3) + 0;
    const int b = (index.normal_index * 3) + 1;
    const int c = (index.normal_index * 3) + 2;
    return glm::vec3(normals[a], normals[b], normals[c]);
  };

  auto getTexCoord = [texCoords](const tinyobj::index_t& index) -> glm::vec2 {
    const int a = (index.texcoord_index * 2) + 0;
    const int b = (index.texcoord_index * 2) + 1;
    return glm::vec2(texCoords[a], texCoords[b]);
  };

  std::map<int, Shape> shapeMap;

  // TODO : copy materials and textures

  for (const tinyobj::shape_t& shape : objReader.GetShapes()) {

    if ((shape.mesh.material_ids.size() * 3) != shape.mesh.indices.size()) {
      assert(false);
      return false;
    }

    for (std::size_t i = 0; i < shape.mesh.indices.size(); i += 3) {

      const tinyobj::mesh_t& mesh = shape.mesh;

      const tinyobj::index_t a = mesh.indices[i];
      const tinyobj::index_t b = mesh.indices[i + 1];
      const tinyobj::index_t c = mesh.indices[i + 2];

      if (!isGoodIndex(a) || !isGoodIndex(b) || !isGoodIndex(c))
        return false;

      const Vertex vertices[3]{ { getPos(a), getNormal(a), getTexCoord(a) },
                                { getPos(b), getNormal(b), getTexCoord(b) },
                                { getPos(c), getNormal(c), getTexCoord(c) } };

      const int materialIndex = mesh.material_ids[i / 3];

      shapeMap[materialIndex].vertices.emplace_back(vertices[0]);
      shapeMap[materialIndex].vertices.emplace_back(vertices[1]);
      shapeMap[materialIndex].vertices.emplace_back(vertices[2]);
    }
  }

  for (auto& shapeEntry : shapeMap) {
    m_impl->m_shapes.emplace_back(std::move(shapeEntry.second));
  }

  return true;
}

std::vector<ObjMeshModel::ShapeView>
ObjMeshModel::getShapeViews() const
{
  std::vector<ShapeView> shapeViews;

  if (!m_impl)
    return shapeViews;

  for (const Shape& shape : m_impl->m_shapes) {

    static_assert(sizeof(Vertex) == (sizeof(float) * 8)); // <- ensure the vertex is packed.

    ShapeView shapeView{ (const float*)shape.vertices.data(), shape.vertices.size() };

    shapeViews.emplace_back(std::move(shapeView));
  }

  return shapeViews;
}

} // namespace Ak
