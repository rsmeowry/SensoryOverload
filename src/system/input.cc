#include "input.h"
#include <sstream>
#include <iostream>
#include "render_system.h"

void InputSystem::RegisterCommand(const std::string& name, CommandCallback callback) {
  commands_[name] = callback;
}

void InputSystem::RegisterAlias(const std::string& alias, const std::string& command_name) {
  if (commands_.find(command_name) != commands_.end()) {
    commands_[alias] = commands_[command_name];
  }
}

bool InputSystem::ExecuteCommand(const std::string& input, GlobalState& state, MapData& map, DataRegistry& registry) {
  if (input.empty()) return false;

  std::istringstream iss(input);
  std::string command_name;
  iss >> command_name;

  std::vector<std::string> args;
  std::string arg;
  while (iss >> arg) {
    args.push_back(arg);
  }

  // full command
  if (commands_.find(input) != commands_.end()) {
    commands_[input]({}, state, map, registry);
    return true;
  }

  // first wrd
  auto it = commands_.find(command_name);
  if (it != commands_.end()) {
    it->second(args, state, map, registry);
    return true;
  }

  RenderSystem::PrintMessage("undefined command: " + command_name);
  return false;
}