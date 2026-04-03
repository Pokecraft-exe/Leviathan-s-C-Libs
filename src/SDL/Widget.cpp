#include "Widgets.hpp"
#include "Window.hpp"
using Iterables::LinkedList;

namespace SimplerDirectMediaLayer {

	BaseWidget::BaseWidget(const BaseWidget& b) {
		*this = b;
	}

	BaseWidget::BaseWidget(DRAW_CALL* _draw, int _x, int _y, int _sizex, int _sizey, ANCHOR _anchor, size_t _id, void* ptr) {
		draw = _draw;
		x = _x;
		y = _y;
		sizex = _sizex;
		sizey = _sizey;
		anchor = _anchor;
		id = _id;
	}

	void BaseWidget::attach(Window& w) {
		id = w.widgets.size();
		w.widgets.push_back(this);
		parent = &w;
	}

	void drawLabel(void* w, void* _this, int x, int y) {
		((Window*)w)->drawString(((Label*)_this)->text, x, y, ((Label*)_this)->color);
	}

	Label::Label(std::string _text, uint32_t _color, int _x, int _y) {
		text = _text;
		color = _color;
		x = _x;
		y = _y;
		sizex = 11 * text.size();
		sizey = 11;
		draw = drawLabel;
	};



	void drawImage(void* w, void* _this, int x, int y) {
		SDL_FRect base = { 0.0f, 0.0f, (float)((Image*)_this)->sizex, (float)((Image*)_this)->sizey };
		SDL_FRect rect = { (float)x, (float)y, (float)((Image*)_this)->sizex, (float)((Image*)_this)->sizey };
		SDL_CreateTextureFromSurface(((Window*)w)->getSDLRenderer(), ((Image*)_this)->image);
		SDL_RenderTexture(((Window*)w)->getSDLRenderer(), SDL_CreateTextureFromSurface(((Window*)w)->getSDLRenderer(), ((Image*)_this)->image), &base, &rect);
#warning "This is very slow, consider using SDL_Textures instead of SDL_Surfaces for images"
		//SDL_BlitSurface(((Image*)_this)->image, &base, ((Window*)w)->getsScreenSurface(), &rect);
	}

	Image::Image(std::string _image, int _x, int _y) {
		image = IMG_Load(_image.data());
		x = _x;
		y = _y;
		sizex = image->w;
		sizey = image->h;
		draw = drawImage;
	};

	void drawButton(void* w, void* _this, int x, int y) {
		((Window*)w)->drawRect(x - 2, y - 2, ((Button*)_this)->sizex + 2, ((Button*)_this)->sizey + 2, 0xAAAAAA);
		((Window*)w)->drawRect(x, y, ((Button*)_this)->sizex, ((Button*)_this)->sizey, ((Button*)_this)->color);
		((Window*)w)->drawString(((Button*)_this)->text, x, y, 0);
	}

	Button::Button(std::string _text, int _x, int _y, int _sizex, int _sizey, uint32_t _color, FUNCTION_CALL* _OnClick, void* _ptr) {
		text = _text;
		x = _x;
		y = _y;
		sizex = _sizex;
		sizey = _sizey;
		color = _color;
		onClick = _OnClick;
		onClickPtr = _ptr;
		Type = WIDGET_TYPE::CLICKABLE;
		draw = drawButton;
	}

	void drawEntry(void* w, void* _this, int x, int y) {
		((Window*)w)->drawRect(x - 2, y - 2, ((Entry*)_this)->sizex + 2, ((Entry*)_this)->sizey + 2, 0xAAAAAA);
		((Window*)w)->drawRect(x, y, ((Entry*)_this)->sizex, ((Entry*)_this)->sizey, 0xffffff);
		if (((Entry*)_this)->keyboardValue == "") {
			((Window*)w)->drawString(((Entry*)_this)->placeHolder, x, y, 0x555555);
		}
		else {

			((Window*)w)->drawString(((Entry*)_this)->keyboardValue, x, y, 0);
		}
	}

	Entry::Entry(std::string _placeHolder, int _x, int _y, int _sizex, int _sizey) {
		placeHolder = _placeHolder;
		keyboardValue = "";
		x = _x;
		y = _y;
		sizex = _sizex;
		sizey = _sizey;
		draw = drawEntry;
		Type = WIDGET_TYPE::FOCUSABLE | WIDGET_TYPE::KBTYPABLE;
	}

	void drawCheckBox(void* w, void* _this, int x, int y) {
		constexpr int PADDING = 2;
		constexpr int SHADOW_SIZE = 1;
		constexpr int X_OFFSET = 15;

		((Window*)w)->drawRect(x - PADDING, y - PADDING, ((CheckBox*)_this)->sizex + PADDING, ((CheckBox*)_this)->sizey + PADDING, 0xAAAAAA);
		((Window*)w)->drawRect(x, y, ((CheckBox*)_this)->sizex, ((CheckBox*)_this)->sizey, 0xffffff);
		if (((CheckBox*)_this)->checked) {
			((Window*)w)->drawChar('X', x + SHADOW_SIZE, y + SHADOW_SIZE, 0);
		}
		((Window*)w)->drawString(((CheckBox*)_this)->text, x + X_OFFSET, y, 0);
	}

	void CheckBoxOnClick(void* _this) {
		((CheckBox*)_this)->checked = !((CheckBox*)_this)->checked;
	}

	CheckBox::CheckBox(std::string _text, int _x, int _y) {
		text = _text;
		x = _x;
		y = _y;
		sizex = 10;
		sizey = 10;
		color = 0;
		onClick = (FUNCTION_CALL*)CheckBoxOnClick;
		onClickPtr = this;
		Type = WIDGET_TYPE::CLICKABLE;
		draw = drawCheckBox;
	}

	void DragScale(void* _this, int Horizontal, int Vertical) {
		if (((Scale*)_this)->horizontal) {
			float offset = Horizontal - ((Scale*)_this)->x;
			((Scale*)_this)->value = offset * (float)((Scale*)_this)->max / (float)((Scale*)_this)->sizex;
		}
		else {
			float offset = Vertical - ((Scale*)_this)->y;
			((Scale*)_this)->value = offset * (float)((Scale*)_this)->max / (float)((Scale*)_this)->sizey;
		}

		if (((Scale*)_this)->value < 0) ((Scale*)_this)->value = 0;
		if (((Scale*)_this)->value > ((Scale*)_this)->max) ((Scale*)_this)->value = ((Scale*)_this)->max;
	}

	void defaultOnDrag(void* a, int b, int c) { return; }

	void DrawScale(void* w, void* _this, int x, int y) {
		constexpr int PADDING = 2;
		constexpr int KNOB_SIZE = 20;

		((Window*)w)->drawRect(x - PADDING, y - PADDING, ((Scale*)_this)->sizex + PADDING, ((Scale*)_this)->sizey + PADDING, 0xAAAAAA);
		((Window*)w)->drawRect(x, y, ((Scale*)_this)->sizex, ((Scale*)_this)->sizey, 0xffffff);

		if (((Scale*)_this)->horizontal) {
			int offset = (((Scale*)_this)->value * (((Scale*)_this)->sizex - KNOB_SIZE)) / ((Scale*)_this)->max;
			((Window*)w)->drawRect(x + offset - PADDING, y - PADDING, KNOB_SIZE + PADDING, ((Scale*)_this)->sizey + PADDING, 0xAAAAAA);
			((Window*)w)->drawRect(x + offset, y, KNOB_SIZE, ((Scale*)_this)->sizey, 0x666666);
		}
		else {
			int offset = (((Scale*)_this)->value * (((Scale*)_this)->sizey - KNOB_SIZE)) / ((Scale*)_this)->max;
			((Window*)w)->drawRect(x - PADDING, y + offset - PADDING, ((Scale*)_this)->sizex + PADDING, KNOB_SIZE + PADDING, 0xAAAAAA);
			((Window*)w)->drawRect(x, y + offset, ((Scale*)_this)->sizex, KNOB_SIZE, 0x666666);
		}
	}

	int clickScale(void* _this) {
		float mousex, mousey;
		SDL_GetMouseState(&mousex, &mousey);
		((Scale*)_this)->drag(_this, mousex, mousey);

		return 0;
	}

	Scale::Scale(int _x, int _y, int _sizex, int _sizey, int _max, bool _horizontal, DRAG_CALL* _onDrag) {
		draw = (DRAW_CALL*)DrawScale;
		x = _x;
		y = _y;
		sizex = _sizex;
		sizey = _sizey;
		horizontal = _horizontal;
		drag = (DRAG_CALL*)DragScale;
		onClick = clickScale;
		onClickPtr = this;
		onDrag = _onDrag;
		if (_onDrag == nullptr) onDrag = (DRAG_CALL*)defaultOnDrag;
		max = _max;
		Type = WIDGET_TYPE::DRAGGABLE | WIDGET_TYPE::FOCUSABLE | WIDGET_TYPE::CLICKABLE;
	}

	Panel::Panel(int x, int y, int sizeX, int sizeY, int flags) {
		base = (Window*)malloc(sizeof(Window));
		base->SCREEN_BASE_HEIGHT = sizeY;
		base->SCREEN_BASE_WIDTH = sizeX;
		base->SCREEN_HEIGHT = sizeY;
		base->SCREEN_WIDTH = sizeX;
	}

	void Panel::Detach() {
		LinkedList<BaseWidget*> widgets = base->widgets.copy();

		free(base);

		base = new Window();
	}

	void Panel::Reattach() {
		delete base;
	}
};