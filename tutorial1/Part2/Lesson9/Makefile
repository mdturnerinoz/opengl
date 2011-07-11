CC = g++
CFLAGS = -Wall
PROG = animation
BROWSER = firefox

SRCS = main.cpp imageloader.cpp md2model.cpp vec3f.cpp
DEPS = imageloader.h  md2model.h  vec3f.h

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

