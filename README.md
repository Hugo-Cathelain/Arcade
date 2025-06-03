# 🕹️ ARCADE - A Retro Gaming Platform

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE.md)
[![Build Status](https://img.shields.io/badge/build-passing-green.svg)]()
[![C++](https://img.shields.io/badge/language-C++-blue.svg)]()

A modular retro gaming platform that lets users choose games to play and keeps track of player scores. Built with a dynamic library architecture for maximum extensibility.

## 📋 Project Overview

Arcade is a sophisticated gaming platform designed with modularity and extensibility at its core. The platform dynamically loads graphics libraries and games at runtime, allowing for seamless integration of new content without recompilation. This architecture ensures that the main program remains independent of specific graphics implementations while providing a rich gaming experience.

### Key Characteristics
- **Dynamic Library Architecture**: Games and graphics backends are loaded as shared libraries (.so files)
- **Runtime Flexibility**: Switch between different graphics backends and games without restarting
- **Score Management**: Persistent tracking of player scores across sessions
- **Cross-Platform Support**: Multiple graphics backends for different environments
- **Retro Gaming Focus**: Classic games like Pac-Man, Snake, and Nibbler

## ✨ Features

### 🎮 Gaming Experience
- Multiple classic arcade games (Pac-Man, Snake, Nibbler)
- Real-time score tracking and leaderboards
- Smooth gameplay with consistent frame rates
- Audio support with game-specific sound effects
- Joystick and gamepad support (including Wii Remote)

### 🖥️ Graphics Backends
- **SFML**: High-performance 2D graphics with audio
- **SDL2**: Cross-platform multimedia library
- **OpenGL**: Hardware-accelerated 3D graphics
- **NCurses**: Terminal-based interface
- **LibCaca**: ASCII art graphics library

### 🏗️ Architecture
- Plugin-based architecture with dynamic loading
- Event-driven communication system
- Modular design for easy extension
- Memory-safe library management
- Robust error handling and recovery

## 🖼️ Gallery

// TODO...

*Note: Screenshots and visual gallery can be found in the `screenshots/` directory*

## 🚀 Installation

### Prerequisites
- **Compiler**: GCC 9.0+ or Clang 10.0+ with C++17 support
- **Build System**: Make
- **Graphics Libraries**: 
  - SFML 2.5+
  - SDL2 2.0+
  - OpenGL 3.3+
  - NCurses 6.0+
  - LibCaca 0.99+

### Ubuntu/Debian
```bash
sudo apt update
sudo apt install build-essential libsfml-dev libsdl2-dev libgl1-mesa-dev libncurses5-dev libcaca-dev
```

### Fedora/CentOS
```bash
sudo dnf install gcc-c++ make SFML-devel SDL2-devel mesa-libGL-devel ncurses-devel libcaca-devel
```

### Arch Linux
```bash
sudo pacman -S gcc make sfml sdl2 mesa ncurses libcaca
```

## 🔧 Build Instructions

### Quick Build
```bash
# Clone the repository
git clone <repository-url>
cd arcade

# Build everything (core + all libraries)
make

# Clean build artifacts
make clean

# Full clean (including libraries)
make fclean

# Rebuild everything
make re
```

### Advanced Build Options
```bash
# Build only the core arcade binary
make arcade

# Build specific graphics backend
make lib/arcade_sfml.so

# Build specific game
make lib/arcade_pacman.so

# Build with debug symbols
make DEBUG=1

# Build with verbose output
make VERBOSE=1
```

### Build Output
After successful compilation, you'll find:
- `arcade`: Main executable in the root directory
- `lib/*.so`: Shared libraries for games and graphics backends
- `build/`: Compiled object files and intermediate artifacts

## 🎯 Usage

### Basic Usage
```bash
# Start with default graphics library
./arcade lib/arcade_sfml.so

# Start with specific graphics backend
./arcade lib/arcade_sdl2.so

# Display help
./arcade --help
```

### In-Game Controls
- **Arrow Keys**: Navigation and movement
- **Space**: Select/Action
- **Escape**: Return to menu/Exit
- **Enter**: Confirm selection
- **R**: Restart game
- **Tab**: Switch graphics backend (runtime)

### Game Selection
1. Launch the arcade with your preferred graphics backend
2. Use arrow keys to navigate the game menu
3. Press Space or Enter to select a game
4. Enjoy playing! Scores are automatically tracked

## 🏛️ Architecture

### Core Components

```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   Main.cpp      │    │     Core.hpp     │    │   Library.hpp   │
│   Entry Point   │────│  Game Loop &     │────│  Dynamic Loader │
│                 │    │  State Mgmt      │    │                 │
└─────────────────┘    └──────────────────┘    └─────────────────┘
         │                       │                       │
         └───────────────────────┼───────────────────────┘
                                 │
         ┌───────────────────────┼───────────────────────┐
         │                       │                       │
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│  IGameModule    │    │     API.hpp      │    │ IGraphicsModule │
│   Interface     │    │  Event System    │    │   Interface     │
│                 │    │                  │    │                 │
└─────────────────┘    └──────────────────┘    └─────────────────┘
```

### Dynamic Library System
The platform uses a sophisticated library loading mechanism:

1. **Library Discovery**: Automatic detection of available `.so` files
2. **Symbol Resolution**: Dynamic binding of `CreateArcadeObject` functions
3. **Type Safety**: Template-based object creation with proper typing
4. **Memory Management**: RAII-based cleanup with `std::shared_ptr`
5. **Error Handling**: Comprehensive error reporting with `DLError` exceptions

### Event System
- **Channel-based Events**: Separate channels for Core, Graphics, and Game events
- **Type-safe Communication**: Strongly typed event parameters
- **Asynchronous Processing**: Non-blocking event handling
- **Event Filtering**: Selective event subscription based on channels

## 📦 Dependencies

### Core Dependencies
| Library | Version | Purpose |
|---------|---------|---------|
| **Standard C++** | C++17+ | Core language features |
| **POSIX** | - | Dynamic library loading (`dlopen`, `dlsym`) |

### Graphics Backend Dependencies
| Backend | Dependencies | Description |
|---------|-------------|-------------|
| **SFML** | libsfml-dev 2.5+ | Simple and Fast Multimedia Library |
| **SDL2** | libsdl2-dev 2.0+ | Cross-platform multimedia library |
| **OpenGL** | libgl1-mesa-dev | Hardware-accelerated graphics |
| **NCurses** | libncurses5-dev | Terminal-based interface |
| **LibCaca** | libcaca-dev | ASCII art graphics |

### Audio Dependencies
| Component | Library | Purpose |
|-----------|---------|---------|
| **Audio Engine** | miniaudio | Cross-platform audio playback |
| **Sound Effects** | Built-in | Game-specific audio assets |

### Optional Dependencies
- **Joystick Support**: Linux input subsystem
- **Wii Remote**: Bluetooth libraries for gamepad support

## 🤝 Contributing

We welcome contributions! Please read our [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

### Development Setup
1. Fork the repository
2. Create a feature branch: `git checkout -b feature/amazing-feature`
3. Follow our coding conventions (see `doc/documentation.md`)
4. Test your changes thoroughly
5. Submit a pull request

### Adding New Games
See the detailed guide in our [documentation](doc/documentation.md#adding-a-new-game) for implementing new games that follow the `IGameModule` interface.

### Adding Graphics Backends
Instructions for implementing new graphics backends can be found in our [documentation](doc/documentation.md#adding-a-new-graphical-backend).

## 📄 License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.

## 👥 Authors

- **Mallory SCOTTON** - Core Architecture & Game Development
- **Hugo CATHELAIN** - Graphics Backends & Audio System  
- **Nathan FIEVET** - Library Management & Platform Integration

See [AUTHORS.md](AUTHORS.md) for detailed contribution information.

## 🔍 Additional Resources

- 📖 **[Complete Documentation](doc/documentation.pdf)** - Comprehensive technical documentation

## 📫 Contact

For questions or feedback, reach out at: `mscotton.pro@gmail.com`

---

*Built with ❤️ for retro gaming enthusiasts*
