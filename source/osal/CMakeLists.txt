add_library(osal)

target_compile_features(osal INTERFACE c_std_99)

add_subdirectory(common)
add_subdirectory(${OS_TYPE})
target_link_libraries(osal PUBLIC core_utils)