#include "Window.hpp"
#include "Widgets.hpp"
#include <SDL3/SDL_ttf.h>
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
			printf(SDL_GetError());
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
	void Window::setSize(unsigned int w, unsigned int h) {
		SCREEN_WIDTH = w;
		SCREEN_HEIGHT = h;

		SDL_SetWindowSize(window, 800, 600);
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
		for (auto& b : widgets) {
			if (b->visible) {
				int x = 0, y = 0;
				switch (b->anchor) {
				case ANCHOR::NORTH_WEST:
					x = b->x;
					y = b->y;
					break;
				case ANCHOR::NORTH_EAST:
					x = b->x - (SCREEN_BASE_WIDTH - SCREEN_WIDTH);
					y = b->y;
					break;
				case ANCHOR::SOUTH_WEST:
					x = b->x;
					y = b->y - (SCREEN_BASE_HEIGHT - SCREEN_HEIGHT);
					break;
				case ANCHOR::SOUTH_EAST:
					x = b->x - (SCREEN_BASE_WIDTH - SCREEN_WIDTH);
					y = b->y - (SCREEN_BASE_HEIGHT - SCREEN_HEIGHT);
					break;
				}
				b->draw(this, &*b, x, y);
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
				for (int i = 0; i < widgets.size(); i++) {
					if (widgets[i]->Type & WIDGET_TYPE_CLICKABLE) {
						if (mousex >= widgets[i]->x &&
							mousey >= widgets[i]->y &&
							mousex <= widgets[i]->x + widgets[i]->sizex &&
							mousey <= widgets[i]->y + widgets[i]->sizey) {
							((Button*)widgets[i])->OnClick(((Button*)widgets[i])->ptr);
							break;
						}
					}
					else if (widgets[i]->Type & WIDGET_TYPE_FOCUSABLE) {
						if (mousex >= widgets[i]->x &&
							mousey >= widgets[i]->y &&
							mousex <= widgets[i]->x + widgets[i]->sizex &&
							mousey <= widgets[i]->y + widgets[i]->sizey) {
							focused = widgets[i];
							break;
						}
					}
					else if (widgets[i]->Type & WIDGET_TYPE_DRAGGABLE) {
						if (mousex >= widgets[i]->x &&
							mousey >= widgets[i]->y &&
							mousex <= widgets[i]->x + widgets[i]->sizex &&
							mousey <= widgets[i]->y + widgets[i]->sizey) {
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
					if (focused->Type & WIDGET_TYPE_DRAGGABLE) {
						((Scale*)focused)->Drag(focused, e.motion.x, e.motion.y);
						((Scale*)focused)->OnDrag(focused, e.motion.x, e.motion.y);
					}
				}
			}
		}
		else if (e.type == KEYDOWN) {
			if (focused != nullptr) {
				if (focused->Type & WIDGET_TYPE_KBTYPABLE) {
					if (e.key.scancode == 42) {
						if (((Entry*)focused)->value.size() > 0)
							((Entry*)focused)->value.pop_back();
					}
					else ((Entry*)focused)->value += SDL_GetKeyName(e.key.key);
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