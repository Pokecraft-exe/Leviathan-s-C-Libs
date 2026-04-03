#include "Window.hpp"
#include "Widgets.hpp"
#include <SDL3/SDL_ttf.h>
#include <iostream>
using Iterables::LinkedList;

namespace SimplerDirectMediaLayer
{
	bool Window::SDL_INITIALIZED = false;
	bool Window::TTF_INITIALIZED = false;
	LinkedList<Window*> Window::CREATED_WINDOWS = {};

	Window::Window() {
		if (!SDL_INITIALIZED) {
			SDL_INITIALIZED = SDL_Init(SDL_INIT_VIDEO);
			TTF_INITIALIZED = TTF_Init();
		}
		CREATED_WINDOWS.push_back(this);
		createWindow("window", 640, 480, false);
	}
	Window::Window(const char* name) {
		if (!SDL_INITIALIZED) {
			SDL_INITIALIZED = SDL_Init(SDL_INIT_VIDEO);
			TTF_INITIALIZED = TTF_Init();
		}
		CREATED_WINDOWS.push_back(this);
		createWindow(name, 640, 480, false);
	}
	Window::Window(const char* name, unsigned int w, unsigned int h) {
		if (!SDL_INITIALIZED) {
			SDL_INITIALIZED = SDL_Init(SDL_INIT_VIDEO);
			TTF_INITIALIZED = TTF_Init();
		}
		CREATED_WINDOWS.push_back(this);
		createWindow(name, w, h, false);
	}
	Window::~Window() {
		CREATED_WINDOWS.eraseIf([this](Window* w) { return w == this; });
		if (CREATED_WINDOWS.size() == 0) {
			if (SDL_INITIALIZED) SDL_Quit();
			if (TTF_INITIALIZED) TTF_Quit();
		}
		destroy();
	}

	bool Window::testClip(int x, int y)
	{
		if (x < 1 &&
			x >= SCREEN_WIDTH - 1 &&
			y < 1 &&
			y >= SCREEN_HEIGHT - 1) return true;
		return false;
	}

	void Window::createWindow(std::string name, unsigned int w, unsigned int h, bool resizable) {
		w_name = name;
		SCREEN_BASE_WIDTH = w;
		SCREEN_BASE_HEIGHT = h;
		SCREEN_WIDTH = w;
		SCREEN_HEIGHT = h;
		window = nullptr;

		window = SDL_CreateWindow(name.data(), SCREEN_WIDTH, SCREEN_HEIGHT, (resizable ? SDL_WINDOW_RESIZABLE : 0));
		if (window == nullptr) {
			throw WindowNotCreatedException();
		}
		renderer = SDL_CreateRenderer(window, 0);
		if (renderer == nullptr) {
			SDL_DestroyWindow(window);
			window = nullptr;
			throw RendererNotCreatedException();
		}
		font = TTF_OpenFont((std::string(FONTS_DIR) + "arial.ttf").c_str(), 48);
		if (font == nullptr) {
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			printf("%s\n", SDL_GetError());
			window = nullptr;
			renderer = nullptr;
			throw FontNotLoadedException();
		}

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);

		this->resizable = resizable;
		this->created = 1;

		return;
	}

	uint32_t const Window::getPixel(uint16_t x, uint16_t y) {
		SDL_Rect r = { x, y, 1, 1 };
		SDL_Surface* s = SDL_RenderReadPixels(renderer, &r);
		return *((uint32_t*)s->pixels);
	};

	void Window::setResizable() {
		destroy();

		created = 0;

		this->createWindow(w_name, SCREEN_WIDTH, SCREEN_HEIGHT, !resizable);
	}
	void Window::setIcon(std::string path) {
		icon = SDL_LoadBMP(path.data());
		SDL_SetWindowIcon(window, icon);
	}
	void Window::setSize(unsigned int w, unsigned int h, bool reset) {
		if (reset) {
			SCREEN_BASE_WIDTH = w;
			SCREEN_BASE_HEIGHT = h;
		}

		SCREEN_WIDTH = w;
		SCREEN_HEIGHT = h;

		SDL_SetWindowSize(window, w, h);
		SDL_SyncWindow(window);
	}
	void Window::setName(std::string name) {
		w_name = name;
		SDL_SetWindowTitle(window, name.c_str());
	}
	void Window::setMousePos(int x, int y) {
		SDL_WarpMouseInWindow(window, x, y);
	}
	void Window::setFullScreen() {
		SDL_SetWindowFullscreen(window, 1);
	}
	void Window::setWindowed() {
		SDL_SetWindowFullscreen(window, 0);
	}
	void Window::setColor(uint32_t color) {
		uint8_t r = (color >> 16) & 0xFF;
		uint8_t g = (color >> 8) & 0xFF;
		uint8_t b = color & 0xFF;

		SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
	}
	void Window::fastPutPixel(unsigned int x, unsigned int y) {
		if (created)
			SDL_RenderPoint(renderer, x, y);
	}
	void Window::fastPutPixel(unsigned int x, unsigned int y, uint32_t color) {
		uint8_t r = (color >> 16) & 0xFF;
		uint8_t g = (color >> 8) & 0xFF;
		uint8_t b = color & 0xFF;

		SDL_SetRenderDrawColor(renderer, r, g, b, 255);
		if (created)
			SDL_RenderPoint(renderer, x, y);
	}
	void Window::putPixel(unsigned int x, unsigned int y) {
		if (x < 1) return;
		if (x >= SCREEN_WIDTH - 1) { x = SCREEN_WIDTH - 1; }
		if (y < 1) return;
		if (y >= SCREEN_HEIGHT - 1) { y = SCREEN_HEIGHT - 1; }
		if (created)
			SDL_RenderPoint(renderer, x, y);
	}
	void Window::putPixel(unsigned int x, unsigned int y, uint32_t color) {
		if (x < 1) return;
		if (x >= SCREEN_WIDTH - 1) { x = SCREEN_WIDTH - 1; }
		if (y < 1) return;
		if (y >= SCREEN_HEIGHT - 1) { y = SCREEN_HEIGHT - 1; }

		uint8_t r = (color >> 16) & 0xFF;
		uint8_t g = (color >> 8) & 0xFF;
		uint8_t b = color & 0xFF;

		SDL_SetRenderDrawColor(renderer, r, g, b, 255);
		if (created)
			SDL_RenderPoint(renderer, x, y);
	}
	void Window::drawLine(int x1, int y1, int x2, int y2, uint32_t c)
	{
		setColor(c);
		SDL_RenderLine(renderer, (float)x1, (float)y1, (float)x2, (float)y2);
	}
	void Window::drawRect(int x, int y, int size_x, int size_y, uint32_t color) {
		if (!created) return;
		SDL_FRect r = { (float)x, (float)y, (float)size_x, (float)size_y };
		setColor(color);
		SDL_RenderFillRect(renderer, &r);
	}
	void Window::drawChar(char c, uint16_t x, uint16_t y, uint32_t color) {
		if (!created) return;
		drawString(std::string(1, c), x, y, color);
	}
	void Window::drawString(std::string text, uint16_t x, uint16_t y, uint32_t color) {
		if (!created) return;
		SDL_Color sdl_color = { (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, (color >> 24) & 0xFF };
		SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), text.length(), sdl_color);
		if (textSurface == nullptr) {
			throw WindowException(SDL_GetError());
		}
		SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
		SDL_FRect renderQuad = { (float)x, (float)y, (float)textSurface->w, (float)textSurface->h };

		SDL_RenderTexture(renderer, textTexture, NULL, &renderQuad);

		SDL_DestroyTexture(textTexture);
		SDL_DestroySurface(textSurface);

		return;
	}
	void Window::drawWidgets() {
		if (!created) return;
		if (widgets.size() == 0) return;
		std::array<int, 2> pos = { 0, 0 };
		for (auto& b : widgets) {
			if (b->visible) {
				pos = getWidgetAbsolutPosition(*b);
				b->draw(this, b, pos[0], pos[1]);
			}
		}
	}
	void Window::clear() {
		if (!created) return;
		fill(0);
	}
	void Window::render() {
		if (!created) return;
		SDL_RenderPresent(renderer);
	}
	void Window::fill(uint32_t color) {
		if (!created) return;
		setColor(color);
		SDL_RenderClear(renderer);
	}
	std::array<int, 2> Window::getWidgetAbsolutPosition(BaseWidget& b) {
		int x = b.x, y = b.y;
		switch (b.anchor) {
			case ANCHOR::NORTH_EAST:
				x += (b.parent->SCREEN_WIDTH - b.parent->SCREEN_BASE_WIDTH);
				break;
			case ANCHOR::SOUTH_WEST:
				y -= (b.parent->SCREEN_BASE_HEIGHT - b.parent->SCREEN_HEIGHT);
				break;
			case ANCHOR::SOUTH_EAST:
				x -= (b.parent->SCREEN_BASE_WIDTH - b.parent->SCREEN_WIDTH);
				y -= (b.parent->SCREEN_BASE_HEIGHT - b.parent->SCREEN_HEIGHT);
				break;
		}
		return { x, y };
	}
	void Window::clip(int& x, int& y)
	{
		if (x <= 1) x = 1;
		if (x >= SCREEN_WIDTH - 1) { x = SCREEN_WIDTH - 1; }
		if (y <= 1) y = 1;
		if (y >= SCREEN_HEIGHT - 1) { y = SCREEN_HEIGHT - 1; }
	}
	bool Window::pollEvent() {
		if (!created) return false;
		int r = SDL_PollEvent(&e);
		if (r == 0) return 0;
		if (e.type == MOUSEPRESSED) {
			if (e.button.button == SDL_BUTTON_LEFT) {
				focused = nullptr;
				float mousex, mousey;
				SDL_GetMouseState(&mousex, &mousey);
				std::array<int, 2> pos = { 0, 0 };

				for (int i = 0; i < widgets.size(); i++) {
					std::cout << "Checking widget " << i << " at " << widgets[i]->x << ", " << widgets[i]->y << " with size " << widgets[i]->sizex << ", " << widgets[i]->sizey << " Type: ";
					if (widgets[i]->Type & WIDGET_TYPE::CLICKABLE) std::cout << "CLICKABLE ";
					if (widgets[i]->Type & WIDGET_TYPE::KBTYPABLE) std::cout << "KBTYPABLE ";
					if (widgets[i]->Type & WIDGET_TYPE::FOCUSABLE) std::cout << "FOCUSABLE ";
					if (widgets[i]->Type & WIDGET_TYPE::DRAGGABLE) std::cout << "DRAGGABLE ";
					std::cout << "\n";

					pos = getWidgetAbsolutPosition(*widgets[i]);
		
					if (widgets[i]->Type & WIDGET_TYPE::CLICKABLE) {
						if (mousex >= pos[0] &&
							mousey >= pos[1] &&
							mousex <= pos[0] + widgets[i]->sizex &&
							mousey <= pos[1] + widgets[i]->sizey &&
							widgets[i]->onClick != nullptr) {
							widgets[i]->onClick(widgets[i]->onClickPtr);
							break;
						}
					}
					if (widgets[i]->Type & WIDGET_TYPE::FOCUSABLE) {
						if (mousex >= pos[0] &&
							mousey >= pos[1] &&
							mousex <= pos[0] + widgets[i]->sizex &&
							mousey <= pos[1] + widgets[i]->sizey) {
							std::cout << "Focus set to widget " << i << "\n";
							focused = widgets[i];
							break;
						}
					}
				}
				isMouseDown = true;
			}
		}
		else if (e.type == MOUSERELEASED) {
			isMouseDown = false;
		}
		else if (e.type == MOUSEMOTION) {
			if (focused != nullptr) {
				if (isMouseDown) {
					if (focused->Type & WIDGET_TYPE::DRAGGABLE &&
						focused->drag != nullptr) {
						focused->drag(focused, e.motion.x, e.motion.y);
						if (focused->onDrag != nullptr)
							focused->onDrag(focused, e.motion.x, e.motion.y);
					}
				}
			}
		}
		else if (e.type == KEYDOWN) {
			if (focused != nullptr) {
				if (focused->Type & WIDGET_TYPE::KBTYPABLE) {
					if (e.key.scancode == 42) {
						if (focused->keyboardValue.size() > 0)
							focused->keyboardValue.pop_back();
					}
					else focused->keyboardValue += SDL_GetKeyName(e.key.key);
				}
			}
		}
		else if (e.window.type == SDL_EVENT_WINDOW_RESIZED) {
			int posx = 0, posy = 0;
			SDL_GetWindowPosition(window, &posx, &posy);
			setSize(e.window.data1, e.window.data2);
		}
		return r;
	}
	void Window::destroy() {
		if (!created) return;
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		TTF_CloseFont(font);
		window = nullptr;
		renderer = nullptr;
		font = nullptr;
		created = 0;
	}
};