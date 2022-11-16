CC=gcc
CFLAGS=-Wall -g

# Directories prefixes
DIR_CMD= src/commands/
DIR_INC= src/include/
DIR_EXEC= src/exec/
DIR_TEST= src/tests/

# Includes paths
INC= src/include/string_parser.o src/include/tar_parser.o

ALL= my_cat my_ls my_mkdir my_rmdir my_rm my_mv my_cp
ALL_EXEC= $(addprefix $(DIR_EXEC), $(ALL))

all: $(ALL) my_shell

# Includes .o
string_parser.o : $(DIR_INC)string_parser.c $(DIR_INC)string_parser.h

tar_parser.o : $(DIR_INC)tar_parser.c $(DIR_INC)tar_parser.h

# Every commands executables to /src/exec
my_cat : $(DIR_CMD)my_cat.c $(INC)
	$(CC) $(CFLAGS) -o $(DIR_EXEC)my_cat $(DIR_CMD)my_cat.c $(INC)

my_ls : $(DIR_CMD)my_ls.c $(INC)
	$(CC) $(CFLAGS) -o $(DIR_EXEC)my_ls $(DIR_CMD)my_ls.c $(INC)

my_mkdir : $(DIR_CMD)my_mkdir.c $(INC)
	$(CC) $(CFLAGS) -o $(DIR_EXEC)my_mkdir $(DIR_CMD)my_mkdir.c $(INC)

my_rmdir : $(DIR_CMD)my_rmdir.c $(INC)
	$(CC) $(CFLAGS) -o $(DIR_EXEC)my_rmdir $(DIR_CMD)my_rmdir.c $(INC)

my_rm : $(DIR_CMD)my_rm.c $(INC)
	$(CC) $(CFLAGS) -o $(DIR_EXEC)my_rm $(DIR_CMD)my_rm.c $(INC)

my_mv : $(DIR_CMD)my_mv.c $(INC)
	$(CC) $(CFLAGS) -o $(DIR_EXEC)my_mv $(DIR_CMD)my_mv.c $(INC)

my_cp : $(DIR_CMD)my_cp.c $(INC)
	$(CC) $(CFLAGS) -o $(DIR_EXEC)my_cp $(DIR_CMD)my_cp.c $(INC)

# Launcher tsh shell in root project
my_shell : $(DIR_CMD)my_shell.c $(INC)
	$(CC) $(CFLAGS) -o tsh $(DIR_CMD)my_shell.c $(INC)

# Test for string_parser
test : $(DIR_TEST)string_parser_test.c $(DIR_INC)string_parser.o
	$(CC) $(CFLAGS) -o $(DIR_TEST)string_parser_test \
    $(DIR_TEST)string_parser_test.c $(DIR_INC)string_parser.o

clean:
	rm -rf $(ALL_EXEC) $(INC) tsh $(DIR_TEST)string_parser_test