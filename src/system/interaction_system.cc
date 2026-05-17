#include "interaction_system.h"
#include "audio_system.h"
#include "render_system.h"
#include <algorithm>
#include <iostream>

void InteractionSystem::onInteract(GlobalState& state, MapData& map, DataRegistry& registry) {
  auto& t = state.player_.transform_;
  auto t_x = t.x_;
  auto t_y = t.y_;
  
  if (t.facing_ == North) t_y -= 1;
  else if (t.facing_ == South) t_y += 1;
  else if (t.facing_ == West) t_x -= 1;
  else if (t.facing_ == East) t_x += 1;

  t_y = std::clamp(t_y, static_cast<uint8_t>(0), map.size_);
  t_x = std::clamp(t_x, static_cast<uint8_t>(0), map.size_);

  std::string text_to_display = "";
  std::string sound_to_play = "";
  bool found_mob = false;

  for (auto& mob : map.mobs_) {
    if (mob.alive_) {
      for (auto& cmp : mob.components_) {
        if (auto m = dynamic_cast<Moveable*>(cmp.get())) {
          if (m->x_ == t_x && m->y_ == t_y) {
            text_to_display = mob.mob_.interact_text_;
            found_mob = true;
            break;
          }
        }
      }
    }
    if (found_mob) break;
  }

  if (!found_mob) {
    auto obj = map.objAt(t_x, t_y);
    text_to_display = obj->interact_text_;
    sound_to_play = obj->sound_id_;

    if (!obj->give_item_.empty()) {
      auto item_id = obj->give_item_;
      if (registry.items_.contains(item_id)) {
        state.player_.inventory_.items_.push_back(registry.items_[item_id]);
        RenderSystem::printMessage("you picked up " + registry.items_[item_id]->name_ + "!");
      }

      if (registry.interact_by_char_.contains('_')) {
        map.setObjAt(t_x, t_y, registry.interact_by_char_['_']);
      }
    }
  }

  RenderSystem::printInteraction(text_to_display, state, registry);
  AudioSystem::play(sound_to_play, state.player_.sensors_, registry);
}
