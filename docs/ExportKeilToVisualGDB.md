# Импортирование проекта Keil в VisualGDB (для микроконтроллера Artery AT32F403a)

## Импорт проекта

1. Запустить VisualStudio, выбрать продолжить без кода.
2. Выбрать `Файл`->`Создать`->`VisualGDBProject`
3. В диалоговом окне `Add New Project item`:  
3.1. Выбрать `Embedded Project Wizard`.  
3.2 В поле `Name` указать имя проекта.  
(Рекомендуется использовать в названии VisualGDB)  
3.3 В поле `Location` указать путь к проекту  
(*Рекомендуется указать каталог `project`, проект для keil при этом распологается в подкаталоге `project\MDK`*)  
3.4 Нажать кнопку `Next`.
4. В диалоговом окне `New Embedded Project`:  
4.1 Выбрать пункт `Automatically import a project in a different format`  
4.2 В переключаетеле `Build with` выбрать `MsBuild`. (*значение по умолчанию*)  
4.3 В раскрывшемся списке выбрать пункт `Import an existing Keil Project`.  
4.4 В переключателе `Build using` выбрать `GCC Compiler` (*значение по умолчанию*).  
4.5 В поле `Project file` указать имя файла проекта keil (*\*.uvprojx*).  
4.6 Нажать кнопку `Next`.
5. В диалоговом окне `New Embedded Project`:  
5.1 Произвести установку `GCC 10.3.1 GDB(GNU)` (*Если не установлен ранее*).  
5.2 Выбрать `GCC 10.3.1 GDB(GNU)`.  
5.3 Установить пак микроконтроллеров `STM32` (*Если не установлен ранее*).  
5.4 Выбрать микроконтроллер `STM32F405ZG` из списка доступных устройств.     
5.4.1 Установить переключатель `Execute from` в положение `Flash` (*значение по умолчанию*).  
5.4.2 Установить переключатель `Floating point support` в положение `Hardware` (*значение по умолчанию*)    
5.4.3 Установить флаг `Exclude the startup file from project` (*по умолчанию выключено, startup file будет добавлен позже*).  
5.5 Нажать кнопку `Next`
6. В диалоговом окне `Debug method`:  
6.1 В поле `Debug using` указать `Segger J-Link`  
6.2 В поле "Debugged device" указать модель микроконтроллера (*`AT32F403AVGT7`*)  
6.3 Отключить флаг `Enable live FLASH memory patching`.  
6.4 Нажать кнопку `Finish`.

## Настройка проекта после импорта
7. Выбрать `Проект`->`Добавить`->`Существующий элемент`.  
7.1 В диалоговом окне указать startup file `..\\..\libraries\cmsis\cm4\device_support\startup\gcc\startup_at32f403a_407.s`.
8. В настройках `VisualGDB` (*двойной клик по файлу `VisualGDB settings->VisualGDB-Debug.vgdbsettings`*) на вкладке `MSBuild settings` в поле `Linker Script` указать путь к файлу `..\\..\libraries\cmsis\cm4\device_support\startup\gcc\linker\AT32F403AxG_FLASH.ld`.
9. Открыть файл проекта `VisualGDB.vcxproj` в текстовом редакторе.  
9.1 Найти секцию дополнительных параметров линкера:
```xml
<ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Debug|VisualGDB'">
            ...
            <Link>
                ...
                <AdditionalOptions />
                ...
            </Link>
            ...
</ItemDefinitionGroup>
```
Заменить ```<AdditionalOptions />``` на
```xml
      <AdditionalOptions>--specs=nano.specs --specs=nosys.specs</AdditionalOptions>
```

10. В папке проекта `VisualGDB` создать каталог `SVD` и скопировать в него файл `AT32F403Axx_v2.svd` (*файл можно найти по месту установки `ArteryPack` для keil*)   
10.1 В файле `stm32.xml` заменить строчку 
```xml
<MCUDefinitionFile>STM32F4xxxx/DeviceDefinitions/stm32f405xx.xml</MCUDefinitionFile>
```
на
```xml
<MCUDefinitionFile>$(ProjectDir)/SVD/AT32F403Axx_v2.svd</MCUDefinitionFile>
```

11. Enjoy




