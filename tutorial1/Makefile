SUBDIRS = setup Part1/Lesson1 Part1/Lesson2 Part1/Lesson3 Part1/Lesson4 Part1/Lesson5 Part1/Lesson6 \
                Part2/Lesson7 Part2/Lesson8 Part2/Lesson9 Part2/Lesson10 Part2/Lesson11             \
                Part3/Lesson12 Part3/Lesson13 Part3/Lesson14 Part3/Lesson15                         \
                Part4/Lesson16 Part4/Lesson17 Part4/Lesson18 Part4/Lesson19 Part4/Lesson20          \
                Part5/Lesson21

all:
	@for i in $(SUBDIRS); do \
	echo "make all in $$i..."; \
	($(MAKE) -C $$i $(MFLAGS) $(MYMAKEFLAGS) all); done

clean:
	rm -f *~ typescript
	@for i in $(SUBDIRS); do \
	echo "Cleaning in $$i..."; \
	($(MAKE) -C $$i $(MFLAGS) $(MYMAKEFLAGS) clean); done 

list:
	@for i in $(SUBDIRS); do  \
	echo "-------  $$i  -------"; \
   (ls --color $$i); done

nompp:
	@for i in $(SUBDIRS); do \
	echo "Cleaning makepp subdir in $$i..."; \
	(cd $$i; rm -fr ./.makepp); done 


