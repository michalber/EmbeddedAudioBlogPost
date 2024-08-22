target_include_directories(osal PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_sources(osal PRIVATE osal_common.c PUBLIC osal.h)

if(${APP_PLATFORM} STREQUAL "stm32")
target_include_directories(osal PUBLIC ${CMAKE_CURRENT_LIST_DIR}/config)
    target_sources(osal PUBLIC config/FreeRTOSConfig.h)
endif()
