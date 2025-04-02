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
## Metadata
###############################################################################

TARGET					?=	arcade
QUIET					?=	0
FLAGS					?=	

AUTHOR					=	mallory-scotton
DATE					=	
HASH					=	

ifeq ($(shell git rev-parse HEAD > /dev/null; echo $$?),0)
	AUTHOR				:=	$(shell git log --format='%aN' | sort -u | awk \
							'{printf "%s, ", $$0}' | rev | cut -c 3- | rev)
	DATE				:=	$(shell git log -1 --date=format:"%Y/%m/%d %T" \
							--format="%ad")
	HASH				:=	$(shell git rev-parse --short HEAD)
endif

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
BACKEND_OPENGL_FLAGS	=	-lGL -lGLU -lglut -lGLEW -lglfw

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
CORE_FLAGS				=	$(LDFLAGS) -ludev -lwiiuse -lbluetooth -lpthread

###############################################################################
## Objects
###############################################################################

INCLUDES				+=	$(addprefix -I, $(CORE_DIR))

CORE_OBJECTS			=	$(CORE_SOURCES:%.cpp=$(BUILD_DIR)/%.o)

###############################################################################
## Color configuration
###############################################################################

COM_COLOR				=	\033[0;34m
OBJ_COLOR				=	\033[0;36m
OK_COLOR				=	\033[0;32m
ERROR_COLOR				=	\033[0;31m
WARN_COLOR				=	\033[0;33m
NO_COLOR				=	\033[m

###############################################################################
## Makefile rules
###############################################################################

all:
	@make QUIET=0 -s core
	@make QUIET=1 -s graphicals
	@make QUIET=1 -s games

header:
	@printf "%b" "$(OK_COLOR)"
ifeq ($(QUIET),0)
	@cat .art
endif
	@echo
ifneq ($(HASH),)
	@printf "%b" "$(OBJ_COLOR)Name:	$(WARN_COLOR)$(TARGET)@$(HASH)\n"
else
	@printf "%b" "$(OBJ_COLOR)Name:	$(WARN_COLOR)$(TARGET)\n"
endif
ifeq ($(QUIET),0)
	@printf "%b" "$(OBJ_COLOR)Author:	$(WARN_COLOR)$(AUTHOR)\n"
	@printf "%b" "$(OBJ_COLOR)Date: 	$(WARN_COLOR)$(DATE)\n\033[m"
endif
	@printf "%b" "$(OBJ_COLOR)CC: 	$(WARN_COLOR)$(CXX)\n\033[m"
ifneq ($(FLAGS),)
	@printf "%b" "$(OBJ_COLOR)Flags: 	$(WARN_COLOR)$(FLAGS)\n\033[m"
endif
	@echo

directories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(LIB_DIR)

core: directories $(CORE_TARGET)

graphicals: directories $(BACKEND_TARGETS)

games: directories $(GAME_TARGETS)

$(LIB_DIR)/arcade_%.so:
	@make QUIET=1 TARGET=$(call lowercase,$@) FLAGS="$($*_FLAGS)" -s header
	@make -s $($*_OBJECTS)
	@$(CXX) -shared $($*_OBJECTS) -o $(call lowercase,$@) $($*_FLAGS)

$(BUILD_DIR)/%.o: %.cpp
	@printf "%b%-64b" "$(COM_COLOR)compiling " "$(OBJ_COLOR)$<$(NO_COLOR)"
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@ 2> "$<.log" \
		 | grep -v '\[.*%\]' | tee -a "$<.log"
	@RESULT=$?; \
	if [[ $RESULT -ne 0 ]]; then \
		printf "%b%b\n" "$(ERROR_COLOR)" "[✖]$(NO_COLOR)          "; \
		echo; \
	elif [ -s "$2.log" ]; then \
		printf "%b%b\n" "$(WARN_COLOR)" "[⚠]$(NO_COLOR)          "; \
	else \
		printf "%b%b\n" "$(OK_COLOR)" "[✓]$(NO_COLOR)          "; \
	fi; \
	if [ -f "$<.log" ]; then \
		cat "$<.log"; \
	fi; \
	rm -f "$<.log";


$(CORE_TARGET):
	@make TARGET=$(CORE_TARGET) QUIET=0 FLAGS="$(CORE_FLAGS)" -s header
	@make -s $(CORE_OBJECTS)
	@$(CXX) $(CORE_OBJECTS) -o $@ $(CORE_FLAGS)

clean:
	@rm -rf $(BUILD_DIR)

fclean: clean
	@rm -rf $(LIB_DIR)
	@rm -f $(CORE_TARGET)

re: fclean all

.PHONY: all clean fclean re directories core graphicals games

-include $(ARC_SOURCES:%.cpp=$(BUILD_DIR)/%.d)

$(BUILD_DIR)/%.d: %.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -MM $< | \
		sed 's,\($*\)\.o[ :]*,$(BUILD_DIR)/\1.o $@ : ,g' > $@

run: all
	./$(CORE_TARGET)

debug: CXXFLAGS += -g3 -DARC_DEBUG
debug: re

valgrind: debug
	valgrind --leak-check=full ./$(CORE_TARGET)

install_wii:
	rm -rf wiiuse
	git clone git@github.com:wiiuse/wiiuse.git
	cd wiiuse && mkdir build && cd build && cmake ../src && sudo make install
	rm -rf wiiuse
