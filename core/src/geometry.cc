#include <axolotl/geometry.hh>
#include <axolotl/line.hh>
#include <axolotl/renderer.hh>
#include <axolotl/window.hh>

namespace axl {

  bool AABBSphereInside(const AABBCollider &aabb, const SphereCollider &sphere) {
    v3 closest = aabb.ClosestPoint(sphere.position);
    f64 dist_sqr = length2(sphere.position - closest);
    f64 radius_sqr = sphere.radius * sphere.radius;
    return dist_sqr <= radius_sqr;
  }

  bool OBBSphereInside(const OBBCollider &obb, const SphereCollider &sphere) {
    v3 closest = obb.ClosestPoint(sphere.position);
    f64 dist_sqr = length2(sphere.position - closest);
    f64 radius_sqr = sphere.radius * sphere.radius;
    return dist_sqr <= radius_sqr;
  }

  bool PlaneSphereInside(const Plane &plane, const SphereCollider &sphere) {
    v3 closest = plane.ClosestPoint(sphere.position);
    f64 dist_sqr = length2(sphere.position - closest);
    f64 radius_sqr = sphere.radius * sphere.radius;
    return dist_sqr <= radius_sqr;
  }

  bool AABBOBBInside(const AABBCollider &aabb, const OBBCollider &obb) {
    const m3 &obb_rotation = obb.GetRotationMatrix();
    std::array<v3, 15> test;
    test[0] = v3(1.0f, 0.0f, 0.0f);
    test[1] = v3(0.0f, 1.0f, 0.0f);
    test[2] = v3(0.0f, 0.0f, 1.0f);
    test[3] = v3(obb_rotation[0][0], obb_rotation[0][1], obb_rotation[0][2]);
    test[4] = v3(obb_rotation[1][0], obb_rotation[1][1], obb_rotation[1][2]);
    test[5] = v3(obb_rotation[2][0], obb_rotation[2][1], obb_rotation[2][2]);

    for (i32 i = 0; i < 3; ++i) {
      test[6 + i * 3 + 0] = cross(test[i], test[3]);
      test[6 + i * 3 + 1] = cross(test[i], test[4]);
      test[6 + i * 3 + 2] = cross(test[i], test[5]);
    }

    for (i32 i = 0; i < 15; ++i) {
      if (!obb.OverlapOnAxis(aabb, test[i]))
        return false;
    }

    return true;
  }

  bool AABBPlaneInside(const AABBCollider &aabb, const Plane &plane) {
    f64 p_len =
      aabb.size.x * fabsf(plane.normal.x) + aabb.size.y * fabsf(plane.normal.y) + aabb.size.z * fabsf(plane.normal.z);
    f64 d = dot(plane.normal, aabb.position);
    f64 dist = d - plane.distance;

    return abs(dist) <= p_len;
  }

  bool OBBPlaneInside(const OBBCollider &obb, const Plane &plane) {
    const m3 &rotation = obb.GetRotationMatrix();
    f64 p_len = obb.size.x * fabsf(dot(plane.normal, rotation[0])) +
                obb.size.y * fabsf(dot(plane.normal, rotation[1])) + obb.size.z * fabsf(dot(plane.normal, rotation[2]));

    f64 d = dot(plane.normal, obb.position);
    f64 dist = d - plane.distance;

    return abs(dist) <= p_len;
  }

  f64 RaySphereInside(const Ray &ray, const SphereCollider &sphere) {
    v3 e = sphere.position - ray.position;
    f64 r_sqr = sphere.radius * sphere.radius;
    f64 e_sqr = length2(e);

    f64 a = dot(e, ray.GetDirection());
    f64 b_sqr = e_sqr - a * a;
    f64 f = sqrt(r_sqr - b_sqr);

    if (r_sqr - (e_sqr - a * a) < 0.0f)
      return -1.0f;
    else if (e_sqr < r_sqr)
      return a + f;

    return a - f;
  }

  f64 RayAABBInside(const Ray &ray, const AABBCollider &aabb) {
    v3 min = aabb.GetMin();
    v3 max = aabb.GetMax();

    v3 dir = ray.GetDirection();
    for (i32 i = 0; i < 3; ++i) {
      if (dir[i] == 0.0f)
        dir[i] = std::numeric_limits<f64>::epsilon();
    }

    std::array<f64, 6> t = {};
    t[0] = (min.x - ray.position.x) / dir.x;
    t[1] = (max.x - ray.position.x) / dir.x;
    t[2] = (min.y - ray.position.y) / dir.y;
    t[3] = (max.y - ray.position.y) / dir.y;
    t[4] = (min.z - ray.position.z) / dir.z;
    t[5] = (max.z - ray.position.z) / dir.z;

    f64 t_min = fmaxf(fmaxf(fminf(t[0], t[1]), fminf(t[2], t[3])), fminf(t[4], t[5]));
    f64 t_max = fminf(fminf(fmaxf(t[0], t[1]), fmaxf(t[2], t[3])), fmaxf(t[4], t[5]));

    if (t_min > t_max || t_max < 0.0f)
      return -1.0f;
    if (t_min < 0.0f)
      return t_max;
    return t_min;
  }

  f64 RayOBBInside(const Ray &ray, const OBBCollider &obb) {
    v3 pointing = obb.position - ray.position;
    const m3 &rotation = obb.GetRotationMatrix();
    v3 f(dot(rotation[0], ray.GetDirection()),
         dot(rotation[1], ray.GetDirection()),
         dot(rotation[2], ray.GetDirection()));
    v3 e(dot(rotation[0], pointing), dot(rotation[1], pointing), dot(rotation[2], pointing));

    std::array<f64, 6> t = {};
    for (i32 i = 0; i < 3; ++i) {
      if (epsilonEqual((f64)f[i], 0.0, std::numeric_limits<f64>::epsilon())) {
        if (-e[i] - obb.size[i] > 0.0f || -e[i] + obb.size[i] < 0.0f)
          return -1.0f;
        f[i] = std::numeric_limits<f64>::epsilon();
      }
      t[i * 2 + 0] = (e[i] + obb.size[i]) / f[i];
      t[i * 2 + 1] = (e[i] - obb.size[i]) / f[i];
    }

    f64 t_min = fmaxf(fmaxf(fminf(t[0], t[1]), fminf(t[2], t[3])), fminf(t[4], t[5]));
    f64 t_max = fminf(fminf(fmaxf(t[0], t[1]), fmaxf(t[2], t[3])), fmaxf(t[4], t[5]));

    if (t_min > t_max || t_max < 0.0f)
      return -1.0f;
    if (t_min < 0.0f)
      return t_max;
    return t_min;
  }

  CollisionManifold SphereOBBCollide(const SphereCollider &sphere, const OBBCollider &obb) {
    CollisionManifold result;
    v3 closest = obb.ClosestPoint(sphere.position);

    f64 d = length2(closest - sphere.position);
    if (d > sphere.radius * sphere.radius)
      return result;

    v3 normal;
    if (epsilonEqual(d, 0.0, std::numeric_limits<f64>::epsilon())) {
      f64 m_sqr = length2(closest - obb.position);
      if (epsilonEqual(m_sqr, 0.0, std::numeric_limits<f64>::epsilon()))
        return result;
      normal = normalize(closest - obb.position);
    } else {
      normal = normalize(sphere.position - closest);
    }

    v3 outside = sphere.position - normal * (f32)sphere.radius;
    f64 dist = length(closest - outside);

    result.colliding = true;
    result.points.push_back(closest + (outside - closest) * 0.5f);
    result.normal = normal;
    result.depth = dist * 0.5;

    return result;
  }

  // -----------------------------------------------------------------------

  f64 Plane::PlaneEquation(const v3 &point) const {
    return dot(point, normal) - distance;
  }

  bool SphereCollider::PointInside(const v3 &point) const {
    f64 m_sqr = length2(point - position);
    f64 rad_sqr = radius * radius;
    return m_sqr < rad_sqr;
  }

  bool SphereCollider::SphereInside(const SphereCollider &other) const {
    f64 dist_sqr = length2(position - other.position);
    f64 rad_sum = radius + other.radius;
    return dist_sqr < (rad_sum * rad_sum);
  }

  bool SphereCollider::AABBInside(const AABBCollider &aabb) const {
    return AABBSphereInside(aabb, *this);
  }

  bool SphereCollider::OBBInside(const OBBCollider &obb) const {
    return OBBSphereInside(obb, *this);
  }

  bool SphereCollider::PlaneInside(const Plane &plane) const {
    return PlaneSphereInside(plane, *this);
  }

  f64 SphereCollider::RayInside(const Ray &ray) const {
    return RaySphereInside(ray, *this);
  }

  v3 SphereCollider::ClosestPoint(const v3 &point) const {
    v3 diff = point - position;
    diff = normalize(diff);
    diff = diff * (f32)radius;
    return position + diff;
  }

  bool AABBCollider::PointInside(const v3 &point) const {
    v3 min = GetMin();
    v3 max = GetMax();

    if (point.x < min.x || point.y < min.y || point.z < min.z)
      return false;
    if (point.x > max.x || point.y > max.y || point.z > max.z)
      return false;

    return true;
  }

  bool AABBCollider::AABBInside(const AABBCollider &other) const {
    v3 a_min = GetMin();
    v3 a_max = GetMax();
    v3 b_min = other.GetMin();
    v3 b_max = other.GetMax();

    return (a_min.x <= b_max.x && a_max.x >= b_min.x) && (a_min.y <= b_max.y && a_max.y >= b_min.y) &&
           (a_min.z <= b_max.z && a_max.z >= b_min.z);
  }

  bool AABBCollider::SphereInside(const SphereCollider &sphere) const {
    return AABBSphereInside(*this, sphere);
  }

  bool AABBCollider::PlaneInside(const Plane &plane) const {
    return AABBPlaneInside(*this, plane);
  }

  bool AABBCollider::OBBInside(const OBBCollider &obb) const {
    return AABBOBBInside(*this, obb);
  }

  f64 AABBCollider::RayInside(const Ray &ray) const {
    return RayAABBInside(ray, *this);
  }

  v3 AABBCollider::ClosestPoint(const v3 &point) const {
    v3 min = GetMin();
    v3 max = GetMax();

    v3 result;
    result.x = clamp(point.x, min.x, max.x);
    result.y = clamp(point.y, min.y, max.y);
    result.z = clamp(point.z, min.z, max.z);

    return result;
  }

  void OBBCollider::SetRotation(const quat &rotation) {
    this->rotation = rotation;
    rotation_matrix = toMat3(rotation);
    for (i32 i = 0; i < 3; ++i)
      rotation_matrix[i][1] = -rotation_matrix[i][1];
  }

  const quat &OBBCollider::GetRotation() const {
    return rotation;
  }

  const m3 &OBBCollider::GetRotationMatrix() const {
    return rotation_matrix;
  }

  bool OBBCollider::OverlapOnAxis(const AABBCollider &aabb, const v3 &axis) const {
    Interval a = aabb.GetInterval(axis);
    Interval b = GetInterval(axis);

    return ((b.min <= a.max) && (a.min <= b.max));
  }

  bool OBBCollider::OverlapOnAxis(const OBBCollider &obb, const v3 &axis) const {
    Interval a = GetInterval(axis);
    Interval b = obb.GetInterval(axis);

    return ((b.min <= a.max) && (a.min <= b.max));
  }

  bool OBBCollider::PointInside(const v3 &point) const {
    v3 dir = point - position;
    for (i32 i = 0; i < 3; ++i) {
      v3 axis(rotation_matrix[i][0], rotation_matrix[i][1], rotation_matrix[i][2]);
      f64 dist = dot(dir, axis);
      if (dist > size[i] || dist < -size[i])
        return false;
    }
    return true;
  }

  bool OBBCollider::SphereInside(const SphereCollider &sphere) const {
    return OBBSphereInside(*this, sphere);
  }

  bool OBBCollider::AABBInside(const AABBCollider &aabb) const {
    return AABBOBBInside(aabb, *this);
  }

  bool OBBCollider::PlaneInside(const Plane &plane) const {
    return OBBPlaneInside(*this, plane);
  }

  bool OBBCollider::OBBInside(const OBBCollider &other) const {
    std::array<v3, 15> test;
    test[0] = v3(rotation_matrix[0][0], rotation_matrix[0][1], rotation_matrix[0][2]);
    test[1] = v3(rotation_matrix[1][0], rotation_matrix[1][1], rotation_matrix[1][2]);
    test[2] = v3(rotation_matrix[2][0], rotation_matrix[2][1], rotation_matrix[2][2]);
    test[3] = v3(other.rotation_matrix[0][0], other.rotation_matrix[0][1], other.rotation_matrix[0][2]);
    test[4] = v3(other.rotation_matrix[1][0], other.rotation_matrix[1][1], other.rotation_matrix[1][2]);
    test[5] = v3(other.rotation_matrix[2][0], other.rotation_matrix[2][1], other.rotation_matrix[2][2]);

    for (i32 i = 0; i < 3; ++i) {
      test[6 + i * 3 + 0] = cross(test[i], test[3]);
      test[6 + i * 3 + 1] = cross(test[i], test[4]);
      test[6 + i * 3 + 2] = cross(test[i], test[5]);
    }

    for (i32 i = 0; i < 15; ++i) {
      if (!OverlapOnAxis(other, test[i]))
        return false;
    }

    return true;
  }

  f64 OBBCollider::RayInside(const Ray &ray) const {
    return RayOBBInside(ray, *this);
  }

  v3 OBBCollider::ClosestPoint(const v3 &point) const {
    v3 result = position;
    v3 dir = point - position;

    for (i32 i = 0; i < 3; ++i) {
      v3 axis(rotation_matrix[i][0], rotation_matrix[i][1], rotation_matrix[i][2]);
      f64 dist = dot(dir, axis);
      if (dist > size[i])
        dist = size[i];
      else if (dist < -size[i])
        dist = -size[i];
      result += (f32)dist * axis;
    }
    return result;
  }

  bool Plane::PointInside(const v3 &point) const {
    f64 d = dot(point, normal);
    return epsilonEqual(d - distance, 0.0, std::numeric_limits<f64>::epsilon());
  }

  bool Plane::OBBInside(const OBBCollider &obb) const {
    return OBBPlaneInside(obb, *this);
  }

  bool Plane::SphereInside(const SphereCollider &sphere) const {
    return PlaneSphereInside(*this, sphere);
  }

  bool Plane::AABBInside(const AABBCollider &aabb) const {
    return AABBPlaneInside(aabb, *this);
  }

  bool Plane::PlaneInside(const Plane &plane) const {
    v3 c = cross(normal, plane.normal);
    return epsilonNotEqual((f64)dot(c, c), 0.0, std::numeric_limits<f64>::epsilon());
  }

  v3 Plane::ClosestPoint(const v3 &point) const {
    f64 d = dot(point, normal);
    f64 dist = d - distance;
    return point - normal * (f32)dist;
  }

  bool Ray::PointInside(const v3 &point) const {
    if (point == position)
      return true;

    v3 norm = normalize(point - position);
    f64 diff = dot(norm, direction);
    return epsilonEqual(diff, 1.0, std::numeric_limits<f64>::epsilon());
  }

  v3 Ray::ClosestPoint(const v3 &point) const {
    f64 t = dot(point - position, direction);
    t = max(t, 0.0);
    return position + direction * (f32)t;
  }

  f64 Ray::SphereInside(const SphereCollider &sphere) const {
    return RaySphereInside(*this, sphere);
  }

  f64 Ray::OBBInside(const OBBCollider &obb) const {
    return RayOBBInside(*this, obb);
  }

  f64 Ray::AABBInside(const AABBCollider &aabb) const {
    return RayAABBInside(*this, aabb);
  }

  void CollisionManifold::Reset() {
    colliding = false;
    normal = v3(0.0f, 0.0f, 1.0f);
    depth = std::numeric_limits<f64>::max();
    points.clear();
  }

  CollisionManifold SphereCollider::SphereCollide(const SphereCollider &other) const {
    CollisionManifold result;
    f64 r = radius + other.radius;
    v3 dist = other.position - position;

    f64 m_sqr = length2(dist);
    if (m_sqr > r * r || m_sqr == 0.0)
      return result;

    dist = normalize(dist);
    result.colliding = true;
    result.normal = dist;
    result.depth = abs(length(dist) - r) * 0.5;

    f64 dist_inter = radius - result.depth;
    v3 contact = position + dist * (f32)dist_inter;
    result.points.push_back(contact);

    return result;
  }

  CollisionManifold SphereCollider::OBBCollide(const OBBCollider &other) const {
    return SphereOBBCollide(*this, other);
  }

  CollisionManifold OBBCollider::SphereCollide(const SphereCollider &other) const {
    return SphereOBBCollide(other, *this);
  }

  std::vector<v3> OBBCollider::GetVertices() const {
    std::vector<v3> v(8);

    v[0] = position + rotation_matrix[0] * size[0] + rotation_matrix[1] * size[1] + rotation_matrix[2] * size[2];
    v[1] = position - rotation_matrix[0] * size[0] + rotation_matrix[1] * size[1] + rotation_matrix[2] * size[2];
    v[2] = position + rotation_matrix[0] * size[0] - rotation_matrix[1] * size[1] + rotation_matrix[2] * size[2];
    v[3] = position + rotation_matrix[0] * size[0] + rotation_matrix[1] * size[1] - rotation_matrix[2] * size[2];
    v[4] = position - rotation_matrix[0] * size[0] - rotation_matrix[1] * size[1] - rotation_matrix[2] * size[2];
    v[5] = position + rotation_matrix[0] * size[0] - rotation_matrix[1] * size[1] - rotation_matrix[2] * size[2];
    v[6] = position - rotation_matrix[0] * size[0] + rotation_matrix[1] * size[1] - rotation_matrix[2] * size[2];
    v[7] = position - rotation_matrix[0] * size[0] - rotation_matrix[1] * size[1] + rotation_matrix[2] * size[2];

    return v;
  }

  std::vector<Line> OBBCollider::GetEdges() const {
    std::vector<Line> e(12);
    std::vector<v3> v = GetVertices();

    i32 index[][2] = { { 6, 1 }, { 6, 3 }, { 6, 4 }, { 2, 7 }, { 2, 5 }, { 2, 0 },
                       { 0, 1 }, { 0, 3 }, { 7, 1 }, { 7, 4 }, { 4, 5 }, { 5, 3 } };

    for (i32 i = 0; i < 12; i++)
      e.push_back(Line(v[index[i][0]], v[index[i][1]]));

    return e;
  }

  std::vector<Plane> OBBCollider::GetPlanes() const {
    std::vector<Plane> p(6);

    p[0] = Plane(dot(rotation_matrix[0], (position + rotation_matrix[0] * size[0])), rotation_matrix[0]);
    p[1] = Plane(-dot(rotation_matrix[0], (position - rotation_matrix[0] * size[0])), rotation_matrix[0] * -1.0f);
    p[2] = Plane(dot(rotation_matrix[1], (position + rotation_matrix[1] * size[1])), rotation_matrix[1]);
    p[3] = Plane(-dot(rotation_matrix[1], (position - rotation_matrix[1] * size[1])), rotation_matrix[1] * -1.0f);
    p[4] = Plane(dot(rotation_matrix[2], (position + rotation_matrix[2] * size[2])), rotation_matrix[2]);
    p[5] = Plane(-dot(rotation_matrix[2], (position - rotation_matrix[2] * size[2])), rotation_matrix[2] * -1.0f);

    return p;
  }

  bool OBBCollider::ClipToPlane(const Plane &plane, const Line &line, v3 &out_point) const {
    v3 ab = line.end - line.start;
    f64 d_ab = dot(plane.normal, ab);

    if (epsilonEqual(d_ab, 0.0, std::numeric_limits<f64>::epsilon()))
      return false;

    f64 d_a = dot(plane.normal, line.start);
    f64 t = (plane.distance - d_a) / d_ab;

    if (t >= 0.0 && t <= 1.0) {
      out_point = line.start + ab * (f32)t;
      return true;
    }

    return false;
  }

  std::vector<v3> OBBCollider::ClipEdgesToOBB(const std::vector<Line> &edges) const {
    std::vector<v3> result;
    result.reserve(edges.size());
    std::vector<Plane> planes = GetPlanes();
    v3 intersection;

    for (i32 i = 0; i < planes.size(); ++i) {
      for (i32 j = 0; j < edges.size(); ++j) {
        if (!ClipToPlane(planes[i], edges[j], intersection))
          continue;
        if (!PointInside(intersection))
          continue;

        result.push_back(intersection);
      }
    }

    return result;
  }

  f64 OBBCollider::PenetrationDepth(const OBBCollider &obb, const v3 &axis, bool &out_should_flip) const {
    Interval ia = GetInterval(normalize(axis));
    Interval ib = obb.GetInterval(normalize(axis));

    if (!((ib.min <= ia.max) && (ia.min <= ib.max)))
      return 0.0;

    f64 len_a = ia.max - ia.min;
    f64 len_b = ib.max - ib.min;
    f64 i_min = min(ia.min, ib.min);
    f64 i_max = max(ia.max, ib.max);
    f64 len = i_max - i_min;

    out_should_flip = ib.min < ia.min;

    return (len_a + len_b) - len;
  }

  CollisionManifold OBBCollider::OBBCollide(const OBBCollider &other) const {
    CollisionManifold result;

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

    std::optional<v3> hit_point;
    bool should_flip = false;

    for (i32 i = 0; i < 15; ++i) {
      if (length2(test[i]) < std::numeric_limits<f32>::epsilon())
        continue;

      f64 depth = PenetrationDepth(other, test[i], should_flip);
      if (depth < std::numeric_limits<f64>::epsilon()) {
        return result;
      } else if (depth < result.depth) {
        if (should_flip)
          test[i] = test[i] * -1.0f;
        result.depth = depth;
        hit_point = test[i];
      }
    }

    if (!hit_point)
      return result;

    v3 axis = normalize(*hit_point);
    std::vector<v3> ca = ClipEdgesToOBB(other.GetEdges());
    std::vector<v3> cb = other.ClipEdgesToOBB(GetEdges());

    result.points.reserve(ca.size() + cb.size());
    result.points.insert(result.points.end(), ca.begin(), ca.end());
    result.points.insert(result.points.end(), cb.begin(), cb.end());

    Interval i = GetInterval(axis);
    f64 dist = (i.max - i.min) * 0.5f - result.depth * 0.5f;
    v3 point_plane = position + axis * (f32)dist;

    for (i32 i = result.points.size() - 1; i >= 0; --i) {
      v3 contact = result.points[i];
      result.points[i] = contact + (axis * dot(axis, point_plane - contact));

      for (i32 j = result.points.size() - 1; j >= 0; --j) {
        if (length2(result.points[j] - result.points[i]) >= std::numeric_limits<f32>::epsilon())
          continue;

        result.points.erase(result.points.begin() + j);
        break;
      }
    }

    result.colliding = true;
    result.normal = axis;

    return result;
  }

  bool OBBCollider::ShowComponent() {
    bool modified = false;

    if (ShowData("Size", size))
      modified = true;

    return modified;
  }

  bool SphereCollider::ShowComponent() {
    bool modified = false;

    if (ShowData("Radius", radius))
      modified = true;

    return modified;
  }

} // namespace axl
