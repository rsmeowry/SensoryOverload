#include "loader.h"
#include "../consts.h"

#include <fstream>

template <typename TKey, typename TParseable>
std::unordered_map<TKey, TParseable*> parseJsons(std::ifstream stream, std::function<TParseable*()> fac, std::function<TKey(TParseable*)> key_getter) {
  nlohmann::json json;
  stream >> json;
  auto as_vec = json.get<std::vector<nlohmann::json>>();
  std::unordered_map<TKey, TParseable*> parsed;

  for(auto v: as_vec) {
    Parseable* o = fac();
    o->load(v);
    auto re_cast = reinterpret_cast<TParseable*>(o);
    parsed.emplace(key_getter(re_cast), re_cast);
  }

  return parsed;
}

void DataRegistry::load() {
  std::ifstream interact(ROOT_DIR + "assets/interactables.json");
  std::ifstream effects(ROOT_DIR + "assets/effects.json");
  std::ifstream mobs(ROOT_DIR + "assets/mobs.json");
  std::ifstream items(ROOT_DIR + "assets/items.json");
  std::ifstream map(ROOT_DIR + "assets/map.txt");

  // interactable
  this->interact_by_char_ = parseJsons<char, Interactable>(std::move(interact), []() { return new Interactable(); }, [](auto i) { return i->map_char_; });
  this->effects_ = parseJsons<std::string, Effect>(std::move(effects), []() { return new Effect(); }, [](auto o) { return o->id_; });
  this->mobs_ = parseJsons<std::string, Mob>(std::move(mobs), []() { return new Mob(); }, [](auto o) { return o->id_; });
  this->items_ = parseJsons<std::string, Item>(std::move(items), []() { return new Item(); }, [](auto o) { return o->id_; });
}