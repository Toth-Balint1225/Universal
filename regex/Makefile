## FLAGS
memflags := --leak-check=full
debug_flags := -Wall -g -fsanitize=address
# -DLOG_ENABLE
release_flags := -Wall -Werror -Wextra -Wpedantic -O2

## OBJECTS
test_obj   := test/tests.o test/regex.o test/parser.o
shared_obj := shared/regex.o shared/parser.o
static_obj := static/regex.o static/parser.o

libs := -lstr

## ARTIFACTS
test := tests
lib := librgx
inc := rgx.h

## PATH
libpath := /usr/lib
incpath := /usr/include

## ARTIFACTS
lib: static shared test

test: $(test)
	./$<

memtest: $(test)
	valgrind $(memflags) ./$<

debug: $(test)
	gdb $<

static: $(static_obj)
	ar rcs $(lib).a $^

shared: $(shared_obj)
	gcc $(release_flags) -shared -fPIC $^ -o $(lib).so $(libs)

$(test): $(test_obj)
	gcc $(debug_flags) $^ -o $@ $(libs)

test/%.o: %.c
	gcc $(debug_flags) -c $< -o $@ $(libs)

static/%.o: %.c
	gcc $(release_flags) -c $< -o $@ $(libs)

shared/%.o: %.c
	gcc $(release_flags) -fPIC -c $< -o $@ $(libs)

clean:
	rm -rf */*.o */*.o */*.o

purge:
	rm -rf */*.o */*.o */*.o
	rm -rf *.a *.so $(test)

install: lib
	cp $(lib).so $(libpath)
	cp $(inc) $(incpath)

uninstall: 
	rm $(libpath)/$(lib).so
	rm $(incpath)/$(inc)

.PHONY: install uninstall clean purge test memtest debug static shared
