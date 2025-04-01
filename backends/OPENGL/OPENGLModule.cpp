///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "backends/OPENGL/OPENGLModule.hpp"
#include "Arcade/core/API.hpp"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc
///////////////////////////////////////////////////////////////////////////////
namespace Arc
{

///////////////////////////////////////////////////////////////////////////////
OPENGLModule::OPENGLModule(void) :  mRatio(4.f)
                                   ,mInterpolationFactor(0.f)
                                   ,mLastFrameTime(0),
                                   mWindowWidth(600),
                                   mWindowHeight(600)
{
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    mWindow = glfwCreateWindow(mWindowWidth, mWindowHeight, "Arcade - OPENGL", nullptr, nullptr);
    if (!mWindow){
        //std::cout << "herro\n";
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(mWindow);
    glewInit();
    glViewport(0, 0, 800, 600); //set the coords. top left (0,0) with height and width of window
    glfwSetKeyCallback(mWindow, KeyCallback);
    glfwSetMouseButtonCallback(mWindow, MouseCallback);

    glfwSetWindowUserPointer(mWindow, this);

    CompileShaders();


}

OPENGLModule::~OPENGLModule(void)
{
    //std::cout << "closing\n";
    glDeleteTextures(1, &mSpriteSheet);
    glDeleteVertexArrays(1, &mVAO);
    // glDeleteBuffers(1, &mVBO);
    // glDeleteBuffers(1, &mEBO);

    if (mWindow) glfwDestroyWindow(mWindow);
    if (mShaderProgram) glDeleteProgram(mShaderProgram);

    glfwTerminate();
    //std::cout << "closing finish\n";

}


///////////////////////////////////////////////////////////////////////////////
EKeyboardKey OPENGLModule::GetKey(int key)
{
    switch (key) {
        case GLFW_KEY_A:           return EKeyboardKey::A;
        case GLFW_KEY_B:           return EKeyboardKey::B;
        case GLFW_KEY_C:           return EKeyboardKey::C;
        case GLFW_KEY_D:           return EKeyboardKey::D;
        case GLFW_KEY_E:           return EKeyboardKey::E;
        case GLFW_KEY_F:           return EKeyboardKey::F;
        case GLFW_KEY_G:           return EKeyboardKey::G;
        case GLFW_KEY_H:           return EKeyboardKey::H;
        case GLFW_KEY_I:           return EKeyboardKey::I;
        case GLFW_KEY_J:           return EKeyboardKey::J;
        case GLFW_KEY_K:           return EKeyboardKey::K;
        case GLFW_KEY_L:           return EKeyboardKey::L;
        case GLFW_KEY_M:           return EKeyboardKey::M;
        case GLFW_KEY_N:           return EKeyboardKey::N;
        case GLFW_KEY_O:           return EKeyboardKey::O;
        case GLFW_KEY_P:           return EKeyboardKey::P;
        case GLFW_KEY_Q:           return EKeyboardKey::Q;
        case GLFW_KEY_R:           return EKeyboardKey::R;
        case GLFW_KEY_S:           return EKeyboardKey::S;
        case GLFW_KEY_T:           return EKeyboardKey::T;
        case GLFW_KEY_U:           return EKeyboardKey::U;
        case GLFW_KEY_V:           return EKeyboardKey::V;
        case GLFW_KEY_W:           return EKeyboardKey::W;
        case GLFW_KEY_X:           return EKeyboardKey::X;
        case GLFW_KEY_Y:           return EKeyboardKey::Y;
        case GLFW_KEY_Z:           return EKeyboardKey::Z;
        case GLFW_KEY_UP:          return EKeyboardKey::UP;
        case GLFW_KEY_DOWN:        return EKeyboardKey::DOWN;
        case GLFW_KEY_LEFT:        return EKeyboardKey::LEFT;
        case GLFW_KEY_RIGHT:       return EKeyboardKey::RIGHT;
        case GLFW_KEY_0:           return EKeyboardKey::NUM0;
        case GLFW_KEY_1:           return EKeyboardKey::NUM1;
        case GLFW_KEY_2:           return EKeyboardKey::NUM2;
        case GLFW_KEY_3:           return EKeyboardKey::NUM3;
        case GLFW_KEY_4:           return EKeyboardKey::NUM4;
        case GLFW_KEY_5:           return EKeyboardKey::NUM5;
        case GLFW_KEY_6:           return EKeyboardKey::NUM6;
        case GLFW_KEY_7:           return EKeyboardKey::NUM7;
        case GLFW_KEY_8:           return EKeyboardKey::NUM8;
        case GLFW_KEY_9:           return EKeyboardKey::NUM9;
        case GLFW_KEY_SPACE:       return EKeyboardKey::SPACE;
        case GLFW_KEY_ENTER:       return EKeyboardKey::ENTER;
        case GLFW_KEY_ESCAPE:      return EKeyboardKey::ESCAPE;
        case GLFW_KEY_BACKSPACE:   return EKeyboardKey::BACKSPACE;
        default:                  return EKeyboardKey::UNKNOWN;

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
void OPENGLModule::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //std::cout << "keycallback\n";
    OPENGLModule* instance = static_cast<OPENGLModule*>(glfwGetWindowUserPointer(window));
    if (action == GLFW_PRESS)
        API::PushEvent(API::Event::Channel::CORE, API::Event::KeyPressed{ instance->GetKey(key) });

    //std::cout << "callback finish\n";
}


///////////////////////////////////////////////////////////////////////////////
void OPENGLModule::MouseCallback(GLFWwindow* window, int button, int action, int mods)
{
    //std::cout << "mousecallback\n";
    OPENGLModule* instance = static_cast<OPENGLModule*>(glfwGetWindowUserPointer(window));

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    if (action == GLFW_PRESS)
        API::PushEvent(API::Event::Channel::GAME, API::Event::MousePressed{
            instance->GetMousePress(button),
            static_cast<int>(mouseX / (GRID_TILE_SIZE * instance->mRatio)),
            static_cast<int>(mouseY / (GRID_TILE_SIZE * instance->mRatio))
        });
        float ndcX = 2.0f / instance->mWindowWidth;
        float ndcY = 2.0f / instance->mWindowHeight;
        float mouseNdcX = (mouseX * ndcX) - 1.0f;
        float mouseNdcY = 1.0f - (mouseY * ndcY);

        std::cout << "Mouse Position in Pixels: (" << mouseX << ", " << mouseY << ")" << std::endl;
        std::cout << "Mouse Position in NDC: (" << mouseNdcX << ", " << mouseNdcY << ")" << std::endl;

}

void OPENGLModule::Update(void)
{
    while (auto event = API::PollEvent(API::Event::GRAPHICS)) {
        if (auto gridSize = event->GetIf<API::Event::GridSize>()) {
            std::cout << "hello im here \n";
            const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
            int screenWidth = mode->width;
            int screenHeight = mode->height;

            // Compute the grid's total size in pixels
            float gridWidth = gridSize->width * GRID_TILE_SIZE;
            float gridHeight = gridSize->height * GRID_TILE_SIZE;
            mgridWidth = gridWidth;
            mgridHeight = gridHeight;
            std::cout << "update value: \n";
            std::cout << "gridsize: (" << gridSize->width << ", " << gridSize->height << ")\n";
            std::cout << "tile size: (" << GRID_TILE_SIZE << ", " << GRID_TILE_SIZE << ")\n";
            std::cout << "combine them innit: " << Vec2f(gridWidth, gridHeight) << std::endl;

            // Compute mRatio based on actual screen size
            mRatio = std::min(
                static_cast<float>(screenWidth - (screenWidth / 4.f)) / gridWidth,
                static_cast<float>(screenHeight - (screenHeight / 4.f)) / gridHeight
            );

            mRatio = std::floor(mRatio);
            // Apply the ratio to determine the new window size
            mWindowWidth = static_cast<int>(gridWidth * mRatio);
            mWindowHeight = static_cast<int>(gridHeight * mRatio);

            // Resize the GLFW window
            glfwSetWindowSize(mWindow, mWindowWidth, mWindowHeight);

            // Update projection matrix
            mProjection = glm::ortho(
                0.0f, gridWidth,
                gridHeight, 0.0f,  // Keep y-axis flipped (OpenGL expects bottom-left origin)
                -1.0f, 1.0f
            );

            // Apply new viewport
            glViewport(0, 0, mWindowWidth, mWindowHeight);
            GLuint projectionLoc = glGetUniformLocation(mShaderProgram, "projection");
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(mProjection));

        } else if (event->Is<API::Event::ChangeGame>()) {
                    mSpritePositions.clear();
                }
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
    //std::cout << "shading\n";


    // Vertex Shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // Check for compilation errors
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Fragment Shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // Check for compilation errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Shader Program
    mShaderProgram = glCreateProgram();
    glAttachShader(mShaderProgram, vertexShader);
    glAttachShader(mShaderProgram, fragmentShader);
    glLinkProgram(mShaderProgram);

    // Delete the shaders as they're linked into our program now and no longer necessary

    glDetachShader(mShaderProgram, vertexShader);
    glDetachShader(mShaderProgram, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // // Vertex data for a rectangle (two triangles)
    // glm::vec2 vertices[6] = {

    //     glm::vec2(-0.5f,  0.5f),  // Top Left
    //     glm::vec2(-0.5f, -0.5f),  // Bottom Left
    //     glm::vec2(0.5f,  0.5f),   // Top Right
    //     glm::vec2(0.5f, -0.5f),   // Bottom Right
    //     glm::vec2(0.5f, -0.5f),   // Bottom Right (for the second triangle)
    //     glm::vec2(-0.5f, -0.5f)   // Bottom Left (for the second triangle)
    // };

    // // Create VAO
    glGenVertexArrays(1, &mVAO);
    // glGenBuffers(1, &mVBO);
    // glGenBuffers(1, &mEBO);

    glBindVertexArray(mVAO);

    // Generate and bind VBO
    // glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

}

void OPENGLModule::Render()
{

    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(mShaderProgram);
    glBindVertexArray(mVAO);

    glViewport(0, 0, mWindowWidth, mWindowHeight);

    while (!API::IsDrawQueueEmpty()) {
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
        glm::vec2 currentPos = mSpritePositions[entityId].current;

        glm::vec2 interpolatedPos = {
            currentPos.x + (targetPos.x - currentPos.x) * mSpritePositions[entityId].factor,
            currentPos.y + (targetPos.y - currentPos.y) * mSpritePositions[entityId].factor
        };

        // original

        // If you want the origin point to be the top-left corner:
        glm::vec2 originPoint = {asset.position.x * GRID_TILE_SIZE, asset.position.y * GRID_TILE_SIZE};

        // If you want the origin point to be the center of the sprite:

        glm::vec2 textureCoords[6] = {
            glm::vec2 ({originPoint}),    // vec2 (left, top),
            glm::vec2 (originPoint.x, originPoint.y + asset.size.y), // vec2 (left, bottom),
            glm::vec2 (originPoint.x + asset.size.x, originPoint.y), // vec2 (right, top),
            glm::vec2 (originPoint.x + asset.size.x, originPoint.y), // vec2 (right, top),
            glm::vec2 (originPoint.x, originPoint.y + asset.size.y), // vec2 (left, bottom),
            glm::vec2 (originPoint.x + asset.size.x, originPoint.y + asset.size.y) // vec2 (right, bottom),

        };


        // Create model matrix for each object you render
        glm::mat4 model = glm::mat4(1.0f); // Identity matrix to start
        // Transform the model based on position, scale, rotation
        model = glm::translate(model, glm::vec3(interpolatedPos.x, interpolatedPos.y, 0.0f)); // Position
        model = glm::scale(model, glm::vec3(asset.size.x, asset.size.y, 1.0f)); // Scale

        // Send model matrix to shader
        GLint modelLoc = glGetUniformLocation(mShaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        GLuint textureCoordsLocation = glGetUniformLocation(mShaderProgram, "textureCoords");
        glUniform2fv(textureCoordsLocation, 6, glm::value_ptr(textureCoords[0]));

        // model based graphics:
        // Assuming asset.size is the original size of the asset
        // glm::vec2 scaleFactor(mWindowWidth / mAtlasWidth, mWindowHeight / mAtlasHeight); // Scaling factor

        // // based on asset/window
        // float scaledWidth = asset.size.x * scaleFactor.x;
        // float scaledHeight = asset.size.y * scaleFactor.y;
        // float ndcX = 2.0f / mWindowWidth;
        // float ndcY = 2.0f / mWindowHeight;
        // float left = (interpolatedPos.x * ndcX) - 1.0f; // Position in NDC
        // float top = 1.0f - (interpolatedPos.y * ndcY); // Position in NDC
        // float right = ((interpolatedPos.x + scaledWidth) * ndcX) - 1.0f; // Position in NDC
        // float bottom = 1.0f - ((interpolatedPos.y + scaledHeight) * ndcY); // Position in NDC

        // // base value
        // float left = (interpolatedPos.x * ndcX) - 1.0f; // Position in NDC
        // float top = 1.0f - (interpolatedPos.y * ndcY); // Position in NDC
        // float right = ((interpolatedPos.x + asset.size.x) * ndcX) - 1.0f; // Position in NDC
        // float bottom = 1.0f - ((interpolatedPos.y + asset.size.y) * ndcY); // Position in NDC


        //fully ratioed:
        // Apply the ratio to scale both position and size
        float ndcX = 2.0f / mWindowWidth;
        float ndcY = 2.0f / mWindowHeight;

        // Apply scaling ratio to position and size
        float scaledX = interpolatedPos.x * mRatio;
        float scaledY = interpolatedPos.y * mRatio;
        float scaledWidth = asset.size.x * mRatio;
        float scaledHeight = asset.size.y * mRatio;

        // Convert to NDC
        float left = (scaledX * ndcX) - 1.0f;
        float top = 1.0f - (scaledY * ndcY);
        float right = ((scaledX + scaledWidth) * ndcX) - 1.0f;
        float bottom = 1.0f - ((scaledY + scaledHeight) * ndcY);

        if (asset.position == Vec2i{11, 9}){

                std::cout << "left: " << left << std::endl
                << "right: " << right << std::endl
                << "top: " << top << std::endl
                << "bottom: " << bottom << std::endl
                << "window height: " << mWindowHeight << std::endl
                << "window width: " << mWindowWidth << std::endl
                << "asset size: " << asset.size << std::endl
                << "ratio: " << mRatio << std::endl
                << "gridsize: " << Vec2f(mgridWidth, mgridHeight) << std::endl
                // << "scalefactor: " << Vec2f(scaleFactor.x, scaleFactor.y) << std::endl
                << "interpolatedPos: (" << interpolatedPos.x << ", " << interpolatedPos.y << ")\n" << std::endl;
        }

        // potential scaling
        glm::vec2 vertices[6] = {
            glm::vec2(left, top),  // Top Left
            glm::vec2(left, bottom),  // Bottom Left
            glm::vec2(right, top),  // Top Right
            glm::vec2(right, top),  // Top Right (duplicate)
            glm::vec2(left, bottom),  // Bottom Left (duplicate)
            glm::vec2(right, bottom) // Bottom Right
        };

        GLuint vertexPositionsLocation = glGetUniformLocation(mShaderProgram, "vertices");
        glUniform2fv(vertexPositionsLocation, 6, glm::value_ptr(vertices[0]));
        // Convert grid position to pixel position
        glDrawArrays(GL_TRIANGLES, 0, 6);

    }

    glBindVertexArray(0);

    glfwSwapBuffers(mWindow);
}

///////////////////////////////////////////////////////////////////////////////
void OPENGLModule::LoadSpriteSheet(const std::string& path)
{
    int channels;
    unsigned char* data = stbi_load(path.c_str(), &mAtlasWidth, &mAtlasHeight, &channels, 4);
    if (!data) return;
    std::cout << "Loaded image: " << path
              << ", Width: " << mAtlasWidth
              << ", Height: " << mAtlasHeight
              << ", Channels: " << channels << std::endl;


    glGenTextures(1, &mSpriteSheet);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mSpriteSheet);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mAtlasWidth, mAtlasHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
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


