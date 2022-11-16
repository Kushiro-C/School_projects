CC=gcc
CFLAGS=-Wall -g -pedantic #-pthread
LDLIBS= -lm -lrt -lpthread

ALL = normalTest forkTest forkTest2 tests ext2Test forkTest3
all : $(ALL)

#m_file : m_file.o
#	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

m_file.o : m_file.c m_file.h
	$(CC) $(CFLAGS) -c $< $(LDLIBS)

normalTest : normalTest.o m_file.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

normalTest.o : normalTest.c
	$(CC) $(CFLAGS) -c $< $(LDLIBS)

forkTest : forkTest.o m_file.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

forkTest.o : forkTest.c
	$(CC) $(CFLAGS) -c $< $(LDLIBS)

forkTest2 : forkTest2.o m_file.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

forkTest2.o : forkTest2.c
	$(CC) $(CFLAGS) -c $< $(LDLIBS)

tests : tests.o m_file.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

tests.o : tests.c
	$(CC) $(CFLAGS) -c $< $(LDLIBS)

ext2Test : ext2Test.o m_file.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

ext2Test.o : ext2Test.c
	$(CC) $(CFLAGS) -c $< $(LDLIBS)

forkTest3 : forkTest3.o m_file.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

forkTest3.o : forkTest3.c
	$(CC) $(CFLAGS) -c $< $(LDLIBS)

clean:
	rm -rf *.o $(ALL) $(EXEC)

#test:
#	./normalTest | ./forkTest



