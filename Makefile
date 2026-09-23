CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude

# Pega main.cpp e todos os .cpp de src/, sem precisar editar aqui quando entrar classe nova
SRCS = main.cpp $(wildcard src/*.cpp)
OBJS = $(SRCS:.cpp=.o)

# No Windows o executavel precisa do .exe
ifeq ($(OS),Windows_NT)
    EXE = estacionamento.exe
else
    EXE = estacionamento
endif

# O make escolhe sozinho entre o cmd do Windows e o sh, e o comando de apagar muda.
# Da pra saber qual deles e porque so o cmd imprime as aspas junto no echo.
ifeq ($(shell echo "x"),"x")
    RM = del /Q
    FIXPATH = $(subst /,\,$1)
else
    RM = rm -f
    FIXPATH = $1
endif

all: $(EXE)

$(EXE): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

# Cada .cpp vira um .o, entao so recompila o que mudou
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(call FIXPATH,$(OBJS) $(EXE))

.PHONY: all clean