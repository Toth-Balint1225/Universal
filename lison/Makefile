memflags := --leak-check=full
debug_flags := -Wall -Wextra -g  -fsanitize=address
# -DLOG_ENABLE
release_flags := -Wall -Werror -Wextra -Wpedantic -O2
libs := -lrgx -lstr

test_obj := test/tests.o test/ast.o test/parser.o test/serde.o
shared_obj := shared/ast.o shared/parser.o shared/serde.o
static_obj := static/ast.o static/parser.o static/serde.o

test := tests
lib := liblsn
inc := lsn.h

libpath := /usr/lib
incpath := /usr/include

lib: static shared test

test: $(test)
	./$<

static: $(static_obj)
	ar rcs $(lib).a $^

shared: $(shared_obj)
	gcc $(release_flags) -shared -fPIC $^ -o $(lib).so -lc

memtest: $(test)
	valgrind $(memflags) ./$<

debug: $(test)
	gdb $<

$(test): $(test_obj)
	gcc $(debug_flags) $^ -o $@ $(libs)

test/%.o: %.c
	gcc $(debug_flags) -c $< -o $@

static/%.o: %.c
	gcc $(release_flags) -c $< -o $@

shared/%.o: %.c
	gcc $(release_flags) -shared -fPIC -c $< -o $@

install: lib
	cp $(lib).so $(libpath)
	cp $(inc) $(incpath)

uninstall: 
	rm $(libpath)/$(lib).so
	rm $(incpath)/$(inc)

clean:
	rm -rf */*.o */*.o */*.o

purge:
	rm -rf */*.o */*.o */*.o *.o
	rm -rf *.a *.so $(test)
