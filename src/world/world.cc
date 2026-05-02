#include "world.h"

void MapData::load(DataLoader* loader, std::string map) {
  std::istringstream str(map);
  std::vector<std::string> lines;
  std::string tk;
  while (std::getline(str, tk, '\n')) {
    lines.push_back(tk);
  }

  std::vector<std::vector<Interactable*>> inter {};
  for (auto line: lines) {
    std::vector<char> vc(line.begin(), line.end());

    inter.push_back(vc);
  }
}