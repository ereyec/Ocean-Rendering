@echo off
g++ -I./include -L./lib glad.c main.cpp -l:libglfw3.a -lopengl32 -lgdi32 -luser32 -o program.exe
pause
