#include "component.h"

#include <iostream>
#include "../world/mob.h"

void Moveable::Act(LivingMob *self, MapData *map, DataRegistry *data) {

}

void Healable::Act(LivingMob *self, MapData *map, DataRegistry *data) {
  self->alive_ = this->current_ > 0;
}