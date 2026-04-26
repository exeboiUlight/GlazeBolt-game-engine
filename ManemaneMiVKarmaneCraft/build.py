import os

os.system("gcc src/main.c include/glad/glad.c -I./include -L./lib -lglfw3 -lOpenAL32 -lgdi32 -lfreetype -o bin/minecraft.exe")