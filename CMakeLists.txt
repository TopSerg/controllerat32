cmake_minimum_required(VERSION 3.21)

project(AT32_Base LANGUAGES C CXX ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Python3 COMPONENTS Interpreter REQUIRED)

set(AT32_SOURCES
    app/CANopenNode/301/CO_Emergency.c
    app/CANopenNode/301/CO_fifo.c
    app/CANopenNode/301/CO_HBconsumer.c
    app/CANopenNode/301/CO_NMT_Heartbeat.c
    app/CANopenNode/301/CO_Node_Guarding.c
    app/CANopenNode/301/CO_ODinterface.c
    app/CANopenNode/301/CO_PDO.c
    app/CANopenNode/301/CO_SDOclient.c
    app/CANopenNode/301/CO_SDOserver.c
    app/CANopenNode/301/CO_SYNC.c
    app/CANopenNode/301/CO_TIME.c
    app/CANopenNode/301/crc16-ccitt.c
    app/CANopenNode/303/CO_LEDs.c
    app/CANopenNode/304/CO_GFC.c
    app/CANopenNode/304/CO_SRDO.c
    app/CANopenNode/305/CO_LSSmaster.c
    app/CANopenNode/305/CO_LSSslave.c
    app/CANopenNode/309/CO_gateway_ascii.c
    app/CANopenNode/CANopen.c
    app/CANopenNode/extra/CO_trace.c
    app/CANopenNode/storage/CO_storage.c
    app/CommInterface/CommInterface.c
    app/CommInterface/CompileTime.c
    app/CommInterface/Message.c
    app/ControlAlg/ControlSystem_v2.c
    app/ControlAlg/ControlSystem_v2_data.c
    app/ControlAlg/Identification.c
    app/ControlAlg/indTable.c
    app/ControlAlg/Logic.c
    app/ControlAlg/PIDs.c
    app/ControlAlg/Position_Observer.c
    app/ControlAlg/Protections.c
    app/ControlAlg/PWMcalc.c
    app/ControlAlg/TrqCalc.c
    app/ControlAlg/utils.c
    app/FreeRTOS-Kernel/event_groups.c
    app/FreeRTOS-Kernel/list.c
    app/FreeRTOS-Kernel/portable/GCC/ARM_CM4F/port.c
    app/FreeRTOS-Kernel/portable/MemMang/heap_4.c
    app/FreeRTOS-Kernel/queue.c
    app/FreeRTOS-Kernel/stream_buffer.c
    app/FreeRTOS-Kernel/tasks.c
    app/FreeRTOS-Kernel/timers.c
    app/CanOpenDrivers/canOpen.c
    app/CanOpenDrivers/CO_driverDrv.c
    app/CanOpenDrivers/CO_storageDrv.c
    app/CanOpenDrivers/OD.c
    app/FreeRtosTasks/WorkTasks.c
    bsp/at32f403a_407_board.c
    bsp/at32f403a_407_clock.c
    bsp/at32f403a_407_int.c
    cmsis/startup_at32f403a_407.s
    cmsis/system_at32f403a_407.c
    firmware/at32f403a_407_acc.c
    firmware/at32f403a_407_adc.c
    firmware/at32f403a_407_bpr.c
    firmware/at32f403a_407_can.c
    firmware/at32f403a_407_crc.c
    firmware/at32f403a_407_crm.c
    firmware/at32f403a_407_dac.c
    firmware/at32f403a_407_debug.c
    firmware/at32f403a_407_dma.c
    firmware/at32f403a_407_emac.c
    firmware/at32f403a_407_exint.c
    firmware/at32f403a_407_flash.c
    firmware/at32f403a_407_gpio.c
    firmware/at32f403a_407_i2c.c
    firmware/at32f403a_407_misc.c
    firmware/at32f403a_407_pwc.c
    firmware/at32f403a_407_rtc.c
    firmware/at32f403a_407_sdio.c
    firmware/at32f403a_407_spi.c
    firmware/at32f403a_407_tmr.c
    firmware/at32f403a_407_usart.c
    firmware/at32f403a_407_usb.c
    firmware/at32f403a_407_wdt.c
    firmware/at32f403a_407_wwdt.c
    firmware/at32f403a_407_xmc.c
    user/adcInit.c
    user/can2Init.c
    user/canInit.c
    user/CAN_DBC_MODULE/canDBCtask.c
    user/CAN_DBC_MODULE/CANDBCUtils.c
    user/CAN_DBC_MODULE/CANErrorsHandlers.c
    user/CAN_DBC_MODULE/CANSignalsMap.c
    user/CAN_DBC_MODULE/DbcDispatcher.cpp
    user/CAN_DBC_MODULE/DBC_MSG/build/McuCurrentVoltage.cpp
    user/CAN_DBC_MODULE/DBC_MSG/build/McuDeratingStatus.cpp
    user/CAN_DBC_MODULE/DBC_MSG/build/McuFailureCode.cpp
    user/CAN_DBC_MODULE/DBC_MSG/build/McuFluxParams.cpp
    user/CAN_DBC_MODULE/DBC_MSG/build/McuResolverCalibration.cpp
    user/CAN_DBC_MODULE/DBC_MSG/build/McuStatus.cpp
    user/CAN_DBC_MODULE/DBC_MSG/build/McuTemperature1.cpp
    user/CAN_DBC_MODULE/DBC_MSG/build/McuTemperature2.cpp
    user/CAN_DBC_MODULE/DBC_MSG/build/McuVCU1.cpp
    user/CAN_DBC_MODULE/DBC_MSG/build/VcuMCU01.cpp
    user/CAN_DBC_MODULE/DBC_MSG/build/VcuMCU02.cpp
    user/CAN_DBC_MODULE/DBC_MSG/build/VcutoMCUCurrentCommand.cpp
    user/flash.c
    user/getBoardSettings.c
    user/gpioInit.c
    user/main.c
    user/nvicInit.c
    user/pwmInit.c
    user/resolverInit.c
    user/tempSpiInit.c
    user/timerInit.c
    user/uartInit.c
)

add_executable(${PROJECT_NAME} ${AT32_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED YES
    C_EXTENSIONS YES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS YES
    SUFFIX ".elf"
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    bsp
    include/libraries/cmsis/cm4/core_support
    include/libraries/cmsis/cm4/device_support
    include/libraries/drivers/inc
    app/CommInterface
    user
    app/FreeRTOS-Kernel/include
    app/FreeRTOS-Kernel/portable/MemMang
    app/FreeRTOS-Kernel/portable/GCC/ARM_CM4F
    app/FreeRtosTasks
    app/CanOpenDrivers
    app/ControlAlg
    app/CANopenNode
    app/CANopenNode/example
    user/CAN_DBC_MODULE
    user/CAN_DBC_MODULE/DBC_MSG
    user/CAN_DBC_MODULE/DBC_MSG/build
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    DEBUG=1
    PORTABLE_WORDSIZES
    USART_NUM
    _CO_USE_GLOBALS
    __TARGET_FPU_VFP
    flash_layout
    AT32F403AVGT7
    USE_STDPERIPH_DRIVER
    AT_START_F403A_V1
    loaderSize=16000
    ARM_MATH_CM4
    STM32F405ZG
)

set(AT32_CPU_FLAGS
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
)

target_compile_options(${PROJECT_NAME} PRIVATE
    ${AT32_CPU_FLAGS}
    $<$<COMPILE_LANGUAGE:C,CXX>:-O3>
    $<$<COMPILE_LANGUAGE:C,CXX>:-g3>
    $<$<COMPILE_LANGUAGE:C,CXX>:-ffunction-sections>
    $<$<COMPILE_LANGUAGE:C,CXX>:-fdata-sections>
)

set(AT32_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/ldscripts/AT32F403AxG_FLASH.ld)
set(AT32_MAP_FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.map)

target_link_options(${PROJECT_NAME} PRIVATE
    ${AT32_CPU_FLAGS}
    -T${AT32_LINKER_SCRIPT}
    --specs=nano.specs
    --specs=nosys.specs
    -Wl,--gc-sections
    -Wl,--print-memory-usage
    -Wl,--emit-relocs
    -Wl,--no-warn-rwx-segments
    -Wl,-Map=${AT32_MAP_FILE}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/bsp/libarm_cortexM4lf_math.a
    m
)

set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY LINK_DEPENDS ${AT32_LINKER_SCRIPT})

set(AT32_HEX_FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(AT32_BIN_FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> ${AT32_BIN_FILE}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/crc_binary_add.py ${AT32_BIN_FILE}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/firmware_info.py ${AT32_BIN_FILE}
    COMMAND ${CMAKE_OBJCOPY} -I binary -O ihex --change-addresses=0x08004000 ${AT32_BIN_FILE} ${AT32_HEX_FILE}
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}>
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Creating HEX/BIN images and adding firmware metadata"
    VERBATIM
)
