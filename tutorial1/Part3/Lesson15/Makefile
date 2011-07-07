CC = g++
CFLAGS = -Wall
PROG = fog
BROWSER = firefox

SRCS = main.cpp imageloader.cpp

ifeq ($(shell uname),Darwin)
	LIBS = -framework OpenGL -framework GLUT
else
	LIBS = -lglut
endif

all: $(PROG)

$(PROG):	$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

clean:
	rm -f $(PROG) *~

run:
	./$(PROG)

text:
	$(BROWSER) text.php.html &

