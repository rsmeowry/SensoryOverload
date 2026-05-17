#include "consts.h"
#include "data/loader.h"
#include "world/world.h"
#include "system/input.h"
#include "system/mob_system.h"
#include "system/render_system.h"

#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <nlohmann/json.hpp>

std::string dirToStr(Direction dir) {
  switch (dir) {
    case North:
      return "north";
      break;
    case South:
      return "south";
      break;
    case West:
      return "west";
      break;
    case East:
      return "east";
      break;
    default:
      return "";
  }
}

int main() {
  DataRegistry registry;
  registry.load();
  MapData map;
  std::ifstream str(ROOT_DIR + "assets/map.txt");
  std::string map_txt(std::istreambuf_iterator<char>(str), {});
  map.load(&registry, map_txt);

  GlobalState state;
  std::ifstream setup_str(ROOT_DIR + "assets/setup.json");
  if (setup_str.is_open()) {
    nlohmann::json setup_json;
    setup_str >> setup_json;
    state.player_.load(setup_json);
    state.player_.inventory_.items_.emplace_back(registry.items_[setup_json.value("starting_item", "fists")]);
  } else {
    std::cerr << "failed to load setup.json" << std::endl;
  }

  InputSystem input_sys;

  input_sys.registerCommand("help", [](const std::vector<std::string>& args, GlobalState& state, MapData& map, DataRegistry& registry) {
    RenderSystem::printMessage("step - walk forward\n"
                               "right - rotate right\n"
                               "left - rotate left\n"
                               "interact - interact with obj in front\n"
                               "attack - attack the obj in front\n"
                               "inventory - check your inventory\n"
                               "equip <idx> - equip item at index\n"
                               "examine - check your status\n"
                               "quit - give up");
  });
  
  input_sys.registerCommand("walk", [](const std::vector<std::string>& args, GlobalState& state, MapData& map, DataRegistry& registry) {
    auto& t = state.player_.transform_;
    auto t_x = t.x_;
    auto t_y = t.y_;
    if (t.facing_ == North) t_y -= 1;
    else if (t.facing_ == South) t_y += 1;
    else if (t.facing_ == West) t_x -= 1;
    else if (t.facing_ == East) t_x += 1;
    t_y = std::clamp(t_y, static_cast<uint8_t>(0), map.size_);
    t_x = std::clamp(t_x, static_cast<uint8_t>(0), map.size_);
    if (map.objAt(t_x, t_y)->solid_) {
      RenderSystem::printMessage("something blocks your way");
    } else {
      t.x_ = t_x;
      t.y_ = t_y;
      RenderSystem::printMessage("you step one tile forward");
      auto obj = map.objAt(t_x, t_y);
      if (!obj->apply_effect_.empty() && obj->apply_effect_time_ > 0) {
        state.player_.sensors_.current_effect_ = obj->apply_effect_;
        state.player_.sensors_.effect_duration_ = obj->apply_effect_time_;
        RenderSystem::printMessage("you feel like there's something wrong with your sensors");
      }
      if (obj->damage_ != 0) {
        state.player_.health_.current_ -= obj->damage_;
        RenderSystem::printMessage("you feel a burst of pain");
      }
    }
  });

  input_sys.registerCommand("right", [](const std::vector<std::string>& args, GlobalState& state, MapData& map, DataRegistry& registry) {
    auto& t = state.player_.transform_;
    if (t.facing_ == North) t.facing_ = East;
    else if (t.facing_ == West) t.facing_ = North;
    else if (t.facing_ == South) t.facing_ = West;
    else if (t.facing_ == East) t.facing_ = South;
    RenderSystem::printMessage("you rotate right. now facing " + dirToStr(t.facing_));
  });

  input_sys.registerCommand("left", [](const std::vector<std::string>& args, GlobalState& state, MapData& map, DataRegistry& registry) {
    auto& t = state.player_.transform_;
    if (t.facing_ == North) t.facing_ = West;
    else if (t.facing_ == East) t.facing_ = North;
    else if (t.facing_ == South) t.facing_ = East;
    else if (t.facing_ == West) t.facing_ = South;
    RenderSystem::printMessage("you rotate left. now facing " + dirToStr(t.facing_));
  });

  input_sys.registerCommand("interact", [](const std::vector<std::string>& args, GlobalState& state, MapData& map, DataRegistry& registry) {
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

      if (!obj->give_item_.empty()) {
        auto item_id = obj->give_item_;
        if (registry.items_.find(item_id) != registry.items_.end()) {
          state.player_.inventory_.items_.push_back(registry.items_[item_id]);
          RenderSystem::printMessage("you picked up " + registry.items_[item_id]->name_ + "!");
        }

        if (registry.interact_by_char_.find('_') != registry.interact_by_char_.end()) {
          map.setObjAt(t_x, t_y, registry.interact_by_char_['_']);
        }
      }
    }

    RenderSystem::printInteraction(text_to_display, state, registry);
  });

  input_sys.registerCommand("attack", [](const std::vector<std::string>& args, GlobalState& state, MapData& map, DataRegistry& registry) {
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
    bool found_mob = false;

    for (auto& mob : map.mobs_) {
      if (mob.alive_) {
        for (auto& cmp : mob.components_) {
          if (auto m = dynamic_cast<Moveable*>(cmp.get())) {
            if (m->x_ == t_x && m->y_ == t_y) {
              RenderSystem::printMessage("you swing and hit something in front of you");
              for (auto& c: mob.components_) {
                if (auto h = dynamic_cast<Healable*>(c.get())) {
                  h->current_ -= state.player_.inventory_.items_[state.player_.inventory_.active_]->damage_;
                }
              }
              found_mob = true;
              break;
            }
          }
        }
      }
      if (found_mob) break;
    }

    if (!found_mob) {
      RenderSystem::printMessage("you swing but reach nothing");
    }
  });

  input_sys.registerCommand("inventory", [](const std::vector<std::string>& args, GlobalState& state, MapData& map, DataRegistry& registry) {
    auto& inv = state.player_.inventory_;
    for (size_t i = 0; i < inv.items_.size(); ++i) {
      RenderSystem::printMessage(std::to_string(i) + " " + inv.items_[i]->name_ + " - " + inv.items_[i]->description_);
    }
    RenderSystem::printMessage("Currently equipped: " + std::to_string(inv.active_));
  });

  input_sys.registerCommand("examine", [](const std::vector<std::string>& args, GlobalState& state, MapData& map, DataRegistry& registry) {
    RenderSystem::printMessage("HEALTH: " + std::to_string(state.player_.health_.current_) + "/" + std::to_string(state.player_.health_.max_));
    RenderSystem::printMessage("SENSORS: " + state.player_.sensors_.current_effect_ + " : " + std::to_string(state.player_.sensors_.effect_duration_) + " T. LEFT");
    RenderSystem::printMessage("POS: " + std::to_string(state.player_.transform_.x_) + ";" + std::to_string(state.player_.transform_.y_));
    RenderSystem::printMessage("FORWARD: " + dirToStr(state.player_.transform_.facing_));
 });

  input_sys.registerCommand("equip", [](const std::vector<std::string>& args, GlobalState& state, MapData& map, DataRegistry& registry) {
    if (args.size() != 1) {
      RenderSystem::printMessage("no such item");
      return;
    }
    try {
      int i = std::stoi(args[0]);
      if (i >= 0 && i < state.player_.inventory_.items_.size()) {
        state.player_.inventory_.active_ = static_cast<uint8_t>(i);
        RenderSystem::printMessage("EQUIPPED " + std::to_string(i));
      } else {
        RenderSystem::printMessage("no such item");
      }
    } catch (...) {
      RenderSystem::printMessage("no such item");
    }
  });

  input_sys.registerAlias("inv", "inventory");
  input_sys.registerAlias("step", "walk");
  input_sys.registerAlias("int", "interact");
  input_sys.registerAlias("atk", "attack");

  RenderSystem::printMessage("your mission begins");

  int total_logs = std::count(map_txt.begin(), map_txt.end(), 'L');

  bool running = true;
  while (running && state.player_.isAlive()) {
    std::cout << "> ";
    std::string input;
    if (!std::getline(std::cin, input)) {
      break;
    }
    
    if (input == "quit" || input == "exit") {
      running = false;
      continue;
    }

    // only ticks if command exected
    if (input_sys.executeCommand(input, state, map, registry)) {
      for (auto& mob : map.mobs_) {
        if (mob.alive_) {
          mob.act(&map, &registry);
        }
      }
      
      MobSystem::update(map, state, registry);
      
      if (state.player_.sensors_.effect_duration_ > 0) {
        state.player_.sensors_.effect_duration_--;
        if (state.player_.sensors_.effect_duration_ == 0) {
          RenderSystem::printMessage("your sensors return to normal.");
          state.player_.sensors_.current_effect_ = "";
        }
      }

      int logs_collected = 0;
      for (auto* item : state.player_.inventory_.items_) {
        if (item->id_ == "log") logs_collected++;
      }
      
      if (total_logs > 0 && logs_collected >= total_logs) {
        RenderSystem::printMessage("congratulations! you collected all the data logs!");
        break;
      }
    }
  }

  if (!state.player_.isAlive()) {
    RenderSystem::printMessage("failure... you died...");
  }

  return 0;
}
