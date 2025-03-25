///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "games/PACMAN/Game.hpp"
#include "games/PACMAN/Assets.hpp"
#include "games/PACMAN/Menu.hpp"
#include "../../Arcade/interfaces/IGameModule.hpp"
#include "../../Arcade/core/API.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc::Pacman
///////////////////////////////////////////////////////////////////////////////
namespace Arc::Pacman
{

///////////////////////////////////////////////////////////////////////////////
Game::Game(void)
    : mTimer(0.f)
    , mPlaying(false)
{}

///////////////////////////////////////////////////////////////////////////////
Game::~Game()
{}

///////////////////////////////////////////////////////////////////////////////
void Game::SetDefaultGums(void)
{
    mGums.clear();

    // Small gums
    for (int y = 0; y < ARCADE_GAME_HEIGHT; y++) {
        for (int x = 0; x < ARCADE_GAME_WIDTH; x++) {
            if (y >= 9 && y <= 19 && x != 6 && x != 21) {
                continue;
            }

            if (PACMAN_MAP[y][x] == TILE_EMPTY) {
                mGums[y * ARCADE_GAME_WIDTH + x] = GumType::SMALL;
            }
        }
    }

    // Big gums
    mGums[3 * ARCADE_GAME_WIDTH + 1] = GumType::BIG;
    mGums[3 * ARCADE_GAME_WIDTH + 26] = GumType::BIG;
    mGums[23 * ARCADE_GAME_WIDTH + 1] = GumType::BIG;
    mGums[23 * ARCADE_GAME_WIDTH + 26] = GumType::BIG;
}

///////////////////////////////////////////////////////////////////////////////
void Game::DrawMapBaseLayer(void)
{
    for (int y = 0; y < ARCADE_GAME_HEIGHT; y++) {
        for (int x = 0; x < ARCADE_GAME_WIDTH; x++) {
            API::Draw(SPRITES[PACMAN_MAP[y][x]], {x, y + ARCADE_OFFSET_Y});
        }
    }

    if (!mPlaying) {
        Menu::Text(
            "READY!", Menu::TextColor::TEXT_YELLOW,
            Vec2i{11, 17 + ARCADE_OFFSET_Y}
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
void Game::DrawScore(void)
{
    Menu::Text("1UP   HIGH SCORE   2UP",
        Menu::TextColor::TEXT_WHITE, Vec2i{3, 0});
    Menu::Text("00", Menu::TextColor::TEXT_WHITE, Vec2i{5, 1});
}

///////////////////////////////////////////////////////////////////////////////
void Game::DrawGums(void)
{
    for (const auto& [index, type] : mGums) {
        Vec2i position(
            index % ARCADE_GAME_WIDTH,
            index / ARCADE_GAME_WIDTH + ARCADE_OFFSET_Y
        );

        if (type == GumType::SMALL) {
            API::Draw(SPRITES[TILE_POINT], position);
        } else {
            API::Draw(SPRITES[TILE_PACGUM], position);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Game::BeginPlay(void)
{
    SetDefaultGums();
}

///////////////////////////////////////////////////////////////////////////////
void Game::Tick(float deltaSeconds)
{
    if (mPlaying) {
        mTimer += deltaSeconds;
    }

    // Updating

    // Drawing
    DrawMapBaseLayer();
    DrawGums();
    DrawScore();
}

///////////////////////////////////////////////////////////////////////////////
void Game::EndPlay(void)
{}

} // namespace Arc::Pacman
