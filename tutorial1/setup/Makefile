CC = g++
CFLAGS = -Wall
PROG = cube
BROWSER = firefox # change if firefox not installed

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
	./cube &

text:
	$(BROWSER) text.php0a.html text.php0b.html text.php0c.html


