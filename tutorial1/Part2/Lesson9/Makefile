CC = g++
CFLAGS = -Wall
PROG = animation
BROWSER = firefox

SRCS = main.cpp imageloader.cpp md2model.cpp vec3f.cpp

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

