PROJECT_NAME=grand-mother-tongue

.PHONY: all run format
all: $(PROJECT_NAME)

$(PROJECT_NAME):
	c++ src/main.cpp -std=c++17 -ggdb -lcurl -o $(PROJECT_NAME)

run: $(PROJECT_NAME)
	./$(PROJECT_NAME) db.txt

format:	
	find src/ -iname *.hpp -o -iname *.cpp | xargs /opt/rh/llvm-toolset-7/root/usr/bin/clang-format -i -style=file

