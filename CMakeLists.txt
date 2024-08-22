cmake_minimum_required(VERSION 3.16)

project(audio_app C CXX ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

if(${APP_PLATFORM} STREQUAL "esp32")
    set(targets "esp32" "esp32s2" "esp32s3" "esp32c3" "esp32c2" "esp32c6" "esp32h2" "esp32p4")
    if("${TARGET}" IN_LIST targets)
        include($ENV{IDF_PATH}/tools/cmake/idf.cmake)
            idf_build_process("${TARGET}"
                            # try and trim the build; additional components
                            # will be included as needed based on dependency tree
                            #
                            # although esptool_py does not generate static library,
                            # processing the component is needed for flashing related
                            # targets and file generation
                            COMPONENTS freertos esptool_py log esp_timer esp_driver_i2s esp_hw_support
                            SDKCONFIG ${CMAKE_CURRENT_LIST_DIR}/sdkconfig
                            BUILD_DIR ${CMAKE_BINARY_DIR})

        add_executable(${CMAKE_PROJECT_NAME}.elf)

        add_subdirectory(source)

        target_link_libraries(${CMAKE_PROJECT_NAME}.elf idf::freertos idf::spi_flash idf::esp_driver_i2s idf::esp_hw_support)
        target_link_libraries(${CMAKE_PROJECT_NAME}.elf osal platform)

        idf_build_executable(${CMAKE_PROJECT_NAME}.elf)
    else()
        message(ERROR "Unknown target ${TARGET}, aborting compilation")
    endif()
elseif(${APP_PLATFORM} STREQUAL "stm32")

    find_package(CMSIS COMPONENTS STM32F769NI REQUIRED)
    find_package(HAL COMPONENTS STM32F769NI REQUIRED)
    find_package(FreeRTOS COMPONENTS ARM_CM7 STM32F7 REQUIRED)

    add_subdirectory(source)

elseif(${APP_PLATFORM} STREQUAL "host")
    add_subdirectory(source)
endif()