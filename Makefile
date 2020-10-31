PROJECT_NAME=grand-mother-tongue

.PHONY: all run
all: $(PROJECT_NAME)

$(PROJECT_NAME):
	c++ src/main.cpp -std=c++17 -ggdb -lcurl -o $(PROJECT_NAME)

run: $(PROJECT_NAME)
	./$(PROJECT_NAME) db.txt

