#include "osal.h"
#include "platform.hpp"

void IdleProcess() {
    while (true) { OSAL_TaskSleep(10000); }
}

#if defined(ESP32)
extern "C" void app_main(void) {
#elif defined(STM32) || defined(HOST)
int main() {
#endif// ESP32
    CPlatform::LaunchPlatform();
    CPlatformI->InitPlatform();
    IdleProcess();

#if defined(STM32) || defined(HOST)
    return 0;
#endif
}
