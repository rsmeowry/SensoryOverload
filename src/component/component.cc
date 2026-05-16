#include "component.h"

#include <iostream>
#include "../world/mob.h"

void Moveable::act(LivingMob *self, MapData *map, DataRegistry *data) {

}

void Healable::act(LivingMob *self, MapData *map, DataRegistry *data) {
  self->alive_ = this->current_ > 0;
}