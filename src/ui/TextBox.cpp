// #include "TextBox.h"

// using Onyx::Math::IVec2, Onyx::Math::Vec2, Onyx::Math::Vec4;

// TextBox::TextBox()
// {
//     m_btnWidth = m_btnHeight = 0;
//     m_textWidth = m_textHeight = 0;
//     m_hover = false;
//     m_win = nullptr;
//     m_normCursor = m_hoverCursor = nullptr;
//     m_input = nullptr;
// }

// TextBox::TextBox(const std::string& text, Onyx::Font& font, const Vec4& btnColor, const Vec4& btnHoverColor, const Vec4& textColor, int padding)
// {
//     m_btnColor = btnColor;
//     m_btnHoverColor = btnHoverColor;
//     m_text = Onyx::TextRenderable(text, font, textColor);
//     m_text.setZIndex(2);
//     m_textWidth = m_text.getWidth();
//     m_textHeight = font.getStringHeight("A");
//     m_btnWidth = m_textWidth + padding * 2;
//     m_btnHeight = m_textHeight + padding * 2;
//     m_button = Onyx::UiRenderable::ColoredQuad(m_btnWidth, m_btnHeight, btnColor);
//     m_button.setZIndex(1);
//     updateTextPos();
//     m_hover = false;
//     m_win = nullptr;
//     m_normCursor = m_hoverCursor = nullptr;
//     m_input = nullptr;
// }

// TextBox::TextBox(const std::string& text, Onyx::Font& font, const Vec4& btnColor, const Vec4& btnHoverColor, const Vec4& textColor, int btnWidth, int btnHeight)
// {
//     m_btnColor = btnColor;
//     m_btnHoverColor = btnHoverColor;
//     m_text = Onyx::TextRenderable(text, font, textColor);
//     m_text.setZIndex(2);
//     m_textWidth = m_text.getWidth();
//     m_textHeight = font.getStringHeight("A");
//     m_btnWidth = btnWidth;
//     m_btnHeight = btnHeight;
//     m_button = Onyx::UiRenderable::ColoredQuad(m_btnWidth, m_btnHeight, btnColor);
//     m_button.setZIndex(1);
//     updateTextPos();
//     m_hover = false;
//     m_win = nullptr;
//     m_normCursor = m_hoverCursor = nullptr;
//     m_input = nullptr;
// }

// void TextBox::update()
// {
//     if (m_input == nullptr) return;

//     double x = m_input->getMousePos().getX(), y = m_input->getMousePos().getY();
//     if (x >= m_button.getPosition().getX() - m_btnWidth / 2.0f &&
//         x <= m_button.getPosition().getX() + m_btnWidth / 2.0f &&
//         y >= m_button.getPosition().getY() - m_btnHeight / 2.0f &&
//         y <= m_button.getPosition().getY() + m_btnHeight / 2.0f) 
//     {
//         if (!m_hover)
//         {
//             m_hover = true;
//             m_button.setColor(m_btnHoverColor);
//             if (m_win && m_hoverCursor) m_win->setCursor(*m_hoverCursor);
//         }
//     }
//     else if (m_hover)
//     {
//         m_hover = false;
//         m_button.setColor(m_btnColor);
//         if (m_win && m_normCursor) m_win->setCursor(*m_normCursor);
//     }
// }

// void TextBox::render(const Onyx::Math::Mat4& ortho)
// {
//     m_button.render(ortho);
//     m_text.render(ortho);
// }

// void TextBox::addToRenderer(Onyx::Renderer* renderer)
// {
//     renderer->add(m_button);
//     renderer->add(m_text);
// }

// void TextBox::setPosition(const Vec2& pos)
// {
//     m_button.setPosition(IVec2(pos));
//     updateTextPos();
// }

// void TextBox::setScale(float scale)
// {
//     m_btnWidth /= m_button.getScale().getX();
//     m_btnHeight /= m_button.getScale().getX();
//     m_textWidth /= m_button.getScale().getX();
//     m_textHeight /= m_button.getScale().getX();
//     m_button.setScale(scale);
//     m_text.setScale(scale);
//     updateTextDims();
//     updateTextPos();
//     m_btnWidth *= scale;
//     m_btnHeight *= scale;
//     m_textWidth *= scale;
//     m_textHeight *= scale;
// }

// void TextBox::setButtonColor(const Vec4& color)
// {
//     m_button.setColor(color);
// }

// void TextBox::setTextColor(const Vec4& color)
// {
//     m_text.setColor(color);
// }

// void TextBox::setWindow(Onyx::Window* window)
// {
//     m_win = window;
// }

// void TextBox::setNormalCursor(Onyx::Cursor* cursor)
// {
//     m_normCursor = cursor;
// }

// void TextBox::setHoverCursor(Onyx::Cursor* cursor)
// {
//     m_hoverCursor = cursor;
// }

// void TextBox::setInputHandler(Onyx::InputHandler* input)
// {
//     m_input = input;
// }

// bool TextBox::isHovered() const
// {
//     return m_hover;
// }

// int TextBox::getWidth() const
// {
//     return m_btnWidth;
// }

// int TextBox::getHeight() const
// {
//     return m_btnHeight;
// }

// const Vec2& TextBox::getPosition() const
// {
//     return m_button.getPosition();
// }

// float TextBox::getScale() const
// {
//     return m_button.getScale().getX();
// }

// void TextBox::updateTextDims()
// {
//     m_textWidth = m_text.getWidth();
//     m_textHeight = m_text.getFont().getStringHeight("A") * m_text.getScale().getX();
// }

// void TextBox::updateTextPos()
// {
//     m_text.setPosition(IVec2(m_button.getPosition() - Vec2(m_textWidth / 2.0f, m_textHeight / 2.0f)));
// }

// void TextBox::Update(std::initializer_list<TextBox*> buttons)
// {
//     for (TextBox* btn : buttons) btn->update();
// }

// void TextBox::SetWindow(Onyx::Window* window, std::initializer_list<TextBox*> buttons)
// {
//     for (TextBox* btn : buttons) btn->setWindow(window);
// }

// void TextBox::SetNormalCursor(Onyx::Cursor* cursor, std::initializer_list<TextBox*> buttons)
// {
//     for (TextBox* btn : buttons) btn->setNormalCursor(cursor);
// }

// void TextBox::SetHoverCursor(Onyx::Cursor* cursor, std::initializer_list<TextBox*> buttons)
// {
//     for (TextBox* btn : buttons) btn->setHoverCursor(cursor);
// }

// void TextBox::SetInputHandler(Onyx::InputHandler* handler, std::initializer_list<TextBox*> buttons)
// {    
//     for (TextBox* btn : buttons) btn->setInputHandler(handler);
// }
