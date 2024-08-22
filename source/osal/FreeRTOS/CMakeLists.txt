target_include_directories(osal INTERFACE ${CMAKE_CURRENT_LIST_DIR})

target_sources(osal PRIVATE osal.c)

if(${APP_PLATFORM} STREQUAL "esp32")
target_link_libraries(osal PRIVATE idf::freertos)
elseif(${APP_PLATFORM} STREQUAL "stm32")
target_link_libraries(osal PUBLIC
    CMSIS::STM32::F7::RTOS
    FreeRTOS::STM32::F7::Heap::4
    FreeRTOS::STM32::F7::ARM_CM7
)
endif()
