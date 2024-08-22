# Embedded audio development blog post code repository

This repository contains code referenced in embedded audio development blog post under sii.pl/blog/.

## Description

Example of embedded audio development techniques using modern C++, including `constexpr` and templates.

## Getting Started

* Clone with all submodules
* Check main file located in `platform/esp32/esp32_platform.cpp:273`, then go where your curiosity will lead

### Dependencies

* OS capable of running Docker
* Docker

### Installing

* Create build enviorment in Docker using scripts in `build_env` folder
* Go into the Docker container and build code

### Building and executing program

* Execute `build-esp32s3.sh` script to build example for ESP32-S3 chip
* Execute `run-esp32s3.sh` script to run example on ESP32-S3 chip

### Additional info

* Build enviorment is capable to build code for STM32 as well as for Host (PC), but concrete implementation is not yet provided.
