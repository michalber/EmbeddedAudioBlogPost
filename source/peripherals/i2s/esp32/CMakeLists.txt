target_include_directories(periph_i2s PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_sources(periph_i2s 
    PUBLIC i2s.hpp
    PRIVATE i2s.cpp
)

target_link_libraries(periph_i2s PUBLIC idf::esp_driver_i2s)