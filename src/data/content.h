#ifndef SENSORYOVERLOAD_CONTENT_H
#define SENSORYOVERLOAD_CONTENT_H

#include <nlohmann/json.hpp>

class Parseable {
public:
  virtual ~Parseable() = default;
  virtual void load(nlohmann::json& obj) = 0;
};

class Interactable final : public Parseable {
public:
  char map_char_;
  bool solid_;
  std::string sound_id_;
  std::string interact_text_;
  int8_t damage_;
  std::string apply_effect_;
  std::string give_item_;

  void load(nlohmann::json &obj) override;

  ~Interactable() override = default;
};

class Effect final : public Parseable {
public:
  std::string id_;
  std::string interact_text_;
  float sound_pitch_ = -1;
  float lowpass_param_ = -1;

  void load(nlohmann::json &obj) override;

  ~Effect() override = default;
};

class Mob final: public Parseable {
public:
  std::string id_;
  int8_t map_x_;
  int8_t map_y_;
  std::string interact_text_;
  int8_t max_health_;
  int8_t move_radius_;
  int8_t damage_;

  void load(nlohmann::json &obj) override;

  ~Mob() override = default;
};

class Item final: public Parseable {
public:
  std::string id_;
  std::string name_;
  std::string description_;
  bool is_weapon_;
  int8_t damage_;

  void load(nlohmann::json &obj) override;

  ~Item() override = default;
};

#endif // SENSORYOVERLOAD_CONTENT_H
