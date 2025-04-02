///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Arcade/shared/Joystick.hpp"
#include "Arcade/shared/JoystickManager.hpp"
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc
///////////////////////////////////////////////////////////////////////////////
namespace Arc
{

///////////////////////////////////////////////////////////////////////////////
bool Joystick::IsConnected(unsigned int joystick)
{
    priv::JoystickManager& manager = priv::JoystickManager::GetInstance();
    return (manager.GetState(joystick).connected);
}

///////////////////////////////////////////////////////////////////////////////
unsigned int Joystick::GetButtonCount(unsigned int joystick)
{
    priv::JoystickManager& manager = priv::JoystickManager::GetInstance();
    return (manager.GetCapabilities(joystick).buttonCount);
}

///////////////////////////////////////////////////////////////////////////////
bool Joystick::HasAxis(unsigned int joystick, Axis axis)
{
    priv::JoystickManager& manager = priv::JoystickManager::GetInstance();
    return (manager.GetCapabilities(joystick).axes[axis]);
}

///////////////////////////////////////////////////////////////////////////////
bool Joystick::IsButtonPressed(unsigned int joystick, unsigned int button)
{
    assert(button < Joystick::ButtonCount &&
        "Button must be less than Joystick::ButtonCount");
    priv::JoystickManager& manager = priv::JoystickManager::GetInstance();
    return (manager.GetState(joystick).buttons[button]);
}

///////////////////////////////////////////////////////////////////////////////
float Joystick::GetAxisPosition(unsigned int joystick, Axis axis)
{
    priv::JoystickManager& manager = priv::JoystickManager::GetInstance();
    return (manager.GetState(joystick).axes[axis]);
}

///////////////////////////////////////////////////////////////////////////////
Joystick::Identification Joystick::GetIdentification(unsigned int joystick)
{
    priv::JoystickManager& manager = priv::JoystickManager::GetInstance();
    return (manager.GetIdentification(joystick));
}

///////////////////////////////////////////////////////////////////////////////
void Joystick::Update(void)
{
    priv::JoystickManager::GetInstance().Update();
}

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc::priv
///////////////////////////////////////////////////////////////////////////////
namespace priv
{

///////////////////////////////////////////////////////////////////////////////
void UdevDeleter::operator()(udev_device* device) const
{
    udev_device_unref(device);
}

///////////////////////////////////////////////////////////////////////////////
void UdevDeleter::operator()(udev_monitor* monitor) const
{
    udev_monitor_unref(monitor);
}

///////////////////////////////////////////////////////////////////////////////
void UdevDeleter::operator()(udev_enumerate* enumerate) const
{
    udev_enumerate_unref(enumerate);
}

///////////////////////////////////////////////////////////////////////////////
void UdevDeleter::operator()(udev* context) const
{
    udev_unref(context);
}

///////////////////////////////////////////////////////////////////////////////
UdevPtr<udev> gUdevContext;             //<!
UdevPtr<udev_monitor> gUdevMonitor;     //<!
JoystickList gJoystickList;             //<!

///////////////////////////////////////////////////////////////////////////////
bool JoystickImpl::IsJoystick(udev_device* udevDevice)
{
    if (!udevDevice) {
        return (false);
    }

    const char* devnode = udev_device_get_devnode(udevDevice);

    if (!devnode || !std::strstr(devnode, "/js")) {
        return (false);
    }

    if (udev_device_get_property_value(udevDevice, "ID_INPUT_JOYSTICK")) {
        return (true);
    }

    if (
        udev_device_get_property_value(udevDevice, "ID_INPUT_ACCELEROMETER") ||
        udev_device_get_property_value(udevDevice, "ID_INPUT_KEY") ||
        udev_device_get_property_value(udevDevice, "ID_INPUT_KEYBOARD") ||
        udev_device_get_property_value(udevDevice, "ID_INPUT_MOUSE") ||
        udev_device_get_property_value(udevDevice, "ID_INPUT_TABLET") ||
        udev_device_get_property_value(udevDevice, "ID_INPUT_TOUCHPAD") ||
        udev_device_get_property_value(udevDevice, "ID_INPUT_TOUCHSCREEN")
    ) {
        return (false);
    }

    if (auto id = udev_device_get_property_value(udevDevice, "ID_CLASS")) {
        if (std::strstr(id, "joystick")) {
            return (true);
        }

        if (
            std::strstr(id, "accelerometer") ||
            std::strstr(id, "key") ||
            std::strstr(id, "keyboard") ||
            std::strstr(id, "mouse") ||
            std::strstr(id, "tablet") ||
            std::strstr(id, "touchpad") ||
            std::strstr(id, "touchscreen")
        ) {
            return (false);
        }
    }

    return (true);
}

///////////////////////////////////////////////////////////////////////////////
void JoystickImpl::UpdatePluggedList(udev_device* udevDevice)
{
    if (udevDevice) {
        if (auto action = udev_device_get_action(udevDevice)) {
            if (IsJoystick(udevDevice)) {
                const char* devnode = udev_device_get_devnode(udevDevice);
                JoystickList::iterator recordIt;

                for (
                    recordIt = gJoystickList.begin();
                    recordIt != gJoystickList.end();
                    ++recordIt
                ) {
                    if (std::strstr(action, "add")) {
                        const char* syspath =
                            udev_device_get_syspath(udevDevice);
                        recordIt->plugged = true;
                        recordIt->systemPath = syspath ? syspath : "";
                        break;
                    } else if (std::strstr(action, "remove")) {
                        recordIt->plugged = false;
                        break;
                    }
                }

                if (recordIt == gJoystickList.end()) {
                    if (std::strstr(action, "add")) {
                        const char* syspath =
                            udev_device_get_syspath(udevDevice);

                        JoystickRecord newRecord;
                        newRecord.deviceNode = devnode;
                        newRecord.systemPath = syspath ? syspath : "";
                        newRecord.plugged = true;
                        gJoystickList.push_back(newRecord);
                    } else if (std::strstr(action, "remove")) {
                        std::cerr << "Trying to disconnect"
                                  << "joystick that wasn't connected"
                                  << std::endl;
                    }
                }
            }

            return;
        }
    }

    for (JoystickRecord& record : gJoystickList) {
        record.plugged = false;
    }

    const auto udevEnumerator = UdevPtr<udev_enumerate>(
        udev_enumerate_new(gUdevContext.get()));

    if (!udevEnumerator) {
        std::cerr << "Error while creating udev enumerator" << std::endl;
        return;
    }

    if (udev_enumerate_add_match_subsystem(udevEnumerator.get(), "input") < 0) {
        std::cerr << "Error while adding udev enumerator match" << std::endl;
        return;
    }

    if (udev_enumerate_scan_devices(udevEnumerator.get()) < 0) {
        std::cerr << "Error while enumerating udev devices" << std::endl;
        return;
    }

    udev_list_entry* devices = udev_enumerate_get_list_entry(
        udevEnumerator.get());
    udev_list_entry* device = nullptr;

    udev_list_entry_foreach(device, devices) {
        const char* syspath = udev_list_entry_get_name(device);
        const auto newUdevDevice = UdevPtr<udev_device>(
            udev_device_new_from_syspath(gUdevContext.get(), syspath));

        if (newUdevDevice && IsJoystick(newUdevDevice.get())) {
            const char* devnode = udev_device_get_devnode(newUdevDevice.get());

            JoystickList::iterator recordIt;

            for (
                recordIt = gJoystickList.begin();
                recordIt != gJoystickList.end();
                ++recordIt
            ) {
                if (recordIt->deviceNode == devnode) {
                    recordIt->plugged = true;
                    break;
                }
            }

            if (recordIt == gJoystickList.end()) {
                JoystickRecord newRecord;
                newRecord.deviceNode = devnode;
                newRecord.systemPath = syspath;
                newRecord.plugged = true;
                gJoystickList.push_back(newRecord);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
bool JoystickImpl::HasMonitorEvent(void)
{
    const int monitorFd = udev_monitor_get_fd(gUdevMonitor.get());

    pollfd fds{monitorFd, POLLIN, 0};

    return ((poll(&fds, 1, 0) > 0) && ((fds.revents & POLLIN) != 0));
}

///////////////////////////////////////////////////////////////////////////////
const char* JoystickImpl::GetUdevAttribute(
    udev_device* udevDevice, const std::string& attributeName)
{
    return (udev_device_get_property_value(udevDevice, attributeName.c_str()));
}

///////////////////////////////////////////////////////////////////////////////
const char* JoystickImpl::GetUsbAttribute(
    udev_device* udevDevice, const std::string& attributeName)
{
    udev_device* udevDeviceParent =
        udev_device_get_parent_with_subsystem_devtype(
            udevDevice, "usb", "usb_device"
        );

    if (!udevDeviceParent) {
        return (nullptr);
    }
    return (udev_device_get_sysattr_value(
        udevDeviceParent, attributeName.c_str()));
}

///////////////////////////////////////////////////////////////////////////////
unsigned int JoystickImpl::GetUsbAttributeUint(
    udev_device* udevDevice, const std::string& attributeName)
{
    if (!udevDevice) {
        return (0);
    }

    if (const char* attr = GetUsbAttribute(udevDevice, attributeName)) {
        return (static_cast<unsigned int>(std::strtoul(attr, nullptr, 16)));
    }
    return (0);
}

///////////////////////////////////////////////////////////////////////////////
unsigned int JoystickImpl::GetUdevAttributeUint(
    udev_device* udevDevice, const std::string& attributeName)
{
    if (!udevDevice) {
        return (0);
    }

    if (const char* attr = GetUdevAttribute(udevDevice, attributeName)) {
        return (static_cast<unsigned int>(std::strtoul(attr, nullptr, 16)));
    }
    return (0);
}

///////////////////////////////////////////////////////////////////////////////
unsigned int JoystickImpl::GetJoystickVendorID(unsigned int index)
{
    if (!gUdevContext) {
        std::cerr << "Failed to get vendor ID of joystick"
                  << gJoystickList[index].deviceNode
                  << std::endl;
        return (0);
    }

    const auto udevDevice = UdevPtr<udev_device>(udev_device_new_from_syspath(
        gUdevContext.get(), gJoystickList[index].systemPath.c_str()
    ));

    if (!udevDevice) {
        std::cerr << "Failed to get vendor ID of joystick"
                  << gJoystickList[index].deviceNode
                  << std::endl;
        return (0);
    }

    if (auto id = GetUdevAttributeUint(udevDevice.get(), "ID_VENDOR_ID")) {
        return (id);
    }

    if (auto id = GetUsbAttributeUint(udevDevice.get(), "idVendor")) {
        return (id);
    }

    std::cerr << "Failed to get vendor ID of joystick"
              << gJoystickList[index].deviceNode
              << std::endl;
    return (0);
}

///////////////////////////////////////////////////////////////////////////////
unsigned int JoystickImpl::GetJoystickProductID(unsigned int index)
{
    if (!gUdevContext) {
        std::cerr << "Failed to get product ID of joystick"
                  << gJoystickList[index].deviceNode
                  << std::endl;
        return (0);
    }

    const auto udevDevice = UdevPtr<udev_device>(udev_device_new_from_syspath(
        gUdevContext.get(), gJoystickList[index].systemPath.c_str()
    ));

    if (!udevDevice) {
        std::cerr << "Failed to get product ID of joystick"
                  << gJoystickList[index].deviceNode
                  << std::endl;
        return (0);
    }

    if (auto id = GetUdevAttributeUint(udevDevice.get(), "ID_MODEL_ID")) {
        return (id);
    }

    if (auto id = GetUsbAttributeUint(udevDevice.get(), "idProduct")) {
        return (id);
    }

    std::cerr << "Failed to get product ID of joystick"
              << gJoystickList[index].deviceNode
              << std::endl;
    return (0);
}

///////////////////////////////////////////////////////////////////////////////
std::string JoystickImpl::GetJoystickName(unsigned int index)
{
    const std::string devnode = gJoystickList[index].deviceNode;

    const int fd = ::open(devnode.c_str(), O_RDONLY | O_NONBLOCK);

    if (fd >= 0) {
        std::array<char, 128> name{};
        const int result = ioctl(fd, JSIOCGNAME(name.size()), name.data());

        ::close(fd);

        if (result >= 0) {
            return (name.data());
        }
    }

    if (gUdevContext) {
        const auto udevDevice = UdevPtr<udev_device>(
            udev_device_new_from_syspath(
                gUdevContext.get(), gJoystickList[index].systemPath.c_str()
            )
        );

        if (udevDevice) {
            if (auto product = GetUsbAttribute(udevDevice.get(), "product")) {
                return {product};
            }
        }
    }

    std::cerr << "Unable to get name for joystick " << devnode << std::endl;
    return ("Unknown Joystick");
}

///////////////////////////////////////////////////////////////////////////////
void JoystickImpl::Initialize(void)
{
    gUdevContext = UdevPtr<udev>(udev_new());

    if (!gUdevContext) {
        std::cerr << "Failed to create udev context, joystick support "
                  << "not available" << std::endl;
        return;
    }

    gUdevMonitor = UdevPtr<udev_monitor>(
        udev_monitor_new_from_netlink(gUdevContext.get(), "udev"));

    if (!gUdevMonitor) {
        std::cerr << "Failed to create udev monitor, joystick connections "
                  << "and disconnections won't be notified" << std::endl;
    } else {
        int error = udev_monitor_filter_add_match_subsystem_devtype(
            gUdevMonitor.get(), "input", nullptr);

        if (error < 0) {
            std::cerr << "Failed to add udev monitor filter, joystick "
                      << "connections and disconnections won't be notified: "
                      << error << std::endl;
            gUdevMonitor.reset();
        } else {
            error = udev_monitor_enable_receiving(gUdevMonitor.get());

            if (error < 0) {
                std::cerr << "Failed to enable udev monitor, joystick "
                          << "connections and disconnections won't be "
                          << "notified: " << error << std::endl;
                gUdevMonitor.reset();
            }
        }
    }

    UpdatePluggedList();
}

///////////////////////////////////////////////////////////////////////////////
void JoystickImpl::Cleanup(void)
{
    gUdevMonitor.reset();
    gUdevContext.reset();
}

///////////////////////////////////////////////////////////////////////////////
bool JoystickImpl::IsConnected(unsigned int index)
{
    if (!gUdevMonitor) {
        UpdatePluggedList();
    } else if (HasMonitorEvent()) {
        const auto udevDevice = UdevPtr<udev_device>(
            udev_monitor_receive_device(gUdevMonitor.get()));
        UpdatePluggedList(udevDevice.get());
    }

    if (index >= gJoystickList.size()) {
        return (false);
    }

    return (gJoystickList[index].plugged);
}

///////////////////////////////////////////////////////////////////////////////
bool JoystickImpl::Open(unsigned int index)
{
    if (index >= gJoystickList.size()) {
        return (false);
    }

    if (gJoystickList[index].plugged) {
        const std::string devnode = gJoystickList[index].deviceNode;

        mFile = ::open(devnode.c_str(), O_RDONLY | O_NONBLOCK);
        if (mFile >= 0) {
            ioctl(mFile, JSIOCGAXMAP, mMapping.data());
            mIdentification.name = GetJoystickName(index);

            if (gUdevContext) {
                mIdentification.vendorID = GetJoystickVendorID(index);
                mIdentification.productID = GetJoystickProductID(index);
            }

            mState = JoystickState();
            return (true);
        }

        std::cerr << "Failed to open joystick " << devnode
                  << ": " << errno << std::endl;
    }

    return (false);
}

///////////////////////////////////////////////////////////////////////////////
void JoystickImpl::Close(void)
{
    ::close(mFile);
    mFile = -1;
}

///////////////////////////////////////////////////////////////////////////////
JoystickCaps JoystickImpl::GetCapabilities(void) const
{
    JoystickCaps caps;

    if (mFile < 0) {
        return (caps);
    }

    char buttonCount = 0;
    ioctl(mFile, JSIOCGBUTTONS, &buttonCount);
    caps.buttonCount = static_cast<unsigned int>(buttonCount);
    if (caps.buttonCount > Joystick::ButtonCount) {
        caps.buttonCount = Joystick::ButtonCount;
    }

    char axesCount = 0;
    ioctl(mFile, JSIOCGAXES, &axesCount);
    for (int i = 0; i < axesCount; i++) {
        switch (mMapping[static_cast<std::size_t>(i)]) {
            case ABS_X:         caps.axes[Joystick::Axis::X]    = true; break;
            case ABS_Y:         caps.axes[Joystick::Axis::Y]    = true; break;
            case ABS_Z:
            case ABS_THROTTLE:  caps.axes[Joystick::Axis::Z]    = true; break;
            case ABS_RZ:
            case ABS_RUDDER:    caps.axes[Joystick::Axis::R]    = true; break;
            case ABS_RX:        caps.axes[Joystick::Axis::U]    = true; break;
            case ABS_RY:        caps.axes[Joystick::Axis::V]    = true; break;
            case ABS_HAT0X:     caps.axes[Joystick::Axis::PovX] = true; break;
            case ABS_HAT0Y:     caps.axes[Joystick::Axis::PovY] = true; break;
            default:                                                    break;
        }
    }

    return (caps);
}

///////////////////////////////////////////////////////////////////////////////
Joystick::Identification JoystickImpl::GetIdentification(void) const
{
    return (mIdentification);
}

///////////////////////////////////////////////////////////////////////////////
JoystickState JoystickImpl::Update(void)
{
    if (mFile < 0) {
        mState = JoystickState();
        return (mState);
    }

    js_event joyState{};
    ssize_t result = read(mFile, &joyState, sizeof(joyState));

    while (result > 0) {
        switch (joyState.type & ~JS_EVENT_INIT) {
            case JS_EVENT_AXIS:
            {
                const float value = joyState.value * 100.f / 32767.f;

                if (joyState.number < mMapping.size()) {
                    switch (mMapping[joyState.number]) {
                        case ABS_X:
                            mState.axes[Joystick::Axis::X] = value;
                            break;
                        case ABS_Y:
                            mState.axes[Joystick::Axis::Y] = value;
                            break;
                        case ABS_Z:
                        case ABS_THROTTLE:
                            mState.axes[Joystick::Axis::Z] = value;
                            break;
                        case ABS_RZ:
                        case ABS_RUDDER:
                            mState.axes[Joystick::Axis::R] = value;
                            break;
                        case ABS_RX:
                            mState.axes[Joystick::Axis::U] = value;
                            break;
                        case ABS_RY:
                            mState.axes[Joystick::Axis::V] = value;
                            break;
                        case ABS_HAT0X:
                            mState.axes[Joystick::Axis::PovX] = value;
                            break;
                        case ABS_HAT0Y:
                            mState.axes[Joystick::Axis::PovY] = value;
                            break;
                    }
                }
                break;
            }
            case JS_EVENT_BUTTON:
            {
                if (joyState.number < Joystick::ButtonCount) {
                    mState.buttons[joyState.number] = (joyState.value != 0);
                }
                break;
            }
        }

        result = read(mFile, &joyState, sizeof(joyState));
    }

    mState.connected = (!result || (errno == EAGAIN));

    return (mState);
}

} // namespace Arc::priv

} // namespace Arc
