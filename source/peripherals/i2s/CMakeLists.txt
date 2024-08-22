add_library(periph_i2s)

target_compile_features(periph_i2s INTERFACE cxx_std_17)

add_subdirectory(${APP_PLATFORM})

target_link_libraries(periph_i2s PRIVATE core_utils)
