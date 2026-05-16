#include "world.h"

#include <iostream>

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

  for (const auto val : loader->mobs_ | std::views::values) {
    LivingMob living(val->map_x_, val->map_y_, *val);
    mobs_.emplace_back(living);
  }
}
Interactable *MapData::obj_at(const int8_t x, const int8_t y) const {
  return map_.at(y).at(x);
}