PROJECT_NAME=grand-mother-tongue

.PHONY: all run format tests valgrind valgrind-full clean
all: $(PROJECT_NAME)

# Taken from https://stackoverflow.com/a/51730966
src/version.cpp:
	printf '"%s"' `git describe --tags --always --dirty` | grep -qsf - src/version.cpp \
	|| printf >src/version.cpp 'const char version[]="%s";\n' `git describe --tags --always --dirty`

3rd_party/aids-patched.hpp: $(wildcard 3rd_party/*.patch) 3rd_party/aids.hpp
	cp 3rd_party/aids.hpp 3rd_party/aids-patched.hpp
	git apply 3rd_party/aids-*.patch

ahnentafel_lookup_table_generator: src/ahnentafel_lookup_table_generator.cpp
	c++ src/ahnentafel_lookup_table_generator.cpp -std=c++17 -fno-exceptions -Wall -Wextra -pedantic -Wno-missing-braces -Wswitch-enum -O0 -fno-builtin -ggdb -o ahnentafel_lookup_table_generator

src/03_ahnentafel_lookup_table.hpp: ahnentafel_lookup_table_generator
	./ahnentafel_lookup_table_generator 2048 > src/03_ahnentafel_lookup_table.hpp

#TODO(#38): Besides `-O0` Consider target `-O3`
$(PROJECT_NAME): $(wildcard src/*.cpp) $(wildcard src/*.hpp) 3rd_party/aids-patched.hpp src/version.cpp src/03_ahnentafel_lookup_table.hpp
	c++ src/main.cpp -std=c++17 -fno-exceptions -Wall -Wextra -pedantic -Wno-missing-braces -Wswitch-enum -O0 -fno-builtin -ggdb -lcurl -o $(PROJECT_NAME)

format:	
	find src/ -iname *.hpp -o -iname *.cpp | xargs /opt/rh/llvm-toolset-7/root/usr/bin/clang-format -i -style=file

tests: $(wildcard src/*.cpp) $(wildcard src/*.hpp) 3rd_party/aids-patched.hpp src/version.cpp src/03_ahnentafel_lookup_table.hpp
	c++ src/main.cpp -DGRAND_MOTHER_TONGUE_TEST -std=c++17 -ggdb -lcurl -o $(PROJECT_NAME)-tests
	./$(PROJECT_NAME)-tests

valgrind:
	#TODO(#31): Unhardcode path to valgrind
	/opt/rh/devtoolset-7/root/usr/bin/valgrind --leak-check=full ./grand-mother-tongue -g 1 -i Q53449

valgrind-full: full.supp
	/opt/rh/devtoolset-7/root/usr/bin/valgrind --leak-check=full --show-leak-kinds=all --suppressions=full.supp ./grand-mother-tongue -g 1 -i Q53449

clean:
	rm -rf 3rd_party/aids-patched.hpp $(PROJECT_NAME) $(PROJECT_NAME)-tests src/version.cpp src/03_ahnentafel_lookup_table.hpp ahnentafel_lookup_table_generator
