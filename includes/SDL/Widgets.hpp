#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>
#include <stddef.h>
#include <string>
#include "Window.hpp"

namespace SimplerDirectMediaLayer {
	typedef void DRAW_CALL(void*, void*, int, int);
	typedef int FUNCTION_CALL(void*);
	typedef int DRAG_CALL(void*, int, int);

	enum WIDGET_TYPE {
		NULL_TYPE = 0b00000000,
		CLICKABLE = 0b00000001,
		KBTYPABLE = 0b00000010,
		FOCUSABLE = 0b00000100,
		DRAGGABLE = 0b00001000,
	};

	enum ANCHOR {
		NORTH_WEST = 0,
		NORTH_EAST,
		SOUTH_WEST,
		SOUTH_EAST,
	};

	class BaseWidget {
	public:
		BaseWidget() {};
		BaseWidget(const BaseWidget& b);
		BaseWidget(DRAW_CALL* _draw, int _x, int _y, int _sizex, int _sizey, ANCHOR _anchor, size_t _id, void* ptr);
		void attach(Window& w);
		Window* parent = nullptr;
		DRAW_CALL* draw = nullptr;
		DRAG_CALL* drag = nullptr;
		FUNCTION_CALL* onClick = nullptr;
		void* onClickPtr = nullptr;
		DRAG_CALL* onDrag = nullptr;
		std::string keyboardValue = "";
		int x = 0;
		int y = 0;
		int sizex = 0;
		int sizey = 0;
		int Type = WIDGET_TYPE::NULL_TYPE;
		ANCHOR anchor = NORTH_WEST;
		size_t id = 0;
		bool visible = true;
		~BaseWidget() {
			if (parent == nullptr) return;
			parent->widgets.eraseIf([this](BaseWidget* w) { return w == this; });
		}
	};

	class Label : public BaseWidget {
	public:
		Label() {}
		Label(std::string text, uint32_t color, int x, int y);
		std::string text = "";
		uint32_t color = 0;
	};

	class Image : public BaseWidget {
	public:
		Image() {}
		Image(std::string image, int x, int y);
		SDL_Surface* image = nullptr;
		~Image() {
			SDL_DestroySurface(image);
		}
	};

	class Button : public BaseWidget {
	public:
		Button() {}
		Button(std::string text, int x, int y, int sizex, int sizey, uint32_t color, FUNCTION_CALL* OnClick, void* ptr);
		std::string text = "";
		uint32_t color = 0;
	};

	class Entry : public BaseWidget {
	public:
		Entry() {}
		Entry(std::string placeHolder, int x, int y, int sizex, int sizey);
		std::string placeHolder = "";
	};

	class CheckBox : public Button {
	public:
		CheckBox() {}
		CheckBox(std::string text, int x, int y);
		bool checked = false;
	};

	void defaultOnDrag(void*, int, int);

	class Scale : public BaseWidget {
	public:
		Scale() {};
		Scale(int x, int y, int sizex, int sizey, int max, bool horizontal = true, DRAG_CALL* onDrag = nullptr);
		bool horizontal = true;
		void* ptr = nullptr;
		float value = 0;
		int max = 100;
	};

	class Panel : public BaseWidget {
	private:
		Window* base = nullptr;
		using ScaleReference = Scale&;
		Scale* scrollX = nullptr;
		Scale* scrollY = nullptr;
		int contentSizeX = 0;
		int contentSizeY = 0;
		int scrollOffsetX = 0;
		int scrollOffsetY = 0;
	public:
		enum class Flags {
			None = 0b0000,
			DynamicSizeX = 0b0001,
			DynamicSizeY = 0b0010,
			ScrollableX = 0b0100,
			ScrollableY = 0b1000,
		};
		enum class ScrollBarDirection {
			None = 0,
			Horizontal = 0b1,
			Vertical = 0b10,
		};
		friend class Window;
		Panel() {};
		Panel(int x, int y, int sizeX, int sizeY, int flags);

		//void addScrollBar(ScrollBarDirection direction, ScaleReference scale);
		Window* getWidgetAttachable() { return base; }
		void Detach();
		void Reattach();
	};
};