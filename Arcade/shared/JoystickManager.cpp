///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Arcade/shared/JoystickManager.hpp"
#include <cassert>

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc::priv
///////////////////////////////////////////////////////////////////////////////
namespace Arc::priv
{

///////////////////////////////////////////////////////////////////////////////
JoystickManager::JoystickManager(void)
{
    JoystickImpl::Initialize();
}

///////////////////////////////////////////////////////////////////////////////
JoystickManager::~JoystickManager()
{
    for (Item& item : mJoysticks) {
        if (item.state.connected) {
            item.joystick.Close();
        }
    }
    JoystickImpl::Cleanup();
}

///////////////////////////////////////////////////////////////////////////////
JoystickManager& JoystickManager::GetInstance(void)
{
    static JoystickManager instance;
    return (instance);
}

///////////////////////////////////////////////////////////////////////////////
const JoystickCaps& JoystickManager::GetCapabilities(
    unsigned int joystick) const
{
    assert(joystick < Joystick::Count &&
        "Joystick index must be less than Joystick::Count");
    return (mJoysticks[joystick].capabilities);
}

///////////////////////////////////////////////////////////////////////////////
const JoystickState& JoystickManager::GetState(unsigned int joystick) const
{
    assert(joystick < Joystick::Count &&
        "Joystick index must be less than Joystick::Count");
    return (mJoysticks[joystick].state);
}

///////////////////////////////////////////////////////////////////////////////
const Joystick::Identification& JoystickManager::GetIdentification(
    unsigned int joystick) const
{
    assert(joystick < Joystick::Count &&
        "Joystick index must be less than Joystick::Count");
    return (mJoysticks[joystick].identification);
}

///////////////////////////////////////////////////////////////////////////////
void JoystickManager::Update(void)
{
    for (unsigned int i = 0; i < Joystick::Count; i++) {
        Item& item = mJoysticks[i];

        if (item.state.connected) {
            item.state = item.joystick.Update();

            if (!item.state.connected) {
                item.joystick.Close();
                item.capabilities = JoystickCaps();
                item.state = JoystickState();
                item.identification = Joystick::Identification();
            }
        } else {
            if (JoystickImpl::IsConnected(i)) {
                if (item.joystick.Open(i)) {
                    item.capabilities = item.joystick.GetCapabilities();
                    item.state = item.joystick.Update();
                    item.identification = item.joystick.GetIdentification();
                }
            }
        }
    }
}

} // namespace Arc::priv
