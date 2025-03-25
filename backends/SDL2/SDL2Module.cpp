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
SDL2Module::SDL2Module(void)
    : mRatio(4.f)
    , mInterpolationFactor(0.f)
    , mLastFrameTime(0)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Cannot init SDL2" << std::endl;
        return;
    }

    mWindow = SDL_CreateWindow(
        "Arcade - SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        600, 600, SDL_WINDOW_SHOWN
    );

    if (!mWindow) {
        std::cerr << "No window created" << std::endl;
        return;
    }

    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);

    if (!mRenderer){
        std::cerr << "No renderer created" << std::endl;
        SDL_DestroyWindow(mWindow);
        return;
    }
}

///////////////////////////////////////////////////////////////////////////////
SDL2Module::~SDL2Module(void)
{
    if (mRenderer) {
        SDL_DestroyRenderer(mRenderer);
    }
    if (mWindow) {
        SDL_DestroyWindow(mWindow);
    }
    SDL_Quit();
}

///////////////////////////////////////////////////////////////////////////////
EKeyboardKey SDL2Module::GetKey(SDL_Keycode key)
{
    switch (key) {
        case SDLK_a:            return (EKeyboardKey::A);
        case SDLK_b:            return (EKeyboardKey::B);
        case SDLK_c:            return (EKeyboardKey::C);
        case SDLK_d:            return (EKeyboardKey::D);
        case SDLK_e:            return (EKeyboardKey::E);
        case SDLK_f:            return (EKeyboardKey::F);
        case SDLK_g:            return (EKeyboardKey::G);
        case SDLK_h:            return (EKeyboardKey::H);
        case SDLK_i:            return (EKeyboardKey::I);
        case SDLK_j:            return (EKeyboardKey::J);
        case SDLK_k:            return (EKeyboardKey::K);
        case SDLK_l:            return (EKeyboardKey::L);
        case SDLK_m:            return (EKeyboardKey::M);
        case SDLK_n:            return (EKeyboardKey::N);
        case SDLK_o:            return (EKeyboardKey::O);
        case SDLK_p:            return (EKeyboardKey::P);
        case SDLK_q:            return (EKeyboardKey::Q);
        case SDLK_r:            return (EKeyboardKey::R);
        case SDLK_s:            return (EKeyboardKey::S);
        case SDLK_t:            return (EKeyboardKey::T);
        case SDLK_u:            return (EKeyboardKey::U);
        case SDLK_v:            return (EKeyboardKey::V);
        case SDLK_w:            return (EKeyboardKey::W);
        case SDLK_x:            return (EKeyboardKey::X);
        case SDLK_y:            return (EKeyboardKey::Y);
        case SDLK_z:            return (EKeyboardKey::Z);
        case SDLK_UP:           return (EKeyboardKey::UP);
        case SDLK_DOWN:         return (EKeyboardKey::DOWN);
        case SDLK_LEFT:         return (EKeyboardKey::LEFT);
        case SDLK_RIGHT:        return (EKeyboardKey::RIGHT);
        case SDLK_0:            return (EKeyboardKey::NUM0);
        case SDLK_1:            return (EKeyboardKey::NUM1);
        case SDLK_2:            return (EKeyboardKey::NUM2);
        case SDLK_3:            return (EKeyboardKey::NUM3);
        case SDLK_4:            return (EKeyboardKey::NUM4);
        case SDLK_5:            return (EKeyboardKey::NUM5);
        case SDLK_6:            return (EKeyboardKey::NUM6);
        case SDLK_7:            return (EKeyboardKey::NUM7);
        case SDLK_8:            return (EKeyboardKey::NUM8);
        case SDLK_9:            return (EKeyboardKey::NUM9);
        case SDLK_SPACE:        return (EKeyboardKey::SPACE);
        case SDLK_RETURN:       return (EKeyboardKey::ENTER);
        case SDLK_ESCAPE:       return (EKeyboardKey::ESCAPE);
        case SDLK_BACKSPACE:    return (EKeyboardKey::BACKSPACE);
        default:                return (EKeyboardKey::UNKNOWN);
    }
}

///////////////////////////////////////////////////////////////////////////////
EMouseButton SDL2Module::GetMousePress(Uint8 click)
{
    switch (click) {
        case SDL_BUTTON_LEFT:       return (EMouseButton::LEFT);
        case SDL_BUTTON_RIGHT:      return (EMouseButton::RIGHT);
        case SDL_BUTTON_MIDDLE:     return (EMouseButton::MIDDLE);
        default:                    return (EMouseButton::LEFT);
    }
}

///////////////////////////////////////////////////////////////////////////////
void SDL2Module::Update(void)
{
    while (auto event = API::PollEvent(API::Event::GRAPHICS)) {
        if (auto gridSize = event->GetIf<API::Event::GridSize>()) {
            SDL_SetWindowSize(mWindow,
                static_cast<unsigned int>(
                    gridSize->width * GRID_TILE_SIZE * mRatio),
                static_cast<unsigned int>(
                    gridSize->height * GRID_TILE_SIZE * mRatio)
            );
            SDL_RenderSetScale(mRenderer, mRatio, mRatio);
        }
    }

    SDL_Event event;
    while (SDL_PollEvent(&event)){
        if (event.type == SDL_QUIT) {
            API::PushEvent(API::Event::Channel::CORE, API::Event::Closed());
        }
        if (event.type == SDL_KEYDOWN){
            SDL_Keycode key = event.key.keysym.sym;
            API::PushEvent(
                API::Event::Channel::CORE,
                API::Event::KeyPressed{GetKey(key)}
            );
        }
        if (event.type == SDL_MOUSEBUTTONDOWN){
            Uint8 click = event.button.button;
            int gridX = event.button.x / (GRID_TILE_SIZE * mRatio);
            int gridY = event.button.y / (GRID_TILE_SIZE * mRatio);
            API::PushEvent(
                API::Event::Channel::GAME,
                API::Event::MousePressed{GetMousePress(click), gridX, gridY}
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
    Uint32 currentTime = SDL_GetTicks();
    Uint32 deltaTime = mLastFrameTime ? (currentTime - mLastFrameTime) : 0;
    mLastFrameTime = currentTime;

    float deltaSeconds = deltaTime / 1000.0f;
    float speed = 10.0f;

    for (auto& [id, interp] : mSpritePositions) {
        interp.factor += deltaSeconds * speed;
        if (interp.factor > 1.0f) {
            interp.factor = 1.0f;
            interp.current = interp.target;
        }
    }

    while (!API::IsDrawQueueEmpty()) {
        auto draw = API::PopDraw();
        auto [asset, pos, color] = draw;
        int entityId = asset.id;

        SDL_FPoint targetPos = {
            pos.x * GRID_TILE_SIZE - (asset.size.x - GRID_TILE_SIZE) / 2.0f,
            pos.y * GRID_TILE_SIZE - (asset.size.y - GRID_TILE_SIZE) / 2.0f
        };

        if (
            entityId == -1 ||
            mSpritePositions.find(entityId) == mSpritePositions.end()
        ) {
            mSpritePositions[entityId] = {targetPos, targetPos, 1.0f};
        } else {
            if (mSpritePositions[entityId].target.x != targetPos.x ||
                mSpritePositions[entityId].target.y != targetPos.y) {
                mSpritePositions[entityId].target = targetPos;
                mSpritePositions[entityId].factor = 0.0f;
            }
        }

        SDL_FPoint currentPos = mSpritePositions[entityId].current;
        SDL_FPoint interpolatedPos = {
            currentPos.x + (targetPos.x - currentPos.x) * mSpritePositions[entityId].factor,
            currentPos.y + (targetPos.y - currentPos.y) * mSpritePositions[entityId].factor
        };

        SDL_Rect srcRect;
        srcRect.x = asset.position.x * GRID_TILE_SIZE;
        srcRect.y = asset.position.y * GRID_TILE_SIZE;
        srcRect.w = asset.size.x;
        srcRect.h = asset.size.y;

        SDL_Rect destRect;
        destRect.x = static_cast<int>(interpolatedPos.x);
        destRect.y = static_cast<int>(interpolatedPos.y);
        destRect.w = asset.size.x;
        destRect.h = asset.size.y;

        SDL_RenderCopy(mRenderer, mSpriteSheet, &srcRect, &destRect);
    }

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
    if (!mRenderer) {
        return;
    }

    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(mRenderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        return;
    }
    mSpriteSheet = texture;
}

} // namespace Arc
