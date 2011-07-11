CC = g++
CFLAGS = -Wall
PROG = backfaceculling
BROWSER = firefox

SRCS = main.cpp imageloader.cpp
DEPS = imageloader.h

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

