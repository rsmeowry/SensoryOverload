#include "mob_system.h"
#include <random>
#include <iostream>
#include <cmath>
#include <algorithm>

void MobSystem::update(MapData& map, GlobalState& state, DataRegistry& registry) {
  std::random_device rd;
  std::mt19937 gen(rd());

  for (auto& mob : map.mobs_) {
    if (!mob.alive_) continue;

    Moveable* mov = nullptr;
    for (auto& cmp : mob.components_) {
      if (auto m = dynamic_cast<Moveable*>(cmp.get())) {
        mov = m;
        break;
      }
    }

    if (!mov) continue;

    int origin_x = mob.mob_.map_x_;
    int origin_y = mob.mob_.map_y_;
    int radius = mob.mob_.move_radius_;

    struct Pos { int x, y; };
    std::vector<Pos> valid_moves;

    Pos current = { mov->x_, mov->y_ };
    Pos neighbors[4] = {
      {current.x, current.y - 1},//north
      {current.x, current.y + 1},// south
      {current.x - 1, current.y},// west
      {current.x + 1, current.y}// east
    };

    for (const auto& n : neighbors) {
      if (n.x >= 0 && n.y >= 0 && n.x < map.size_ && n.y < map.size_) {
        bool is_player = (n.x == state.player_.transform_.x_ && n.y == state.player_.transform_.y_);
        
        auto obj = map.objAt(n.x, n.y);
        if (is_player || (obj && !obj->solid_)) {
          if (std::max(std::abs(n.x - origin_x), std::abs(n.y - origin_y)) <= radius) {
            valid_moves.push_back(n);
          }
        }
      }
    }

    if (!valid_moves.empty()) {
      std::uniform_int_distribution<> dist(0, valid_moves.size() - 1);
      Pos next_move = valid_moves[dist(gen)];

      if (next_move.x == state.player_.transform_.x_ && next_move.y == state.player_.transform_.y_) {
        int base_damage = mob.mob_.damage_;
        std::uniform_int_distribution<> dmg_dist(
          static_cast<int>(base_damage * 0.85),
          static_cast<int>(std::ceil(base_damage * 1.15))
        );
        int damage = dmg_dist(gen);

        if (state.player_.health_.current_ > damage) {
          state.player_.health_.current_ -= damage;
        } else {
          state.player_.health_.current_ = 0;
        }
        
        std::cout << "you feel a burst of pain from somewhere. you took " << damage << " damage." << std::endl;
      } else {
        mov->x_ = next_move.x;
        mov->y_ = next_move.y;
      }
    }
  }
}
