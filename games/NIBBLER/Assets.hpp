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
#define ARCADE_SCREEN_HEIGHT    32
#define ARCADE_GAME_WIDTH       27
#define ARCADE_GAME_HEIGHT      27
#define ARCADE_OFFSET_Y         (ARCADE_SCREEN_HEIGHT - ARCADE_GAME_HEIGHT - 1)

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
    IGameModule::Asset({1, 0}, " \\", CLR_WHITE),      //<! BORDER_IN_BL
    IGameModule::Asset({2, 0}, "\\ ", CLR_WHITE),      //<! BORDER_OUT_BL
    IGameModule::Asset({0, 1}, "\\ ", CLR_WHITE),      //<! BORDER_IN_TR
    IGameModule::Asset({1, 1}, " /", CLR_WHITE),      //<! BORDER_IN_TL
    IGameModule::Asset({2, 1}, "/ ", CLR_WHITE),      //<! BORDER_OUT_TL
    IGameModule::Asset({0, 2}, " |", CLR_WHITE),      //<! WALL_RIGHT
    IGameModule::Asset({1, 2}, "| ", CLR_WHITE),      //<! WALL_LEFT
    IGameModule::Asset({2, 2}, " /", CLR_WHITE),      //<! BORDER_OUT_BR
    IGameModule::Asset({0, 3}, "--", CLR_WHITE),      //<! WALL_BOT
    IGameModule::Asset({1, 3}, "--", CLR_WHITE),      //<! WALL_TOP
    IGameModule::Asset({2, 3}, " \\", CLR_WHITE),      //<! BORDER_OUT_TR

    IGameModule::Asset({6, 10}, "/\\", CLR_RED, {8, 24}),        //<! SNAKE_TAIL_TOP
    IGameModule::Asset({2, 5}, "=-", CLR_RED, {24, 8}),        //<! SNAKE_TAIL_RIGHT
    IGameModule::Asset({5, 5}, "-=", CLR_RED, {24, 8}),        //<! SNAKE_TAIL_LEFT
    IGameModule::Asset({6, 7}, "\\/", CLR_RED, {8, 24}),        //<! SNAKE_TAIL_BOTTOM
    IGameModule::Asset({0, 4}, "==", CLR_RED),        //<! SNAKE_BODY_HSLIM
    IGameModule::Asset({0, 5}, "||", CLR_RED),        //<! SNAKE_BODY_VSLIM
    IGameModule::Asset({0, 6}, "//", CLR_RED),        //<! SNAKE_TURN_TOP_RIGHT
    IGameModule::Asset({1, 6}, "\\\\", CLR_RED),       //<! SNAKE_TURN_TOP_LEFT
    IGameModule::Asset({0, 7}, "\\\\", CLR_RED),       //<! SNAKE_TURN_BOTTOM_LEFT
    IGameModule::Asset({1, 7}, "//", CLR_RED),        //<! SNAKE_TURN_BOTTOM_RIGHT
    IGameModule::Asset({0, 8}, "()", CLR_RED, {18,16}),        //<! SNAKE_HEAD_RIGHT
    IGameModule::Asset({3, 8}, "()", CLR_RED, {18,16}),        //<! SNAKE_HEAD_LEFT
    IGameModule::Asset({0, 10}, "()", CLR_RED, {16,18}),        //<! SNAKE_HEAD_BOTTOM
    IGameModule::Asset({3, 10}, "()", CLR_RED, {16,18}),        //<! SNAKE_HEAD_TOP

    IGameModule::Asset({13, 0}, "##", CLR_YELLOW),      //<! TRON_SQUARE

    IGameModule::Asset({14, 0}, "##", CLR_YELLOW),      //<! FRUIT_START

    IGameModule::Asset({22, 0}, "  ", CLR_YELLOW),      //<! EMPTY
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
            WALL_TOP, EMPTY, WALL_BOT, BORDER_IN_BR, WALL_LEFT, BORDER_IN_TR,
            WALL_TOP, EMPTY, WALL_BOT, BORDER_IN_BR,
            WALL_LEFT, WALL_LEFT, BORDER_OUT_TL, EMPTY, WALL_BOT, WALL_TOP,
            EMPTY, WALL_BOT, BORDER_IN_BR, WALL_LEFT, BORDER_IN_TR, WALL_TOP,
            EMPTY, WALL_BOT, WALL_TOP, EMPTY, WALL_BOT
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
            WALL_TOP, EMPTY, BORDER_OUT_BL, WALL_LEFT, WALL_LEFT, WALL_LEFT,
            BORDER_OUT_TL, EMPTY, WALL_BOT, WALL_TOP, EMPTY, WALL_BOT, WALL_TOP,
            EMPTY, WALL_BOT, WALL_TOP, EMPTY, BORDER_OUT_BL, WALL_LEFT,
            WALL_LEFT, WALL_LEFT, BORDER_OUT_TL, EMPTY, WALL_BOT, WALL_TOP,
            EMPTY, WALL_BOT
        },
        { // colums 8
            WALL_TOP, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
            WALL_BOT, WALL_TOP, EMPTY, WALL_BOT, WALL_TOP, EMPTY, WALL_BOT,
            WALL_TOP, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
            WALL_BOT, WALL_TOP, EMPTY, WALL_BOT
        },
        { // colums 9
            WALL_TOP, EMPTY, BORDER_OUT_BR, WALL_RIGHT, WALL_RIGHT, WALL_RIGHT,
            WALL_RIGHT, WALL_RIGHT, BORDER_IN_TL, WALL_TOP, EMPTY, WALL_BOT,
            WALL_TOP, EMPTY, WALL_BOT, WALL_TOP, EMPTY, BORDER_OUT_BR,
            WALL_RIGHT, WALL_RIGHT, WALL_RIGHT, WALL_RIGHT, WALL_RIGHT,
            BORDER_IN_TL, WALL_TOP, EMPTY, WALL_BOT
        },
        { // colums 10
            WALL_TOP, EMPTY, BORDER_OUT_BL, WALL_LEFT, WALL_LEFT, WALL_LEFT,
            WALL_LEFT, WALL_LEFT,WALL_LEFT,  BORDER_OUT_TL, EMPTY, WALL_BOT,
            WALL_TOP, EMPTY, BORDER_OUT_BL, BORDER_OUT_TL, EMPTY, BORDER_OUT_BL,
            WALL_LEFT, WALL_LEFT, WALL_LEFT, WALL_LEFT, WALL_LEFT, WALL_LEFT,
            BORDER_OUT_TL, EMPTY, WALL_BOT
        },
        { // colums 11
            WALL_TOP, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
            EMPTY, EMPTY, WALL_BOT, WALL_TOP, EMPTY, EMPTY, EMPTY, EMPTY,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
            WALL_BOT
        },
        { // colums 12
            WALL_TOP, EMPTY, BORDER_OUT_BR, BORDER_OUT_TR, EMPTY, BORDER_OUT_BR,
            WALL_RIGHT, WALL_RIGHT, WALL_RIGHT, BORDER_OUT_TR, EMPTY, WALL_BOT,
            WALL_TOP, EMPTY, BORDER_OUT_BR, WALL_RIGHT, WALL_RIGHT, WALL_RIGHT,
            BORDER_OUT_TR, EMPTY, BORDER_OUT_BR, WALL_RIGHT, WALL_RIGHT, WALL_RIGHT,
            BORDER_OUT_TR, EMPTY, WALL_BOT
        },
        { // colums 13
            WALL_TOP, EMPTY, WALL_BOT, WALL_TOP, EMPTY, BORDER_OUT_BL,
            WALL_LEFT, WALL_LEFT, WALL_LEFT, BORDER_OUT_TL, EMPTY, BORDER_OUT_BL,
            BORDER_OUT_TL, EMPTY, WALL_BOT, BORDER_IN_BR, WALL_LEFT, BORDER_IN_TR,
            WALL_TOP, EMPTY, WALL_BOT, BORDER_IN_BR, WALL_LEFT, BORDER_IN_TR,
            WALL_TOP, EMPTY, WALL_BOT
        },
        { // colums 14
            WALL_TOP, EMPTY, WALL_BOT, WALL_TOP, EMPTY, EMPTY, EMPTY, EMPTY,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL_BOT, WALL_TOP,
            EMPTY, WALL_BOT, WALL_TOP, EMPTY, WALL_BOT, WALL_TOP,
            EMPTY, WALL_BOT, WALL_TOP, EMPTY, WALL_BOT
        },
        { // colums 15 = 13
            WALL_TOP, EMPTY, WALL_BOT, WALL_TOP, EMPTY, BORDER_OUT_BR,
            WALL_RIGHT, WALL_RIGHT, WALL_RIGHT, BORDER_OUT_TR, EMPTY, BORDER_OUT_BR,
            BORDER_OUT_TR, EMPTY, WALL_BOT, BORDER_IN_BL, WALL_RIGHT, BORDER_IN_TL,
            WALL_TOP, EMPTY, WALL_BOT, BORDER_IN_BL, WALL_RIGHT, BORDER_IN_TL,
            WALL_TOP, EMPTY, WALL_BOT
        },
        { // colums 16 = 12
            WALL_TOP, EMPTY, BORDER_OUT_BL, BORDER_OUT_TL, EMPTY, BORDER_OUT_BL,
            WALL_LEFT, WALL_LEFT, WALL_LEFT, BORDER_OUT_TL, EMPTY, WALL_BOT,
            WALL_TOP, EMPTY, BORDER_OUT_BL, WALL_LEFT, WALL_LEFT, WALL_LEFT,
            BORDER_OUT_TL, EMPTY, BORDER_OUT_BL, WALL_LEFT, WALL_LEFT, WALL_LEFT,
            BORDER_OUT_TL, EMPTY, WALL_BOT
        },
        { // colums 17 = 11
            WALL_TOP, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
            EMPTY, EMPTY, WALL_BOT, WALL_TOP, EMPTY, EMPTY, EMPTY, EMPTY,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
            WALL_BOT
        },
        { // colums 18 = 10
            WALL_TOP, EMPTY, BORDER_OUT_BR, WALL_RIGHT, WALL_RIGHT, WALL_RIGHT,
            WALL_RIGHT, WALL_RIGHT, WALL_RIGHT, BORDER_OUT_TR, EMPTY, WALL_BOT,
            WALL_TOP, EMPTY, BORDER_OUT_BR, BORDER_OUT_TR, EMPTY, BORDER_OUT_BR,
            WALL_RIGHT, WALL_RIGHT, WALL_RIGHT, WALL_RIGHT, WALL_RIGHT, WALL_RIGHT,
            BORDER_OUT_TR, EMPTY, WALL_BOT
        },
        { // colums 19 = 9
            WALL_TOP, EMPTY, BORDER_OUT_BL, WALL_LEFT, WALL_LEFT, WALL_LEFT,
            WALL_LEFT, WALL_LEFT, BORDER_IN_TR, WALL_TOP, EMPTY, WALL_BOT,
            WALL_TOP, EMPTY, WALL_BOT, WALL_TOP, EMPTY, BORDER_OUT_BL,
            WALL_LEFT, WALL_LEFT, WALL_LEFT, WALL_LEFT, WALL_LEFT,
            BORDER_IN_TR, WALL_TOP, EMPTY, WALL_BOT
        },
        { // colums 20 = 8
            WALL_TOP, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
            WALL_BOT, WALL_TOP, EMPTY, WALL_BOT, WALL_TOP, EMPTY, WALL_BOT,
            WALL_TOP, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
            WALL_BOT, WALL_TOP, EMPTY, WALL_BOT
        },
        { // colums 21 = 7
            WALL_TOP, EMPTY, BORDER_OUT_BR, WALL_RIGHT, WALL_RIGHT, WALL_RIGHT,
            BORDER_OUT_TR, EMPTY, WALL_BOT, WALL_TOP, EMPTY, WALL_BOT, WALL_TOP,
            EMPTY, WALL_BOT, WALL_TOP, EMPTY, BORDER_OUT_BR, WALL_RIGHT,
            WALL_RIGHT, WALL_RIGHT, BORDER_OUT_TR, EMPTY, WALL_BOT, WALL_TOP,
            EMPTY, WALL_BOT
        },
        { // colums 22 = 6
            WALL_TOP, EMPTY, WALL_BOT, BORDER_IN_BR, WALL_LEFT, BORDER_IN_TR,
            WALL_TOP, EMPTY, WALL_BOT, WALL_TOP, EMPTY, BORDER_OUT_BL,
            BORDER_OUT_TL, EMPTY, WALL_BOT, WALL_TOP, EMPTY, WALL_BOT,
            BORDER_IN_BR, WALL_LEFT, BORDER_IN_TR, WALL_TOP,
            EMPTY, WALL_BOT, WALL_TOP, EMPTY, WALL_BOT
        },
        { // colums 23 = 5
            WALL_TOP, EMPTY, WALL_BOT, WALL_TOP, EMPTY, WALL_BOT, WALL_TOP,
            EMPTY, WALL_BOT, WALL_TOP, EMPTY, EMPTY, EMPTY, EMPTY, WALL_BOT,
            WALL_TOP, EMPTY, WALL_BOT, WALL_TOP, EMPTY, WALL_BOT, WALL_TOP,
            EMPTY, WALL_BOT, WALL_TOP, EMPTY, WALL_BOT
        },
        { // colums 24 = 4
            WALL_TOP, EMPTY, WALL_BOT, BORDER_IN_BL, WALL_RIGHT, BORDER_IN_TL,
            WALL_TOP, EMPTY, WALL_BOT, BORDER_IN_BL,
            WALL_RIGHT, WALL_RIGHT, BORDER_OUT_TR, EMPTY, WALL_BOT, WALL_TOP,
            EMPTY, WALL_BOT, BORDER_IN_BL, WALL_RIGHT, BORDER_IN_TL, WALL_TOP,
            EMPTY, WALL_BOT, WALL_TOP, EMPTY, WALL_BOT
        },
        { // colums 25 = 3
            WALL_TOP, EMPTY, BORDER_OUT_BL, WALL_LEFT, WALL_LEFT, WALL_LEFT,
            BORDER_OUT_TL, EMPTY, BORDER_OUT_BL, WALL_LEFT, WALL_LEFT, WALL_LEFT,
            BORDER_OUT_TL, EMPTY, BORDER_OUT_BL, BORDER_OUT_TL, EMPTY, BORDER_OUT_BL,
            WALL_LEFT, WALL_LEFT, WALL_LEFT, BORDER_OUT_TL, EMPTY, BORDER_OUT_BL,
            BORDER_OUT_TL, EMPTY, WALL_BOT,
        },
        { // colums 26 = 2
            WALL_TOP, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
            EMPTY, EMPTY, EMPTY, EMPTY, WALL_BOT
        },
        { // colums 27 = 1
            BORDER_IN_BL, WALL_RIGHT, WALL_RIGHT, WALL_RIGHT, WALL_RIGHT, WALL_RIGHT, WALL_RIGHT, WALL_RIGHT,
            WALL_RIGHT, WALL_RIGHT, WALL_RIGHT, WALL_RIGHT, WALL_RIGHT, WALL_RIGHT, WALL_RIGHT, WALL_RIGHT,
            WALL_RIGHT, WALL_RIGHT, WALL_RIGHT, WALL_RIGHT, WALL_RIGHT, WALL_RIGHT,
            WALL_RIGHT, WALL_RIGHT, WALL_RIGHT, WALL_RIGHT, BORDER_IN_TL
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
