///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "backends/LIBCACA/LIBCACAModule.hpp"
#include "Arcade/core/API.hpp"
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc
///////////////////////////////////////////////////////////////////////////////
namespace Arc
{

///////////////////////////////////////////////////////////////////////////////
LIBCACAModule::LIBCACAModule(void)
    : mWindow(nullptr)
    , mCanva(nullptr)
    , mSpriteSheet(nullptr)
    , mInterpolationFactor(0.f)
    , mLastFrameTime(0)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Cannot init SDL2" << std::endl;
        return;
    }

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image initialization failed: "
                  << IMG_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    mCanva = caca_create_canvas(0, 0);
    mWindow = caca_create_display(mCanva);
    if (!mCanva || !mWindow) return;
    caca_set_canvas_size(mCanva, 600, 600);
}

///////////////////////////////////////////////////////////////////////////////
LIBCACAModule::~LIBCACAModule(void)
{
    if (mCanva){
        caca_free_canvas(mCanva);
        mCanva = nullptr;
    }
    if (mWindow){
        caca_free_display(mWindow);
        mWindow = nullptr;
    }
    if (mSpriteSheet){
        SDL_FreeSurface(mSpriteSheet);
        mSpriteSheet = nullptr;
    }
    IMG_Quit();
    SDL_Quit();
}

///////////////////////////////////////////////////////////////////////////////
EKeyboardKey LIBCACAModule::GetKey(int key)
{
    switch(key) {

        case 'A': case 'a':         return (EKeyboardKey::A);
        case 'B': case 'b':         return (EKeyboardKey::B);
        case 'C': case 'c':         return (EKeyboardKey::C);
        case 'D': case 'd':         return (EKeyboardKey::D);
        case 'E': case 'e':         return (EKeyboardKey::E);
        case 'F': case 'f':         return (EKeyboardKey::F);
        case 'G': case 'g':         return (EKeyboardKey::G);
        case 'H': case 'h':         return (EKeyboardKey::H);
        case 'I': case 'i':         return (EKeyboardKey::I);
        case 'J': case 'j':         return (EKeyboardKey::J);
        case 'K': case 'k':         return (EKeyboardKey::K);
        case 'L': case 'l':         return (EKeyboardKey::L);
        case 'M': case 'm':         return (EKeyboardKey::M);
        case 'N': case 'n':         return (EKeyboardKey::N);
        case 'O': case 'o':         return (EKeyboardKey::O);
        case 'P': case 'p':         return (EKeyboardKey::P);
        case 'Q': case 'q':         return (EKeyboardKey::Q);
        case 'R': case 'r':         return (EKeyboardKey::R);
        case 'S': case 's':         return (EKeyboardKey::S);
        case 'T': case 't':         return (EKeyboardKey::T);
        case 'U': case 'u':         return (EKeyboardKey::U);
        case 'V': case 'v':         return (EKeyboardKey::V);
        case 'W': case 'w':         return (EKeyboardKey::W);
        case 'X': case 'x':         return (EKeyboardKey::X);
        case 'Y': case 'y':         return (EKeyboardKey::Y);
        case 'Z': case 'z':         return (EKeyboardKey::Z);
        case '0':                   return (EKeyboardKey::NUM0);
        case '1':                   return (EKeyboardKey::NUM1);
        case '2':                   return (EKeyboardKey::NUM2);
        case '3':                   return (EKeyboardKey::NUM3);
        case '4':                   return (EKeyboardKey::NUM4);
        case '5':                   return (EKeyboardKey::NUM5);
        case '6':                   return (EKeyboardKey::NUM6);
        case '7':                   return (EKeyboardKey::NUM7);
        case '8':                   return (EKeyboardKey::NUM8);
        case '9':                   return (EKeyboardKey::NUM9);
        case 0x20:                  return (EKeyboardKey::SPACE);
        case CACA_KEY_RETURN:       return (EKeyboardKey::ENTER);
        case CACA_KEY_BACKSPACE:    return (EKeyboardKey::BACKSPACE);
        case CACA_KEY_ESCAPE:       return (EKeyboardKey::ESCAPE);
        case CACA_KEY_UP:           return (EKeyboardKey::UP);
        case CACA_KEY_DOWN:         return (EKeyboardKey::DOWN);
        case CACA_KEY_LEFT:         return (EKeyboardKey::LEFT);
        case CACA_KEY_RIGHT:        return (EKeyboardKey::RIGHT);
        default:                    return (EKeyboardKey::UNKNOWN);
    }
}

///////////////////////////////////////////////////////////////////////////////
EMouseButton LIBCACAModule::GetMousePress(int click)
{
    switch (click){
        case '1':                   return (EMouseButton::LEFT);
        case '2':                   return (EMouseButton::RIGHT);
        case '3':                   return (EMouseButton::MIDDLE);
        default:                    return (EMouseButton::LEFT);
    }
}

///////////////////////////////////////////////////////////////////////////////
void LIBCACAModule::Update(void)
{
    while (auto event = API::PollEvent(API::Event::GRAPHICS)) {
        if (auto gridSize = event->GetIf<API::Event::GridSize>()){
            float gridWidth = gridSize->width * GRID_TILE_SIZE;
            float gridHeight = gridSize->height * GRID_TILE_SIZE;

            SDL_DisplayMode info;
            SDL_GetDesktopDisplayMode(0, &info);

            mRatio = std::min(
                static_cast<float>(info.w - (info.w / 4)) / gridWidth,
                static_cast<float>(info.h - (info.h / 4)) / gridHeight
            );
            mRatio = std::floor(mRatio);

            mWindowWidth = static_cast<int>(gridWidth * mRatio);
            mWindowHeight = static_cast<int>(gridHeight * mRatio);

            caca_free_canvas(mCanva);
            caca_free_display(mWindow);
            mCanva = caca_create_canvas(
                gridSize->width * mRatio * 2,
                gridSize->height * mRatio / 2
            );
            mWindow = caca_create_display(mCanva);

        } else if (event->Is<API::Event::ChangeGame>()){
            mSpritePositions.clear();
        }
    }
    caca_event event;
    while (caca_get_event(mWindow, CACA_EVENT_ANY, &event, 0)){
        if (event.type == CACA_EVENT_QUIT){
            API::PushEvent(API::Event::Channel::CORE, API::Event::Closed());
        } else if (event.type == CACA_EVENT_KEY_PRESS){
            int key = caca_get_event_key_ch(&event);
            API::PushEvent(
                API::Event::Channel::CORE,
                API::Event::KeyPressed{(GetKey(key))}
            );
        } else if (event.type == CACA_EVENT_MOUSE_PRESS) {
            int click = event.data.mouse.button;
            int gridX = event.data.mouse.x / (GRID_TILE_SIZE * mRatio);
            int gridY = event.data.mouse.y / (GRID_TILE_SIZE * mRatio);
            API::PushEvent(
                API::Event::Channel::GAME,
                API::Event::MousePressed{GetMousePress(click), gridX, gridY}
            );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void LIBCACAModule::Clear(void)
{
    caca_clear_canvas(mCanva);
}

///////////////////////////////////////////////////////////////////////////////
void LIBCACAModule::Render(void)
{
    uint32_t currentTime = caca_get_display_time(mWindow);
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
     int pixelsPerCharX = 1;
     int pixelsPerCharY = 1;
     int bufferWidth = caca_get_canvas_width(mCanva) * pixelsPerCharX;
     int bufferHeight = caca_get_canvas_height(mCanva) * pixelsPerCharY;

     SDL_Surface* bufferSurface = SDL_CreateRGBSurface(
         SDL_SWSURFACE,
         bufferWidth,
         bufferHeight,
         32,
         0x00ff0000,
         0x0000ff00,
         0x000000ff,
         0xff000000
     );

     if (!bufferSurface) return;
     float scaleX = static_cast<float>(bufferWidth) / mWindowWidth * mRatio;
     float scaleY = static_cast<float>(bufferHeight) / mWindowHeight * mRatio;
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

    while (!API::IsDrawQueueEmpty()) {
        auto draw = API::PopDraw();
        auto [asset, pos, color] = draw;
        int entityId = asset.id;

        Vec2f targetPos = {
            pos.x * GRID_TILE_SIZE - (asset.size.x - GRID_TILE_SIZE) / 2.0f,
            pos.y * GRID_TILE_SIZE - (asset.size.y - GRID_TILE_SIZE) / 2.0f
        };

        if (
            entityId == -1 ||
            mSpritePositions.find(entityId) == mSpritePositions.end()
        ) {
            mSpritePositions[entityId] = {targetPos, targetPos, 1.0f};
        } else {
            float length = std::sqrt(
                std::pow(targetPos.x - mSpritePositions[entityId].current.x, 2) +
                std::pow(targetPos.y - mSpritePositions[entityId].current.y, 2)
            );

            if (length > 15.f) {
                mSpritePositions[entityId].target = targetPos;
                mSpritePositions[entityId].current = targetPos;
                mSpritePositions[entityId].factor = 1.0f;
            } else if (mSpritePositions[entityId].target.x != targetPos.x ||
                mSpritePositions[entityId].target.y != targetPos.y) {
                mSpritePositions[entityId].target = targetPos;
                mSpritePositions[entityId].factor = 0.0f;
            }
        }

        Vec2f currentPos = mSpritePositions[entityId].current;
        Vec2f interpolatedPos = {
            currentPos.x + (targetPos.x - currentPos.x) *
                mSpritePositions[entityId].factor,
            currentPos.y + (targetPos.y - currentPos.y) *
                mSpritePositions[entityId].factor
        };

        SDL_Surface *spriteSurface = SDL_CreateRGBSurface(
            SDL_SWSURFACE,
            asset.size.x,
            asset.size.y,
            32,
            0x00ff0000,
            0x0000ff00,
            0x000000ff,
            0xff000000
        );
        if (!spriteSurface) return;

        SDL_Rect srcRect;
        srcRect.x = asset.position.x * GRID_TILE_SIZE;
        srcRect.y = asset.position.y * GRID_TILE_SIZE;
        srcRect.w = asset.size.x;
        srcRect.h = asset.size.y;

        SDL_BlitSurface(mSpriteSheet, &srcRect, spriteSurface, NULL);

        SDL_Rect destRect;
        destRect.x = static_cast<int>(interpolatedPos.x * scaleX);
        destRect.y = static_cast<int>(interpolatedPos.y * scaleY);
        destRect.w = static_cast<int>(asset.size.x * scaleX);
        destRect.h = static_cast<int>(asset.size.y * scaleY);
        SDL_BlitScaled(mSpriteSheet, &srcRect, bufferSurface, &destRect);
        SDL_FreeSurface(spriteSurface);

    }

    caca_dither_t* dither = caca_create_dither(
        32,
        bufferSurface->w,
        bufferSurface->h,
        bufferSurface->pitch,
        bufferSurface->format->Rmask,
        bufferSurface->format->Gmask,
        bufferSurface->format->Bmask,
        bufferSurface->format->Amask
    );

    if (dither) {
        caca_set_dither_algorithm(dither, "fstein");
        caca_set_dither_color(dither, "rgb");

        caca_set_dither_brightness(dither, 0.8f);
        caca_set_dither_gamma(dither, 1.0f);
        caca_set_dither_contrast(dither, 0.8f);
        // caca_set_dither_charset()
        caca_dither_bitmap(
            mCanva,
            0, 0,
            caca_get_canvas_width(mCanva),
            caca_get_canvas_height(mCanva),
            dither,
            bufferSurface->pixels
        );

        caca_free_dither(dither);
    }

    SDL_FreeSurface(bufferSurface);
    caca_refresh_display(mWindow);
}

///////////////////////////////////////////////////////////////////////////////
void LIBCACAModule::SetTitle(const std::string& title)
{
    caca_set_display_title(mWindow, title.c_str());
}

///////////////////////////////////////////////////////////////////////////////
void LIBCACAModule::LoadSpriteSheet(const std::string& path)
{
    if (!mCanva) return;

    mSpriteSheet = IMG_Load(path.c_str());
    if (!mSpriteSheet){
        std::cerr << "not loaded";
        return;
    }
}

///////////////////////////////////////////////////////////////////////////////
std::string LIBCACAModule::GetName(void) const
{
    return ("LIBCACA");
}

} // namespace Arc
