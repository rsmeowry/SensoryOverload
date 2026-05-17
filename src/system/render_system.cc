#include "render_system.h"
#include <iostream>

void RenderSystem::PrintMessage(const std::string& msg) {
  std::cout << msg << std::endl;
}

void RenderSystem::PrintInteraction(const std::string& default_text, GlobalState& state, DataRegistry& registry) {
  std::string text_to_display = default_text;
  
  if (!state.player_.sensors_.current_effect_.empty()) {
    auto it = registry.effects_.find(state.player_.sensors_.current_effect_);
    if (it != registry.effects_.end() && !it->second->interact_text_.empty()) {
      text_to_display = it->second->interact_text_;
    }
  }

  std::cout << text_to_display << std::endl;
}
