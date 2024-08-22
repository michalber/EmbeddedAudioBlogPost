target_include_directories(periph_i2s PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_sources(periph_i2s 
    PUBLIC i2s.hpp
    PRIVATE i2s.cpp
)

target_link_libraries(periph_i2s PUBLIC platform)
target_link_libraries(periph_i2s PUBLIC
    HAL::STM32::F7::RCC
    HAL::STM32::F7::GPIO
    HAL::STM32::F7::I2S
    HAL::STM32::F7::CORTEX
    CMSIS::STM32::F769NI
    STM32::Nano
)