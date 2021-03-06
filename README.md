<pre>
This series of files represents my efforts to understand OpenGL using a (quite good) tutorial 
found at http://www.videotutorialsrock.com/index.php.

All technical (i.e. OpenGL) content is from the presentation; I can take absolutely no credit 
for any of it per se. I have made the following changes so it would be more portable using git:

1. Created a git repository at the top of the tree.
2. Embellished the each makefile so that it's more easily used from a top-level makefile.
3. Added a top-level makefile.
4. Added a  "#include <string.h>" file to a couple of the main.cpp files because GCC returned 
   an error on my Linux system (I tested on Mandriva Linux 2010.2 and Mac OS X 10.6).

I plan further enhancements to this project as I get more time and a better understanding of 
OpenGL.

Thanks to the author of the tutorial noted above as it's the first one I've found that really
helped me understand how start using OpenGL (albeit using GLUT on top of OpenGL).

Building the Code

After doing a git clone of the project, you can perform a make (all implied) at the top level 
of the tutorial (./Tutorial1). The top-level make targets and project strict is as follows:

TOP-LEVEL Makefile (follow each subdir via the tutorial website table of contents):

cd to tutorial1

SUBDIRS: setup 
         Part1/Lesson1 Part1/Lesson2 Part1/Lesson3 Part1/Lesson4 Part1/Lesson5 Part1/Lesson6 
         Part2/Lesson7 Part2/Lesson8 Part2/Lesson9 Part2/Lesson10 Part2/Lesson11             
         Part3/Lesson12 Part3/Lesson13 Part3/Lesson14 Part3/Lesson15                         
         Part4/Lesson16 Part4/Lesson17 Part4/Lesson18 Part4/Lesson19 Part4/Lesson20          
         Part5/Lesson21

all:   Build the entire project (you can also cd to a particular directory to focus on running it
       etc.)

clean: Clean up all executables/junk etc.

list:  List (ls) the contents of all projects.


PROJECT Makefile Targets (all subdir Makefiles)

all:     Builds all executables and libraries in a given project (the default target)

$(PROG):	The program name of a given target.

clean:   Clean up all executable, objects, and temporary files.

run:     Run the target program.

text:    Output the local saved html pages captured from the webiste (in case the webiste ever
         goes down or goes away). (Note this target assumes a $(BROWSER) variable of "firefox".


Marty Turner (mdturnerinoz@gmail.com) July 2011



</pre>
