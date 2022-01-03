#include <axolotl/geometry.hh>

namespace axl {

  bool AABBSphereInside(const AABB &aabb, const Sphere &sphere) {
    v3 closest = aabb.ClosestPoint(sphere.position);
    f32 dist_sqr = length2(sphere.position - closest);
    f32 radius_sqr = sphere.radius * sphere.radius;
    return dist_sqr <= radius_sqr;
  }

  bool OBBSphereInside(const OBB &obb, const Sphere &sphere) {
    v3 closest = obb.ClosestPoint(sphere.position);
    f32 dist_sqr = length2(sphere.position - closest);
    f32 radius_sqr = sphere.radius * sphere.radius;
    return dist_sqr <= radius_sqr;
  }

  bool PlaneSphereInside(const Plane &plane, const Sphere &sphere) {
    v3 closest = plane.ClosestPoint(sphere.position);
    f32 dist_sqr = length2(sphere.position - closest);
    f32 radius_sqr = sphere.radius * sphere.radius;
    return dist_sqr <= radius_sqr;
  }

  bool AABBOBBInside(const AABB &aabb, const OBB &obb) {
    const m3 &obb_rotation = obb.GetRotationMatrix();
    std::array<v3, 15> test;
    test[0] = v3(1.0f, 0.0f, 0.0f);
    test[1] = v3(0.0f, 1.0f, 0.0f);
    test[2] = v3(0.0f, 0.0f, 1.0f);
    test[3] = v3(obb_rotation[0][0], obb_rotation[0][1], obb_rotation[0][2]);
    test[4] = v3(obb_rotation[1][0], obb_rotation[1][1], obb_rotation[1][2]);
    test[5] = v3(obb_rotation[2][0], obb_rotation[2][1], obb_rotation[2][2]);

    for (i32 i = 0; i < 3; ++i) {
      test[6 + i * 3 + 0] = cross(test[i], test[0]);
      test[6 + i * 3 + 1] = cross(test[i], test[1]);
      test[6 + i * 3 + 2] = cross(test[i], test[2]);
    }

    for (i32 i = 0; i < 15; ++i) {
      if (!obb.OverlapOnAxis(aabb, test[i]))
        return false;
    }

    return true;
  }

  bool AABBPlaneInside(const AABB &aabb, const Plane &plane) {
    f32 p_len =
      aabb.size.x * fabsf(plane.normal.x) + aabb.size.y * fabsf(plane.normal.y) + aabb.size.z * fabsf(plane.normal.z);
    f32 d = dot(plane.normal, aabb.position);
    f32 dist = d - plane.distance;

    return fabsf(dist) <= p_len;
  }

  bool OBBPlaneInside(const OBB &obb, const Plane &plane) {
    const m3 &rotation = obb.GetRotationMatrix();
    f32 p_len = obb.size.x * fabsf(dot(plane.normal, rotation[0])) +
                obb.size.y * fabsf(dot(plane.normal, rotation[1])) + obb.size.z * fabsf(dot(plane.normal, rotation[2]));

    f32 d = dot(plane.normal, obb.position);
    f32 dist = d - plane.distance;

    return fabsf(dist) <= p_len;
  }

  f32 RaySphereInside(const Ray &ray, const Sphere &sphere) {
    v3 e = sphere.position - ray.position;
    f32 r_sqr = sphere.radius * sphere.radius;
    f32 e_sqr = length2(e);

    f32 a = dot(e, ray.GetDirection());
    f32 b_sqr = e_sqr - a * a;
    f32 f = sqrt(r_sqr - b_sqr);

    if (r_sqr - (e_sqr - a * a) < 0.0f)
      return -1.0f;
    else if (e_sqr < r_sqr)
      return a + f;

    return a - f;
  }

  f32 RayAABBInside(const Ray &ray, const AABB &aabb) {
    v3 min = aabb.GetMin();
    v3 max = aabb.GetMax();

    v3 dir = ray.GetDirection();
    for (i32 i = 0; i < 3; ++i) {
      if (dir[i] == 0.0f)
        dir[i] = std::numeric_limits<f32>::epsilon();
    }

    std::array<f32, 6> t = {};
    t[0] = (min.x - ray.position.x) / dir.x;
    t[1] = (max.x - ray.position.x) / dir.x;
    t[2] = (min.y - ray.position.y) / dir.y;
    t[3] = (max.y - ray.position.y) / dir.y;
    t[4] = (min.z - ray.position.z) / dir.z;
    t[5] = (max.z - ray.position.z) / dir.z;

    f32 t_min = fmaxf(fmaxf(fminf(t[0], t[1]), fminf(t[2], t[3])), fminf(t[4], t[5]));
    f32 t_max = fminf(fminf(fmaxf(t[0], t[1]), fmaxf(t[2], t[3])), fmaxf(t[4], t[5]));

    if (t_min > t_max || t_max < 0.0f)
      return -1.0f;
    if (t_min < 0.0f)
      return t_max;
    return t_min;
  }

  f32 RayOBBInside(const Ray &ray, const OBB &obb) {
    v3 pointing = obb.position - ray.position;
    const m3 &rotation = obb.GetRotationMatrix();
    v3 f(dot(rotation[0], ray.GetDirection()),
         dot(rotation[1], ray.GetDirection()),
         dot(rotation[2], ray.GetDirection()));
    v3 e(dot(rotation[0], pointing), dot(rotation[1], pointing), dot(rotation[2], pointing));

    std::array<f32, 6> t = {};
    for (i32 i = 0; i < 3; ++i) {
      if (epsilonEqual(f[i], 0.0f, std::numeric_limits<f32>::epsilon())) {
        if (-e[i] - obb.size[i] > 0.0f || -e[i] + obb.size[i] < 0.0f)
          return -1.0f;
        f[i] = std::numeric_limits<f32>::epsilon();
      }
      t[i * 2 + 0] = (e[i] + obb.size[i]) / f[i];
      t[i * 2 + 1] = (e[i] - obb.size[i]) / f[i];
    }

    f32 t_min = fmaxf(fmaxf(fminf(t[0], t[1]), fminf(t[2], t[3])), fminf(t[4], t[5]));
    f32 t_max = fminf(fminf(fmaxf(t[0], t[1]), fmaxf(t[2], t[3])), fmaxf(t[4], t[5]));

    if (t_min > t_max || t_max < 0.0f)
      return -1.0f;
    if (t_min < 0.0f)
      return t_max;
    return t_min;
  }

  // -----------------------------------------------------------------------

  f32 Plane::PlaneEquation(const v3 &point) const {
    return dot(point, normal) - distance;
  }

  bool Sphere::PointInside(const v3 &point) const {
    f32 mag_sqr = length2(point - position);
    f32 rad_sqr = radius * radius;
    return mag_sqr < rad_sqr;
  }

  bool Sphere::SphereInside(const Sphere &other) const {
    f32 dist_sqr = length2(position - other.position);
    f32 rad_sum = radius + other.radius;
    return dist_sqr < (rad_sum * rad_sum);
  }

  bool Sphere::AABBInside(const AABB &aabb) const {
    return AABBSphereInside(aabb, *this);
  }

  bool Sphere::OBBInside(const OBB &obb) const {
    return OBBSphereInside(obb, *this);
  }

  bool Sphere::PlaneInside(const Plane &plane) const {
    return PlaneSphereInside(plane, *this);
  }

  f32 Sphere::RayInside(const Ray &ray) const {
    return RaySphereInside(ray, *this);
  }

  v3 Sphere::ClosestPoint(const v3 &point) const {
    v3 diff = point - position;
    diff = normalize(diff);
    diff = diff * radius;
    return position + diff;
  }

  bool AABB::PointInside(const v3 &point) const {
    v3 min = GetMin();
    v3 max = GetMax();

    if (point.x < min.x || point.y < min.y || point.z < min.z)
      return false;
    if (point.x > max.x || point.y > max.y || point.z > max.z)
      return false;

    return true;
  }

  bool AABB::AABBInside(const AABB &other) const {
    v3 a_min = GetMin();
    v3 a_max = GetMax();
    v3 b_min = other.GetMin();
    v3 b_max = other.GetMax();

    return (a_min.x <= b_max.x && a_max.x >= b_min.x) && (a_min.y <= b_max.y && a_max.y >= b_min.y) &&
           (a_min.z <= b_max.z && a_max.z >= b_min.z);
  }

  bool AABB::SphereInside(const Sphere &sphere) const {
    return AABBSphereInside(*this, sphere);
  }

  bool AABB::PlaneInside(const Plane &plane) const {
    return AABBPlaneInside(*this, plane);
  }

  bool AABB::OBBInside(const OBB &obb) const {
    return AABBOBBInside(*this, obb);
  }

  f32 AABB::RayInside(const Ray &ray) const {
    return RayAABBInside(ray, *this);
  }

  v3 AABB::ClosestPoint(const v3 &point) const {
    v3 min = GetMin();
    v3 max = GetMax();

    v3 result;
    result.x = clamp(point.x, min.x, max.x);
    result.y = clamp(point.y, min.y, max.y);
    result.z = clamp(point.z, min.z, max.z);

    return result;
  }

  void OBB::SetRotation(const quat &rotation) {
    this->rotation = rotation;
    rotation_matrix = toMat3(rotation);
  }

  const quat &OBB::GetRotation() const {
    return rotation;
  }

  const m3 &OBB::GetRotationMatrix() const {
    return rotation_matrix;
  }

  bool OBB::OverlapOnAxis(const AABB &aabb, const v3 &axis) const {
    Interval a = aabb.GetInterval(axis);
    Interval b = GetInterval(axis);

    return ((b.min <= a.max) && (a.min <= b.max));
  }

  bool OBB::OverlapOnAxis(const OBB &obb, const v3 &axis) const {
    Interval a = GetInterval(axis);
    Interval b = obb.GetInterval(axis);

    return ((b.min <= a.max) && (a.min <= b.max));
  }

  bool OBB::PointInside(const v3 &point) const {
    v3 dir = point - position;
    for (i32 i = 0; i < 3; ++i) {
      v3 axis(rotation_matrix[i][0], rotation_matrix[i][1], rotation_matrix[i][2]);
      f32 dist = dot(dir, axis);
      if (dist > size[i] || dist < -size[i])
        return false;
    }
    return true;
  }

  bool OBB::SphereInside(const Sphere &sphere) const {
    return OBBSphereInside(*this, sphere);
  }

  bool OBB::AABBInside(const AABB &aabb) const {
    return AABBOBBInside(aabb, *this);
  }

  bool OBB::PlaneInside(const Plane &plane) const {
    return OBBPlaneInside(*this, plane);
  }

  bool OBB::OBBInside(const OBB &other) const {
    std::array<v3, 15> test;
    test[0] = v3(rotation_matrix[0][0], rotation_matrix[0][1], rotation_matrix[0][2]);
    test[1] = v3(rotation_matrix[1][0], rotation_matrix[1][1], rotation_matrix[1][2]);
    test[2] = v3(rotation_matrix[2][0], rotation_matrix[2][1], rotation_matrix[2][2]);
    test[3] = v3(other.rotation_matrix[0][0], other.rotation_matrix[0][1], other.rotation_matrix[0][2]);
    test[4] = v3(other.rotation_matrix[1][0], other.rotation_matrix[1][1], other.rotation_matrix[1][2]);
    test[5] = v3(other.rotation_matrix[2][0], other.rotation_matrix[2][1], other.rotation_matrix[2][2]);

    for (i32 i = 0; i < 3; ++i) {
      test[6 + i * 3 + 0] = cross(test[i], test[0]);
      test[6 + i * 3 + 1] = cross(test[i], test[1]);
      test[6 + i * 3 + 2] = cross(test[i], test[2]);
    }

    for (i32 i = 0; i < 15; ++i) {
      if (!OverlapOnAxis(other, test[i]))
        return false;
    }

    return true;
  }

  f32 OBB::RayInside(const Ray &ray) const {
    return RayOBBInside(ray, *this);
  }

  v3 OBB::ClosestPoint(const v3 &point) const {
    v3 result = position;
    v3 dir = point - position;
    for (i32 i = 0; i < 3; ++i) {
      v3 axis(rotation_matrix[i][0], rotation_matrix[i][1], rotation_matrix[i][2]);
      f32 dist = dot(dir, axis);
      if (dist > size[i])
        dist = size[i];
      else if (dist < -size[i])
        dist = -size[i];
      result += dist * axis;
    }
    return result;
  }

  bool Plane::PointInside(const v3 &point) const {
    f32 d = dot(point, normal);
    return epsilonEqual(d - distance, 0.0f, std::numeric_limits<f32>::epsilon());
  }

  bool Plane::OBBInside(const OBB &obb) const {
    return OBBPlaneInside(obb, *this);
  }

  bool Plane::SphereInside(const Sphere &sphere) const {
    return PlaneSphereInside(*this, sphere);
  }

  bool Plane::AABBInside(const AABB &aabb) const {
    return AABBPlaneInside(aabb, *this);
  }

  bool Plane::PlaneInside(const Plane &plane) const {
    v3 c = cross(normal, plane.normal);
    return epsilonNotEqual(dot(c, c), 0.0f, std::numeric_limits<f32>::epsilon());
  }

  v3 Plane::ClosestPoint(const v3 &point) const {
    f32 d = dot(point, normal);
    f32 dist = d - distance;
    return point - normal * dist;
  }

  bool Line::PointInside(const v3 &point) const {
    v3 closest = ClosestPoint(point);
    f32 dist_sqr = length2(closest - point);
    return epsilonEqual(dist_sqr, 0.0f, std::numeric_limits<f32>::epsilon());
  }

  v3 Line::ClosestPoint(const v3 &point) const {
    v3 lvec = end - start;
    f32 t = dot(point - start, lvec) / dot(lvec, lvec);
    t = clamp(t, 0.0f, 1.0f);
    return start + lvec * t;
  }

  bool Ray::PointInside(const v3 &point) const {
    if (point == position)
      return true;

    v3 norm = normalize(point - position);
    f32 diff = dot(norm, direction);
    return epsilonEqual(diff, 1.0f, std::numeric_limits<f32>::epsilon());
  }

  v3 Ray::ClosestPoint(const v3 &point) const {
    f32 t = dot(point - position, direction);
    t = max(t, 0.0f);
    return position + direction * t;
  }

  f32 Ray::SphereInside(const Sphere &sphere) const {
    return RaySphereInside(*this, sphere);
  }

  f32 Ray::OBBInside(const OBB &obb) const {
    return RayOBBInside(*this, obb);
  }

  f32 Ray::AABBInside(const AABB &aabb) const {
    return RayAABBInside(*this, aabb);
  }

} // namespace axl
