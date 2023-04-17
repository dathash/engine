
SRC = $(wildcard ../src/*.cpp src/*.c) $(wildcard ../ext/imgui/*.cpp ../ext/imgui/backends/*.cpp ../ext/imguizmo/*.cpp)
CSRC = ../ext/glad/src/glad.c 
OBJ = ${SRC:.cpp=.o} ${CSRC:.c=.o}

LDFLAGS = -L. -L../ext/glfw/lib/ -L../ext/freetype/lib/ -L../ext/assimp/lib -lglfw -lfreetype -lassimp
INCFLAGS = -I. -I../ext -I../ext/glad/include -I../ext/glfw/include -I../ext/glm/include -I../ext/freetype/include/freetype2 -I../ext/assimp/include -I../ext/imgui/backends/ -I../ext/imgui/ -I../ext/imguizmo/
FLAGS = -std=c++20 -Wno-deprecated -glldb -O0 -Wall
OUT = wasteland

CC = clang++

# For debugging
print-%  : ; @echo $* = $($*)

$(OUT): $(OBJ)
	@$(CC) $(FLAGS) $(INCFLAGS) $(LDFLAGS) $^ -o $@
	@printf "\e[33mLinking\e[90m %s\e[0m\n" $@
	@printf "\e[34mDone!\e[0m\n"

%.o: %.cpp
	@$(CC) $(FLAGS) $(INCFLAGS) -c $< -o $@
	@printf "\e[36mCompile\e[90m %s\e[0m\n" $@

%.o: %.c
	@$(CC) $(FLAGS) $(INCFLAGS) -c $< -o $@
	@printf "\e[36mCompile\e[90m %s\e[0m\n" $@

../src/main.o: ../src/*.h

clean:
	@rm -f $(OUT) $(OBJ)
	@printf "\e[34mAll clear!\e[0m\n"
