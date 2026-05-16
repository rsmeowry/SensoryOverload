#include "world.h"

#include <iostream>
#include <sstream>
#include <ranges>
#include "../data/loader.h"
#include "../world/mob.h"

void MapData::load(DataRegistry *loader, std::string map) {
  std::istringstream str(map);
  std::string tok;
  auto y = 0;
  while (std::getline(str, tok, '\n')) {
    // parsing line
    std::vector<Interactable*> interactables;
    const char *chs = tok.c_str();
    auto x = 0;
    for (const char *p = chs; *p != '\0'; p++) {
      interactables.push_back(loader->interact_by_char_.at(*p));
      x++;
    }
    y++;
    this->map_.emplace_back(interactables);
  }
  size_ = y;

  for (const auto val : loader->mobs_ | std::views::values) {
    LivingMob living(*val);
    mobs_.emplace_back(*val);
  }
}
Interactable *MapData::objAt(const uint8_t x, const uint8_t y) const {
  return map_.at(y).at(x);
}

void MapData::setObjAt(uint8_t x, uint8_t y, Interactable* obj) {
  if (y < map_.size() && x < map_[y].size()) {
    map_[y][x] = obj;
  }
}