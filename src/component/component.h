#ifndef SENSORYOVERLOAD_COMPONENT_H
#define SENSORYOVERLOAD_COMPONENT_H
#include <cstdint>

class LivingMob;
class DataRegistry;
struct MapData;

enum Direction {
  North,
  South,
  West,
  East
};

struct Component {
  virtual ~Component() = default;
  virtual void act(LivingMob* self, MapData* map, DataRegistry* data) = 0;
};

struct Moveable: Component {
  uint8_t x_;
  uint8_t y_;
  Direction facing_;
  void act(LivingMob* self, MapData* map, DataRegistry* data) override;
};

struct Healable: Component {
  int8_t current_;
  uint8_t max_;
  void act(LivingMob* self, MapData* map, DataRegistry* data) override;
};

#endif // SENSORYOVERLOAD_COMPONENT_H
