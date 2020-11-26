PROJECT_NAME=grand-mother-tongue

.PHONY: all run format tests clean
all: $(PROJECT_NAME)

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

clean:
	rm -rf 3rd_party/aids-patched.hpp $(PROJECT_NAME)
