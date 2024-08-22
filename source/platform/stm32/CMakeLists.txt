target_include_directories(platform INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(platform PUBLIC ${CMAKE_CURRENT_LIST_DIR}/cfg)

target_sources(platform PRIVATE stm32_platform.cpp PUBLIC stm32_platform.hpp cfg/stm32f7xx_hal_conf.h)

target_link_libraries(platform PRIVATE
    HAL::STM32::F7::RCC
    HAL::STM32::F7::GPIO
    HAL::STM32::F7::USART
    HAL::STM32::F7::CORTEX
    CMSIS::STM32::F769NI
    FreeRTOS::STM32::F7::Timers
    FreeRTOS::STM32::F7::Heap::1
    FreeRTOS::STM32::F7::ARM_CM7
    STM32::Nano
)

target_compile_definitions(platform PRIVATE
    USE_CMSIS_RTOS
    CMSIS_RTOS_V2_DEVICE_HEADER="stm32f7xx_hal.h"
)