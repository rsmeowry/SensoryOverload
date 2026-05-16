#include "consts.h"
#include "data/loader.h"
#include "world/world.h"

#include <fstream>
#include <iostream>

int main() {
  DataRegistry registry;
  registry.load();
  MapData map;
  std::ifstream str(ROOT_DIR + "assets/map.txt");
  std::string map_txt(std::istreambuf_iterator<char>(str), {});
  map.load(&registry, map_txt);
  return 0;
}
