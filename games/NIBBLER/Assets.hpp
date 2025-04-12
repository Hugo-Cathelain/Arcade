///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Arcade/interfaces/IGameModule.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc
///////////////////////////////////////////////////////////////////////////////
namespace Arc
{
///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
#define ARCADE_SCREEN_WIDTH     28
#define ARCADE_SCREEN_HEIGHT    36
#define ARCADE_GAME_WIDTH       28
#define ARCADE_GAME_HEIGHT      31
#define ARCADE_OFFSET_Y         (ARCADE_SCREEN_HEIGHT - ARCADE_GAME_HEIGHT - 2)

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
#define CLR_RED    Color{255, 0, 0}
#define CLR_GREEN  Color{0, 255, 0}
#define CLR_BLUE   Color{0, 0, 255}
#define CLR_YELLOW Color{255, 255, 0}
#define CLR_CYAN   Color{0, 255, 255}
#define CLR_ORANGE Color{255, 165, 0}
#define CLR_PINK   Color{255, 192, 203}
#define CLR_BLACK  Color{0, 0, 0}
#define CLR_WHITE  Color{255, 255, 255}

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
enum SpriteType {
    BORDER_IN_BR,
    BORDER_IN_BL,
    BORDER_OUT_BL,
    BORDER_IN_TR,
    BORDER_IN_TL,
    BORDER_OUT_TL,
    WALL_RIGHT,
    WALL_LEFT,
    BORDER_OUT_BR,
    WALL_BOT,
    WALL_TOP,
    BORDER_OUT_TR,

    SNAKE_TAIL_TOP,
    SNAKE_TAIL_RIGHT,
    SNAKE_TAIL_LEFT,
    SNAKE_TAIL_BOTTOM,
    SNAKE_BODY_H,
    SNAKE_BODY_V,
    SNAKE_TURN_TOP_RIGHT,
    SNAKE_TURN_TOP_LEFT,
    SNAKE_TURN_BOTTOM_LEFT,
    SNAKE_TURN_BOTTOM_RIGHT,
    SNAKE_HEAD_RIGHT,
    SNAKE_HEAD_LEFT,
    SNAKE_HEAD_BOTTOM,
    SNAKE_HEAD_TOP,

    TRON_SQUARE,

    FRUIT_START,

    EMPTY
};

/// auto sprite = SPRITE[BORDER]
/// srpite.position.x + OFFSETT_COLOR

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
static const std::vector<IGameModule::Asset> SPRITES = {
    IGameModule::Asset({0, 0}, "/ ", CLR_WHITE),      //<! BORDER_IN_BR
    IGameModule::Asset({0, 1}, " \\", CLR_WHITE),      //<! BORDER_IN_BL
    IGameModule::Asset({0, 2}, "\\ ", CLR_WHITE),      //<! BORDER_OUT_BL
    IGameModule::Asset({1, 0}, "\\ ", CLR_WHITE),      //<! BORDER_IN_TR
    IGameModule::Asset({1, 1}, " /", CLR_WHITE),      //<! BORDER_IN_TL
    IGameModule::Asset({1, 2}, "/ ", CLR_WHITE),      //<! BORDER_OUT_TL
    IGameModule::Asset({2, 0}, " |", CLR_WHITE),      //<! WALL_RIGHT
    IGameModule::Asset({2, 1}, "| ", CLR_WHITE),      //<! WALL_LEFT
    IGameModule::Asset({2, 2}, " /", CLR_WHITE),      //<! BORDER_OUT_BR
    IGameModule::Asset({3, 0}, "--", CLR_WHITE),      //<! WALL_BOT
    IGameModule::Asset({3, 1}, "--", CLR_WHITE),      //<! WALL_TOP
    IGameModule::Asset({3, 2}, " \\", CLR_WHITE),      //<! BORDER_OUT_TR

    IGameModule::Asset({4, 0}, "/\\", CLR_RED),        //<! SNAKE_TAIL_TOP
    IGameModule::Asset({4, 1}, "=-", CLR_RED),        //<! SNAKE_TAIL_RIGHT
    IGameModule::Asset({5, 0}, "=-", CLR_RED),        //<! SNAKE_TAIL_LEFT
    IGameModule::Asset({5, 1}, "\\/", CLR_RED),        //<! SNAKE_TAIL_BOTTOM
    IGameModule::Asset({4, 2}, "==", CLR_RED),        //<! SNAKE_BODY_HSLIM
    IGameModule::Asset({5, 2}, "||", CLR_RED),        //<! SNAKE_BODY_VSLIM
    IGameModule::Asset({6, 0}, "//", CLR_RED),        //<! SNAKE_TURN_TOP_RIGHT
    IGameModule::Asset({6, 1}, "\\\\", CLR_RED),       //<! SNAKE_TURN_TOP_LEFT
    IGameModule::Asset({7, 0}, "\\\\", CLR_RED),       //<! SNAKE_TURN_BOTTOM_LEFT
    IGameModule::Asset({7, 1}, "//", CLR_RED),        //<! SNAKE_TURN_BOTTOM_RIGHT
    IGameModule::Asset({8, 0}, "()", CLR_RED, {18,16}),        //<! SNAKE_HEAD_RIGHT
    IGameModule::Asset({8, 3}, "()", CLR_RED, {18,16}),        //<! SNAKE_HEAD_LEFT
    IGameModule::Asset({10, 0}, "()", CLR_RED, {16,18}),        //<! SNAKE_HEAD_BOTTOM
    IGameModule::Asset({10, 3}, "()", CLR_RED, {16,18}),        //<! SNAKE_HEAD_TOP

    IGameModule::Asset({0, 13}, "##", CLR_YELLOW),      //<! TRON_SQUARE

    IGameModule::Asset({0, 14}, "##", CLR_YELLOW),      //<! FRUIT_START

    IGameModule::Asset({0, 22}, "  ", CLR_YELLOW),      //<! EMPTY
};

// maps

static const SpriteType MAPS_PER_LEVEL[4][27][27] = {
    // level 1
    {
        { // colums 1
            BORDER_IN_BR, WALL_LEFT, WALL_LEFT, WALL_LEFT, WALL_LEFT, WALL_LEFT, WALL_LEFT, WALL_LEFT,
            WALL_LEFT, WALL_LEFT, WALL_LEFT, WALL_LEFT, WALL_LEFT, WALL_LEFT, WALL_LEFT, WALL_LEFT,
            WALL_LEFT, WALL_LEFT, WALL_LEFT, WALL_LEFT, WALL_LEFT, WALL_LEFT,
            WALL_LEFT, WALL_LEFT, WALL_LEFT, WALL_LEFT, BORDER_IN_TR
        },
        { // colums 2
            WALL_TOP, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
            EMPTY, EMPTY, EMPTY, EMPTY, WALL_BOT
        },
        { // colums 3
            WALL_TOP, EMPTY, BORDER_OUT_BR, WALL_RIGHT, WALL_RIGHT, WALL_RIGHT,
            BORDER_OUT_TR, EMPTY, BORDER_OUT_BR, WALL_RIGHT, WALL_RIGHT, WALL_RIGHT,
            BORDER_OUT_TR, EMPTY, BORDER_OUT_BR, BORDER_OUT_TR, EMPTY, BORDER_OUT_BR,
            WALL_RIGHT, WALL_RIGHT, WALL_RIGHT, BORDER_OUT_TR, EMPTY, BORDER_OUT_BR,
            BORDER_OUT_TR, EMPTY, WALL_BOT,
        },
        { // colums 4
            WALL_TOP, EMPTY, WALL_BOT, BORDER_IN_BR, WALL_LEFT, WALL_LEFT,
            WALL_LEFT, BORDER_IN_TR, WALL_TOP, EMPTY, WALL_BOT, BORDER_IN_BR,
            WALL_LEFT, WALL_LEFT, BORDER_OUT_TL, EMPTY, WALL_BOT, WALL_TOP,
            EMPTY, WALL_BOT, BORDER_IN_BR, WALL_LEFT, BORDER_IN_TR, WALL_TOP,
            WALL_BOT, EMPTY, WALL_BOT,
        },
        { // colums 5
            WALL_TOP, EMPTY, WALL_BOT, WALL_TOP, EMPTY, WALL_BOT, WALL_TOP,
            EMPTY, WALL_BOT, WALL_TOP, EMPTY, EMPTY, EMPTY, EMPTY, WALL_BOT,
            WALL_TOP, EMPTY, WALL_BOT, WALL_TOP, EMPTY, WALL_BOT, WALL_TOP,
            EMPTY, WALL_BOT, WALL_TOP, EMPTY, WALL_BOT
        },
        { // colums 6
            WALL_TOP, EMPTY, WALL_BOT, BORDER_IN_BL, WALL_RIGHT, BORDER_IN_TL,
            WALL_TOP, EMPTY, WALL_BOT, WALL_TOP, EMPTY, BORDER_OUT_BR,
            BORDER_OUT_TR, EMPTY, WALL_BOT, WALL_TOP, EMPTY, WALL_BOT,
            BORDER_IN_BL, WALL_RIGHT, BORDER_IN_TL, WALL_TOP,
            EMPTY, WALL_BOT, WALL_TOP, EMPTY, WALL_BOT
        },
        { // colums 7
            WALL_TOP, EMPTY, 
        },
        { // colums 8
            BORDER_IN_BR, BORDER_IN_BL,
            BORDER_OUT_BL,
            BORDER_IN_TR,
            BORDER_IN_TL,
            BORDER_OUT_TL,
        },
        { // colums 9
            WALL_LEFT,
            EMPTY,
            EMPTY,
            EMPTY,
            EMPTY,
            EMPTY,
        },
        { // colums 10
            WALL_LEFT,
            EMPTY,
            EMPTY,
        },
        { // colums 11
            WALL_LEFT,
            EMPTY,
        },
        { // colums 12
            WALL_LEFT,
        },
        { // colums 13
            BORDER_OUT_BR,
        },
    },

    // level 2
    {},

    // level 3
    {},

    // level 4
    {},

};

} // namespace Arc
