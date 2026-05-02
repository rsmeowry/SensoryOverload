#ifndef SENSORYOVERLOAD_LOADER_H
#define SENSORYOVERLOAD_LOADER_H
#include "content.h"

#include <unordered_map>

class DataRegistry {
public:
  std::unordered_map<char, Interactable *> interact_by_char_;
  std::unordered_map<std::string, Mob*> mobs_;
  std::unordered_map<std::string, Item*> items_;
  std::unordered_map<std::string, Effect*> effects_;

  void load();
};

#endif // SENSORYOVERLOAD_LOADER_H
