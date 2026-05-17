#ifndef SENSORYOVERLOAD_PLAYER_H
#define SENSORYOVERLOAD_PLAYER_H

#include "../component/component.h"
#include "../data/content.h"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

struct Transform {
  uint8_t x_;
  uint8_t y_;
  Direction facing_;
};

struct Health {
  uint8_t current_;
  uint8_t max_;
};

struct SensorState {
  std::string current_effect_;
  uint8_t effect_duration_ = 0;
};

struct Inventory {
  std::vector<Item *> items_;
  uint8_t active_;
};

struct PlayerState {
  Transform transform_;
  Health health_;
  SensorState sensors_;
  Inventory inventory_;

  void Load(const nlohmann::json &obj) {
    if (obj.contains("player")) {
      auto p = obj["player"];
      transform_.x_ = p.value("x", 0);
      transform_.y_ = p.value("y", 0);

      std::string face = p.value("facing", "South");
      if (face == "North")
        transform_.facing_ = North;
      else if (face == "South")
        transform_.facing_ = South;
      else if (face == "West")
        transform_.facing_ = West;
      else if (face == "East")
        transform_.facing_ = East;
      else
        transform_.facing_ = South;

      health_.current_ = p.value("health", 100);
      health_.max_ = p.value("max_health", 100);
    }
  }

  bool IsAlive() const { return health_.current_ > 0; }
};

#endif // SENSORYOVERLOAD_PLAYER_H
