#pragma once

#include <string>
#include <initializer_list>

#include <Onyx/Core.h>
#include <Onyx/UiRenderable.h>
#include <Onyx/TextRenderable.h>
#include <Onyx/Math.h>
#include <Onyx/Camera.h>
#include <Onyx/Renderer.h>

#include "Align.h"

class TextBox
{
public:
    TextBox();

    TextBox(Onyx::Font& font, Align align, const Onyx::Math::Vec4& bgColor, 
    const Onyx::Math::Vec4& bgHoverColor, const Onyx::Math::Vec4& cursorColor, 
    const Onyx::Math::Vec4& textColor, const Onyx::Math::Vec4& placeholderTextColor, 
    const std::string& placeholderText, int padding = 10);

    TextBox(Onyx::Font& font, Align align, const Onyx::Math::Vec4& bgColor, 
    const Onyx::Math::Vec4& bgHoverColor, const Onyx::Math::Vec4& cursorColor, 
    const Onyx::Math::Vec4& textColor, const Onyx::Math::Vec4& placeholderTextColor, 
    const std::string& placeholderText, int bgWidth, int bgHeight, int padding = 10);

    void update();
    void render(const Onyx::Math::Mat4& ortho);
    void addToRenderer(Onyx::Renderer* renderer);
    void focus();
    void unfocus();
    void hide();
    void show();

    void setPosition(const Onyx::Math::Vec2& pos);
    void setScale(float scale);
    void setBackgroundColor(const Onyx::Math::Vec4& color);
    void setCursorColor(const Onyx::Math::Vec4& color);
    void setTextColor(const Onyx::Math::Vec4& color);
    void setText(const std::string& text);
    void setPlaceholderTextColor(const Onyx::Math::Vec4& color);
    void setPlaceholderText(const std::string& text);
    void setAlign(Align align);

    void setWindow(Onyx::Window* window);
    void setNormalCursor(Onyx::Cursor* cursor);
    void setHoverCursor(Onyx::Cursor* cursor);
    void setInputHandler(Onyx::InputHandler* input);
    void setAllPtrs(Onyx::Window* window, Onyx::Cursor* normCursor, Onyx::Cursor* hoverCursor, Onyx::InputHandler* input);

    bool isHovered() const;
    bool isFocused() const;
    bool isHidden() const;
    const std::string& getText() const;
    int getWidth() const;
    int getHeight() const;
    const Onyx::Math::Vec2& getPosition() const;
    float getScale() const;

    static void Update(std::initializer_list<TextBox*> textBoxes);
    static void AddToRenderer(Onyx::Renderer* renderer, std::initializer_list<TextBox*> textBoxes);
    static void SetWindow(Onyx::Window* window, std::initializer_list<TextBox*> textBoxes);
    static void SetNormalCursor(Onyx::Cursor* cursor, std::initializer_list<TextBox*> textBoxes);
    static void SetHoverCursor(Onyx::Cursor* cursor, std::initializer_list<TextBox*> textBoxes);
    static void SetInputHandler(Onyx::InputHandler* handler, std::initializer_list<TextBox*> textBoxes);
    static void SetAllPtrs(Onyx::Window* window, Onyx::Cursor* normCursor, Onyx::Cursor* hoverCursor, Onyx::InputHandler* input, std::initializer_list<TextBox*> textBoxes);

private:
    int m_bgWidth, m_bgHeight;
    int m_cursorWidth, m_cursorHeight;
    int m_textWidth, m_textHeight;
    int m_plTextWidth;
    int m_padding;
    int m_cursorIdx;
    Align m_align;
    Onyx::Math::Vec4 m_bgColor, m_bgHoverColor;
    Onyx::UiRenderable m_bg;
    Onyx::UiRenderable m_cursor;
    Onyx::TextRenderable m_plText;
    Onyx::TextRenderable m_text;
    Onyx::Shader m_textShader, m_plTextShader;
    bool m_hover, m_focus;
    float m_cursorTimer, m_cursorShowLockTimer;

    Onyx::Window* m_win;
    Onyx::Cursor* m_normCursor, * m_hoverCursor;
    Onyx::InputHandler* m_input;

    void updateTextDims();
    void updateTextPos();
    void updateCursorDims();
    void updateCursorPos();
    void updatePlTextDims();
    void updatePlTextPos();

    void addChar(char c);
    void rmChar();
};
