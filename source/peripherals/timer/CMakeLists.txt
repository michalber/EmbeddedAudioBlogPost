add_library(periph_timer)

target_compile_features(periph_timer INTERFACE cxx_std_17)

target_include_directories(periph_timer PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_sources(periph_timer 
    INTERFACE timer_int.hpp
)

add_subdirectory(${APP_PLATFORM})

target_link_libraries(periph_timer PRIVATE core_utils)
