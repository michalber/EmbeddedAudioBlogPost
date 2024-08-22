#include "stm32_platform.hpp"

#include "FreeRTOS.h"
#include <task.h>

#include "osal.h"

#include <inttypes.h>
#include <string_view>
#include <stdio.h>

#include "dsp_self_test.hpp"
#include <stm32f7xx_hal.h>

#define LED_PORT GPIOJ
#define LED_PIN GPIO_PIN_13
#define LED_PORT_CLK_ENABLE __HAL_RCC_GPIOJ_CLK_ENABLE

volatile unsigned int *DWT_CYCCNT   = (volatile unsigned int *)0xE0001004;
volatile unsigned int *DWT_CONTROL  = (volatile unsigned int *)0xE0001000;
volatile unsigned int *DWT_LAR      = (volatile unsigned int *)0xE0001FB0;
volatile unsigned int *SCB_DHCSR    = (volatile unsigned int *)0xE000EDF0;
volatile unsigned int *SCB_DEMCR    = (volatile unsigned int *)0xE000EDFC;
volatile unsigned int *ITM_TER      = (volatile unsigned int *)0xE0000E00;
volatile unsigned int *ITM_TCR      = (volatile unsigned int *)0xE0000E80;

USART_HandleTypeDef UsartHandle;
static void send_char(char c);

static void EnableTiming(void)
{ 
  *SCB_DEMCR |= 0x01000000;
  *DWT_LAR = 0xC5ACCE55; // enable access
  *DWT_CYCCNT = 0; // reset the counter
  *DWT_CONTROL |= 1 ; // enable the counter
}

static unsigned int GetCpuCyclesCnt()
{
    return *DWT_CYCCNT;
}

void initGPIO() {
    GPIO_InitTypeDef GPIO_Config;

    GPIO_Config.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_Config.Pull  = GPIO_NOPULL;
    GPIO_Config.Speed = GPIO_SPEED_FREQ_HIGH;

    GPIO_Config.Pin = LED_PIN;

    LED_PORT_CLK_ENABLE();
    HAL_GPIO_Init(LED_PORT, &GPIO_Config);
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);


    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef    gpio;

    gpio.Pin   = GPIO_PIN_10;
    gpio.Mode  = GPIO_MODE_AF_PP;
    gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

    HAL_GPIO_Init(GPIOA, &gpio);

    gpio.Pin  = GPIO_PIN_9;
    gpio.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(GPIOA, &gpio);


    UsartHandle.Instance = USART1;

    UsartHandle.Init.BaudRate   = 115200;
    UsartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    UsartHandle.Init.StopBits   = UART_STOPBITS_1;
    UsartHandle.Init.Parity     = UART_PARITY_NONE;
    UsartHandle.Init.Mode       = UART_MODE_TX_RX;
    HAL_USART_Init(&UsartHandle);
}


namespace {
static void timeit(const std::string_view &name, void fn(void)) {
    vTaskDelay(1);
    tv[0] = tv[1] = tv[2] = tv[3] = tv[4] = tv[5] = tv[6] = tv[7] = 1;
    // get time since boot in microseconds
    unsigned ccount = GetCpuCyclesCnt();
    unsigned icount = 0, ccount_new;
    fn();
    ccount_new = GetCpuCyclesCnt();
    // RSR(ICOUNT, icount);
    uint64_t time = ((ccount_new - ccount) / SystemCoreClock) / 1000;
    // float cpi = (float)(ccount_new - ccount) / icount;
    printf("%s \t %f MOP/S   \tCPI=None\n", name.data(), (float)N / time);
}

static void RunDspTests() {
    timeit("Integer Addition", addint);
    timeit("Integer Multiply", mulint);
    timeit("Integer Division", divint);
    timeit("Integer Multiply-Add", muladdint);

    timeit("Float Addition ", addfloat);
    timeit("Float Multiply ", mulfloat);
    timeit("Float Division ", divfloat);
    timeit("Float Multiply-Add", muladdfloat);

    timeit("Double Addition", adddouble);
    timeit("Double Multiply", muldouble);
    timeit("Double Division", divdouble);
    timeit("Double Multiply-Add", muladddouble);
}
}// namespace

static void TaskFn(void *ctx) {
    send_char('a');
    send_char('\n');
    RunDspTests();
    while (true) {}
}


CPlatform_STM32::CPlatform_STM32() {}

CPlatform_STM32::~CPlatform_STM32() {}

void CPlatform::Assert(const char *rpMsg, const char *rpFileName, int rdLine) {}

void CPlatform::LaunchPlatform() {
    SystemInit();
    HAL_Init();
    __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

    CPlatform *platform = new CPlatform_STM32;
    (void)platform;
}


void CPlatform_STM32::InitDrivers() { initGPIO(); }

void CPlatform_STM32::InitPlatform() {
    InitDrivers();
    EnableTiming();
    xTaskCreate(TaskFn, "init", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    vTaskStartScheduler();
}

void send_char(char c) {
    HAL_USART_Transmit(&UsartHandle, (uint8_t*)&c, 1, HAL_MAX_DELAY);
}

int __io_putchar(int c) {
    send_char(c);
    return c;
}

extern "C" void vApplicationTickHook(void) {}

extern "C" void vApplicationIdleHook(void) {}

extern "C" void vApplicationMallocFailedHook(void) {
    taskDISABLE_INTERRUPTS();
    for (;;);
}

extern "C" void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName) {
    (void)pcTaskName;
    (void)pxTask;

    taskDISABLE_INTERRUPTS();
    for (;;);
}
