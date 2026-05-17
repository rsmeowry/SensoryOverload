Текстовый RPG, Data-Driven, на принципе ООП + гибридного ECS.
Данные хранятся в JSON файлах, для загрузки используется nlohmann/json.
Для звуков используется OpenAL. Рендер - вывод консоли (stdout)

## World State
Мир состоит из нескольких частей: карты (Map), реестра data-driven объектов (Registry), состояния игрока (GlobalState), менеджера сущностей (MobSystem).

Карта - фиксированная сетка с фиксированными препятствиями по миру. Двумерный массив типов тайлов, которые задаются в файлах.
Задается через файлы содержащие карту вида сверху в 2D с символами, а также через файл с определениями тайлов по символам.

## Данные в компонентах
Все данные хранятся в JSON реестрах - массивах из объектов. 

Примеры структур для компонентов:
*Interactable*
* string: Символ на карте
- string: ID звука взаимодействия
- string: Текст стандартного взаимодействия
- int: Урон, наносящийся при взаимодействия
- string: ID эффекта накладывающегося на игрока

*Effect*
- string: ID эффекта
- string: Замещающий текст взаимодействия, либо пустая строка
- float: Новый питч звуков взаимодействий или -1
- float: Параметр для lowpass фильтра или -1

*Mob*
* string: Символ на карте
* string: Текст взаимодействия
* int: Здоровье
* int: радиус перемещения
* int: Наносимый урон (варьируется в игре на 15% в обе стороны)

*Item*
- string: ID предмета
- string: Название предмета
- string: Описание предмета
- bool: Флаг, является ли предмет оружием
- int: Урон, наносимый если экипировать предмет

## Системы

#### InteractionSystem
Обрабатывает взаимодействия с объектами:
1. Проверяет наличие объекта напротив игрока через Transform
2. Считывает Interactable с объекта -> затем из реестра берет данные
3. Согласно состоянию сенсоров проигрывает звук.
4. Выводит корректный текст, либо замещенный, в зависимости от сенсора 

#### MobSystem
Обрабатывает перемещение и логику мобов
1. Для каждой сущности с компонентом Mob выполняет это
2. Выбирает случайную проходимую соседнюю клетку в заданном радиусе от точки спавна
3. Если эта клетка занята игроком, игроку наносится урон
4. Перемещает сущность

#### AudioSystem
Отвечает за вывод звука
1. По полученному id звука ищет его в реестре
2. В зависимости от состояния сенсора выводит либо модулированный (lowpass+pitch), либо изначальный звук

#### RenderSystem
Отвечает за отображение игры в консоли
1. Выводит полученный текст со взаимодействий и команд

## Диаграмы

#### Классовая диаграмма
Показывает актуальную структуру сущностей, компонентов и реестра данных:
```mermaid
%%{init: {'theme': 'default', 'flowchart': {'useMaxWidth': true}}}%%
classDiagram

class Parseable {
	<<interface>>
	+Load(json obj)*
}
class Interactable {
	+char map_char_
	+bool solid_
	+string sound_id_
	+string interact_text_
	+int8_t damage_
	+string apply_effect_
	+int8_t apply_effect_time_
	+string give_item_
}
class Effect {
	+string id_
	+string interact_text_
	+float sound_pitch_
	+float lowpass_param_
}
class Mob {
	+string id_
	+int8_t map_x_
	+int8_t map_y_
	+string interact_text_
	+int8_t max_health_
	+int8_t move_radius_
	+int8_t damage_
}
class Item {
	+string id_
	+string name_
	+string description_
	+bool is_weapon_
	+int8_t damage_
}

Parseable <|-- Interactable
Parseable <|-- Effect
Parseable <|-- Mob
Parseable <|-- Item

class DataRegistry {
	+unordered_map~char, Interactable*~ interact_by_char_
	+unordered_map~string, Mob*~ mobs_
	+unordered_map~string, Item*~ items_
	+unordered_map~string, Effect*~ effects_
	+Load()
}

class Component {
	<<interface>>
	+Act(LivingMob* self, MapData* map, DataRegistry* data)*
}

class Moveable {
	+uint8_t x_
	+uint8_t y_
	+Direction facing_
	+Act(...)
}

class Healable {
	+int8_t current_
	+uint8_t max_
	+Act(...)
}

Component <|-- Moveable
Component <|-- Healable

class LivingMob {
	+Mob mob_
	+bool alive_
	+vector~unique_ptr~Component~~ components_
	+act(MapData* map, DataRegistry* data)
}

LivingMob *-- Component

class Transform {
	+uint8_t x_
	+uint8_t y_
	+Direction facing_
}
class Health {
	+uint8_t current_
	+uint8_t max_
}
class SensorState {
	+string current_effect_
	+uint8_t effect_duration_
}
class Inventory {
	+vector~Item*~ items_
	+uint8_t active_
}
class PlayerState {
	+Transform transform_
	+Health health_
	+SensorState sensors_
	+Inventory inventory_
	+Load(...)
	+IsAlive()
}

PlayerState *-- Transform
PlayerState *-- Health
PlayerState *-- SensorState
PlayerState *-- Inventory

class MapData {
	-vector~vector~Interactable*~~ map_
	+vector~LivingMob~ mobs_
	+uint8_t size_
	+Load(...)
	+ObjAt(...)
	+SetObjAt(...)
}

class GlobalState {
	+PlayerState player_
}

GlobalState *-- PlayerState
```

#### Последовательность (взаимодействия)
Процесс обработки взаимодействия игрока с объектом на карте:
```mermaid
%%{init: {'theme': 'default', 'flowchart': {'useMaxWidth': true}}}%%

sequenceDiagram

participant P as Игрок (ввод)
participant IS as InteractionSystem
participant GS as GlobalState (Игрок)
participant MD as MapData (Карта)
participant OR as DataRegistry (Реестр)
participant AS as AudioSystem
participant RS as RenderSystem

P->>IS: "взаимодействовать/взаим/исп"
IS->>GS: Получить transform_ и sensors_ игрока
IS->>MD: Проверить объект на тайле перед игроком (ObjAt)
MD-->>IS: Возвращает Interactable*
alt Объект существует
	IS->>OR: Найти данные эффекта в реестре (effects_)
	IS->>RS: Вывести текст взаимодействия
	alt У игрока активный эффект?
		RS-->>P: Вывести замещенный эффектом текст (из Effect)
	else
		RS-->>P: Вывести стандартный текст взаимодействия (из Interactable)
	end
	IS->>AS: Проиграть звук (sound_id_)
	alt У игрока активный эффект?
		AS-->>P: Проиграть аудио с pitch/lowpass модуляцией
	else
		AS-->>P: Проиграть аудио без искажений
	end
	opt Объект наносит урон или накладывает эффект/предмет
		IS->>GS: Нанести урон / применить эффект / добавить предмет в инвентарь
	end
end
```

#### Игровой цикл

```mermaid
%%{init: {'theme': 'default', 'flowchart': {'useMaxWidth': true}}}%%
flowchart TD
Start((Начало)) --> Load[Загрузка данных в реестр из JSON]
Load --> Loop{Игрок жив && Игрок не победил && Игра запущена?}
Loop --> Да --> Input[Ожидаем ввод игрока]
Input --> Action{Команда}
Action -- Взаимодействие --> Interact[Система взаимодействия // чек статус сенсоров, объект напротив]
Interact --> MobUpdate[Обновление системы мобов]
Action -- Атака --> Attack[Система атаки // чек предмета игрока, объект напротив] --> MobUpdate
Action -- Инвентарь --> Inventory[Система инвентаря // вывод предметов, выбор оружия] --> MobUpdate

MobUpdate --> PlayerUpdate[Обновление состояния игрока]
PlayerUpdate --> RenderSystem[Рендер в консоль]
RenderSystem --> CheckWin{Проверка условий победы}
CheckWin -- HP <= 0 --> End
CheckWin -- Собраны все логи на карте --> End
CheckWin -- Нет --> Loop
Loop --> Нет --> End((Конец))
```
