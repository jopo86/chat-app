// #include <string>
// #include <initializer_list>

// #include <Onyx/Core.h>
// #include <Onyx/UiRenderable.h>
// #include <Onyx/TextRenderable.h>
// #include <Onyx/Math.h>
// #include <Onyx/Camera.h>
// #include <Onyx/Renderer.h>

// class TextBox
// {
// public:
//     TextBox();
//     TextBox(const std::string& text, Onyx::Font& font, const Onyx::Math::Vec4& bgColor, const Onyx::Math::Vec4& bgHoverColor, const Onyx::Math::Vec4& textColor, int padding = 10);
//     TextBox(const std::string& text, Onyx::Font& font, const Onyx::Math::Vec4& bgColor, const Onyx::Math::Vec4& bgHoverColor, const Onyx::Math::Vec4& textColor, int bgWidth, int bgHeight);

//     void update();
//     void render(const Onyx::Math::Mat4& ortho);
//     void addToRenderer(Onyx::Renderer* renderer);

//     void setPosition(const Onyx::Math::Vec2& pos);
//     void setScale(float scale);
//     void setButtonColor(const Onyx::Math::Vec4& color);
//     void setTextColor(const Onyx::Math::Vec4& color);

//     void setWindow(Onyx::Window* window);
//     void setNormalCursor(Onyx::Cursor* cursor);
//     void setHoverCursor(Onyx::Cursor* cursor);
//     void setInputHandler(Onyx::InputHandler* input);

//     bool isHovered() const;
//     int getWidth() const;
//     int getHeight() const;
//     const Onyx::Math::Vec2& getPosition() const;
//     float getScale() const;

//     static void Update(std::initializer_list<TextBox*> buttons);
//     static void SetWindow(Onyx::Window* window, std::initializer_list<TextBox*> buttons);
//     static void SetNormalCursor(Onyx::Cursor* cursor, std::initializer_list<TextBox*> buttons);
//     static void SetHoverCursor(Onyx::Cursor* cursor, std::initializer_list<TextBox*> buttons);
//     static void SetInputHandler(Onyx::InputHandler* handler, std::initializer_list<TextBox*> buttons);

// private:
//     int m_bgWidth, m_bgHeight;
//     int m_textWidth, m_textHeight;
//     Onyx::Math::Vec4 m_bgColor, m_bgHoverColor;
//     Onyx::UiRenderable m_button;
//     Onyx::TextRenderable m_text;
//     bool m_hover;

//     Onyx::Window* m_win;
//     Onyx::Cursor* m_normCursor, * m_hoverCursor;
//     Onyx::InputHandler* m_input;

//     void updateTextDims();
//     void updateTextPos();
// };
