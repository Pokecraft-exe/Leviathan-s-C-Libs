# Leviathan's C++ Libraries

A lightweight C++ utility library providing a custom `LinkedList` container and a simplified SDL3 windowing/widget layer (`SimplerDirectMediaLayer`). Designed for use in game and application development with SDL3.

---

## Table of Contents

- [Overview](#overview)
- [Project Structure](#project-structure)
- [Dependencies](#dependencies)
- [Building](#building)
- [Modules](#modules)
  - [Iterables::LinkedList](#iterableslinkedlist)
  - [SimplerDirectMediaLayer::Window](#simplerdirectmedialayerwindow)
  - [SimplerDirectMediaLayer Widgets](#simplerdirectmedialayer-widgets)
- [Usage Examples](#usage-examples)
- [Extending Widgets](#extending-widgets)
- [License](#license)

---

## Overview

This library provides two main components:

- **`Iterables::LinkedList<T>`** — A templated doubly-linked list with forward iterator support, suitable as a drop-in for custom memory-managed containers.
- **`SimplerDirectMediaLayer`** — A thin C++ wrapper around SDL3 that simplifies window creation, rendering, event polling, and GUI widgets (buttons, labels, images, text entries, checkboxes, sliders, and panels).

---

## Project Structure

```
.
├── includes/
│   ├── Iterables/
│   │   └── LinkedList.hpp        # Templated doubly-linked list
│   └── SDL/
│       ├── Window.hpp            # Window and renderer wrapper
│       ├── Widgets.hpp           # Widget definitions (Button, Label, etc.)
│       └── SDL3/                 # SDL3 header bundle
│           ├── SDL.h
│           └── ... (full SDL3 headers)
└── src/
    ├── Iterables/
    │   └── LinkedList.cpp
    └── SDL/
        ├── Window.cpp            # Window implementation
        └── Widget.cpp            # Widget implementations
```

---

## Dependencies

### Required Libraries

This library **requires** the following SDL3 libraries to be present on your system or linked into your build.

#### Static / Import Libraries (link at compile time)

| Library | Purpose |
|---|---|
| `SDL3.lib` | Core SDL3 windowing, rendering, input, events |
| `SDL3_image.lib` | Image loading (PNG, JPG, BMP, etc.) for the `Image` widget |
| `SDL3_ttf.lib` | TrueType font rendering for `Label`, `Button`, `Entry` widgets |

#### Dynamic Libraries (must be present at runtime)

| Library | Purpose |
|---|---|
| `SDL3.dll` | SDL3 runtime |
| `SDL3_image.dll` | SDL3_image runtime |
| `SDL3_ttf.dll` | SDL3_ttf runtime |

> **Note:** The dynamic libraries (`.dll` on Windows, `.so` on Linux, `.dylib` on macOS) must be placed alongside your executable or be available on your system's library path at runtime. Without them, the application will fail to launch even if it compiled successfully.

### Platform Notes

- **Windows:** The `Window.hpp` header assumes fonts are located at `C:\Windows\Fonts\`. This is automatically set via the `FONTS_DIR` constant for Win32/Win64 builds.
- **Linux / macOS:** You will need to provide a font path manually when using text-rendering features.

---

## Building

### Include Paths

Add the following to your compiler's include directories:

```
includes/
includes/SDL/
```

### Linker Settings (Windows / MSVC example)

```
SDL3.lib
SDL3_image.lib
SDL3_ttf.lib
```

### CMake Example

```cmake
cmake_minimum_required(VERSION 3.20)
project(MyProject)

set(CMAKE_CXX_STANDARD 17)

# Add library sources
add_library(leviathan_libs
    src/Iterables/LinkedList.cpp
    src/SDL/Window.cpp
    src/SDL/Widget.cpp
)

target_include_directories(leviathan_libs PUBLIC
    includes/
    includes/SDL/
)

# Link SDL3 dependencies
target_link_libraries(leviathan_libs
    SDL3
    SDL3_image
    SDL3_ttf
)

# Your application
add_executable(MyApp main.cpp)
target_link_libraries(MyApp leviathan_libs)
```

---

## Modules

### `Iterables::LinkedList`

A templated doubly-linked list stored in `includes/Iterables/LinkedList.hpp`.

**Key features:**

- `push_front(T)` / `push_back(T)` — insert at either end
- `pop_front()` / `pop_back()` — remove and return from either end
- `insert(T, size_t pos)` — insert at position
- `remove(size_t pos)` — remove by index
- `erase(iterator)` — remove by iterator
- `eraseIf(predicate)` — remove all elements matching a condition
- `operator[](int)` — random access by index
- `size()` — element count
- `copy()` — deep copy of the list
- `is(other)` — equality comparison
- `compare(other, comparator)` — comparison with a custom binary predicate
- Forward iterator (`begin()` / `end()`) compatible with range-based for loops

> **Memory:** Nodes are allocated with `malloc`/`free`. Objects stored in the list should be trivially copyable or have their copy handled carefully, as `memcpy` is used internally for data copying.

---

### `SimplerDirectMediaLayer::Window`

Defined in `includes/SDL/Window.hpp` and implemented in `src/SDL/Window.cpp`.

Wraps `SDL_Window` and `SDL_Renderer` with a convenient C++ interface.

**Construction:**

```cpp
Window();                                      // default 640x480
Window(const char* name);
Window(const char* name, unsigned int w, unsigned int h);
```

**Key methods:**

| Method | Description |
|---|---|
| `pollEvent()` | Poll the SDL event queue; returns false on quit |
| `drawWidgets()` | Draw all attached widgets to the renderer |
| `render()` | Present the current frame |
| `clear()` | Clear the renderer |
| `fill(uint32_t color)` | Fill screen with a color |
| `drawRect(...)` | Draw a filled rectangle |
| `drawLine(...)` | Draw a line |
| `drawString(...)` | Render a string using the loaded TTF font |
| `drawChar(...)` | Render a single character |
| `setResizable()` | Make the window resizable |
| `setFullScreen()` | Switch to fullscreen |
| `setWindowed()` | Switch back to windowed mode |
| `setIcon(path)` | Set the window icon from an image file |
| `setSize(w, h)` | Resize the window |
| `getEvent()` | Access the current `SDL_Event` |
| `getSizeX/Y()` | Current window dimensions |
| `destroy()` | Destroy the window and free resources |

**Exception types** thrown on failure:

- `Window::WindowNotCreatedException`
- `Window::RendererNotCreatedException`
- `Window::FontNotLoadedException`
- `Window::WindowException`

---

### `SimplerDirectMediaLayer` Widgets

Defined in `includes/SDL/Widgets.hpp`. All widgets inherit from `BaseWidget` and can be attached to a `Window` via `widget.attach(window)`.

| Widget | Description |
|---|---|
| `Label` | Static text with a color |
| `Image` | Loads and displays an image file via SDL_image |
| `Button` | Clickable button with text, color, and an `OnClick` callback |
| `Entry` | Text input field with placeholder support |
| `CheckBox` | Toggle checkbox derived from `Button` |
| `Scale` | Draggable slider (horizontal or vertical) with an `OnDrag` callback |
| `Panel` | Scrollable sub-container that hosts its own widget layer |

**Widget type flags** (bitfield, stored in `BaseWidget::Type`):

```cpp
WIDGET_TYPE_NULL       // 0b00000000
WIDGET_TYPE_CLICKABLE  // 0b00000001
WIDGET_TYPE_KBTYPABLE  // 0b00000010
WIDGET_TYPE_FOCUSABLE  // 0b00000100
WIDGET_TYPE_DRAGGABLE  // 0b00001000
```

**Anchor positions** control how widget coordinates are interpreted:

```cpp
NORTH_WEST  // top-left (default)
NORTH_EAST  // top-right
SOUTH_WEST  // bottom-left
SOUTH_EAST  // bottom-right
```

---

## Usage Examples

### Basic window loop

```cpp
#include "SDL/Window.hpp"

int main() {
    SimplerDirectMediaLayer::Window window("My App", 800, 600);

    while (window.pollEvent()) {
        window.clear();
        window.fill(0x1E1E2EFF);
        window.drawWidgets();
        window.render();
    }

    return 0;
}
```

### Adding a button

```cpp
#include "SDL/Window.hpp"
#include "SDL/Widgets.hpp"

using namespace SimplerDirectMediaLayer;

int onButtonClick(void* ptr) {
    printf("Button clicked!\n");
    return 0;
}

int main() {
    Window window("Demo", 640, 480);

    Button btn("Click Me", 100, 100, 120, 40, 0xFF5500FF, onButtonClick, nullptr);
    btn.attach(window);

    while (window.pollEvent()) {
        window.clear();
        window.fill(0x222222FF);
        window.drawWidgets();
        window.render();
    }

    return 0;
}
```

### Using LinkedList

```cpp
#include "Iterables/LinkedList.hpp"
#include <stdio.h>

int main() {
    Iterables::LinkedList<int> list;

    list.push_back(10);
    list.push_back(20);
    list.push_back(30);
    list.push_front(5);

    for (auto& val : list) {
        printf("%d\n", val);  // 5, 10, 20, 30
    }

    list.eraseIf([](int& v) { return v % 2 == 0; });
    // list now contains: 5

    return 0;
}
```

---

## Extending Widgets

### Custom draw functions

Every widget holds a `DRAW_CALL* draw` function pointer with the signature:

```cpp
void myDraw(void* widget, void* windowPtr, int x, int y);
```

You can swap it out on any widget instance after construction to completely override how it is rendered, without subclassing anything:

```cpp
void myCustomDraw(void* widget, void* windowPtr, int x, int y) {
    auto* btn = static_cast<SimplerDirectMediaLayer::Button*>(widget);
    auto* win = static_cast<SimplerDirectMediaLayer::Window*>(windowPtr);
    // draw whatever you like using win's draw primitives
    win->drawRect(x, y, btn->sizex, btn->sizey, 0x00FF88FF);
    win->drawString(btn->text, x + 4, y + 4, 0xFFFFFFFF);
}

// somewhere after construction:
myButton.draw = myCustomDraw;
```

### Creating your own widgets

You are free to create custom widgets by inheriting from any of the provided base classes. `BaseWidget` is the root of the hierarchy and is the correct starting point for fully custom widgets:

```cpp
#include "SDL/Widgets.hpp"

class ProgressBar : public SimplerDirectMediaLayer::BaseWidget {
public:
    float progress = 0.0f;  // 0.0 to 1.0
    uint32_t fillColor  = 0x00AA55FF;
    uint32_t emptyColor = 0x333333FF;

    ProgressBar(int x, int y, int w, int h) {
        this->x     = x;
        this->y     = y;
        this->sizex = w;
        this->sizey = h;
        this->draw  = ProgressBar::drawFn;
        this->Type  = WIDGET_TYPE_NULL;
    }

    static void drawFn(void* self, void* winPtr, int x, int y) {
        auto* bar = static_cast<ProgressBar*>(self);
        auto* win = static_cast<SimplerDirectMediaLayer::Window*>(winPtr);
        win->drawRect(x, y, bar->sizex, bar->sizey, bar->emptyColor);
        win->drawRect(x, y, (int)(bar->sizex * bar->progress), bar->sizey, bar->fillColor);
    }
};
```

You can also inherit from a more specific class (e.g. `Button`, `Scale`) if you want to keep its existing interaction logic and only change appearance or add data members.

### ⚠ Do not modify widget type constants

The `WIDGET_TYPE_*` constants and the `BaseWidget::Type` bitfield **must not be redefined, reordered, or given conflicting values**. The library's internal event dispatch and hit-testing code uses raw pointer arithmetic and bitfield checks that depend on the exact numeric values and memory layout established by these constants. Changing them will silently corrupt event routing and may cause crashes.

If you need a widget that combines behaviours (e.g. both clickable and draggable) simply OR the existing flags together when setting `Type` on your custom class — never invent new flag bits:

```cpp
// correct — combine existing flags
this->Type = WIDGET_TYPE_CLICKABLE | WIDGET_TYPE_DRAGGABLE;

// WRONG — do not define new flag values
// constexpr int WIDGET_TYPE_MYTYPE = 0b00010000;  ← breaks the library
```

---

## License

This project is licensed under the **Creative Commons Attribution 4.0 International (CC BY 4.0)** license.

[![CC BY 4.0](https://licensebuttons.net/l/by/4.0/88x31.png)](https://creativecommons.org/licenses/by/4.0/)

You are free to:

- **Share** — copy and redistribute the material in any medium or format
- **Adapt** — remix, transform, and build upon the material for any purpose, even commercially

Under the following terms:

- **Attribution** — You must give appropriate credit to the original author, provide a link to the license, and indicate if changes were made. You may do so in any reasonable manner, but not in any way that suggests the author endorses you or your use.

> Full license text: https://creativecommons.org/licenses/by/4.0/legalcode

---

SDL3, SDL3_image, and SDL3_ttf are separate projects licensed under the **zlib license** and are not covered by the above. See [https://www.libsdl.org](https://www.libsdl.org) for details.
