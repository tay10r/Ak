#pragma once

#include <Ak/ObjMeshModel.h>

#include <bvh/bvh.hpp>
#include <bvh/primitive_intersectors.hpp>
#include <bvh/single_ray_traverser.hpp>
#include <bvh/sweep_sah_builder.hpp>
#include <bvh/triangle.hpp>

#include <cstddef>

namespace Ak {

template<typename Float>
class RTMeshModel final
{
public:
  using Vec2 = bvh::Vector<Float, 2>;

  using Vec3 = bvh::Vector<Float, 3>;

  using Triangle = bvh::Triangle<Float>;

  using Bvh = bvh::Bvh<Float>;

  using Ray = bvh::Ray<Float>;

  using ClosestIntersector = bvh::ClosestPrimitiveIntersector<Bvh, Triangle, true>;

  using AnyIntersector = bvh::AnyPrimitiveIntersector<Bvh, Triangle, true>;

  using Traverser = bvh::SingleRayTraverser<Bvh>;

  using ClosestHit = typename ClosestIntersector::Result;

  using AnyHit = typename AnyIntersector::Result;

  struct Attrib final
  {
    Vec3 normals[3];

    Vec2 texCoords[3];
  };

  static std::vector<RTMeshModel> fromObjModel(const ObjMeshModel& objMeshModel);

  void commit();

  void useObjModel(const ObjMeshModel& objMeshModel);

  std::optional<AnyHit> findAnyHit(const Ray& ray) const;

  std::optional<ClosestHit> findClosestHit(const Ray& ray) const;

  const Triangle& getTriangle(size_t index) const noexcept { return m_triangles[index]; }

  const Attrib& getAttrib(size_t index) const noexcept { return m_attribs[index]; }

private:
  static std::size_t getTriangleCount(const ObjMeshModel& objMeshModel);

private:
  size_t m_triangleCount = 0;

  std::unique_ptr<Triangle[]> m_triangles;

  std::unique_ptr<Attrib[]> m_attribs;

  Bvh m_bvh;
};

template<typename Float>
std::vector<RTMeshModel<Float>>
RTMeshModel<Float>::fromObjModel(const ObjMeshModel& objMeshModel)
{
  std::vector<RTMeshModel<Float>> output;

  for (const ObjMeshModel::ShapeView& shapeView : objMeshModel.getShapeViews()) {

    output.emplace_back();

    RTMeshModel& rtMeshModel = output.back();

    rtMeshModel.m_triangles.reset(new Triangle[shapeView.vertexCount / 3]);

    rtMeshModel.m_triangleCount = shapeView.vertexCount / 3;

    for (size_t i = 0; i < shapeView.vertexCount; i += 3) {

      const Vec3 p0(shapeView.px(i + 0), shapeView.py(i + 0), shapeView.pz(i + 0));
      const Vec3 p1(shapeView.px(i + 1), shapeView.py(i + 1), shapeView.pz(i + 1));
      const Vec3 p2(shapeView.px(i + 2), shapeView.py(i + 2), shapeView.pz(i + 2));

      const Vec3 n0(shapeView.nx(i + 0), shapeView.ny(i + 0), shapeView.nz(i + 0));
      const Vec3 n1(shapeView.nx(i + 1), shapeView.ny(i + 1), shapeView.nz(i + 1));
      const Vec3 n2(shapeView.nx(i + 2), shapeView.ny(i + 2), shapeView.nz(i + 2));

      const Vec2 t0(shapeView.tx(i + 0), shapeView.ty(i + 0));
      const Vec2 t1(shapeView.tx(i + 1), shapeView.ty(i + 1));
      const Vec2 t2(shapeView.tx(i + 2), shapeView.ty(i + 2));

      rtMeshModel.m_triangles[i] = Triangle(p0, p1, p2);

      rtMeshModel.m_attribs[i] = Attrib{ { n0, n1, n2 }, { t0, t1, t2 } };
    }
  }

  return output;
}

template<typename Float>
void
RTMeshModel<Float>::useObjModel(const ObjMeshModel& objMeshModel)
{
  m_triangleCount = getTriangleCount(objMeshModel);

  m_triangles.reset(new Triangle[m_triangleCount]);

  size_t triangleIndex = 0;

  for (const ObjMeshModel::ShapeView& shapeView : objMeshModel.getShapeViews()) {

    for (size_t i = 0; i < shapeView.vertexCount; i += 3) {

      const Vec3 p0(shapeView.px(i + 0), shapeView.py(i + 0), shapeView.pz(i + 0));
      const Vec3 p1(shapeView.px(i + 1), shapeView.py(i + 1), shapeView.pz(i + 1));
      const Vec3 p2(shapeView.px(i + 2), shapeView.py(i + 2), shapeView.pz(i + 2));

      m_triangles[triangleIndex] = Triangle(p0, p1, p2);

      triangleIndex++;
    }
  }
}

template<typename Float>
std::size_t
RTMeshModel<Float>::getTriangleCount(const ObjMeshModel& objMeshModel)
{
  std::size_t triCount = 0;

  for (const ObjMeshModel::ShapeView& shapeView : objMeshModel.getShapeViews())
    triCount += shapeView.vertexCount / 3;

  return triCount;
}

template<typename Float>
void
RTMeshModel<Float>::commit()
{
  auto [bboxes, centers] = bvh::compute_bounding_boxes_and_centers(m_triangles.get(), m_triangleCount);

  auto global_bbox = bvh::compute_bounding_boxes_union(bboxes.get(), m_triangleCount);

  bvh::SweepSahBuilder<Bvh> builder(m_bvh);

  builder.build(global_bbox, bboxes.get(), centers.get(), m_triangleCount);

  m_triangles = permute_primitives(m_triangles.get(), m_bvh.primitive_indices.get(), m_triangleCount);
}

template<typename Float>
auto
RTMeshModel<Float>::findAnyHit(const Ray& ray) const -> std::optional<AnyHit>
{
  AnyIntersector intersector(m_bvh, m_triangles.get());

  Traverser traverser(m_bvh);

  return traverser.traverse(ray, intersector);
}

template<typename Float>
auto
RTMeshModel<Float>::findClosestHit(const Ray& ray) const -> std::optional<ClosestHit>
{
  ClosestIntersector intersector(m_bvh, m_triangles.get());

  Traverser traverser(m_bvh);

  return traverser.traverse(ray, intersector);
}

} // namespace Ak
