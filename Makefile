GPP = /mnt/c/msys64/mingw64/bin/g++.exe
EXE = App.exe
FLAGS = -g -w -I./include
LINK = -Llib -lglfw3 -lopengl32 -lgdi32
OBJS = main.o
OBJS += $(wildcard ./build/*.o)
SRC = $(wildcard ./src/*.cpp)
SRC += $(wildcard ./src/*.h)

all: $(EXE)
	@echo Build complete

$(EXE): $(OBJS)
	$(GPP) -o $@ $^ $(FLAGS) $(LINK)

main.o: $(SRC) 
	$(GPP) -c -o $@ $< $(FLAGS) 

clean: 
	rm -f main.o $(EXE)

 
