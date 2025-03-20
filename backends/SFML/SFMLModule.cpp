///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "backends/SFML/SFMLModule.hpp"
#include "Arcade/core/API.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc
///////////////////////////////////////////////////////////////////////////////
namespace Arc
{

///////////////////////////////////////////////////////////////////////////////
SFMLModule::SFMLModule(void)
    : mRatio(4.f)
{
    mWindow = std::make_unique<sf::RenderWindow>(
        sf::VideoMode(600, 600), "Arcade - SFML"
    );
}

///////////////////////////////////////////////////////////////////////////////
SFMLModule::~SFMLModule()
{
    if (mWindow && mWindow->isOpen()) {
        mWindow->close();
    }
    mWindow.reset();
}

///////////////////////////////////////////////////////////////////////////////
EKeyboardKey SFMLModule::GetKey(sf::Keyboard::Key key)
{
    switch (key) {
        case sf::Keyboard::A:           return (EKeyboardKey::A);
        case sf::Keyboard::B:           return (EKeyboardKey::B);
        case sf::Keyboard::C:           return (EKeyboardKey::C);
        case sf::Keyboard::D:           return (EKeyboardKey::D);
        case sf::Keyboard::E:           return (EKeyboardKey::E);
        case sf::Keyboard::F:           return (EKeyboardKey::F);
        case sf::Keyboard::G:           return (EKeyboardKey::G);
        case sf::Keyboard::H:           return (EKeyboardKey::H);
        case sf::Keyboard::I:           return (EKeyboardKey::I);
        case sf::Keyboard::J:           return (EKeyboardKey::J);
        case sf::Keyboard::K:           return (EKeyboardKey::K);
        case sf::Keyboard::L:           return (EKeyboardKey::L);
        case sf::Keyboard::M:           return (EKeyboardKey::M);
        case sf::Keyboard::N:           return (EKeyboardKey::N);
        case sf::Keyboard::O:           return (EKeyboardKey::O);
        case sf::Keyboard::P:           return (EKeyboardKey::P);
        case sf::Keyboard::Q:           return (EKeyboardKey::Q);
        case sf::Keyboard::R:           return (EKeyboardKey::R);
        case sf::Keyboard::S:           return (EKeyboardKey::S);
        case sf::Keyboard::T:           return (EKeyboardKey::T);
        case sf::Keyboard::U:           return (EKeyboardKey::U);
        case sf::Keyboard::V:           return (EKeyboardKey::V);
        case sf::Keyboard::W:           return (EKeyboardKey::W);
        case sf::Keyboard::X:           return (EKeyboardKey::X);
        case sf::Keyboard::Y:           return (EKeyboardKey::Y);
        case sf::Keyboard::Z:           return (EKeyboardKey::Z);
        case sf::Keyboard::Up:          return (EKeyboardKey::UP);
        case sf::Keyboard::Down:        return (EKeyboardKey::DOWN);
        case sf::Keyboard::Left:        return (EKeyboardKey::LEFT);
        case sf::Keyboard::Right:       return (EKeyboardKey::RIGHT);
        case sf::Keyboard::Num0:        return (EKeyboardKey::NUM0);
        case sf::Keyboard::Num1:        return (EKeyboardKey::NUM1);
        case sf::Keyboard::Num2:        return (EKeyboardKey::NUM2);
        case sf::Keyboard::Num3:        return (EKeyboardKey::NUM3);
        case sf::Keyboard::Num4:        return (EKeyboardKey::NUM4);
        case sf::Keyboard::Num5:        return (EKeyboardKey::NUM5);
        case sf::Keyboard::Num6:        return (EKeyboardKey::NUM6);
        case sf::Keyboard::Num7:        return (EKeyboardKey::NUM7);
        case sf::Keyboard::Num8:        return (EKeyboardKey::NUM8);
        case sf::Keyboard::Num9:        return (EKeyboardKey::NUM9);
        case sf::Keyboard::Space:       return (EKeyboardKey::SPACE);
        case sf::Keyboard::Return:      return (EKeyboardKey::ENTER);
        case sf::Keyboard::Escape:      return (EKeyboardKey::ESCAPE);
        case sf::Keyboard::BackSpace:   return (EKeyboardKey::BACKSPACE);
        default:                        return (EKeyboardKey::UNKNOWN);
    }
}

///////////////////////////////////////////////////////////////////////////////
void SFMLModule::Update(void)
{
    while (auto event = API::PollEvent(API::Event::GRAPHICS)) {
        if (auto gridSize = event->GetIf<API::Event::GridSize>()) {
            mWindow->setSize(sf::Vector2u(
                static_cast<unsigned int>(gridSize->width * 8 * mRatio),
                static_cast<unsigned int>(gridSize->height * 8 * mRatio)
            ));
            mWindow->setView(sf::View(
                {
                    0, 0,
                    gridSize->width * 8.f,
                    gridSize->height * 8.f
                }
            ));
        }
    }

    sf::Event event;
    while (mWindow->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            API::PushEvent(API::Event::Channel::CORE, API::Event::Closed());
        }
        if (event.type == sf::Event::KeyPressed) {
            API::PushEvent(
                API::Event::Channel::GAME,
                API::Event::KeyPressed{GetKey(event.key.code)}
            );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void SFMLModule::Clear(void)
{
    mWindow->clear();
}

///////////////////////////////////////////////////////////////////////////////
void SFMLModule::Render(void)
{
    float offset = 4.f;

    while (mSpriteSheet && !API::IsDrawQueueEmpty()) {
        auto draw = API::PopDraw();
        auto [asset, x, y] = draw;

        sf::Sprite sprite;
        sprite.setTexture(*mSpriteSheet);
        sprite.setTextureRect(sf::IntRect(
            asset.position.x * 8,
            asset.position.y * 8,
            asset.size.x,
            asset.size.y
        ));
        sprite.setOrigin({asset.size.x / 2.f, asset.size.y / 2.f});
        sprite.setPosition(x * 8 + offset, y * 8 + offset);
        mWindow->draw(sprite);
    }
    mWindow->display();
}

///////////////////////////////////////////////////////////////////////////////
void SFMLModule::SetTitle(const std::string& title)
{
    mWindow->setTitle(title);
}

///////////////////////////////////////////////////////////////////////////////
void SFMLModule::LoadSpriteSheet(const std::string& path)
{
    sf::Texture* texture = new sf::Texture();

    if (!texture->loadFromFile(path)) {
        delete texture;
        return;
    }
    mSpriteSheet.reset(texture);
}

} // namespace Arc
