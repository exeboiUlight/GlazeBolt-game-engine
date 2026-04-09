import os

# compile editor
os.system("gcc -Wl,-subsystem,windows -O2 src/editor.cpp include/glad/glad.c -I./include -I./core -L./lib -lgdi32 -lopengl32 -lglfw3 -lopencv_videoio_ffmpeg4110_64 -llua54 -lopenal32 -llibstdc++ -o bin/editor.exe")
