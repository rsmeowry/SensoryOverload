#include "consts.h"
#include "data/loader.h"
#include "world/world.h"
#include "system/input.h"

#include <fstream>
#include <iostream>
#include <string>
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
  } else {
    std::cerr << "Failed to load setup.json" << std::endl;
  }

  InputSystem input_sys;

  input_sys.registerCommand("help", [](const std::vector<std::string>& args, GlobalState& state, MapData& map, DataRegistry& registry) {
    std::cout << "step - walk forward" << std::endl;
    std::cout << "right - rotate right" << std::endl;
    std::cout << "left - rotate left" << std::endl;
    std::cout << "interact - interact with obj in front" << std::endl;
    std::cout << "attack - attack the obj in front" << std::endl;
    std::cout << "inventory - check your inventory" << std::endl;
    std::cout << "equip <idx> - equip item at index" << std::endl;
    std::cout << "examine - check your status" << std::endl;
    std::cout << "quit - give up" << std::endl;
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
      std::cout << "something blocks your way" << std::endl;
    } else {
      t.x_ = t_x;
      t.y_ = t_y;
      std::cout << "you step one tile forward" << std::endl;
      auto obj = map.objAt(t_x, t_y);
      if (!obj->apply_effect_.empty() && obj->apply_effect_time_ > 0) {
        state.player_.sensors_.current_effect_ = obj->apply_effect_;
        state.player_.sensors_.effect_duration_ = obj->apply_effect_time_;
        std::cout << "you feel like there's something wrong with your sensors" << std::endl;
      }
      if (obj->damage_ != 0) {
        state.player_.health_.current_ -= obj->damage_;
        std::cout << "you feel a burst of pain" << std::endl;
      }
    }
  });

  input_sys.registerCommand("right", [](const std::vector<std::string>& args, GlobalState& state, MapData& map, DataRegistry& registry) {
    auto& t = state.player_.transform_;
    if (t.facing_ == North) t.facing_ = East;
    else if (t.facing_ == West) t.facing_ = North;
    else if (t.facing_ == South) t.facing_ = West;
    else if (t.facing_ == East) t.facing_ = South;
    std::cout << "you rotate right. now facing " << dirToStr(t.facing_) << std::endl;
  });

  input_sys.registerCommand("left", [](const std::vector<std::string>& args, GlobalState& state, MapData& map, DataRegistry& registry) {
    auto& t = state.player_.transform_;
    if (t.facing_ == North) t.facing_ = West;
    else if (t.facing_ == East) t.facing_ = North;
    else if (t.facing_ == South) t.facing_ = East;
    else if (t.facing_ == West) t.facing_ = South;
    std::cout << "you rotate left. now facing " << dirToStr(t.facing_) << std::endl;
  });

  input_sys.registerCommand("interact", [](const std::vector<std::string>& args, GlobalState& state, MapData& map, DataRegistry& registry) {
    std::cout << "BOOP" << std::endl;
  });

  input_sys.registerCommand("attack", [](const std::vector<std::string>& args, GlobalState& state, MapData& map, DataRegistry& registry) {
    std::cout << "ATTACKED" << std::endl;
  });

  input_sys.registerCommand("inventory", [](const std::vector<std::string>& args, GlobalState& state, MapData& map, DataRegistry& registry) {
    std::cout << "INV" << std::endl;
  });

  input_sys.registerCommand("examine", [](const std::vector<std::string>& args, GlobalState& state, MapData& map, DataRegistry& registry) {
    std::cout << "HEALTH: " << static_cast<int>(state.player_.health_.current_) << "/" << static_cast<int>(state.player_.health_.max_) << std::endl;
    std::cout << "SENSORS: " << state.player_.sensors_.current_effect_ << " : " << state.player_.sensors_.effect_duration_ << " T. LEFT" << std::endl;
    std::cout << "POS: " << static_cast<int>(state.player_.transform_.x_) << ";" << static_cast<int>(state.player_.transform_.y_) << std::endl;
    std::cout << "FORWARD: " << dirToStr(state.player_.transform_.facing_) << std::endl;
 });

  input_sys.registerCommand("equip", [](const std::vector<std::string>& args, GlobalState& state, MapData& map, DataRegistry& registry) {
    if (args.size() != 1) {
      std::cout << "no such item" << std::endl;
      return;
    }
    auto i = std::stoi(args[0]);
    std::cout << "EQUIPPED " << i << std::endl;
  });

  input_sys.registerAlias("inv", "inventory");
  input_sys.registerAlias("step", "walk");
  input_sys.registerAlias("int", "interact");
  input_sys.registerAlias("atk", "attack");

  std::cout << "your mission begins" << std::endl;

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
      
      if (state.player_.sensors_.effect_duration_ > 0) {
        state.player_.sensors_.effect_duration_--;
        if (state.player_.sensors_.effect_duration_ == 0) {
          std::cout << "your sensors return to normal." << std::endl;
          state.player_.sensors_.current_effect_ = "";
        }
      }
    }
  }

  return 0;
}
