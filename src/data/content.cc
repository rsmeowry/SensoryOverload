#include "content.h"
void Interactable::load(nlohmann::json &obj) {
  map_char_ = obj["map_char"].get<std::string>();
  sound_id_ = obj["sound_id"].get<std::string>();
  interact_text_ = obj["interact_text"].get<std::string>();
  damage_ = obj["damage"].get<int8_t>();
  apply_effect_ = obj["apply_effect"].get<std::string>();
  give_item_ = obj["give_item"].get<std::string>();
}

void Effect::load(nlohmann::json &obj) {
  id_ = obj["id"].get<std::string>();
  interact_text_ = obj["interact_text"].get<std::string>();
  sound_pitch_ = obj["sound_pitch"].get<float>();
  lowpass_param_ = obj["lowpass_param"].get<float>();
}

void Mob::load(nlohmann::json &obj) {
  map_x_ = obj["x"].get<int8_t>();
  map_y_ = obj["y"].get<int8_t>();
  interact_text_ = obj["interact_text"].get<std::string>();
  max_health_ = obj["max_health"].get<int8_t>();
  move_radius_ = obj["move_radius"].get<int8_t>();
  damage_ = obj["damage"].get<int8_t>();
}

void Item::load(nlohmann::json &obj){
  id_ = obj["id"].get<std::string>();
  name_ = obj["name"].get<std::string>();
  description_ = obj["description"].get<std::string>();
  is_weapon_ = obj["is_weapon"].get<bool>();
  damage_ = obj["damage"].get<int8_t>();
}