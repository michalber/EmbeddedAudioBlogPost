add_library(platform)

add_subdirectory(common)
add_subdirectory(${APP_PLATFORM})
target_link_libraries(platform PUBLIC osal utils_dsp periph_i2s utils_audio_pipeline periph_timer)