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
#define CLR_RED    Color{255, 0, 0}
#define CLR_GREEN  Color{0, 255, 0}
#define CLR_BLACK  Color{0, 0, 0}

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
enum SpriteType {
    SNAKE_HEAD_T,
    SNAKE_HEAD_L,
    SNAKE_HEAD_R,
    SNAKE_HEAD_B,
    SNAKE_TAIL_B,
    SNAKE_TAIL_R,
    SNAKE_TAIL_T,
    SNAKE_TAIL_L,
    SNAKE_BODY_TB,
    SNAKE_BODY_TR,
    SNAKE_BODY_TL,
    SNAKE_BODY_RL,
    SNAKE_BODY_BR,
    SNAKE_BODY_BL,
    APPLE,
    EMPTY
};

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
static const std::vector<IGameModule::Asset> SPRITES = {
    IGameModule::Asset({0, 0}, "()", CLR_GREEN, {16, 16}),   //<! SNAKE_HEAD_T
    IGameModule::Asset({2, 0}, "()", CLR_GREEN, {16, 16}),   //<! SNAKE_HEAD_L
    IGameModule::Asset({4, 0}, "()", CLR_GREEN, {16, 16}),   //<! SNAKE_HEAD_R
    IGameModule::Asset({6, 0}, "()", CLR_GREEN, {16, 16}),   //<! SNAKE_HEAD_B
    IGameModule::Asset({0, 2}, "\\/", CLR_GREEN, {16, 16}),  //<! SNAKE_TAIL_B
    IGameModule::Asset({2, 2}, "-|", CLR_GREEN, {16, 16}),   //<! SNAKE_TAIL_R
    IGameModule::Asset({4, 2}, "/\\", CLR_GREEN, {16, 16}),  //<! SNAKE_TAIL_T
    IGameModule::Asset({6, 2}, "|-", CLR_GREEN, {16, 16}),   //<! SNAKE_TAIL_L
    IGameModule::Asset({0, 4}, "||", CLR_GREEN, {16, 16}),   //<! SNAKE_BODY_TB
    IGameModule::Asset({2, 4}, "\\\\", CLR_GREEN, {16, 16}), //<! SNAKE_BODY_TR
    IGameModule::Asset({4, 4}, "//", CLR_GREEN, {16, 16}),   //<! SNAKE_BODY_TL
    IGameModule::Asset({6, 4}, "==", CLR_GREEN, {16, 16}),   //<! SNAKE_BODY_RL
    IGameModule::Asset({0, 6}, "//", CLR_GREEN, {16, 16}),   //<! SNAKE_BODY_BR
    IGameModule::Asset({2, 6}, "\\\\", CLR_GREEN, {16, 16}), //<! SNAKE_BODY_BL
    IGameModule::Asset({4, 6}, "[]", CLR_RED, {16, 16}),     //<! APPLE
    IGameModule::Asset({6, 6}, "  ", CLR_BLACK, {16, 16})    //<! EMPTY
};
} // namespace Arc
