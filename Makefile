###############################################################################
## Compiler Options
###############################################################################

CXX						=	g++
CXXFLAGS				=	-std=c++17 -Wall -Wextra -fPIC

LDFLAGS					=	-ldl -fno-gnu-unique -rdynamic

FINDFLAGS				=	-type f -iname "*.cpp"

BUILD_DIR				=	build
LIB_DIR					=	lib

INCLUDES				=	-I.

###############################################################################
## Sources
###############################################################################

ARC_TARGET				=	arcade
ARC_DIR					=	arcade
ARC_SOURCES				=	$(shell find $(ARC_DIR) $(FINDFLAGS))
ARC_FLAGS				=	$(LDFLAGS)

SFML_TARGET				=	$(LIB_DIR)/arcade_sfml.so
SFML_DIR				=	backends/SFML
SFML_SOURCES			=	$(shell find $(SFML_DIR) $(FINDFLAGS))
SFML_FLAGS				=	-lsfml-graphics -lsfml-window -lsfml-system

NCURSES_TARGET			=	$(LIB_DIR)/arcade_ncurses.so
NCURSES_DIR				=	backends/NCURSES
NCURSES_SOURCES			=	$(shell find $(NCURSES_DIR) $(FINDFLAGS))
NCURSES_FLAGS			=	-lncurses

EXAMPLE_TARGET			=	$(LIB_DIR)/arcade_example.so
EXAMPLE_DIR				=	games/Example
EXAMPLE_SOURCES			=	$(shell find $(EXAMPLE_DIR) $(FINDFLAGS))
EXAMPLE_FLAGS			=

###############################################################################
## Objects
###############################################################################

INCLUDES				+=	$(addprefix -I, $(ARC_DIR))

ARC_OBJECTS				=	$(ARC_SOURCES:%.cpp=$(BUILD_DIR)/%.o)
SFML_OBJECTS			=	$(SFML_SOURCES:%.cpp=$(BUILD_DIR)/%.o)
NCURSES_OBJECTS			=	$(NCURSES_SOURCES:%.cpp=$(BUILD_DIR)/%.o)
EXAMPLE_OBJECTS			=	$(EXAMPLE_SOURCES:%.cpp=$(BUILD_DIR)/%.o)

###############################################################################
## Makefile rules
###############################################################################

all: core graphicals games

directories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(LIB_DIR)

core: directories $(ARC_TARGET)
graphicals: directories $(SFML_TARGET) $(NCURSES_TARGET)
games: directories $(EXAMPLE_TARGET)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(ARC_TARGET): $(ARC_OBJECTS)
	$(CXX) $^ -o $@ $(ARC_FLAGS)

$(SFML_TARGET): $(SFML_OBJECTS)
	$(CXX) -shared $^ -o $@ $(SFML_FLAGS)

$(NCURSES_TARGET): $(NCURSES_OBJECTS)
	$(CXX) -shared $^ -o $@ $(NCURSES_FLAGS)

$(EXAMPLE_TARGET): $(EXAMPLE_OBJECTS)
	$(CXX) -shared $^ -o $@ $(EXAMPLE_FLAGS)

clean:
	rm -rf $(BUILD_DIR)

fclean: clean
	rm -rf $(LIB_DIR)
	rm -f $(ARC_TARGET)

re: fclean all

.PHONY: all clean fclean re directories core graphicals games

-include $(ARC_SOURCES:%.cpp=$(BUILD_DIR)/%.d)

$(BUILD_DIR)/%.d: %.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -MM $< | \
		sed 's,\($*\)\.o[ :]*,$(BUILD_DIR)/\1.o $@ : ,g' > $@

run: all
	./$(ARC_TARGET)

debug: CXXFLAGS += -g3 -DARC_DEBUG
debug: re

valgrind: debug
	valgrind --leak-check=full ./$(ARC_TARGET)
