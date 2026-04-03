#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_ttf.h>
#include <string>
#include <vector>
#include "LinkedList.hpp"

namespace SimplerDirectMediaLayer {
	class BaseWidget;
	class Panel;

	constexpr int QUIT = SDL_EVENT_QUIT;
	constexpr int KEYDOWN = SDL_EVENT_KEY_DOWN;
	constexpr int KEYUP = SDL_EVENT_KEY_UP;
	constexpr int MOUSEMOTION = SDL_EVENT_MOUSE_MOTION;
	constexpr int MOUSEPRESSED = SDL_EVENT_MOUSE_BUTTON_DOWN;
	constexpr int MOUSERELEASED = SDL_EVENT_MOUSE_BUTTON_UP;
	constexpr int GUI_BUTTONPRESSED = 0x9345;

#if defined(_WIN32) || defined(_WIN64)
	constexpr char FONTS_DIR[] = "C:\\Windows\\Fonts\\";
#endif

	class Window {
	private:
		using SDL_Font = TTF_Font;
		SDL_Window* window = nullptr;
		SDL_Renderer* renderer = nullptr;
		SDL_Surface* icon = nullptr;
		SDL_Font* font = nullptr;
		std::string		w_name = "";
		SDL_Event		e = {};

		unsigned int SCREEN_HEIGHT = 480;
		unsigned int SCREEN_WIDTH = 640;
		unsigned int SCREEN_BASE_HEIGHT = 480;
		unsigned int SCREEN_BASE_WIDTH = 640;

		bool isMouseDown = false;
		bool created = 0;
		bool resizable = false;

		static bool SDL_INITIALIZED;
		static bool TTF_INITIALIZED;
		static Iterables::LinkedList<Window*> CREATED_WINDOWS;

		bool testClip(int x, int y);
		void createWindow(std::string name, unsigned int w, unsigned int h, bool resizable);
	public:
		BaseWidget* focused = nullptr;
		Iterables::LinkedList<BaseWidget*> widgets = {};

		class WindowNotCreatedException : public std::exception {
		public:
			WindowNotCreatedException() : exception("Failed to create Window.") {};
		};

		class RendererNotCreatedException : public std::exception {
		public:
			RendererNotCreatedException() : exception("Failed to create Renderer.") {};
		};

		class FontNotLoadedException : public std::exception {
		public:
			FontNotLoadedException() : exception("Failed to load font.") {};
		};

		class WindowException : public std::exception {
		public:
			WindowException(const char* message) : exception(message) {
				printf("Exception thrown %s\n", message);
			}
		};

		Window();
		Window(const char* name);
		Window(const char* name, unsigned int w, unsigned int h);

		inline int	const		getSizeX() { return SCREEN_WIDTH; }
		inline int	const		getSizeY() { return SCREEN_HEIGHT; }
		inline int	const		getBaseSizeX() { return SCREEN_BASE_WIDTH; }
		inline int	const		getBaseSizeY() { return SCREEN_BASE_HEIGHT; }
		inline bool	const		getCreated() { return created; }
		inline bool	const		getResizable() { return resizable; }
		inline uint32_t const	getPixel(uint16_t x, uint16_t y);
		inline SDL_Window** getSDLWindow() { return &window; }
		inline SDL_Renderer* getSDLRenderer() { return renderer; };
		inline SDL_Event& getEvent() { return e; }

		void setResizable();
		void setIcon(std::string path);
		void setSize(unsigned int w, unsigned int h);
		void setName(std::string name);
		void setMousePos(int x, int y);
		void setFullScreen();
		void setWindowed();

		void setColor(uint32_t color);
		void fastPutPixel(unsigned int x, unsigned int y);
		void fastPutPixel(unsigned int x, unsigned int y, uint32_t color);
		void putPixel(unsigned int x, unsigned int y);
		void putPixel(unsigned int x, unsigned int y, uint32_t color);
		void drawLine(int x1, int y1, int x2, int y2, uint32_t color);
		void drawRect(int x, int y, int size_x, int size_y, uint32_t color);
		void drawChar(char c, uint16_t x, uint16_t y, uint32_t color);
		void drawString(std::string _string, uint16_t x, uint16_t y, uint32_t color);
		void drawWidgets();
		void clear();
		void render();
		void fill(uint32_t c);
		void clip(int& x, int& y);
		bool pollEvent();
		static inline bool showCursor() { return SDL_ShowCursor(); };
		static inline bool hideCursor() { return SDL_HideCursor(); };

		void destroy(); // Only destroy the window, also deconstructor
		~Window();
		friend class Panel;
	};
};