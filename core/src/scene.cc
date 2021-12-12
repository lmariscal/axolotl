#include <axolotl/ento.hh>
#include <axolotl/mesh.hh>
#include <axolotl/renderer.hh>
#include <axolotl/scene.hh>
#include <axolotl/shader.hh>
#include <axolotl/transform.hh>
#include <fstream>

namespace nlohmann {

  template<>
  struct adl_serializer<entt::registry> {

    static void to_json(json &j, const entt::registry &registry) {
      using namespace axl;
      using namespace entt::literals;

      registry.each([&](entt::entity entity) {
        json e;
        e["components"] = {};

        registry.visit(entity, [&](const entt::type_info &info) {
          json c = {};
          auto meta = entt::resolve(info);
          if (!meta) {
            log::error("Could not resolve meta for type {}", info.name());
            return;
          }
          std::string component_name = meta.prop("name"_hs).value().cast<std::string>();
          auto handle = meta.func("get"_hs).invoke({}, entt::forward_as_meta(registry), entity);

          c["data"] = {};
          c["type"] = component_name;
          c["hash"] = meta.func("GetHash"_hs).invoke({}).cast<i32>();
          for (entt::meta_data data : handle.type().data()) {
            std::string name = data.prop("name"_hs).value().cast<std::string>();
            auto member = data.get(handle);
            member.allow_cast<json>();
            json *member_values = member.try_cast<json>();
            if (!member_values) {
              log::error("Could not convert to json {} | {} inside {}",
                         data.type().info().name(),
                         name,
                         handle.type().info().name());
              continue;
            }

            json mj = {};
            mj["name"] = name;
            mj["data"] = *member_values;

            c["data"].push_back(mj);
          }
          e["components"].push_back(c);
        });

        j.push_back(e);
      });
    }

    static void from_json(const json &j, entt::registry &registry) {
      using namespace axl;
      using namespace entt::literals;

      registry = entt::registry();

      for (auto &e : j) {
        auto entity = registry.create();
        for (auto &c : e["components"]) {

          auto hs = entt::hashed_string::value(c["type"].get<std::string>().c_str());
          auto meta = entt::resolve(hs);
          auto handle = meta.func("emplace"_hs).invoke({}, entt::forward_as_meta(registry), entity);

          i32 i = 0;
          for (auto d : meta.data()) {
            if (c["data"].empty()) {
              log::error("No data for component {}", c["type"]);
              continue;
            }
            const json &data = c["data"][i++];
            if (data["name"] != d.prop("name"_hs).value().cast<std::string>()) {
              log::error("Data name does not match component name");
              continue;
            }

            auto member = d.get(handle);
            d.type().func("FromJSON"_hs).invoke({}, entt::forward_as_meta(data["data"]), member.as_ref());
            d.set(handle, member);
          }
        }
      }
    }
  };

} // namespace nlohmann

namespace axl {

  void Scene::SetActiveScene(Scene *scene) {
    _active_scene = scene;
  }

  Ento Scene::CreateEntity(const std::string &name) {
    Ento e(_registry.create(), this);
    e.AddComponent<Transform>();
    e.AddComponent<HierarchyComponent>();

    Tag &tag = e.AddComponent<Tag>();
    name.empty() ? tag.value = Tag::DefaultTag : tag.value = name;

    Ento::_uuid_ento_map.insert(std::make_pair(e.id, e));
    Ento::_handle_ento_map.insert(std::make_pair(e.handle, e));

    log::debug("Created ento {}", uuids::to_string(e.id));

    return e;
  }

  void Scene::RemoveEntity(Ento e) {
    AXL_ASSERT_MESSAGE(_registry.valid(e.handle),
                       "Trying to remove invalid entity {}->{}",
                       e.handle,
                       uuids::to_string(e.id));

    if (e.HasChildren())
      for (auto &child : e.Children())
        RemoveEntity(child);

    if (e.HasParent()) e.Parent().RemoveChild(e);

    Ento::_uuid_ento_map.erase(e.id);
    Ento::_handle_ento_map.erase(e.handle);

    _registry.destroy(e.handle);
    log::debug("Removed entity with id {}", uuids::to_string(e.id));
  }

  Ento Scene::FromID(uuid id) {
    if (!Ento::_uuid_ento_map.count(id)) return {};

    return Ento::_uuid_ento_map[id];
  }

  Ento Scene::FromHandle(entt::entity handle) {
    if (!Ento::_handle_ento_map.count(handle)) return {};

    return Ento::_handle_ento_map[handle];
  }

  void Scene::Draw(Renderer &renderer, bool show_data) {
    renderer.Render(*this, show_data, focused);
  }

  Scene *Scene::GetActiveScene() {
    return _active_scene;
  }

  entt::registry &Scene::GetRegistry() {
    return _registry;
  }

  json Scene::Serialize() {
    json j;
    j["entities"] = _registry;

    entt::registry b = j["entities"];

    // output j to a file
    std::ofstream o("scene.json");
    o << j.dump(2);
    o.close();

    std::ofstream o2("scene_comp.json");
    o2 << ((json)b).dump(2);
    o2.close();

    return j;
  }

} // namespace axl
