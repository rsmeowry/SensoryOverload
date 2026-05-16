#include "consts.h"
#include "data/loader.h"
#include "world/world.h"

#include <fstream>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>

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

  std::cout << "Game started!" << std::endl;
  std::cout << "Player starts at (" << (int)state.player_.transform_.x_ << ", " << (int)state.player_.transform_.y_ << ") with " << (int)state.player_.health_.current_ << " HP." << std::endl;
  
  bool running = true;
  while (running) {
    std::cout << "> ";
    std::string input;
    if (!std::getline(std::cin, input)) {
      break;
    }
    
    if (input == "quit" || input == "exit") {
      running = false;
      continue;
    }

    // Placeholder logic with input
    std::cout << "Player executed: " << input << std::endl;

    // Tick all mobs
    for (auto& mob : map.mobs_) {
      if (mob.alive_) {
        mob.act(&map, &registry);
      }
    }
  }

  return 0;
}
