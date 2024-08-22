if(${APP_PLATFORM} STREQUAL "esp32")
    target_sources(${CMAKE_PROJECT_NAME}.elf PRIVATE main.cpp)
    target_compile_definitions(${CMAKE_PROJECT_NAME}.elf PRIVATE
        ESP32
    )
elseif(${APP_PLATFORM} STREQUAL "stm32")
    add_executable(${CMAKE_PROJECT_NAME}-bin main.cpp)
        target_include_directories(${CMAKE_PROJECT_NAME}-bin PUBLIC ${CMAKE_CURRENT_LIST_DIR})

    target_link_libraries(${CMAKE_PROJECT_NAME}-bin PRIVATE osal platform)

    target_compile_definitions(${CMAKE_PROJECT_NAME}-bin PRIVATE
        STM32
    )

    target_link_libraries(${CMAKE_PROJECT_NAME}-bin PRIVATE
        HAL::STM32::F7::RCC
        HAL::STM32::F7::GPIO
        HAL::STM32::F7::CORTEX
        CMSIS::STM32::F769NI
        FreeRTOS::STM32::F7::Timers
        FreeRTOS::STM32::F7::Heap::4
        FreeRTOS::STM32::F7::ARM_CM7
        STM32::Nano
    )

    target_compile_definitions(${CMAKE_PROJECT_NAME}-bin PRIVATE
        USE_CMSIS_RTOS
        CMSIS_RTOS_V2_DEVICE_HEADER="stm32f7xx_hal.h"
    )
    
    target_link_options(${CMAKE_PROJECT_NAME}-bin PRIVATE "--specs=nosys.specs")

    stm32_print_size_of_target(${CMAKE_PROJECT_NAME}-bin)

elseif(${APP_PLATFORM} STREQUAL "host")
    add_executable(${CMAKE_PROJECT_NAME}-bin)
    target_sources(${CMAKE_PROJECT_NAME}-bin PRIVATE main.cpp)
    target_compile_definitions(${CMAKE_PROJECT_NAME}-bin PRIVATE
        HOST
    )
    target_link_libraries(${CMAKE_PROJECT_NAME}-bin PRIVATE osal platform)
endif()
