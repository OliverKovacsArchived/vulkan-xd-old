# vulkan-xd

[![Open in Visual Studio Code](https://open.vscode.dev/badges/open-in-vscode.svg)](https://open.vscode.dev/OliverKovacs/vulkan-xd)
[![License](https://img.shields.io/github/license/OliverKovacs/vulkan-xd)](https://github.com/OliverKovacs/vulkan-xd/blob/main/LICENSE.md)
[![Size](https://img.shields.io/github/repo-size/OliverKovacs/vulkan-xd)]()
[![Vulkan](https://img.shields.io/badge/dependencies-Vulkan-green)](https://www.vulkan.org/)
[![GLFW](https://img.shields.io/badge/dependencies-GLFW-green)](https://github.com/glfw/glfw)
[![GLM](https://img.shields.io/badge/dependencies-GLM-green)](https://github.com/g-truc/glm)
[![STB](https://img.shields.io/badge/dependencies-stb-green)](https://github.com/nothings/stb)

Higher-dimensional renderer implemented in [Vulkan](https://www.vulkan.org/). This is mainly a reseach project. 

Documentation will be available in the future.

## Linux

### Prerequisites

You need to have [Git](https://git-scm.com/), [CMake](https://cmake.org/), [Make](https://www.gnu.org/software/make/) and [Clang](https://clang.llvm.org/) installed on your system.

Make sure you have Vulkan drivers installed and your hardware supports Vulkan. 

Download the Vulkan SDK from [here](https://vulkan.lunarg.com/sdk/home).

Follow the setup [instructions](https://vulkan.lunarg.com/doc/sdk/1.2.182.0/linux/getting_started.html).

### Download
```bash
git clone https://github.com/OliverKovacs/vulkan-xd
cd vulkan-xd
```

### Building
```bash
cmake -S . -B build -G "Unix Makefiles"
cmake --build build
```

### Running
```bash
cd build
./main
```

## Windows
Windows is currently not supported, but it will be in the future.

## Known Issues
VS Code IntelliSense is sometimes broken on Windows.

## Acknowledgements
The basic Vulkan boilerplate code is largely based on [Alexander Overvoorde](https://github.com/Overv)'s [Vulkan Tutorial](https://vulkan-tutorial.com/).

## Author
[Oliver Kovacs](https://github.com/OliverKovacs)

## License
MIT
