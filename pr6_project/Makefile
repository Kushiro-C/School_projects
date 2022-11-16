CC=gcc
CFLAGS= -Wall -g3 -Wall -pthread
JCC = javac
JFLAGS = -g

C_FILES= user1
JAVA_FILES= Manager.class Diffuser.class Formatter.class Parser.class
ALL= $(C_FILES) $(JAVA_FILES)
all: $(ALL)

# C Files
formatter.o: formatter.c formatter.h
parser.o: parser.c parser.h
user1: user1.c formatter.o parser.o
	$(CC) $(CFLAGS) user1.c -o user1 formatter.o parser.o

# Java Files
Diffuser.class: Diffuser1.java
	$(JCC) $(JFLAGS) Diffuser1.java
Manager.class: Manager1.java
	$(JCC) $(JFLAGS) Manager1.java
Formatter.class: Formatter.java
	$(JCC) $(JFLAGS) Formatter.java
Parser.class: Parser.java
	$(JCC) $(JFLAGS) Parser.java

clean:
	rm -rf *.o *.class user1
