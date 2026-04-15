import os

debug = True

# Целевая платформа Linux (можно изменить на x86_64-linux, aarch64-linux и т.д.)
TARGET = "x86_64-linux-gnu.2.28"  # или x86_64-linux-musl для статической линковки

# Флаги компилятора
if not debug:
    DEBUG_FLAGS = "-O2 -DNDEBUG"
else:
    DEBUG_FLAGS = "-O0 -g"

# Zig cc вместо g++
COMPILER = f"zig cc -target {TARGET}"

# Замените Windows-специфичные библиотеки на Linux-версии
# Для Linux не нужны -lgdi32 и -lopengl32
# Вместо glfw3, lua54, openal32, freetype нужны их Linux-версии

# Библиотеки для Linux:
# -lglfw (а не glfw3)
# -llua5.4 (или -llua54 в зависимости от дистрибутива)
# -lopenal (а не openal32)
# -lfreetype (то же имя)
# -lGL (OpenGL для Linux)

# Если библиотеки установлены в нестандартных путях:
LIB_PATHS = ""
INCLUDE_PATHS = "-I./include -I./core"

# Команда сборки
cmd = (f'{COMPILER} {DEBUG_FLAGS} '
       f'src/editor.cpp include/glad/glad.c '
       f'{INCLUDE_PATHS} '
       f'{LIB_PATHS} '
       f'-lglfw -llua5.4 -lopenal -lfreetype -lGL '
       f'-o bin/editor_linux')

print(f"Running: {cmd}")
os.system(cmd)