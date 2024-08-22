target_include_directories(platform INTERFACE ${CMAKE_CURRENT_LIST_DIR})

target_sources(platform PRIVATE esp32_platform.cpp logger.cpp esp32s3_firLoop_float.s PUBLIC esp32_platform.hpp)

target_link_libraries(platform PRIVATE idf::freertos idf::log idf::esp_timer idf::esp_hw_support utils_esp_dsp)