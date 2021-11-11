#pragma once

#include <vector>

#include <cstddef>

namespace Ak {

class ObjMeshModelImpl;

class ObjMeshModel final
{
public:
  struct ShapeView final
  {
    const float* vertexBuffer = nullptr;

    std::size_t vertexCount = 0;

    constexpr float px(size_t vertexIndex) const noexcept { return vertexBuffer[(vertexIndex * 8) + 0]; }
    constexpr float py(size_t vertexIndex) const noexcept { return vertexBuffer[(vertexIndex * 8) + 1]; }
    constexpr float pz(size_t vertexIndex) const noexcept { return vertexBuffer[(vertexIndex * 8) + 2]; }

    constexpr float nx(size_t vertexIndex) const noexcept { return vertexBuffer[(vertexIndex * 8) + 3]; }
    constexpr float ny(size_t vertexIndex) const noexcept { return vertexBuffer[(vertexIndex * 8) + 4]; }
    constexpr float nz(size_t vertexIndex) const noexcept { return vertexBuffer[(vertexIndex * 8) + 5]; }

    constexpr float tx(size_t vertexIndex) const noexcept { return vertexBuffer[(vertexIndex * 8) + 6]; }
    constexpr float ty(size_t vertexIndex) const noexcept { return vertexBuffer[(vertexIndex * 8) + 7]; }
  };

  ObjMeshModel();

  ObjMeshModel(ObjMeshModel&&);

  ~ObjMeshModel();

  bool loadFile(const char* path);

  std::vector<ShapeView> getShapeViews() const;

private:
  ObjMeshModelImpl* m_impl;
};

} // namespace Ak
