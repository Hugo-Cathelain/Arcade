///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "backends/SDL2/SDL2Module.hpp"
#include "Arcade/core/API.hpp"
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc
///////////////////////////////////////////////////////////////////////////////
namespace Arc
{

///////////////////////////////////////////////////////////////////////////////
SDL2Module::SDL2Module(void) : mRatio(1.f)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        return;

    mWindow = SDL_CreateWindow("Arcade - SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 600, SDL_WINDOW_SHOWN);
    if (!mWindow) return;

    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!mRenderer){
        SDL_DestroyWindow(mWindow);
        return;
    }
}

///////////////////////////////////////////////////////////////////////////////
SDL2Module::~SDL2Module(void)
{
    if (mRenderer)
    SDL_DestroyRenderer(mRenderer);
    if (mWindow)
        SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

///////////////////////////////////////////////////////////////////////////////
EKeyboardKey SDL2Module::GetKey(SDL_Keycode key)
{
    switch (key) {
        case SDLK_a:           return (EKeyboardKey::A);
        case SDLK_b:           return (EKeyboardKey::B);
        case SDLK_c:           return (EKeyboardKey::C);
        case SDLK_d:           return (EKeyboardKey::D);
        case SDLK_e:           return (EKeyboardKey::E);
        case SDLK_f:           return (EKeyboardKey::F);
        case SDLK_g:           return (EKeyboardKey::G);
        case SDLK_h:           return (EKeyboardKey::H);
        case SDLK_i:           return (EKeyboardKey::I);
        case SDLK_j:           return (EKeyboardKey::J);
        case SDLK_k:           return (EKeyboardKey::K);
        case SDLK_l:           return (EKeyboardKey::L);
        case SDLK_m:           return (EKeyboardKey::M);
        case SDLK_n:           return (EKeyboardKey::N);
        case SDLK_o:           return (EKeyboardKey::O);
        case SDLK_p:           return (EKeyboardKey::P);
        case SDLK_q:           return (EKeyboardKey::Q);
        case SDLK_r:           return (EKeyboardKey::R);
        case SDLK_s:           return (EKeyboardKey::S);
        case SDLK_t:           return (EKeyboardKey::T);
        case SDLK_u:           return (EKeyboardKey::U);
        case SDLK_v:           return (EKeyboardKey::V);
        case SDLK_w:           return (EKeyboardKey::W);
        case SDLK_x:           return (EKeyboardKey::X);
        case SDLK_y:           return (EKeyboardKey::Y);
        case SDLK_z:           return (EKeyboardKey::Z);
        case SDLK_UP:          return (EKeyboardKey::UP);
        case SDLK_DOWN:        return (EKeyboardKey::DOWN);
        case SDLK_LEFT:        return (EKeyboardKey::LEFT);
        case SDLK_RIGHT:       return (EKeyboardKey::RIGHT);
        case SDLK_0:           return (EKeyboardKey::NUM0);
        case SDLK_1:           return (EKeyboardKey::NUM1);
        case SDLK_2:           return (EKeyboardKey::NUM2);
        case SDLK_3:           return (EKeyboardKey::NUM3);
        case SDLK_4:           return (EKeyboardKey::NUM4);
        case SDLK_5:           return (EKeyboardKey::NUM5);
        case SDLK_6:           return (EKeyboardKey::NUM6);
        case SDLK_7:           return (EKeyboardKey::NUM7);
        case SDLK_8:           return (EKeyboardKey::NUM8);
        case SDLK_9:           return (EKeyboardKey::NUM9);
        case SDLK_SPACE:       return (EKeyboardKey::SPACE);
        case SDLK_RETURN:      return (EKeyboardKey::ENTER);
        case SDLK_ESCAPE:      return (EKeyboardKey::ESCAPE);
        case SDLK_BACKSPACE:   return (EKeyboardKey::BACKSPACE);
        default:               return (EKeyboardKey::UNKNOWN);
    }
}

///////////////////////////////////////////////////////////////////////////////
void SDL2Module::Update(void)
{
    while (auto event = API::PollEvent(API::Event::GRAPHICS)) {
        if (auto gridSize = event->GetIf<API::Event::GridSize>()) {
            SDL_SetWindowSize(mWindow,
                static_cast<unsigned int>(gridSize->width * 8 * mRatio),
                static_cast<unsigned int>(gridSize->height * 8 * mRatio)
            );
            SDL_Rect viewport = {
                0, 0,
                static_cast<int>(gridSize->width * 8 * mRatio),
                static_cast<int>(gridSize->height * 8 * mRatio)
            };
            SDL_RenderSetViewport(mRenderer, &viewport);
            // Optionally, set the scale factor (similar to SFML's setScale)
            SDL_RenderSetScale(mRenderer, mRatio, mRatio);
            }
    }

    SDL_Event event;
    while (SDL_PollEvent(&event)){
        if (event.type == SDL_QUIT){
            SDL_DestroyRenderer(mRenderer);
            SDL_DestroyWindow(mWindow);
            SDL_Quit();
        }
        if (event.type == SDL_KEYDOWN){
            SDL_Keycode key = event.key.keysym.sym;
            API::PushEvent(
                API::Event::Channel::GAME,
                API::Event::KeyPressed{GetKey(key)}
            );
        }
    }
}
///////////////////////////////////////////////////////////////////////////////
void SDL2Module::Clear(void)
{
    SDL_RenderClear(mRenderer);
}

///////////////////////////////////////////////////////////////////////////////
void SDL2Module::Render(void)
{
    float offset = 4 * mRatio;

    while (!API::IsDrawQueueEmpty()) {
        auto draw = API::PopDraw();
        auto [asset, x, y] = draw;

        // Define source rect (part of the sprite sheet to render)
        SDL_Rect srcRect;
        srcRect.x = asset.position.x * 8;
        srcRect.y = asset.position.y * 8;
        srcRect.w = asset.size.x;
        srcRect.h = asset.size.y;

        // Define destination rect (where on the window to render)
        SDL_Rect destRect;
        destRect.x = x * 8 * mRatio + offset;
        destRect.y = y * 8 * mRatio + offset;
        destRect.w = asset.size.x * mRatio;
        destRect.h = asset.size.y * mRatio;

        // Render the sprite from sprite sheet
        SDL_RenderCopy(mRenderer, mSpriteSheet, &srcRect, &destRect);
    }

    // Present the renderer (similar to SFML's display())
    SDL_RenderPresent(mRenderer);

}

///////////////////////////////////////////////////////////////////////////////
void SDL2Module::SetTitle(const std::string& title)
{
    SDL_SetWindowTitle(mWindow, title.c_str());
}

///////////////////////////////////////////////////////////////////////////////
void SDL2Module::LoadSpriteSheet(const std::string& path)
{
    if (!mRenderer) return;

    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) return;
    SDL_Texture *texture = SDL_CreateTextureFromSurface(mRenderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) return;
    mSpriteSheet = texture;
}

} // namespace Arc

