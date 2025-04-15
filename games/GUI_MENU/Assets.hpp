///////////////////////////////////////////////////////////////////////////////
// Header Guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Arcade/interfaces/IGameModule.hpp"
#include <string>
#include <vector>
#include <map>

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc
///////////////////////////////////////////////////////////////////////////////
namespace Arc
{

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
#define CLR_WHITE Color{255, 255, 255}

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
#define POSTER_H 256
#define POSTER_W 196
#define POSTER {POSTER_W, POSTER_H}
#define THUMB_W 256
#define THUMB_H 128
#define THUMB {THUMB_W, THUMB_H}

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
enum SpriteType
{
    GRAPH_UNKNOWN,
    GRAPH_NCURSES,
    GRAPH_VULKAN,
    GRAPH_XLIB,
    GRAPH_SDL2,
    GRAPH_ALLEGRO,
    GRAPH_OPENGL,
    GRAPH_SFML,
    GAME_UNKNOWN,
    GAME_TRON,
    GAME_NIBBLER,
    GAME_PACMAN,
    GAME_CENTIPEDE,
    GAME_SOLARFOX,
    GAME_MINESWEEPER,
    GAME_GALAGA,
    GAME_SPACE_INVADERS,
    GAME_SABOTAGE,
    GAME_STREET_FIGHTER_2,
    GAME_MORTAL_KOMBAT
};

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
static const std::vector<IGameModule::Asset> SPRITES = {
    IGameModule::Asset({0, 0}, "??", CLR_WHITE, THUMB),    //<! GRAPH_UNKNOWN
    IGameModule::Asset({0, 16}, "??", CLR_WHITE, THUMB),   //<! GRAPH_NCURSES
    IGameModule::Asset({0, 32}, "??", CLR_WHITE, THUMB),   //<! GRAPH_VULKAN
    IGameModule::Asset({0, 48}, "??", CLR_WHITE, THUMB),   //<! GRAPH_XLIB
    IGameModule::Asset({0, 64}, "??", CLR_WHITE, THUMB),   //<! GRAPH_SDL2
    IGameModule::Asset({0, 80}, "??", CLR_WHITE, THUMB),   //<! GRAPH_ALLEGRO
    IGameModule::Asset({0, 96}, "??", CLR_WHITE, THUMB),   //<! GRAPH_OPENGL
    IGameModule::Asset({0, 112}, "??", CLR_WHITE, THUMB),  //<! GRAPH_SFML
    IGameModule::Asset({32, 0}, "??", CLR_WHITE, POSTER),  //<! GAME_UNKNOWN
    IGameModule::Asset({32, 32}, "??", CLR_WHITE, POSTER), //<! GAME_TRON
    IGameModule::Asset({32, 64}, "??", CLR_WHITE, POSTER), //<! GAME_NIBBLER
    IGameModule::Asset({32, 96}, "??", CLR_WHITE, POSTER), //<! GAME_PACMAN
    IGameModule::Asset({57, 0}, "??", CLR_WHITE, POSTER),  //<! GAME_CENTIPEDE
    IGameModule::Asset({57, 32}, "??", CLR_WHITE, POSTER), //<! GAME_SOLARFOX
    IGameModule::Asset({57, 64}, "??", CLR_WHITE, POSTER), //<! GAME_MINESWEEPER
    IGameModule::Asset({57, 96}, "??", CLR_WHITE, POSTER), //<! GAME_GALAGA
    IGameModule::Asset({82, 0}, "??", CLR_WHITE, POSTER),  //<! GAME_SPACE_INVADERS
    IGameModule::Asset({82, 32}, "??", CLR_WHITE, POSTER), //<! GAME_SABOTAGE
    IGameModule::Asset({82, 64}, "??", CLR_WHITE, POSTER), //<! GAME_STREET_FIGHTER_2
    IGameModule::Asset({82, 96}, "??", CLR_WHITE, POSTER)  //<! GAME_MORTAL_KOMBAT
};

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
static const std::map<std::string, SpriteType> SPRITE_MAP = {
    {"NCURSES", GRAPH_NCURSES},
    {"VULKAN", GRAPH_VULKAN},
    {"XLIB", GRAPH_XLIB},
    {"SDL2", GRAPH_SDL2},
    {"ALLEGRO", GRAPH_ALLEGRO},
    {"OPENGL", GRAPH_OPENGL},
    {"SFML", GRAPH_SFML},
    {"TRON", GAME_TRON},
    {"NIBBLER", GAME_NIBBLER},
    {"PACMAN", GAME_PACMAN},
    {"CENTIPEDE", GAME_CENTIPEDE},
    {"SOLARFOX", GAME_SOLARFOX},
    {"MINESWEEPER", GAME_MINESWEEPER},
    {"GALAGA", GAME_GALAGA},
    {"SPACE_INVADERS", GAME_SPACE_INVADERS},
    {"SABOTAGE", GAME_SABOTAGE},
    {"STREET_FIGHTER_2", GAME_STREET_FIGHTER_2},
    {"MORTAL_KOMBAT", GAME_MORTAL_KOMBAT}
};

} // namespace Arc
