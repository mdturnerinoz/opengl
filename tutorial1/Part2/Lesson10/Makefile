CC = g++
CFLAGS = -Wall
PROG = collisions

SRCS = main.cpp imageloader.cpp vec3f.cpp
DEPS = imageloader.h  vec3f.h

ifeq ($(shell uname),Darwin)
	LIBS = -framework OpenGL -framework GLUT
else
	LIBS = -lglut
endif

all: $(PROG)

$(PROG):	$(SRCS) $(DEPS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

clean:
	rm -f $(PROG) *~

run: $(PROG)
	./$(PROG) &

text:
	firefox text.php.html & 
