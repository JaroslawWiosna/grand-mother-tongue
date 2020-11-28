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

$(PROJECT_NAME): $(wildcard src/*.cpp) $(wildcard src/*.hpp) 3rd_party/aids-patched.hpp src/version.cpp
	c++ src/main.cpp -std=c++17 -ggdb -lcurl -o $(PROJECT_NAME)

format:	
	find src/ -iname *.hpp -o -iname *.cpp | xargs /opt/rh/llvm-toolset-7/root/usr/bin/clang-format -i -style=file

tests:
	#TODO(#18): No tests written

valgrind:
	#TODO(#31): Unhardcode path to valgrind
	/opt/rh/devtoolset-7/root/usr/bin/valgrind --leak-check=full ./grand-mother-tongue -g 1 -i Q53449

valgrind-full: full.supp
	/opt/rh/devtoolset-7/root/usr/bin/valgrind --leak-check=full --show-leak-kinds=all --suppressions=full.supp ./grand-mother-tongue -g 1 -i Q53449

clean:
	rm -rf 3rd_party/aids-patched.hpp $(PROJECT_NAME) src/version.cpp
