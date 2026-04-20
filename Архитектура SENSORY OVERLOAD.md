Текстовый RPG, Data-Driven, на принципе ECS.
Данные хранятся в JSON файлах, для загрузки используется nlohmann/json.
Для звуков используется OpenAL. Рендер - вывод консоли (stdout)

## World State
Мир состоит из нескольких частей: карты (Map), реестра data-driven объектов (Registry), менеджера сущностей для ECS (EntityManager), глобального состояния: логи т.п. (GlobalState).

Карта - фиксированная сетка с фиксированными препятствиями по миру. Двумерный массив типов тайлов, которые задаются в файлах.
Задается через файлы содержащие карту вида сверху в 2D с символами, а также через файл с определениями тайлов по символам.

## Компоненты
Основные компоненты:
- Transform - координаты на сетке, направление
- Healable - объект содержащий здоровье
- SensorState - состояние сенсоров. Влияют на восприятие мира.
- Inventory - список всех предметов у сущности
- Interactable - содержит тег, указывающий на локацию в реестре, определяющее звук, текст взаимодействия и т.п.
- Mob - содержит состояние и логику для мобов (для рандомного перемещения)

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
2. Выбирает случайную проходимую соседнюю клетку в заданном радиусе
3. Через Transform перемещает сущность

#### AudioSystem
Отвечает за вывод звука
1. По полученному id звука ищет его в реестре
2. В зависимости от состояния сенсора выводит либо модулированный (lowpass+pitch), либо сломанный звук шума, либо изначальный звук

#### RenderSystem
Отвечает за отображение игры в консоли
1. Выводит полученный текст со взаимодействий и команд

## Диаграмы

#### Классовая диаграмма
Показывает примерную структуру ECS
```mermaid
%%{init: {'theme': 'default', 'flowchart': {'useMaxWidth': true}}}%%
classDiagram

class ObjectRegistry {
	map~string, InteractableData~ interactables_
	map~string, MobData~ mobs_
	map~string, EffectData~ effects_
	map~string, Sound~ sounds_
}

class EntityManager {
	map~uint64_t, ComponentList~ entities_

	createEntity()
	addComponent(entity, component)
	getComponents~T~()
}
class InteractionSystem {
	onInteract(EntityManager mgr, uint64_t player, uint64_t target)
}
class RenderSystem {
	render(EntityManager mgr)
}
class MobSystem {
	update(EntityManager mgr)
}
class AudioSystem {
	play(string soundId, SensorState sensors)
}
class Component {
<<interface>>
}
class Transform {
	int x_
	int y_
	Direction facing_
}
class Healable {
	int current_
	int max_
}
class SensorState {
	string current_effect_
}
class Interactable {
	string object_id_
}
class Mob {
	string mob_id_
}

EntityManager --> Component
EntityManager ..> ObjectRegistry
MobSystem ..> EntityManager
AudioSystem ..> EntityManager
InteractionSystem ..> EntityManager
RenderSystem ..> EntityManager

Component --> Transform
Component --> Healable
Component --> SensorState
Component --> Interactable
Component --> Mob

```

#### Последовательность (взаимодействия)
Что происходит при взаимодействии с объектом
```mermaid
%%{init: {'theme': 'default', 'flowchart': {'useMaxWidth': true}}}%%

sequenceDiagram

participant P as Игрок (ввод)
participant IS as InteractionSystem
participant EM as EntityManager
participant OR as ObjectRegistry
participant AS as AudioSystem
participant RS as RenderSystem

P->>IS: "взаимодействовать/взаим/исп"
IS->>EM: Получить компоненты Transform, SensorState с игрока
IS->>EM: Получить сущн. на тайле спереди игрока
IS->>EM: Получить компонент Interactable с объекта
IS->>OR: Получить данные объекта
IS->>RS: Вывести текст взаимодействия
RS->>OR: Получить данные эффекта на игроке
alt Эффект влияет на текст?
	RS-->>P: Вывести замещенный эффектом текст
else
	RS-->>P: Вывести текст взаимодействия
end
IS->>AS: Проиграть звук взаимодействия
AS->>OR: Получить данные эффекта на игроке
alt Эффект влияет на аудио?
	AS-->>P: Проиграть модулированное аудио
else
	AS-->>P: Проиграть обычное аудио
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
CheckWin -- 5 логов + выход --> End
CheckWin -- Нет --> Loop
Loop --> Нет --> End((Конец))
```

