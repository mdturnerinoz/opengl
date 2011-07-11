CC = g++
CFLAGS = -Wall
PROG = crabpong
BROWSER = firefox

SRCS = main.cpp game.cpp gamedrawer.cpp imageloader.cpp md2model.cpp text3d.cpp vec3f.cpp
DEPS = gamedrawer.h  game.h  imageloader.h  md2model.h  text3d.h  vec3f.h

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
	$(BROWSER) text.php.html &
