#include <axolotl/camera.hh>
#include <axolotl/ento.hh>
#include <axolotl/material.hh>
#include <axolotl/mesh.hh>
#include <axolotl/model.hh>
#include <axolotl/renderer.hh>
#include <axolotl/scene.hh>
#include <axolotl/shader.hh>
#include <axolotl/transform.hh>
#include <iostream>

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

    if (e.HasParent())
      e.Parent().RemoveChild(e);

    Ento::_uuid_ento_map.erase(e.id);
    Ento::_handle_ento_map.erase(e.handle);

    _registry.destroy(e.handle);
    log::debug("Removed entity with id {}", uuids::to_string(e.id));
  }

  Ento Scene::FromID(uuid id) {
    if (!Ento::_uuid_ento_map.count(id))
      return {};

    return Ento::_uuid_ento_map[id];
  }

  Ento Scene::FromHandle(entt::entity handle) {
    if (!Ento::_handle_ento_map.count(handle))
      return {};

    return Ento::_handle_ento_map[handle];
  }

  void Scene::Draw(Renderer &renderer, bool show_data, Camera *camera, Transform *camera_transform) {
    if (!camera)
      camera = Camera::GetActiveCamera();
    if (!camera_transform)
      camera_transform = &Ento::FromComponent(*camera).GetComponent<Transform>();

    renderer.Render(*this, show_data, focused, *camera, *camera_transform);
  }

  Scene *Scene::GetActiveScene() {
    return _active_scene;
  }

  entt::registry &Scene::GetRegistry() {
    return _registry;
  }

  json Scene::Serialize() {
    using namespace entt::literals;

    json j;

    _registry.each([&](entt::entity entity) {
      json e;
      e["id"] = FromHandle(entity).id;
      e["components"] = {};

      _registry.visit(entity, [&](const entt::type_info &info) {
        json c = {};
        auto meta = entt::resolve(info);
        if (!meta) {
          // log::error("Could not resolve meta for type {}", info.name());
          return;
        }
        std::string component_name = meta.prop("name"_hs).value().cast<std::string>();
        auto handle = meta.func("get"_hs).invoke({}, entt::forward_as_meta(_registry), entity);

        c["data"] = {};
        c["type"] = component_name;
        c["hash"] = meta.func("GetHash"_hs).invoke({}).cast<i32>();
        for (entt::meta_data data : handle.type().data()) {
          std::string name = data.prop("name"_hs).value().cast<std::string>();
          auto member = data.get(handle);
          member.allow_cast<json>();
          json *member_values = member.try_cast<json>();
          if (!member_values) {
            // log::error("Could not convert to json {} | {} inside {}",
            //            data.type().info().name(),
            //            name,
            //            handle.type().info().name());
            continue;
          }

          json mj = {};
          mj["name"] = name;
          mj["data"] = *member_values;

          c["data"].push_back(mj);
        }
        e["components"].push_back(c);
      });

      j["entities"].push_back(e);
    });
    return j;
  }

  void Scene::Deserialize(const json &j) {
    std::vector<Texture2D> textures_tmp;
    for (auto itr = TextureStore::_data.begin(); itr != TextureStore::_data.end(); itr++) {
      Texture2D texture;
      texture.texture_id = itr->first;
      TextureStore::_data[itr->first].instances++;
      textures_tmp.emplace_back(std::move(texture));
    }

    std::vector<Ento> to_destroy;
    _registry.each([&](entt::entity entity) {
      Ento ento = FromHandle(entity);
      if (!ento.HasParent())
        to_destroy.push_back(ento);
    });
    for (Ento &ento : to_destroy)
      RemoveEntity(ento);

    Ento::_uuid_ento_map.clear();
    Ento::_handle_ento_map.clear();
    _registry = entt::registry();

    using namespace entt::literals;

    for (auto &e : j["entities"]) {
      entt::entity entity = _registry.create();
      Ento ento(entity, this);
      uuid id = e["id"];
      ento.id = id;
      Ento::_uuid_ento_map.insert(std::make_pair(id, ento));
      Ento::_handle_ento_map.insert(std::make_pair(entity, ento));

      for (auto &c : e["components"]) {
        auto hs = entt::hashed_string::value(c["type"].get<std::string>().c_str());
        auto meta = entt::resolve(hs);
        auto handle = meta.func("emplace"_hs).invoke({}, entt::forward_as_meta(_registry), entity);

        for (auto d : meta.data()) {
          const std::string name = d.prop("name"_hs).value().cast<std::string>();
          bool found = false;
          for (auto dj : c["data"]) {
            if (dj["name"] != name)
              continue;
            found = true;

            auto member = d.get(handle);
            d.type().func("FromJSON"_hs).invoke({}, entt::forward_as_meta(dj["data"]), member.as_ref());
            d.set(handle, member);
            break;
          }
          if (!found) {
            // log::error("Could not find data {} in component {}", name, c["type"]);
          }
        }
      }
    }

    _registry.each([&](entt::entity entity) {
      _registry.visit(entity, [&](const auto info) {
        auto meta = entt::resolve(info);
        if (!meta) {
          // log::error("Could not resolve meta for type {}", info.name());
          return;
        }
        auto handle = meta.func("get"_hs).invoke({}, entt::forward_as_meta(_registry), entity);
        meta.func("Init"_hs).invoke(handle);
      });
    });
  }

} // namespace axl
