target_include_directories(periph_timer PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_sources(periph_timer
    PUBLIC timer.cpp 
    PUBLIC timer.hpp
)

target_link_libraries(periph_timer PUBLIC idf::esp_timer)
