#ifndef SENSORYOVERLOAD_INPUT_H
#define SENSORYOVERLOAD_INPUT_H

#include "../world/world.h"
#include "../data/loader.h"
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

using CommandCallback = std::function<void(const std::vector<std::string>& args, GlobalState& state, MapData& map, DataRegistry& registry)>;

class InputSystem {
public:
  std::unordered_map<std::string, CommandCallback> commands_;

  void RegisterCommand(const std::string& name, CommandCallback callback);
  void RegisterAlias(const std::string& alias, const std::string& command_name);
  
  bool ExecuteCommand(const std::string& input, GlobalState& state, MapData& map, DataRegistry& registry);
};

#endif // SENSORYOVERLOAD_INPUT_H
