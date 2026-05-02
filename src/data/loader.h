
#ifndef SENSORYOVERLOAD_LOADER_H
#define SENSORYOVERLOAD_LOADER_H

class DataLoader {
public:
  std::unordered_map<char, Interactable*> interact_by_char_;

  void load();
};

#endif // SENSORYOVERLOAD_LOADER_H
