CC = g++
CFLAGS = -Wall
PROG = drawtext
BROWSER = firefox

SRCS = main.cpp text3d.cpp
DEPS = text3d.h

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

