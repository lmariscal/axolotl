#include <axolotl/serialization.h>

#include <axolotl/scene.h>

namespace axl {

  // Everything is linked to a scene, a scene is the root how everything is
  // serialized and organized in a structured tree.
  // We need to be able to serialize and deserialize a scene, which includes all of
  // its components and its location in the tree, including parent and children.
  // We need a way to identify each entity, so we are able to properly deserialize into
  // the correct position in the tree.
  // I think the parent and children are going to be managed by the transform component, since
  // that is the only component that is affected by the parent and child relationships.

  // We have to also think about prefabs, which are a way to create entities with prefilled data and
  // behaviour. Maybe we can manage parent and children relationship for transformation in the
  // transform component, and then we can manage prefabs as a special kind of entity, which is a
  // scene that is attachable to other scenes.

} // namespace axl
