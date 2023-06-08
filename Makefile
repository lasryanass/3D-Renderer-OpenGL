CC = g++
CFLAGS = -std=c++11 -Wall -Wextra -pedantic
LDFLAGS = -lopengl32 -lglu32 -lfreeglut

OBJ = obj_viewer.o

all: obj_viewer

obj_viewer: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f obj_viewer $(OBJ)
