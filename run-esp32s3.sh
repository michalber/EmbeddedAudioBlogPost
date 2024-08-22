#!/usr/bin/env bash
cd build
python3 $IDF_PATH/components/esptool_py/esptool/esptool.py -p $1 write_flash @flash_project_args
python3 -m esp_idf_monitor -p $1 audio_app.elf
cd ..