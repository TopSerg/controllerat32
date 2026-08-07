## Сборка проекта под Windows

1. Установите [Visual Studio 2022](https://visualstudio.microsoft.com/ru/downloads/).
2. Скачайте файл [VisualGDB-5.6r8_retail.msi](https://fost.ws/soft/visualgdb-5-6-r8-key-2023/?ysclid=m8cre3j7go885579343) и установите
3. Скачайте [Git](https://git-scm.com/downloads/win/)
4. Создайте рабочую папку и откройте там git command line
5. Введите команду для клонирования репозитория 

```git clone https://gitlab.com/on4ip84/controllerat32.git```

6. Перейдите в созданную папку с помощью команды

```cd controllerat32/```

7. Подгрузите подмодули из других репозиториев
### ВНИМАНИЕ!!! Эти модули нельзя загрузить с рабочего интернета, только с внешнего

```git submodule update --init --recursive```

8. Откройте файл AT32_Base.sln и соберите проект



## Возможные ошибки
Если видите ошибку attempt to rename spec ```'link' to already defined spec 'nano_link'```, то зайдите в файл stm32.props и удалите строку, содержащую: ```<AdditionalOptions>--specs=nano.specs --specs=nosys.specs -Wl,--no-warn-rwx-segments %(Link.AdditionalOptions)</AdditionalOptions>```

Если не загружаются submodules, то попробуйте поменять сеть WI-Fi

## Настройка на разные моторы 
Рабочая ветка прошивки ```Stend verison``` на Git
На стенде стоит спарка 2х моторов , если смотреть на муфту моментометра то по левую руку мотор
LEFT по правую RIGHT. В файле getBoardSettings.с есть DEFINE который определяет мотор и настройку датчика положения
При сборке прошивки под Левый мотор DEFINE должен быть MOTOR_LEFT, под правый MOTOR_RIGHT.



# Controller AT32 – Ветка генератора DBC

**Ветка:** `StendVersion_dbc_generator`
**Базовая ветка:** `StendVersion`
**Суть изменений:** переход от устаревших C‑диспетчеров и разрозненных C‑модулей сообщений к единому уровню C++‑сообщений с `DbcDispatcher` и сгенерированными типами.

---

## Кратко

* **Переход на сгенерированный слой сообщений C++** в каталоге `user/CAN_DBC_MODULE/DBC_MSG/build/`.
* **Единый диспетчер:** `DbcDispatcher.{cpp,hpp,h}` вместо старых `CANSendDispatcher.c` и `CANReceiveDispatcher.c`.
* **Удалены** отдельные C‑файлы для каждого сообщения (`*_MSG.c/.h`); **добавлены** типобезопасные C++ классы (`McuStatus`, `VcuMCU01`, `VcuMCU02` и др.).
* **Обновлены проектные файлы** (`AT32_Base.vcxproj`, `.filters`).
* **user/main.c** переработан под новый API.

---

## Подробности изменений

### 1) Структура проекта и сборка

* **Изменены**: `AT32_Base.vcxproj`, `AT32_Base.vcxproj.filters` – добавлены новые C++ исходники, удалены старые C‑модули.
* **Мелкие изменения**: в `app/CanOpenDrivers/*`, `stm32.xml`, `user/can2Init.c`, `user/tempSpiInit.c`.

### 2) CAN/DBC модуль

**Удалено (старое, C):**

* `user/CAN_DBC_MODULE/CANReceiveDispatcher.c`
* `user/CAN_DBC_MODULE/CANSendDispatcher.c`
* `user/CAN_DBC_MODULE/DBC_MSG/*_MSG.c/.h` (например, `MCU_Status_MSG.c`, `VCU_MCU01_MSG.h` и т.д.)
* `user/CAN_DBC_MODULE/SignalUpdateHandlers.h`

**Добавлено (новое, C++):**

* `user/CAN_DBC_MODULE/DBC_MSG/build/` – сгенерированные, типобезопасные классы сообщений:

  * `McuCurrentVoltage`, `McuDeratingStatus`, `McuFailureCode`, `McuFluxParams`,
    `McuStatus`, `McuTemperature1`, `McuTemperature2`, `McuVCU1`,
    `VcuMCU01`, `VcuMCU02`, `VcutoMCUCurrentCommand` и др.
  * Каждый имеет **`.cpp`, `.h`, `.hpp`**.
* Центральный диспетчер и интерфейсы:

  * `DbcDispatcher.{cpp,hpp,h}`
  * `IMsg.hpp`
  * `generatedParams.hpp`
  * `defaultStructures.h`
* Обновлены: `CANDBCUtils.c/.h`, `SignalUpdateHandlers.c`.

**Итог:** один диспетчер (`DbcDispatcher`) занимается маршрутизацией RX и сборкой TX на основе объектов сообщений.

### 3) Прикладной код

* **`user/main.c`** – обновлён для работы с новым API (~250 строк изменено).

---

## Памятка по миграции (с ветки `StendVersion`)

1. **Подключаемые файлы**

   * Удалить:

     ```c
     #include "CAN_DBC_MODULE/CANSendDispatcher.h"
     #include "CAN_DBC_MODULE/CANReceiveDispatcher.h"
     #include "CAN_DBC_MODULE/DBC_MSG/MCU_Status_MSG.h"
     #include "CAN_DBC_MODULE/SignalUpdateHandlers.h"
     ```
   * Добавить:

     ```c
     #include "CAN_DBC_MODULE/DbcDispatcher.h"
     #include "CAN_DBC_MODULE/DBC_MSG/build/McuStatus.h"
     #include "CAN_DBC_MODULE/DBC_MSG/build/VcuMCU02.h"
     ```

2. **C и C++ вместе**

   * Убедиться, что в `DbcDispatcher.h` есть `extern "C"` для вызовов из C.
   * Все `.cpp` собираются как C++.

3. **Инициализация**

   ```c
   DbcDispatcher_Handle* dbc = DbcDispatcher_Create(can_if_ptr);
   DbcDispatcher_Register_McuStatus(dbc, my_mcu_status_handler);
   DbcDispatcher_Start(dbc);
   ```

4. **Передача (TX)**

   ```c
   VcuMCU02 msg = VcuMCU02_Default();
   msg.torque_cmd = desiredTorque;
   msg.enable = 1;
   DbcDispatcher_Send_VcuMCU02(dbc, &msg);
   ```

5. **Приём (RX)**

   ```c
   void my_mcu_status_handler(const McuStatus* st, void* ctx) {
       // использовать st->inverter_state, st->fault и т.д.
   }
   ```

---

## Сопоставление старое → новое

| Старый `*_MSG` (C)       | Новый класс (C++)   | Комментарий                       |
| ------------------------ | ------------------- | --------------------------------- |
| `MCU_Status_MSG`         | `McuStatus`         | RX, callback `Register_McuStatus` |
| `MCU_Temperature1_MSG`   | `McuTemperature1`   | RX температуры 1                  |
| `MCU_Temperature2_MSG`   | `McuTemperature2`   | RX температуры 2                  |
| `MCU_FailureCode_MSG`    | `McuFailureCode`    | RX коды ошибок                    |
| `MCU_DeratingStatus_MSG` | `McuDeratingStatus` | RX дерейтинг                      |
| `MCU_VCU_1_MSG`          | `McuVCU1`           | RX статус VCU→MCU                 |
| `VCU_MCU01_MSG`          | `VcuMCU01`          | TX/RX                             |
| `VCU_MCU02_MSG`          | `VcuMCU02`          | TX: момент/enable                 |
| (разные токи/напряжения) | `McuCurrentVoltage` | RX                                |

---

## Замечания

* Следить за `extern "C"` в заголовках.
* Новые классы сами масштабируют сигналы по DBC → убрать дублирующий код.
* Проверить тайминги TX (`0x046=20мс`, `0x047=100мс`, `0x300=10мс`).
* Исключить из репозитория все `*.user` файлы.

---

## Changelog

* 🔁 Замена: старые C‑модули сообщений → сгенерированные C++ классы.
* 🧭 Добавлен: `DbcDispatcher` для единого RX/TX.
* 🧹 Удалены: `SignalUpdateHandlers.h`, старые диспетчеры.
* 🧩 Переписан `user/main.c` под новый API.

---

## Дальнейшие шаги

* Собрать и протестировать на стенде, проверить работу сигналов.
* Полностью заменить вызовы старого API.
* Создать MR из `StendVersion_dbc_generator` → `StendVersion` с этим README.

