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
#define CLR_BLUE   Color{32, 32, 246}
#define CLR_YELLOW Color{255, 255, 0}
#define CLR_RED    Color{255, 0, 0}
#define CLR_PINK   Color{255, 183, 255}
#define CLR_CYAN   Color{1, 255, 255}
#define CLR_ORANGE Color{255, 182, 80}
#define CLR_PALE   Color{255, 183, 174}
#define CLR_WHITE  Color{255, 255, 255}

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
enum SpriteType {
    PACMAN,
    RED_R1,
    RED_R2,
    RED_L1,
    RED_L2,
    RED_U1,
    RED_U2,
    RED_D1,
    RED_D2,
    PINK_R1,
    PINK_R2,
    PINK_L1,
    PINK_L2,
    PINK_U1,
    PINK_U2,
    PINK_D1,
    PINK_D2,
    CYAN_R1,
    CYAN_R2,
    CYAN_L1,
    CYAN_L2,
    CYAN_U1,
    CYAN_U2,
    CYAN_D1,
    CYAN_D2,
    ORANGE_R1,
    ORANGE_R2,
    ORANGE_L1,
    ORANGE_L2,
    ORANGE_U1,
    ORANGE_U2,
    ORANGE_D1,
    ORANGE_D2,
    SCARED_1,
    SCARED_2,
    DOOR_LEFT,
    DOOR_MID,
    DOOR_RIGHT,
    TURN_QTR,
    TURN_QTL,
    TURN_QBL,
    TURN_QBR,
    WALL_QT,
    WALL_QB,
    WALL_QR,
    WALL_QL,
    TILE_EMPTY,
    TILE_NOTHING,
    TILE_PACGUM,
    TILE_POINT,
    TURN_BR2,
    TURN_LB2,
    TURN_TR2,
    TURN_LT2,
    TURN_TLB2,
    TURN_TBR2,
    TURN_LTR2,
    TURN_LBR2,
    TURN_RTL2,
    TURN_RLB2,
    WALL_L2,
    WALL_T2,
    WALL_B2,
    WALL_R2,
    TURN_SBR1,
    TURN_SLB1,
    TURN_STR1,
    TURN_SLT1,
    TURN_XBR1,
    TURN_XLB1,
    TURN_XTR1,
    TURN_XLT1,
    WALL_L1,
    WALL_T1,
    WALL_B1,
    WALL_R1,
    WHITE_PTS,
    NAMCO_LOGO,
    SCORE_100,
    SCORE_200,
    SCORE_300,
    SCORE_400,
    SCORE_500,
    SCORE_700,
    SCORE_800,
    SCORE_1000,
    SCORE_1600,
    SCORE_2000,
    SCORE_3000,
    SCORE_5000
};

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
static const std::vector<IGameModule::Asset> SPRITES = {
    IGameModule::Asset({0, 4}, "()", CLR_YELLOW, {16, 16}),      //<! PACMAN
    IGameModule::Asset({12, 0}, "/\\", CLR_RED, {16, 16}, 2),    //<! RED_R1
    IGameModule::Asset({14, 0}, "/\\", CLR_RED, {16, 16}, 2),    //<! RED_R2
    IGameModule::Asset({16, 0}, "/\\", CLR_RED, {16, 16}, 2),    //<! RED_L1
    IGameModule::Asset({18, 0}, "/\\", CLR_RED, {16, 16}, 2),    //<! RED_L2
    IGameModule::Asset({20, 0}, "/\\", CLR_RED, {16, 16}, 2),    //<! RED_U1
    IGameModule::Asset({22, 0}, "/\\", CLR_RED, {16, 16}, 2),    //<! RED_U2
    IGameModule::Asset({24, 0}, "/\\", CLR_RED, {16, 16}, 2),    //<! RED_D1
    IGameModule::Asset({26, 0}, "/\\", CLR_RED, {16, 16}, 2),    //<! RED_D2
    IGameModule::Asset({12, 2}, "/\\", CLR_PINK, {16, 16}, 3),   //<! PINK_R1
    IGameModule::Asset({14, 2}, "/\\", CLR_PINK, {16, 16}, 3),   //<! PINK_R2
    IGameModule::Asset({16, 2}, "/\\", CLR_PINK, {16, 16}, 3),   //<! PINK_L1
    IGameModule::Asset({18, 2}, "/\\", CLR_PINK, {16, 16}, 3),   //<! PINK_L2
    IGameModule::Asset({20, 2}, "/\\", CLR_PINK, {16, 16}, 3),   //<! PINK_U1
    IGameModule::Asset({22, 2}, "/\\", CLR_PINK, {16, 16}, 3),   //<! PINK_U2
    IGameModule::Asset({24, 2}, "/\\", CLR_PINK, {16, 16}, 3),   //<! PINK_D1
    IGameModule::Asset({26, 2}, "/\\", CLR_PINK, {16, 16}, 3),   //<! PINK_D2
    IGameModule::Asset({12, 4}, "/\\", CLR_CYAN, {16, 16}, 4),   //<! CYAN_R1
    IGameModule::Asset({14, 4}, "/\\", CLR_CYAN, {16, 16}, 4),   //<! CYAN_R2
    IGameModule::Asset({16, 4}, "/\\", CLR_CYAN, {16, 16}, 4),   //<! CYAN_L1
    IGameModule::Asset({18, 4}, "/\\", CLR_CYAN, {16, 16}, 4),   //<! CYAN_L2
    IGameModule::Asset({20, 4}, "/\\", CLR_CYAN, {16, 16}, 4),   //<! CYAN_U1
    IGameModule::Asset({22, 4}, "/\\", CLR_CYAN, {16, 16}, 4),   //<! CYAN_U2
    IGameModule::Asset({24, 4}, "/\\", CLR_CYAN, {16, 16}, 4),   //<! CYAN_D1
    IGameModule::Asset({26, 4}, "/\\", CLR_CYAN, {16, 16}, 4),   //<! CYAN_D2
    IGameModule::Asset({12, 6}, "/\\", CLR_ORANGE, {16, 16}, 5), //<! ORANGE_R1
    IGameModule::Asset({14, 6}, "/\\", CLR_ORANGE, {16, 16}, 5), //<! ORANGE_R2
    IGameModule::Asset({16, 6}, "/\\", CLR_ORANGE, {16, 16}, 5), //<! ORANGE_L1
    IGameModule::Asset({18, 6}, "/\\", CLR_ORANGE, {16, 16}, 5), //<! ORANGE_L2
    IGameModule::Asset({20, 6}, "/\\", CLR_ORANGE, {16, 16}, 5), //<! ORANGE_U1
    IGameModule::Asset({22, 6}, "/\\", CLR_ORANGE, {16, 16}, 5), //<! ORANGE_U2
    IGameModule::Asset({24, 6}, "/\\", CLR_ORANGE, {16, 16}, 5), //<! ORANGE_D1
    IGameModule::Asset({26, 6}, "/\\", CLR_ORANGE, {16, 16}, 5), //<! ORANGE_D2
    IGameModule::Asset({12, 8}, "/\\", CLR_BLUE, {16, 16}, 6),   //<! SCARED_1
    IGameModule::Asset({14, 8}, "/\\", CLR_BLUE, {16, 16}, 6),   //<! SCARED_2
    IGameModule::Asset({2, 4}, "=]", CLR_BLUE),                  //<! DOOR_LEFT
    IGameModule::Asset({3, 4}, "--", CLR_PINK),                  //<! DOOR_MID
    IGameModule::Asset({2, 5}, "[=", CLR_BLUE),                  //<! DOOR_RIGHT
    IGameModule::Asset({5, 6}, "==", CLR_BLUE),                  //<! TURN_QTR
    IGameModule::Asset({4, 6}, "==", CLR_BLUE),                  //<! TURN_QTL
    IGameModule::Asset({4, 7}, "==", CLR_BLUE),                  //<! TURN_QBL
    IGameModule::Asset({5, 7}, "==", CLR_BLUE),                  //<! TURN_QBR
    IGameModule::Asset({3, 6}, "==", CLR_BLUE),                  //<! WALL_QT
    IGameModule::Asset({2, 7}, "==", CLR_BLUE),                  //<! WALL_QB
    IGameModule::Asset({2, 6}, "||", CLR_BLUE),                  //<! WALL_QR
    IGameModule::Asset({3, 7}, "||", CLR_BLUE),                  //<! WALL_QL
    IGameModule::Asset({16, 16}, "  "),                          //<! TILE_EMPTY
    IGameModule::Asset({3, 5}, "  "),                            //<! TILE_NOTHING
    IGameModule::Asset({1, 6}, "[]", CLR_PALE),                  //<! TILE_PACGUM
    IGameModule::Asset({1, 7}, "..", CLR_PALE),                  //<! TILE_POINT
    IGameModule::Asset({10, 8}, "//", CLR_BLUE),                 //<! TURN_BR2
    IGameModule::Asset({11, 8}, "\\\\", CLR_BLUE),               //<! TURN_LB2
    IGameModule::Asset({10, 9}, "\\\\", CLR_BLUE),               //<! TURN_TR2
    IGameModule::Asset({11, 9}, "//", CLR_BLUE),                 //<! TURN_LT2
    IGameModule::Asset({0, 6}, "\\\\", CLR_BLUE),                //<! TURN_TLB2
    IGameModule::Asset({0, 7}, "//", CLR_BLUE),                  //<! TURN_TBR2
    IGameModule::Asset({7, 8}, "|\\", CLR_BLUE),                 //<! TURN_LTR2
    IGameModule::Asset({7, 9}, "|/", CLR_BLUE),                  //<! TURN_LBR2
    IGameModule::Asset({6, 8}, "/|", CLR_BLUE),                  //<! TURN_RTL2
    IGameModule::Asset({6, 9}, "\\|", CLR_BLUE),                 //<! TURN_RLB2
    IGameModule::Asset({8, 8}, "||", CLR_BLUE),                  //<! WALL_L2
    IGameModule::Asset({9, 8}, "==", CLR_BLUE),                  //<! WALL_T2
    IGameModule::Asset({8, 9}, "==", CLR_BLUE),                  //<! WALL_B2
    IGameModule::Asset({9, 9}, "||", CLR_BLUE),                  //<! WALL_R2
    IGameModule::Asset({6, 4}, " /", CLR_BLUE),                  //<! TURN_SBR1
    IGameModule::Asset({7, 4}, "\\ ", CLR_BLUE),                 //<! TURN_SLB1
    IGameModule::Asset({6, 5}, " \\", CLR_BLUE),                 //<! TURN_STR1
    IGameModule::Asset({7, 5}, "/ ", CLR_BLUE),                  //<! TURN_SLT1
    IGameModule::Asset({6, 6}, " /", CLR_BLUE),                  //<! TURN_XBR1
    IGameModule::Asset({7, 6}, "\\ ", CLR_BLUE),                 //<! TURN_XLB1
    IGameModule::Asset({6, 7}, " \\", CLR_BLUE),                 //<! TURN_XTR1
    IGameModule::Asset({7, 7}, "/ ", CLR_BLUE),                  //<! TURN_XLT1
    IGameModule::Asset({4, 4}, " |", CLR_BLUE),                  //<! WALL_L1
    IGameModule::Asset({5, 4}, "--", CLR_BLUE),                  //<! WALL_T1
    IGameModule::Asset({4, 5}, "--", CLR_BLUE),                  //<! WALL_B1
    IGameModule::Asset({5, 5}, "| ", CLR_BLUE),                  //<! WALL_R1,
    IGameModule::Asset({13, 11}, "pts", CLR_WHITE, {24, 8}),     //<! WHITE_PTS
    IGameModule::Asset({0, 21}, "NAMCO", CLR_PINK, {56, 8}),     //<! NAMCO_LOGO
    IGameModule::Asset({17, 14}, "100", CLR_WHITE, {16, 16}),    //<! SCORE_100
    IGameModule::Asset({17, 12}, "200", CLR_WHITE, {16, 16}),    //<! SCORE_200
    IGameModule::Asset({19, 14}, "800", CLR_WHITE, {16, 16}),    //<! SCORE_300
    IGameModule::Asset({19, 12}, "400", CLR_WHITE, {16, 16}),    //<! SCORE_400
    IGameModule::Asset({21, 14}, "500", CLR_WHITE, {16, 16}),    //<! SCORE_500
    IGameModule::Asset({23, 14}, "700", CLR_WHITE, {16, 16}),    //<! SCORE_700
    IGameModule::Asset({21, 12}, "800", CLR_WHITE, {16, 16}),    //<! SCORE_800
    IGameModule::Asset({25, 14}, "1000", CLR_WHITE, {24, 16}),   //<! SCORE_1000
    IGameModule::Asset({23, 12}, "1600", CLR_WHITE, {32, 16}),   //<! SCORE_1600
    IGameModule::Asset({24, 16}, "2000", CLR_WHITE, {32, 16}),   //<! SCORE_2000
    IGameModule::Asset({24, 18}, "3000", CLR_WHITE, {32, 16}),   //<! SCORE_3000
    IGameModule::Asset({24, 20}, "5000", CLR_WHITE, {32, 16}),   //<! SCORE_5000
};

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
#define PACMAN_XY(x, y) \
    IGameModule::Asset({x, y}, "()", CLR_YELLOW, {16, 16}, -1)

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
#define PACMAN_XYI(x, y) \
    IGameModule::Asset({x, y}, "()", CLR_YELLOW, {16, 16}, 1)

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
static const SpriteType PACMAN_MAP[31][28] = {
    { // Row 1
        TURN_BR2, WALL_T2, WALL_T2, WALL_T2,
        WALL_T2, WALL_T2, WALL_T2, WALL_T2,
        WALL_T2, WALL_T2, WALL_T2, WALL_T2,
        WALL_T2, TURN_TLB2, TURN_TBR2, WALL_T2,
        WALL_T2, WALL_T2, WALL_T2, WALL_T2,
        WALL_T2, WALL_T2, WALL_T2, WALL_T2,
        WALL_T2, WALL_T2, WALL_T2, TURN_LB2
    },
    { // Row 2
        WALL_L2, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, WALL_L1, WALL_R1, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, WALL_R2
    },
    { // Row 3
        WALL_L2, TILE_EMPTY, TURN_SBR1, WALL_T1,
        WALL_T1, TURN_SLB1, TILE_EMPTY, TURN_SBR1,
        WALL_T1, WALL_T1, WALL_T1, TURN_SLB1,
        TILE_EMPTY, WALL_L1, WALL_R1, TILE_EMPTY,
        TURN_SBR1, WALL_T1, WALL_T1, WALL_T1,
        TURN_SLB1, TILE_EMPTY, TURN_SBR1, WALL_T1,
        WALL_T1, TURN_SLB1, TILE_EMPTY, WALL_R2
    },
    { // Row 4
        WALL_L2, TILE_EMPTY, WALL_L1, TILE_NOTHING,
        TILE_NOTHING, WALL_R1, TILE_EMPTY, WALL_L1,
        TILE_NOTHING, TILE_NOTHING, TILE_NOTHING, WALL_R1,
        TILE_EMPTY, WALL_L1, WALL_R1, TILE_EMPTY,
        WALL_L1, TILE_NOTHING, TILE_NOTHING, TILE_NOTHING,
        WALL_R1, TILE_EMPTY, WALL_L1, TILE_NOTHING,
        TILE_NOTHING, WALL_R1, TILE_EMPTY, WALL_R2
    },
    { // Row 5
        WALL_L2, TILE_EMPTY, TURN_STR1, WALL_B1,
        WALL_B1, TURN_SLT1, TILE_EMPTY, TURN_STR1,
        WALL_B1, WALL_B1, WALL_B1, TURN_SLT1,
        TILE_EMPTY, TURN_STR1, TURN_SLT1, TILE_EMPTY,
        TURN_STR1, WALL_B1, WALL_B1, WALL_B1,
        TURN_SLT1, TILE_EMPTY, TURN_STR1, WALL_B1,
        WALL_B1, TURN_SLT1, TILE_EMPTY, WALL_R2
    },
    { // Row 6
        WALL_L2, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, WALL_R2
    },
    { // Row 7
        WALL_L2, TILE_EMPTY, TURN_SBR1, WALL_T1,
        WALL_T1, TURN_SLB1, TILE_EMPTY, TURN_SBR1,
        TURN_SLB1, TILE_EMPTY, TURN_SBR1, WALL_T1,
        WALL_T1, WALL_T1, WALL_T1, WALL_T1,
        WALL_T1, TURN_SLB1, TILE_EMPTY, TURN_SBR1,
        TURN_SLB1, TILE_EMPTY, TURN_SBR1, WALL_T1,
        WALL_T1, TURN_SLB1, TILE_EMPTY, WALL_R2
    },
    { // Row 8
        WALL_L2, TILE_EMPTY, TURN_STR1, WALL_B1,
        WALL_B1, TURN_SLT1, TILE_EMPTY, WALL_L1,
        WALL_R1, TILE_EMPTY, TURN_STR1, WALL_B1,
        WALL_B1, TURN_XLB1, TURN_XBR1, WALL_B1,
        WALL_B1, TURN_SLT1, TILE_EMPTY, WALL_L1,
        WALL_R1, TILE_EMPTY, TURN_STR1, WALL_B1,
        WALL_B1, TURN_SLT1, TILE_EMPTY, WALL_R2
    },
    { // Row 9
        WALL_L2, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, WALL_L1,
        WALL_R1, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, WALL_L1, WALL_R1, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, WALL_L1,
        WALL_R1, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, WALL_R2
    },
    { // Row 10
        TURN_TR2, WALL_B2, WALL_B2, WALL_B2, WALL_B2,
        TURN_SLB1, TILE_EMPTY, WALL_L1, TURN_XTR1,
        WALL_T1, WALL_T1, TURN_SLB1, TILE_EMPTY,
        WALL_L1, WALL_R1, TILE_EMPTY, TURN_SBR1,
        WALL_T1, WALL_T1, TURN_XLT1, WALL_R1,
        TILE_EMPTY, TURN_SBR1, WALL_B2, WALL_B2,
        WALL_B2, WALL_B2, TURN_LT2
    },
    { // Row 11
        TILE_NOTHING, TILE_NOTHING, TILE_NOTHING, TILE_NOTHING,
        TILE_NOTHING, WALL_L2, TILE_EMPTY, WALL_L1,
        TURN_XBR1, WALL_B1, WALL_B1, TURN_SLT1,
        TILE_EMPTY, TURN_STR1, TURN_SLT1,
        TILE_EMPTY, TURN_STR1, WALL_B1, WALL_B1,
        TURN_XLB1, WALL_R1, TILE_EMPTY, WALL_R2,
        TILE_NOTHING, TILE_NOTHING, TILE_NOTHING, TILE_NOTHING,
        TILE_NOTHING
    },
    { // Row 12
        TILE_NOTHING, TILE_NOTHING, TILE_NOTHING, TILE_NOTHING,
        TILE_NOTHING, WALL_L2, TILE_EMPTY, WALL_L1,
        WALL_R1, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, WALL_L1,
        WALL_R1, TILE_EMPTY, WALL_R2, TILE_NOTHING,
        TILE_NOTHING, TILE_NOTHING, TILE_NOTHING,
        TILE_NOTHING,
    },
    { // Row 13
        TILE_NOTHING, TILE_NOTHING, TILE_NOTHING, TILE_NOTHING,
        TILE_NOTHING, WALL_L2, TILE_EMPTY, WALL_L1,
        WALL_R1, TILE_EMPTY, TURN_QTL, WALL_QT,
        DOOR_LEFT, DOOR_MID, DOOR_MID, DOOR_RIGHT,
        WALL_QT, TURN_QTR, TILE_EMPTY, WALL_L1,
        WALL_R1, TILE_EMPTY, WALL_R2, TILE_NOTHING,
        TILE_NOTHING, TILE_NOTHING, TILE_NOTHING, TILE_NOTHING
    },
    { // Row 14
        WALL_T2, WALL_T2, WALL_T2, WALL_T2, WALL_T2,
        TURN_SLT1, TILE_EMPTY, TURN_STR1, TURN_SLT1,
        TILE_EMPTY, WALL_QL, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        WALL_QR, TILE_EMPTY, TURN_STR1, TURN_SLT1,
        TILE_EMPTY, TURN_STR1, WALL_T2, WALL_T2,
        WALL_T2, WALL_T2, WALL_T2
    },
    { // Row 15
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, WALL_QL, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, WALL_QR, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY
    },
    { // Row 16
        WALL_B2, WALL_B2, WALL_B2, WALL_B2, WALL_B2,
        TURN_SLB1, TILE_EMPTY, TURN_SBR1, TURN_SLB1,
        TILE_EMPTY, WALL_QL, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        WALL_QR, TILE_EMPTY, TURN_SBR1, TURN_SLB1,
        TILE_EMPTY, TURN_SBR1, WALL_B2, WALL_B2,
        WALL_B2, WALL_B2, WALL_B2
    },
    { // Row 17
        TILE_NOTHING, TILE_NOTHING, TILE_NOTHING, TILE_NOTHING,
        TILE_NOTHING, WALL_L2, TILE_EMPTY, WALL_L1,
        WALL_R1, TILE_EMPTY, TURN_QBL, WALL_QB,
        WALL_QB, WALL_QB, WALL_QB, WALL_QB,
        WALL_QB, TURN_QBR, TILE_EMPTY, WALL_L1,
        WALL_R1, TILE_EMPTY, WALL_R2, TILE_NOTHING,
        TILE_NOTHING, TILE_NOTHING, TILE_NOTHING, TILE_NOTHING
    },
    { // Row 18
        TILE_NOTHING, TILE_NOTHING, TILE_NOTHING, TILE_NOTHING,
        TILE_NOTHING, WALL_L2, TILE_EMPTY, WALL_L1,
        WALL_R1, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, WALL_L1,
        WALL_R1, TILE_EMPTY, WALL_R2, TILE_NOTHING,
        TILE_NOTHING, TILE_NOTHING, TILE_NOTHING, TILE_NOTHING
    },
    { // Row 19
        TILE_NOTHING, TILE_NOTHING, TILE_NOTHING, TILE_NOTHING,
        TILE_NOTHING, WALL_L2, TILE_EMPTY, WALL_L1,
        WALL_R1, TILE_EMPTY, TURN_SBR1, WALL_T1,
        WALL_T1, WALL_T1, WALL_T1, WALL_T1,
        WALL_T1, TURN_SLB1, TILE_EMPTY, WALL_L1,
        WALL_R1, TILE_EMPTY, WALL_R2, TILE_NOTHING,
        TILE_NOTHING, TILE_NOTHING, TILE_NOTHING, TILE_NOTHING
    },
    { // Row 20
        TURN_BR2, WALL_T2, WALL_T2, WALL_T2,
        WALL_T2, TURN_SLT1, TILE_EMPTY, TURN_STR1,
        TURN_SLT1, TILE_EMPTY, TURN_STR1, WALL_B1,
        WALL_B1, TURN_XLB1, TURN_XBR1, WALL_B1,
        WALL_B1, TURN_SLT1, TILE_EMPTY, TURN_STR1,
        TURN_SLT1, TILE_EMPTY, TURN_STR1, WALL_T2,
        WALL_T2, WALL_T2, WALL_T2, TURN_LB2
    },
    { // Row 21
        WALL_L2, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, WALL_L1, WALL_R1, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, WALL_R2
    },
    { // Row 22
        WALL_L2, TILE_EMPTY, TURN_SBR1, WALL_T1,
        WALL_T1, TURN_SLB1, TILE_EMPTY, TURN_SBR1,
        WALL_T1, WALL_T1, WALL_T1, TURN_SLB1,
        TILE_EMPTY, WALL_L1, WALL_R1, TILE_EMPTY,
        TURN_SBR1, WALL_T1, WALL_T1, WALL_T1,
        TURN_SLB1, TILE_EMPTY, TURN_SBR1, WALL_T1,
        WALL_T1, TURN_SLB1, TILE_EMPTY, WALL_R2
    },
    { // Row 23
        WALL_L2, TILE_EMPTY, TURN_STR1, WALL_B1,
        TURN_XLB1, WALL_R1, TILE_EMPTY, TURN_STR1,
        WALL_B1, WALL_B1, WALL_B1, TURN_SLT1,
        TILE_EMPTY, TURN_STR1, TURN_SLT1, TILE_EMPTY,
        TURN_STR1, WALL_B1, WALL_B1, WALL_B1,
        TURN_SLT1, TILE_EMPTY, WALL_L1, TURN_XBR1,
        WALL_B1, TURN_SLT1, TILE_EMPTY, WALL_R2
    },
    { // Row 24
        WALL_L2, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        WALL_L1, WALL_R1, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, WALL_L1, WALL_R1,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, WALL_R2
    },
    { // Row 25
        TURN_LTR2, WALL_T1, TURN_SLB1, TILE_EMPTY,
        WALL_L1, WALL_R1, TILE_EMPTY, TURN_SBR1,
        TURN_SLB1, TILE_EMPTY, TURN_SBR1, WALL_T1,
        WALL_T1, WALL_T1, WALL_T1, WALL_T1,
        WALL_T1, TURN_SLB1, TILE_EMPTY, TURN_SBR1,
        TURN_SLB1, TILE_EMPTY, WALL_L1, WALL_R1,
        TILE_EMPTY, TURN_SBR1, WALL_T1, TURN_RTL2
    },
    { // Row 26
        TURN_LBR2, WALL_B1, TURN_SLT1, TILE_EMPTY,
        TURN_STR1, TURN_SLT1, TILE_EMPTY, WALL_L1,
        WALL_R1, TILE_EMPTY, TURN_STR1, WALL_B1,
        WALL_B1, TURN_XLB1, TURN_XBR1, WALL_B1, WALL_B1,
        TURN_SLT1, TILE_EMPTY, WALL_L1, WALL_R1,
        TILE_EMPTY, TURN_STR1, TURN_SLT1, TILE_EMPTY,
        TURN_STR1, WALL_B1, TURN_RLB2
    },
    { // Row 27
        WALL_L2, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, WALL_L1,
        WALL_R1, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, WALL_L1, WALL_R1, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, WALL_L1,
        WALL_R1, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, WALL_R2
    },
    { // Row 28
        WALL_L2, TILE_EMPTY, TURN_SBR1, WALL_T1,
        WALL_T1, WALL_T1, WALL_T1, TURN_XLT1,
        TURN_XTR1, WALL_T1, WALL_T1, TURN_SLB1,
        TILE_EMPTY, WALL_L1, WALL_R1, TILE_EMPTY,
        TURN_SBR1, WALL_T1, WALL_T1, TURN_XLT1,
        TURN_XTR1, WALL_T1, WALL_T1, WALL_T1,
        WALL_T1, TURN_SLB1, TILE_EMPTY, WALL_R2
    },
    { // Row 29
        WALL_L2, TILE_EMPTY, TURN_STR1, WALL_B1,
        WALL_B1, WALL_B1, WALL_B1, WALL_B1, WALL_B1,
        WALL_B1, WALL_B1, TURN_SLT1, TILE_EMPTY,
        TURN_STR1, TURN_SLT1, TILE_EMPTY, TURN_STR1,
        WALL_B1, WALL_B1, WALL_B1, WALL_B1,
        WALL_B1, WALL_B1, WALL_B1, WALL_B1,
        TURN_SLT1, TILE_EMPTY, WALL_R2
    },
    { // Row 30
        WALL_L2, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
        TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, WALL_R2
    },
    { // Row 31
        TURN_TR2, WALL_B2, WALL_B2, WALL_B2, WALL_B2,
        WALL_B2, WALL_B2, WALL_B2, WALL_B2, WALL_B2,
        WALL_B2, WALL_B2, WALL_B2, WALL_B2, WALL_B2,
        WALL_B2, WALL_B2, WALL_B2, WALL_B2, WALL_B2,
        WALL_B2, WALL_B2, WALL_B2, WALL_B2, WALL_B2,
        WALL_B2, WALL_B2, TURN_LT2
    }
};

} // namespace Arc
