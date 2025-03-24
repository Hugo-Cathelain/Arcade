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
## Utility function
###############################################################################

lowercase				=	$(shell echo "$(1)" | tr '[:upper:]' '[:lower:]')
uppercase				=	$(shell echo "$(1)" | tr '[:lower:]' '[:upper:]')

###############################################################################
## Dynamic Backend Discovery
###############################################################################

# Find all backends directories
BACKEND_DIRS			=	$(wildcard backends/*)

# Generate targets, objects, and flags for each backend
BACKEND_TARGETS			=	$(BACKEND_DIRS:backends/%=$(LIB_DIR)/arcade_%.so)
BACKEND_OBJECTS			=	$(foreach dir,$(BACKEND_DIRS),$($(notdir $(dir))_OBJECTS))

# Backend-Specific library flags
BACKEND_SFML_FLAGS		=	-lsfml-graphics -lsfml-window -lsfml-system
BACKEND_SDL2_FLAGS		=	-lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
BACKEND_NCURSES_FLAGS	=	-lncurses

EMPTY_FLAGS				=

# Dynamic object generation for each backend
define BACKEND_TEMPLATE
	$(1)_DIR			=	backends/$(1)
	$(1)_SOURCES		=	$$(shell find $$($(1)_DIR) $(FINDFLAGS))
	$(1)_OBJECTS		=	$$($(1)_SOURCES:%.cpp=$(BUILD_DIR)/%.o)
	$(1)_FLAGS			=	$$(or $$(BACKEND_$(1)_FLAGS),$$(EMPTY_FLAGS))
endef

# Instanciate template for each backends
$(foreach backend,$(notdir $(BACKEND_DIRS)),$(eval $(call BACKEND_TEMPLATE,$(backend))))

###############################################################################
## Dynamic Game Discovery
###############################################################################

# Find all game directories
GAME_DIRS				=	$(wildcard games/*)

# Generate targets, object, and flags for each game
GAME_TARGETS			=	$(GAME_DIRS:games/%=$(LIB_DIR)/arcade_%.so)
GAME_OBJECTS			=	$(foreach dir,$(GAME_DIRS),$($(notdir $(dir))_OBJECTS))

# Dynamic object generation for each game
define GAME_TEMPLATE
	$(1)_DIR			=	games/$(1)
	$(1)_SOURCES		=	$$(shell find $$($(1)_DIR) $(FINDFLAGS))
	$(1)_OBJECTS		=	$$($(1)_SOURCES:%.cpp=$(BUILD_DIR)/%.o)
	$(1)_FLAGS			=
endef

# Instanciate template for each game
$(foreach game,$(notdir $(GAME_DIRS)),$(eval $(call GAME_TEMPLATE,$(game))))

###############################################################################
## Sources
###############################################################################

CORE_TARGET				=	arcade
CORE_DIR				=	Arcade
CORE_SOURCES			=	$(shell find $(CORE_DIR) $(FINDFLAGS))
CORE_FLAGS				=	$(LDFLAGS)

###############################################################################
## Objects
###############################################################################

INCLUDES				+=	$(addprefix -I, $(CORE_DIR))

CORE_OBJECTS			=	$(CORE_SOURCES:%.cpp=$(BUILD_DIR)/%.o)

###############################################################################
## Makefile rules
###############################################################################

all: core graphicals games

directories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(LIB_DIR)

core: directories $(CORE_TARGET)

graphicals: directories $(BACKEND_TARGETS)

games: directories $(GAME_TARGETS)

$(LIB_DIR)/arcade_%.so:
	@make $($*_OBJECTS) | grep -v "Entering" | grep -v "Leaving"
	$(CXX) -shared $($*_OBJECTS) -o $(call lowercase,$@) $($*_FLAGS)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(CORE_TARGET): $(CORE_OBJECTS)
	$(CXX) $^ -o $@ $(CORE_FLAGS)

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
