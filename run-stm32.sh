#!/usr/bin/env bash
STM32_CUBE_PROGRAMMER="/Applications/STMicroelectronics/STM32Cube/STM32CubeProgrammer/STM32CubeProgrammer.app/Contents/MacOs/bin/STM32_Programmer_CLI"

cd build
${STM32_CUBE_PROGRAMMER} -c port=SWD -w source/app/audio_app-bin.elf
cd ..