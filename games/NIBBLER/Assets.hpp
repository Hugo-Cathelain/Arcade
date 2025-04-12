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

    EMPTY,

    TEXT_HISCORE,
    TEXT_PLAYER,
    TEXT_1,
    TEXT_2,
    TEXT_LEFT,
    TEXT_BONUS,

    GAME_NAME,
    GAME_MASCOT
};

/// auto sprite = SPRITE[BORDER]
/// srpite.position.x + OFFSETT_COLOR

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
static const std::vector<IGameModule::Asset> SPRITES = {
    IGameModule::Asset({0, 0}, "/ ", CLR_WHITE),        //<! BORDER_IN_BR
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

    IGameModule::Asset({11, 18}, "HI SCORE", CLR_RED, {40, 8}), //<! TEXT_HISCORE,
    IGameModule::Asset({16, 20}, "PLAYER", CLR_YELLOW, {24, 8}), //<! TEXT_PLAYER,
    IGameModule::Asset({19, 20}, "1", CLR_YELLOW), //<! TEXT_1,
    IGameModule::Asset({20, 20}, "2", CLR_YELLOW), //<! TEXT_2,
    IGameModule::Asset({21, 20}, "LEFT", CLR_WHITE, {24, 8}), //<! TEXT_LEFT,
    IGameModule::Asset({24, 20}, "BONUS", CLR_WHITE, {24, 8}), //<! TEXT_BONUS,
    IGameModule::Asset({0, 27}, "NIBBLER", CLR_WHITE, {216, 56}), //<! GAME_NAME,
    IGameModule::Asset({27, 21}, "()", CLR_RED, {104, 104}) //<! GAME_MASCOT
};

} // namespace Arc
