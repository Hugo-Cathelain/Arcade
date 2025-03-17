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

CORE_TARGET				=	arcade
CORE_DIR				=	Arcade
CORE_SOURCES			=	$(shell find $(CORE_DIR) $(FINDFLAGS))
CORE_FLAGS				=	$(LDFLAGS)

SFML_TARGET				=	$(LIB_DIR)/arcade_sfml.so
SFML_DIR				=	backends/SFML
SFML_SOURCES			=	$(shell find $(SFML_DIR) $(FINDFLAGS))
SFML_FLAGS				=	-lsfml-graphics -lsfml-window -lsfml-system

NCURSES_TARGET			=	$(LIB_DIR)/arcade_ncurses.so
NCURSES_DIR				=	backends/NCURSES
NCURSES_SOURCES			=	$(shell find $(NCURSES_DIR) $(FINDFLAGS))
NCURSES_FLAGS			=	-lncurses

NIBBLER_TARGET			=	$(LIB_DIR)/arcade_nibbler.so
NIBBLER_DIR				=	games/NIBBLER
NIBBLER_SOURCES			=	$(shell find $(NIBBLER_DIR) $(FINDFLAGS))
NIBBLER_FLAGS			=

###############################################################################
## Objects
###############################################################################

INCLUDES				+=	$(addprefix -I, $(CORE_DIR))

CORE_OBJECTS			=	$(CORE_SOURCES:%.cpp=$(BUILD_DIR)/%.o)
SFML_OBJECTS			=	$(SFML_SOURCES:%.cpp=$(BUILD_DIR)/%.o)
NCURSES_OBJECTS			=	$(NCURSES_SOURCES:%.cpp=$(BUILD_DIR)/%.o)
NIBBLER_OBJECTS			=	$(NIBBLER_SOURCES:%.cpp=$(BUILD_DIR)/%.o)

###############################################################################
## Makefile rules
###############################################################################

all: core graphicals games

directories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(LIB_DIR)

core: directories $(CORE_TARGET)

graphicals: directories $(SFML_TARGET) $(NCURSES_TARGET)

games: directories $(NIBBLER_TARGET)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(CORE_TARGET): $(CORE_OBJECTS)
	$(CXX) $^ -o $@ $(CORE_FLAGS)

$(SFML_TARGET): $(SFML_OBJECTS)
	$(CXX) -shared $^ -o $@ $(SFML_FLAGS)

$(NCURSES_TARGET): $(NCURSES_OBJECTS)
	$(CXX) -shared $^ -o $@ $(NCURSES_FLAGS)

$(NIBBLER_TARGET): $(NIBBLER_OBJECTS)
	$(CXX) -shared $^ -o $@ $(NIBBLER_FLAGS)

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
