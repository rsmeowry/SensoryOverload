#include "consts.h"
#include "data/loader.h"
#include "system/audio_system.h"
#include "system/input.h"
#include "system/interaction_system.h"
#include "system/mob_system.h"
#include "system/render_system.h"
#include "world/world.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

std::string DirToStr(Direction dir) {
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
  registry.Load();
  MapData map;
  std::ifstream str(kRootDir + "assets/map.txt");
  std::string map_txt(std::istreambuf_iterator<char>(str), {});
  map.Load(&registry, map_txt);

  GlobalState state;
  std::ifstream setup_str(kRootDir + "assets/setup.json");
  if (setup_str.is_open()) {
    nlohmann::json setup_json;
    setup_str >> setup_json;
    state.player_.Load(setup_json);
    std::string starting_item = "fists";
    if (setup_json.contains("player")) {
      starting_item = setup_json["player"].value("starting_item", "fists");
    }
    state.player_.inventory_.items_.emplace_back(
        registry.items_[starting_item]);
  } else {
    std::cerr << "failed to load setup.json" << std::endl;
  }

  AudioSystem::Init();

  InputSystem input_sys;

  input_sys.RegisterCommand("help", [](const std::vector<std::string> &args,
                                       GlobalState &state, MapData &map,
                                       DataRegistry &registry) {
    RenderSystem::PrintMessage("step - walk forward\n"
                               "right - rotate right\n"
                               "left - rotate left\n"
                               "interact - interact with obj in front\n"
                               "attack - attack the obj in front\n"
                               "inventory - check your inventory\n"
                               "equip <idx> - equip item at index\n"
                               "examine - check your status\n"
                               "quit - give up");
  });

  input_sys.RegisterCommand("walk", [](const std::vector<std::string> &args,
                                       GlobalState &state, MapData &map,
                                       DataRegistry &registry) {
    auto &t = state.player_.transform_;
    auto t_x = t.x_;
    auto t_y = t.y_;
    if (t.facing_ == North)
      t_y -= 1;
    else if (t.facing_ == South)
      t_y += 1;
    else if (t.facing_ == West)
      t_x -= 1;
    else if (t.facing_ == East)
      t_x += 1;
    t_y = std::clamp(t_y, static_cast<uint8_t>(0), map.size_);
    t_x = std::clamp(t_x, static_cast<uint8_t>(0), map.size_);
    if (map.ObjAt(t_x, t_y)->solid_) {
      RenderSystem::PrintMessage("something blocks your way");
    } else {
      t.x_ = t_x;
      t.y_ = t_y;
      RenderSystem::PrintMessage("you step one tile forward");
      auto obj = map.ObjAt(t_x, t_y);
      if (!obj->apply_effect_.empty() && obj->apply_effect_time_ > 0) {
        state.player_.sensors_.current_effect_ = obj->apply_effect_;
        state.player_.sensors_.effect_duration_ = obj->apply_effect_time_;
        RenderSystem::PrintMessage(
            "you feel like there's something wrong with your sensors");
      }
      if (obj->damage_ != 0) {
        state.player_.health_.current_ -= obj->damage_;
        RenderSystem::PrintMessage("you feel a burst of pain");
      }
    }
  });

  input_sys.RegisterCommand("right", [](const std::vector<std::string> &args,
                                        GlobalState &state, MapData &map,
                                        DataRegistry &registry) {
    auto &t = state.player_.transform_;
    if (t.facing_ == North)
      t.facing_ = East;
    else if (t.facing_ == West)
      t.facing_ = North;
    else if (t.facing_ == South)
      t.facing_ = West;
    else if (t.facing_ == East)
      t.facing_ = South;
    RenderSystem::PrintMessage("you rotate right. now facing " +
                               DirToStr(t.facing_));
  });

  input_sys.RegisterCommand("left", [](const std::vector<std::string> &args,
                                       GlobalState &state, MapData &map,
                                       DataRegistry &registry) {
    auto &t = state.player_.transform_;
    if (t.facing_ == North)
      t.facing_ = West;
    else if (t.facing_ == East)
      t.facing_ = North;
    else if (t.facing_ == South)
      t.facing_ = East;
    else if (t.facing_ == West)
      t.facing_ = South;
    RenderSystem::PrintMessage("you rotate left. now facing " +
                               DirToStr(t.facing_));
  });

  input_sys.RegisterCommand("interact", [](const std::vector<std::string> &args,
                                           GlobalState &state, MapData &map,
                                           DataRegistry &registry) {
    InteractionSystem::OnInteract(state, map, registry);
  });

  input_sys.RegisterCommand("attack", [](const std::vector<std::string> &args,
                                         GlobalState &state, MapData &map,
                                         DataRegistry &registry) {
    auto &t = state.player_.transform_;
    auto t_x = t.x_;
    auto t_y = t.y_;
    if (t.facing_ == North)
      t_y -= 1;
    else if (t.facing_ == South)
      t_y += 1;
    else if (t.facing_ == West)
      t_x -= 1;
    else if (t.facing_ == East)
      t_x += 1;

    t_y = std::clamp(t_y, static_cast<uint8_t>(0), map.size_);
    t_x = std::clamp(t_x, static_cast<uint8_t>(0), map.size_);

    std::string text_to_display = "";
    bool found_mob = false;

    for (auto &mob : map.mobs_) {
      if (mob.alive_) {
        for (auto &cmp : mob.components_) {
          if (auto m = dynamic_cast<Moveable *>(cmp.get())) {
            if (m->x_ == t_x && m->y_ == t_y) {
              RenderSystem::PrintMessage(
                  "you swing and hit something in front of you");
              for (auto &c : mob.components_) {
                if (auto h = dynamic_cast<Healable *>(c.get())) {
                  h->current_ -= state.player_.inventory_
                                     .items_[state.player_.inventory_.active_]
                                     ->damage_;
                }
              }
              found_mob = true;
              break;
            }
          }
        }
      }
      if (found_mob)
        break;
    }

    if (!found_mob) {
      RenderSystem::PrintMessage("you swing but reach nothing");
    }
  });

  input_sys.RegisterCommand(
      "inventory", [](const std::vector<std::string> &args, GlobalState &state,
                      MapData &map, DataRegistry &registry) {
        auto &inv = state.player_.inventory_;
        for (size_t i = 0; i < inv.items_.size(); ++i) {
          RenderSystem::PrintMessage(std::to_string(i) + " " +
                                     inv.items_[i]->name_ + " - " +
                                     inv.items_[i]->description_);
        }
        RenderSystem::PrintMessage("Currently equipped: " +
                                   std::to_string(inv.active_));
      });

  input_sys.RegisterCommand("examine", [](const std::vector<std::string> &args,
                                          GlobalState &state, MapData &map,
                                          DataRegistry &registry) {
    RenderSystem::PrintMessage(
        "HEALTH: " + std::to_string(state.player_.health_.current_) + "/" +
        std::to_string(state.player_.health_.max_));
    RenderSystem::PrintMessage(
        "SENSORS: " + state.player_.sensors_.current_effect_ + " : " +
        std::to_string(state.player_.sensors_.effect_duration_) + " T. LEFT");
    RenderSystem::PrintMessage(
        "POS: " + std::to_string(state.player_.transform_.x_) + ";" +
        std::to_string(state.player_.transform_.y_));
    RenderSystem::PrintMessage("FORWARD: " +
                               DirToStr(state.player_.transform_.facing_));
  });

  input_sys.RegisterCommand("equip", [](const std::vector<std::string> &args,
                                        GlobalState &state, MapData &map,
                                        DataRegistry &registry) {
    if (args.size() != 1) {
      RenderSystem::PrintMessage("no such item");
      return;
    }
    try {
      int i = std::stoi(args[0]);
      if (i >= 0 && i < state.player_.inventory_.items_.size()) {
        state.player_.inventory_.active_ = static_cast<uint8_t>(i);
        RenderSystem::PrintMessage("EQUIPPED " + std::to_string(i));
      } else {
        RenderSystem::PrintMessage("no such item");
      }
    } catch (...) {
      RenderSystem::PrintMessage("no such item");
    }
  });

  input_sys.RegisterAlias("inv", "inventory");
  input_sys.RegisterAlias("step", "walk");
  input_sys.RegisterAlias("int", "interact");
  input_sys.RegisterAlias("atk", "attack");

  RenderSystem::PrintMessage("your mission begins");

  int total_logs = std::count(map_txt.begin(), map_txt.end(), 'L');

  bool running = true;
  while (running && state.player_.IsAlive()) {
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
    if (input_sys.ExecuteCommand(input, state, map, registry)) {
      for (auto &mob : map.mobs_) {
        if (mob.alive_) {
          mob.act(&map, &registry);
        }
      }

      MobSystem::Update(map, state, registry);

      if (state.player_.sensors_.effect_duration_ > 0) {
        state.player_.sensors_.effect_duration_--;
        if (state.player_.sensors_.effect_duration_ == 0) {
          RenderSystem::PrintMessage("your sensors return to normal.");
          state.player_.sensors_.current_effect_ = "";
        }
      }

      int logs_collected = 0;
      for (auto *item : state.player_.inventory_.items_) {
        if (item->id_ == "log")
          logs_collected++;
      }

      if (total_logs > 0 && logs_collected >= total_logs) {
        RenderSystem::PrintMessage(
            "congratulations! you collected all the data logs!");
        break;
      }
    }
  }

  if (!state.player_.IsAlive()) {
    RenderSystem::PrintMessage("failure... you died...");
  }

  AudioSystem::Cleanup();
  return 0;
}
