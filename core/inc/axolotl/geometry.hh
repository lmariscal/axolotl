#pragma once

#include <axolotl/types.hh>

namespace axl {

  class Ray;
  class Sphere;
  class AABB;
  class OBB;
  class Triangle;
  class Plane;
  class Line;
  class Interval;

  class Interval {
   public:
    f32 min;
    f32 max;

    inline Interval(): min(0.0f), max(0.0f) { }
    inline Interval(f32 min, f32 max): min(min), max(max) { }

    inline f32 GetLength() const { }
  };

  class Ray {
   public:
    v3 position;

    inline Ray(): direction(0.0f, 0.0f, 1.0f) { }
    inline Ray(const v3 &position, const v3 &direction = v3(0.0f, 0.0f, 1.0f)): position(position) {
      SetDirection(direction);
    }

    inline void SetDirection(const v3 &direction) {
      this->direction = normalize(direction);
    }

    inline const v3 &GetDirection() const {
      return direction;
    }

    static inline Ray FromPoints(const v3 &from, const v3 &to) {
      return Ray(from, normalize(to - from));
    }

    bool PointInside(const v3 &point) const;
    v3 ClosestPoint(const v3 &point) const;

   protected:
    v3 direction;
  };

  class Sphere {
   public:
    v3 position;
    f32 radius;

    inline Sphere(): radius(1.0f) { }
    inline Sphere(const v3 &position, f32 radius = 1.0f): position(position), radius(radius) { }

    bool PointInside(const v3 &point) const;
    bool SphereInside(const Sphere &sphere) const;
    bool AABBInside(const AABB &aabb) const;
    bool OBBInside(const OBB &obb) const;
    bool PlaneInside(const Plane &plane) const;
    v3 ClosestPoint(const v3 &point) const;
  };

  class AABB {
   public:
    v3 position;
    v3 size; // half-size

    inline AABB(): size(1.0f) { }
    inline AABB(const v3 &position, const v3 &size = v3(1.0f)): position(position), size(size) { }

    inline v3 GetMin() const {
      v3 p0 = position + size;
      v3 p1 = position - size;

      return v3(min(p0.x, p1.x), min(p0.y, p1.y), min(p0.z, p1.z));
    }

    inline v3 GetMax() const {
      v3 p0 = position + size;
      v3 p1 = position - size;

      return v3(max(p0.x, p1.x), max(p0.y, p1.y), max(p0.z, p1.z));
    }

    inline Interval GetInterval(const v3 &axis) const {
      v3 min = GetMin();
      v3 max = GetMax();

      v3 vertex[8] = { { min.x, max.y, max.z }, { min.x, max.y, min.z }, { min.x, min.y, max.z },
                       { min.x, min.y, min.z }, { max.x, max.y, max.z }, { max.x, max.y, min.z },
                       { max.x, min.y, max.z }, { max.x, min.y, min.z } };

      Interval result;
      result.min = result.max = dot(axis, vertex[0]);

      for (i32 i = 1; i < 8; ++i) {
        f32 projection = dot(axis, vertex[i]);
        result.min = (projection < result.min) ? projection : result.min;
        result.max = (projection > result.max) ? projection : result.max;
      }

      return result;
    }

    static inline AABB FromMinMax(const v3 &min, const v3 &max) {
      return AABB((min + max) * 0.5f, (max - min) * 0.5f);
    }

    bool PointInside(const v3 &point) const;
    bool AABBInside(const AABB &aabb) const;
    bool SphereInside(const Sphere &sphere) const;
    bool PlaneInside(const Plane &plane) const;
    v3 ClosestPoint(const v3 &point) const;
  };

  class OBB {
   public:
    v3 position;
    v3 size; // half-size

    void SetRotation(const quat &rotation);
    const quat &GetRotation() const;
    const m3 &GetRotationMatrix() const;

    inline OBB(): size(1.0f), rotation(), rotation_matrix(1.0f) { }
    inline OBB(const v3 &position, const v3 &size, const quat &rotation = quat()):
      position(position),
      size(size),
      rotation(rotation) {
      rotation_matrix = toMat3(rotation);
    }

    inline Interval GetInterval(const v3 &axis) const {
      v3 vertex[8];
      v3 center = position;
      v3 extents = size;

      vertex[0] =
        center + rotation_matrix[0] * extents[0] + rotation_matrix[1] * extents[1] + rotation_matrix[2] * extents[2];
      vertex[1] =
        center - rotation_matrix[0] * extents[0] + rotation_matrix[1] * extents[1] + rotation_matrix[2] * extents[2];
      vertex[2] =
        center + rotation_matrix[0] * extents[0] - rotation_matrix[1] * extents[1] + rotation_matrix[2] * extents[2];
      vertex[3] =
        center + rotation_matrix[0] * extents[0] + rotation_matrix[1] * extents[1] - rotation_matrix[2] * extents[2];
      vertex[4] =
        center - rotation_matrix[0] * extents[0] - rotation_matrix[1] * extents[1] - rotation_matrix[2] * extents[2];
      vertex[5] =
        center + rotation_matrix[0] * extents[0] - rotation_matrix[1] * extents[1] - rotation_matrix[2] * extents[2];
      vertex[6] =
        center - rotation_matrix[0] * extents[0] + rotation_matrix[1] * extents[1] - rotation_matrix[2] * extents[2];
      vertex[7] =
        center - rotation_matrix[0] * extents[0] - rotation_matrix[1] * extents[1] + rotation_matrix[2] * extents[2];

      Interval result;
      result.min = result.max = dot(axis, vertex[0]);

      for (i32 i = 1; i < 8; ++i) {
        f32 projection = dot(axis, vertex[i]);
        result.min = (projection < result.min) ? projection : result.min;
        result.max = (projection > result.max) ? projection : result.max;
      }

      return result;
    }

    bool OverlapOnAxis(const AABB &aabb, const v3 &axis) const;
    bool OverlapOnAxis(const OBB &obb, const v3 &axis) const;
    bool PointInside(const v3 &point) const;
    bool SphereInside(const Sphere &sphere) const;
    bool AABBInside(const AABB &aabb) const;
    bool OBBInside(const OBB &obb) const;
    bool PlaneInside(const Plane &plane) const;
    v3 ClosestPoint(const v3 &point) const;

   protected:
    quat rotation;
    m3 rotation_matrix;
  };

  class Plane {
   public:
    f32 distance;
    v3 normal;

    inline Plane(): distance(0.0f), normal(0.0f, 0.0f, 1.0f) { }
    inline Plane(f32 distance, const v3 &normal = v3(0.0f, 0.0f, 1.0f)): normal(normal), distance(distance) { }

    f32 PlaneEquation(const v3 &point) const;
    bool PointInside(const v3 &point) const;
    bool SphereInside(const Sphere &sphere) const;
    bool AABBInside(const AABB &aabb) const;
    bool OBBInside(const OBB &obb) const;
    bool PlaneInside(const Plane &plane) const;
    v3 ClosestPoint(const v3 &point) const;
  };

  class Triangle {
   public:
    std::array<v3, 3> points;

    inline Triangle(): points({ v3(0.0f), v3(0.0f), v3(0.0f) }) { }
    inline Triangle(const v3 &p0, const v3 &p1, const v3 &p2): points({ p0, p1, p2 }) { }
  };

  class Line {
   public:
    v3 start;
    v3 end;

    inline Line(): start(0.0f), end(0.0f) { }
    inline Line(const v3 &start, const v3 &end): start(start), end(end) { }

    bool PointInside(const v3 &point) const;
    v3 ClosestPoint(const v3 &point) const;
  };

} // namespace axl
