///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once
// #define STB_IMAGE_IMPLEMENTATION

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Arcade/interfaces/IGraphicsModule.hpp"
#include "Arcade/enums/Inputs.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <unordered_map>
// #include "stb_image.h" // For texture loading
#include <glm/glm.hpp>            // Core GLM types (vec2, vec3, vec4, mat4, etc.)
#include <glm/gtc/matrix_transform.hpp> // For glm::ortho and other transformation functions
#include <glm/gtc/type_ptr.hpp>   // For glm::value_ptr
#include "utils/Vec2.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc
///////////////////////////////////////////////////////////////////////////////
namespace Arc
{
    
///////////////////////////////////////////////////////////////////////////////
/// \brief The OPENGLModule class
///
///////////////////////////////////////////////////////////////////////////////
class OPENGLModule : public IGraphicsModule
{


private:
struct EntityInterpolation
{
    glm::vec2 current;
    glm::vec2 target;
    float factor;
};

private:
    ///////////////////////////////////////////////////////////////////////////
    // Data members
    ///////////////////////////////////////////////////////////////////////////
    GLFWwindow* mWindow;                        //<! The window container
    GLuint mSpriteSheet;                        //<! The texture
    float mRatio;                               //<! The scaling ratio
    std::unordered_map<
        int, EntityInterpolation
    > mSpritePositions;
    float mInterpolationFactor;                 //<!
    GLuint mLastFrameTime;                      //<!
    GLuint mVAO;                                //<! The Vertex Array Object
    GLuint mShaderProgram;                      //<! The shaderProgram pointer
    int mAtlasHeight;                           //<! The Height of the Atlas
    int mAtlasWidth;                            //<! The Width of the Atlas
    int mWindowWidth;                           //<! The Height of the window
    int mWindowHeight;                          //<! The Width of the window
    int mgridWidth;                             //<! The Height of the grid
    int mgridHeight;                            //<! The Height of the grid

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor
    ///
    ///////////////////////////////////////////////////////////////////////////
    OPENGLModule(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Destructor
    ///
    ///////////////////////////////////////////////////////////////////////////
    ~OPENGLModule();

private:


const char *vertexShaderSource = R"(
    #version 430 core

    layout (location = 0) out vec2 textureCoordsOut;
    uniform mat4 projection;
    uniform vec2 textureCoords[6];
    uniform vec2 vertices[6];

    void main() {
        gl_Position = vec4(vertices[gl_VertexID], 0.0, 1.0);
        textureCoordsOut = textureCoords[gl_VertexID];
    }
)";

const char* fragmentShaderSource = R"(
    #version 430 core

    layout (location = 0) in vec2 textureCoordsIn;
    layout (location = 0) out vec4 fragColor;
    layout (location = 0) uniform sampler2D textureAtlas;
    void main()
    {
        vec4 textureColor = texelFetch(textureAtlas, ivec2(textureCoordsIn), 0);
        fragColor = textureColor;
    }
)";

    ///////////////////////////////////////////////////////////////////////////
    /// \brief sets and compiles shaders; the logic of how vertices are drawn
    ///
    ///////////////////////////////////////////////////////////////////////////
    void CompileShaders();

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief whenever a key is pressed, eventpoll automatically will call back
    /// this function; getting the api key version
    ///
    /// \param window The window current key press was registered
    /// \param key The key pressed or released
    /// \param scancode The physical key
    /// \param action action that caused calling. ie press, release, hold
    /// \param mods The modifying key press(s) simulataniously pressed with key (shift, ctrl, etc)
    ///
    /// \return The Arcade key pressed
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief whenever a key is pressed, eventpoll automatically will call back
    /// this function; getting the api key version
    ///
    /// \param window The window current key press was registered
    /// \param button The button pressed or released
    /// \param action action that caused calling. ie press, release, hold
    /// \param mods The modifying key press(s) simulataniously pressed with mouse (shift, ctrl, etc)
    ///
    /// \return The Arcade key pressed
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void MouseCallback(GLFWwindow* window, int button, int action, int mods);


    ///////////////////////////////////////////////////////////////////////////
    /// \brief Convert a OPENGL key to an Arcade key
    ///
    /// \param key The OPENGL key
    ///
    /// \return The Arcade key
    ///
    ///////////////////////////////////////////////////////////////////////////
    EKeyboardKey GetKey(int key);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Convert a OPENGL mouse press to an Arcade mouse press
    ///
    /// \param key The OPENGL mouse press
    ///
    /// \return The Arcade mouse press
    ///
    ///////////////////////////////////////////////////////////////////////////
    EMouseButton GetMousePress(int click);

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Update the graphics module
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void Update(void) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Clear the graphics module
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void Clear(void) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Render the graphics module
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void Render(void) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the title of the graphics module
    ///
    /// \param title The title of the graphics module
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void SetTitle(const std::string& title) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Load sprite sheet
    ///
    /// \param path The path to the sprite sheet
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void LoadSpriteSheet(const std::string& path) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the name of the library
    ///
    /// \return The name of the library
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual std::string GetName(void) const override;
};

} // namespace Arc
