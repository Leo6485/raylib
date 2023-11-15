#!/bin/bash

clear
printf "[ Thread ] \033[1;34mInstalando dependências...\033[0m\n"
sudo apt install libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev > /dev/null &&
printf "[ Thread ] \033[1;32mDependências instaladas\033[0m\n\n" &
printf "\033[1;34mClonando repositório...\033[0m\n"
git clone https://github.com/raysan5/raylib.git raylib && clear && printf "[ * ] \033[1;32Repositório clonado\033[0m\n"
cd raylib/src/
make clean > /dev/null
make PLATFORM=PLATFORM_DESKTOP > /dev/null
printf "Instalando...\033[0m\n"
sudo make install && clear && rm -rf raylib
printf "\033[1;32mConcluído\033[0m\n"
