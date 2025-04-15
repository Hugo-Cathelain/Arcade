///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "backends/OPENGL/OPENGLModule.hpp"
#include "Arcade/core/API.hpp"
#include "Arcade/errors/GraphicalException.hpp"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc
///////////////////////////////////////////////////////////////////////////////
namespace Arc
{

///////////////////////////////////////////////////////////////////////////////
OPENGLModule::OPENGLModule(void) : mInterpolationFactor(0.f)
                                   ,mLastFrameTime(0),
                                   mWindowWidth(600),
                                   mWindowHeight(600)
{
    if (!glfwInit()) {
        throw GraphicalException("Failed to initialize GLFW");
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    mWindow = glfwCreateWindow(
        mWindowWidth, mWindowHeight, "Arcade - OPENGL", nullptr, nullptr
    );
    if (!mWindow){
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(mWindow);
    glewInit();
    glViewport(0, 0, 800, 600);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glfwSetKeyCallback(mWindow, KeyCallback);
    glfwSetMouseButtonCallback(mWindow, MouseCallback);

    glfwSetWindowUserPointer(mWindow, this);

    CompileShaders();
}

OPENGLModule::~OPENGLModule(void)
{
    glDeleteTextures(1, &mSpriteSheet);
    glDeleteVertexArrays(1, &mVAO);

    if (mWindow) glfwDestroyWindow(mWindow);
    if (mShaderProgram) glDeleteProgram(mShaderProgram);

    glfwTerminate();
}

///////////////////////////////////////////////////////////////////////////////
EKeyboardKey OPENGLModule::GetKeyByCharacter(int key, int scancode)
{
    const char* keyName = glfwGetKeyName(key, scancode);

    if (keyName && keyName[0] && !keyName[1]) {
        char c = tolower(keyName[0]);

        if (c >= 'a' && c <= 'z') {
            return (static_cast<EKeyboardKey>(static_cast<int>
                (EKeyboardKey::A) + (c - 'a')));
        }
        if (c >= '0' && c <= '9') {
            return (static_cast<EKeyboardKey>(static_cast<int>
                (EKeyboardKey::NUM0) + (c - '0')));
        }
    }

    switch (key) {
        case GLFW_KEY_UP: return EKeyboardKey::UP;
        case GLFW_KEY_DOWN: return EKeyboardKey::DOWN;
        case GLFW_KEY_LEFT: return EKeyboardKey::LEFT;
        case GLFW_KEY_RIGHT: return EKeyboardKey::RIGHT;
        case GLFW_KEY_SPACE: return EKeyboardKey::SPACE;
        case GLFW_KEY_ENTER: return EKeyboardKey::ENTER;
        case GLFW_KEY_ESCAPE: return EKeyboardKey::ESCAPE;
        case GLFW_KEY_BACKSPACE: return EKeyboardKey::BACKSPACE;
        default: return EKeyboardKey::UNKNOWN;
    }
}

///////////////////////////////////////////////////////////////////////////////
EMouseButton OPENGLModule::GetMousePress(int click)
{
    switch (click) {
        case GLFW_MOUSE_BUTTON_1:               return (EMouseButton::LEFT);
        case GLFW_MOUSE_BUTTON_2:              return (EMouseButton::RIGHT);
        case GLFW_MOUSE_BUTTON_3:             return (EMouseButton::MIDDLE);
        default:                            return (EMouseButton::LEFT);
    }
}

///////////////////////////////////////////////////////////////////////////////
void OPENGLModule::KeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods
)
{
    (void)mods;

    OPENGLModule* instance = static_cast<OPENGLModule*>
        (glfwGetWindowUserPointer(window));
    if (action == GLFW_PRESS){
        API::PushEvent(API::Event::Channel::CORE, API::Event::KeyPressed{
            instance->GetKeyByCharacter(key, scancode)
        });
    }
}


///////////////////////////////////////////////////////////////////////////////
void OPENGLModule::MouseCallback(
    GLFWwindow* window, int button, int action, int mods
)
{
    (void)action;
    (void)mods;

    OPENGLModule* instance = static_cast<OPENGLModule*>
        (glfwGetWindowUserPointer(window));
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    if (action == GLFW_PRESS){
        API::PushEvent(API::Event::Channel::GAME, API::Event::MousePressed{
            instance->GetMousePress(button),
            static_cast<int>(mouseX / (GRID_TILE_SIZE * instance->mRatio)),
            static_cast<int>(mouseY / (GRID_TILE_SIZE * instance->mRatio))
        });
    }
}

///////////////////////////////////////////////////////////////////////////////
void OPENGLModule::Update(void)
{
    while (auto event = API::PollEvent(API::Event::GRAPHICS)) {
        if (auto gridSize = event->GetIf<API::Event::GridSize>()) {
            const GLFWvidmode* mode =
                glfwGetVideoMode(glfwGetPrimaryMonitor());
            int screenWidth = mode->width;
            int screenHeight = mode->height;

            // Compute the grid's total size in pixels
            float gridWidth = gridSize->width * GRID_TILE_SIZE;
            float gridHeight = gridSize->height * GRID_TILE_SIZE;
            mgridWidth = gridWidth;
            mgridHeight = gridHeight;

            // get ratio and standardize it
            mRatio = std::min(
                static_cast<float>(screenWidth -
                    (screenWidth / 4.f)) / gridWidth,
                static_cast<float>(screenHeight -
                    (screenHeight / 4.f)) / gridHeight
            );
            mRatio = std::round(mRatio);

            mWindowWidth = static_cast<int>(gridWidth * mRatio);
            mWindowHeight = static_cast<int>(gridHeight * mRatio);

            glfwSetWindowSize(mWindow, mWindowWidth, mWindowHeight);
            glViewport(0, 0, mWindowWidth, mWindowHeight);

        } else if (event->Is<API::Event::ChangeGame>())
                    mSpritePositions.clear();
    }

    glfwPollEvents();
    if (glfwWindowShouldClose(mWindow))
        API::PushEvent(API::Event::Channel::CORE, API::Event::Closed());
}


///////////////////////////////////////////////////////////////////////////////
void OPENGLModule::Clear(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OPENGLModule::CompileShaders()
{
    // Vertex Shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        throw(GraphicalException("VERTEX::FRAGMENT::COMPILATION_FAILED"));
    }

    // Fragment Shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        throw(GraphicalException("SHADER::FRAGMENT::COMPILATION_FAILED"));
    }

    // Shader Program
    mShaderProgram = glCreateProgram();
    glAttachShader(mShaderProgram, vertexShader);
    glAttachShader(mShaderProgram, fragmentShader);
    glLinkProgram(mShaderProgram);

    // Delete the shaders no longer necessary as they are linked to the program
    glDetachShader(mShaderProgram, vertexShader);
    glDetachShader(mShaderProgram, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

}

///////////////////////////////////////////////////////////////////////////////
void OPENGLModule::Render()
{

    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(mShaderProgram);
    glBindVertexArray(mVAO);

    glViewport(0, 0, mWindowWidth, mWindowHeight);

    while (!API::IsDrawQueueEmpty) {
        auto draw = API::PopDraw();
        auto [asset, pos, color] = draw;
        int entityId = asset.id;

        glm::vec2 targetPos = {
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
                std::pow(targetPos.x - mSpritePositions[entityId].current.x, 2)
                    +
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
        glm::vec2 currentPos = mSpritePositions[entityId].current;

        glm::vec2 interpolatedPos = {
            currentPos.x + (targetPos.x - currentPos.x)
                * mSpritePositions[entityId].factor,
            currentPos.y + (targetPos.y - currentPos.y)
                * mSpritePositions[entityId].factor
        };

        // set origin point to top-left corner:
        glm::vec2 originPoint = {asset.position.x * GRID_TILE_SIZE,
                asset.position.y * GRID_TILE_SIZE};

        glm::vec2 textureCoords[6] = {
                // vec2 (left, top),
            glm::vec2 ({originPoint}),
                // vec2 (left, bottom),
            glm::vec2 (originPoint.x, originPoint.y + asset.size.y),
                // vec2 (right, top),
            glm::vec2 (originPoint.x + asset.size.x, originPoint.y),
                // vec2 (right, top),
            glm::vec2 (originPoint.x + asset.size.x, originPoint.y),
                // vec2 (left, bottom),
            glm::vec2 (originPoint.x, originPoint.y + asset.size.y),
                // vec2 (right, bottom),
            glm::vec2 (originPoint.x + asset.size.x,
                originPoint.y + asset.size.y)
        };

        float ndcX = 2.0f / mWindowWidth;
        float ndcY = 2.0f / mWindowHeight;

        float scaledX = interpolatedPos.x * mRatio;
        float scaledY = interpolatedPos.y * mRatio;
        float scaledWidth = asset.size.x * mRatio;
        float scaledHeight = asset.size.y * mRatio;

        float left = (scaledX * ndcX) - 1.0f;
        float top = 1.0f - (scaledY * ndcY);
        float right = ((scaledX + scaledWidth) * ndcX) - 1.0f;
        float bottom = 1.0f - ((scaledY + scaledHeight) * ndcY);

        glm::vec2 vertices[6] = {
            glm::vec2(left, top),  // Top Left
            glm::vec2(left, bottom),  // Bottom Left
            glm::vec2(right, top),  // Top Right
            glm::vec2(right, top),  // Top Right (duplicate)
            glm::vec2(left, bottom),  // Bottom Left (duplicate)
            glm::vec2(right, bottom) // Bottom Right
        };

        GLuint textureCoordsLocation = glGetUniformLocation(
            mShaderProgram, "textureCoords"
        );

        glUniform2fv(
            textureCoordsLocation, 6, glm::value_ptr(textureCoords[0])
        );

        GLuint vertexPositionsLocation = glGetUniformLocation(
            mShaderProgram, "vertices"
        );
        glUniform2fv(vertexPositionsLocation, 6, glm::value_ptr(vertices[0]));

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glBindVertexArray(0);

    glfwSwapBuffers(mWindow);
}


///////////////////////////////////////////////////////////////////////////////
void OPENGLModule::LoadSpriteSheet(const std::string& path)
{
    int channels;
    unsigned char* data = stbi_load(
        path.c_str(), &mAtlasWidth, &mAtlasHeight, &channels, 4
    );
    if (!data) return;

    glGenTextures(1, &mSpriteSheet);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mSpriteSheet);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA,
        mAtlasWidth, mAtlasHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data
    );
    stbi_image_free(data);
}


///////////////////////////////////////////////////////////////////////////////
void OPENGLModule::SetTitle(const std::string& title)
{
    glfwSetWindowTitle(mWindow, title.c_str());
}


///////////////////////////////////////////////////////////////////////////////
std::string OPENGLModule::GetName(void) const
{
    return ("OPENGL");
}

} // Arc
