PROJECT_NAME=grand-mother-tongue

.PHONY: all run format
all: $(PROJECT_NAME)

3rd_party/aids-patched.hpp: $(wildcard 3rd_party/*.patch) 3rd_party/aids.hpp
	cp 3rd_party/aids.hpp 3rd_party/aids-patched.hpp
	git apply 3rd_party/aids-*.patch

$(PROJECT_NAME): $(wildcard src/*.cpp) $(wildcard src/*.hpp) 3rd_party/aids-patched.hpp
	c++ src/main.cpp -std=c++17 -ggdb -lcurl -o $(PROJECT_NAME)

format:	
	find src/ -iname *.hpp -o -iname *.cpp | xargs /opt/rh/llvm-toolset-7/root/usr/bin/clang-format -i -style=file

clean:
	rm -rf 3rd_party/aids-patched.hpp $(PROJECT_NAME)
