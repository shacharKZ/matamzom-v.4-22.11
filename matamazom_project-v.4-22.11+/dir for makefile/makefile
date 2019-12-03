CC = gcc
DEBUG_FLAGS = -g
CC_FLAGS = -std=c99 -Wall -Werror -pedantic-errors
OBJS_MZ = matamazom.o matamazom_print.o matamazom_main.o matamazom_tests.o product.o order.o
OBJS_AS = amount_set.o amount_set_tests.o amount_set_main.o
EXE = matamazom
LIBS = -L. -lm -lmtm

# builds all the project exe
$(EXE): $(OBJS_MZ) $(OBJS_AS)
	$(CC) $(DEBUG_FLAGS) $(CC_FLAGS) $(OBJS_MZ) -o $@ $(LIBS)

# build amount_set and its tests file (main for testing)
amount_set: $(OBJS_AS)
	$(CC) $(DEBUG_FLAGS) $(CC_FLAGS) $(OBJS_AS) -o $@


# build amount_set
amount_set.o: amount_set.c amount_set.h
	$(CC) -c $(DEBUG_FLAGS) $(CC_FLAGS) $*.c


# build amount_set_tests
amount_set_tests.o: amount_set.o
	$(CC) -c $(DEBUG_FLAGS) $(CC_FLAGS) amount_set_tests.c

# build amount_main
amount_set_main.o: amount_set_main.c test_utilities.h
	$(CC) -c $(DEBUG_FLAGS) $(CC_FLAGS) amount_set_main.c


# build all the files for matamazom
matamazom.o: matamazom.c matamazom.h product.h matamazom_print.h order.h
	$(CC) -c $(DEBUG_FLAGS) $(CC_FLAGS) $*.c


# build matamazom_print
matamazom_print.o: matamazom_print.c matamazom_print.h
	$(CC) -c $(DEBUG_FLAGS) $(CC_FLAGS) $*.c

# build matamazom_main (for running tests)
matamazom_main.o: matamazom_tests.o matamazom.o
	$(CC) -c $(DEBUG_FLAGS) $(CC_FLAGS) matamazom_main.c

# build all the files connected to testing matamazom
matamazom_tests.o: matamazom_tests.c matamazom_tests.h test_utilities.h matamazom.o
	$(CC) -c $(DEBUG_FLAGS) $(CC_FLAGS) matamazom_tests.c

# build product - a module used by matamazom
product.o: product.c product.h product.h matamazom_print.h matamazom.h
	$(CC) -c $(DEBUG_FLAGS) $(CC_FLAGS) $*.c


# build order - a module used by matamazom
order.o: order.c order.h product.h matamazom_print.h matamazom.h
	$(CC) -c $(DEBUG_FLAGS) $(CC_FLAGS) $*.c

# clean working directory
clean:
	rm -f $(OBJS_MZ) $(OBJS_AS) $(EXE)
