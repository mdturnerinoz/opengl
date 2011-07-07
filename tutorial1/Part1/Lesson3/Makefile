CC = g++
CFLAGS = -Wall
PROG = color
BROWSER = firefox

SRCS = main.cpp

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

run: $(PROG)
	./$(PROG)

text:
	$(BROWSER) text.php.html &

