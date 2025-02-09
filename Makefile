# Компилятор и флаги
CXX = g++
CXXFLAGS = -std=c++17 -Wall -pthread -g -fPIC
LDFLAGS = -shared -fPIC

# Директории
OBJ_DIR = obj
BIN_DIR = bin

# Файлы исходников
SRC = main.cpp logger.cpp
OBJ = $(SRC:%.cpp=$(OBJ_DIR)/%.o)

# Файлы сборки
EXEC = $(BIN_DIR)/logger_app
LIB = $(BIN_DIR)/liblogger.so

# Сборка всего
all: $(LIB) $(EXEC)

# Сборка динамической библиотеки
$(LIB): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(LDFLAGS) -o $@ $^

# Сборка исполняемого файла
$(EXEC): $(OBJ) $(LIB)
	@mkdir -p $(BIN_DIR)
	$(CXX) $^ -o $@ -L$(BIN_DIR) -llogger -Wl,-rpath=$(BIN_DIR)

# Компиляция каждого .cpp в .o
$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Цель для очистки
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Пересборка
rebuild: clean all

# Сборка только библиотеки
lib:
	$(MAKE) $(LIB)

# Сборка только программы
app:
	$(MAKE) $(EXEC)
