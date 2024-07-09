#include "TextButton.h"

using Onyx::Math::IVec2, Onyx::Math::Vec2, Onyx::Math::Vec4;

TextButton::TextButton()
{
    m_btnWidth = m_btnHeight = 0;
    m_textWidth = m_textHeight = 0;
    m_hover = false;
    m_win = nullptr;
    m_normCursor = m_hoverCursor = nullptr;
    m_input = nullptr;
}

TextButton::TextButton(const std::string& text, Onyx::Font& font, Align textAlign, const Vec4& btnColor, const Vec4& btnHoverColor, const Vec4& textColor, int padding)
{
    m_btnColor = btnColor;
    m_btnHoverColor = btnHoverColor;
    m_align = textAlign;
    m_padding = padding;
    m_text = Onyx::TextRenderable(text, font, textColor);
    m_text.setZIndex(2);
    m_textWidth = m_text.getWidth();
    m_textHeight = font.getStringHeight("A");
    m_btnWidth = m_textWidth + padding * 2;
    m_btnHeight = m_textHeight + padding * 2;
    m_button = Onyx::UiRenderable::ColoredQuad(m_btnWidth, m_btnHeight, btnColor);
    m_button.setZIndex(1);
    updateTextPos();
    m_hover = false;
    m_win = nullptr;
    m_normCursor = m_hoverCursor = nullptr;
    m_input = nullptr;
}

TextButton::TextButton(const std::string& text, Onyx::Font& font, Align textAlign, const Vec4& btnColor, const Vec4& btnHoverColor, const Vec4& textColor, int btnWidth, int btnHeight, int padding)
{
    m_btnColor = btnColor;
    m_btnHoverColor = btnHoverColor;
    m_align = textAlign;
    m_padding = padding;
    m_text = Onyx::TextRenderable(text, font, textColor);
    m_text.setZIndex(2);
    m_textWidth = m_text.getWidth();
    m_textHeight = font.getStringHeight("A");
    m_btnWidth = btnWidth;
    m_btnHeight = btnHeight;
    m_button = Onyx::UiRenderable::ColoredQuad(m_btnWidth, m_btnHeight, btnColor);
    m_button.setZIndex(1);
    updateTextPos();
    m_hover = false;
    m_win = nullptr;
    m_normCursor = m_hoverCursor = nullptr;
    m_input = nullptr;
}

void TextButton::update()
{
    if (m_input == nullptr) return;

    double x = m_input->getMousePos().getX(), y = m_input->getMousePos().getY();
    if (x >= m_button.getPosition().getX() - m_btnWidth / 2.0f &&
        x <= m_button.getPosition().getX() + m_btnWidth / 2.0f &&
        y >= m_button.getPosition().getY() - m_btnHeight / 2.0f &&
        y <= m_button.getPosition().getY() + m_btnHeight / 2.0f) 
    {
        if (!m_hover)
        {
            m_hover = true;
            m_button.setColor(m_btnHoverColor);
            if (m_win && m_hoverCursor) m_win->setCursor(*m_hoverCursor);
        }
    }
    else if (m_hover)
    {
        m_hover = false;
        m_button.setColor(m_btnColor);
        if (m_win && m_normCursor) m_win->setCursor(*m_normCursor);
    }
}

void TextButton::render(const Onyx::Math::Mat4& ortho)
{
    m_button.render(ortho);
    m_text.render(ortho);
}

void TextButton::addToRenderer(Onyx::Renderer* renderer)
{
    renderer->add(m_button);
    renderer->add(m_text);
}

void TextButton::setPosition(const Vec2& pos)
{
    m_button.setPosition(IVec2(pos));
    updateTextPos();
}

void TextButton::setScale(float scale)
{
    m_btnWidth /= m_button.getScale().getX();
    m_btnHeight /= m_button.getScale().getX();
    m_textWidth /= m_button.getScale().getX();
    m_textHeight /= m_button.getScale().getX();
    m_button.setScale(scale);
    m_text.setScale(scale);
    updateTextDims();
    updateTextPos();
    m_btnWidth *= scale;
    m_btnHeight *= scale;
    m_textWidth *= scale;
    m_textHeight *= scale;
}

void TextButton::setButtonColor(const Vec4& color)
{
    m_button.setColor(color);
}

void TextButton::setTextColor(const Vec4& color)
{
    m_text.setColor(color);
}

void TextButton::setWindow(Onyx::Window* window)
{
    m_win = window;
}

void TextButton::setNormalCursor(Onyx::Cursor* cursor)
{
    m_normCursor = cursor;
}

void TextButton::setHoverCursor(Onyx::Cursor* cursor)
{
    m_hoverCursor = cursor;
}

void TextButton::setInputHandler(Onyx::InputHandler* input)
{
    m_input = input;
}

bool TextButton::isHovered() const
{
    return m_hover;
}

int TextButton::getWidth() const
{
    return m_btnWidth;
}

int TextButton::getHeight() const
{
    return m_btnHeight;
}

const Vec2& TextButton::getPosition() const
{
    return m_button.getPosition();
}

float TextButton::getScale() const
{
    return m_button.getScale().getX();
}

void TextButton::updateTextDims()
{
    m_textWidth = m_text.getWidth();
    m_textHeight = m_text.getFont().getStringHeight("A") * m_text.getScale().getX();
}

void TextButton::updateTextPos()
{
    switch (m_align)
    {
        case Align::Center:
            m_text.setPosition(IVec2(m_button.getPosition() + Vec2(-m_textWidth / 2.0f, -m_textHeight / 2.0f)));
            break;
        case Align::Left:
            m_text.setPosition(IVec2(m_button.getPosition() + Vec2(-m_btnWidth / 2 + m_padding, -m_textHeight / 2.0f)));
            break;
        case Align::Right:
            m_text.setPosition(IVec2(m_button.getPosition() + Vec2(m_btnWidth / 2 - m_padding - m_textWidth, -m_textHeight / 2.0f)));
            break;
        case Align::Top:
            m_text.setPosition(IVec2(m_button.getPosition() + Vec2(-m_textWidth / 2.0f, m_btnHeight / 2 - m_padding - m_textHeight)));
            break;
        case Align::Bottom:
            m_text.setPosition(IVec2(m_button.getPosition() + Vec2(-m_textWidth / 2.0f, -m_btnHeight / 2 + m_padding)));
            break;
        case Align::TopLeft:
            m_text.setPosition(IVec2(m_button.getPosition() + Vec2(-m_btnWidth / 2 + m_padding, m_btnHeight / 2 - m_padding - m_textHeight)));
            break;
        case Align::TopRight:
            m_text.setPosition(IVec2(m_button.getPosition() + Vec2(m_btnWidth / 2 - m_padding - m_textWidth, m_btnHeight / 2 - m_padding - m_textHeight)));
            break;
        case Align::BottomLeft:
            m_text.setPosition(IVec2(m_button.getPosition() + Vec2(-m_btnWidth / 2 + m_padding, -m_btnHeight / 2 + m_padding)));
            break;
        case Align::BottomRight:
            m_text.setPosition(IVec2(m_button.getPosition() + Vec2(m_btnWidth / 2 - m_padding - m_textWidth, -m_btnHeight / 2 + m_padding)));
            break;
    }
}

void TextButton::AddToRenderer(Onyx::Renderer* renderer, std::initializer_list<TextButton*> buttons)
{
    for (TextButton* btn : buttons)
    {
        renderer->add(btn->m_button);
        renderer->add(btn->m_text);
    }
}

void TextButton::Update(std::initializer_list<TextButton*> buttons)
{
    for (TextButton* btn : buttons) btn->update();
}

void TextButton::SetWindow(Onyx::Window* window, std::initializer_list<TextButton*> buttons)
{
    for (TextButton* btn : buttons) btn->setWindow(window);
}

void TextButton::SetNormalCursor(Onyx::Cursor* cursor, std::initializer_list<TextButton*> buttons)
{
    for (TextButton* btn : buttons) btn->setNormalCursor(cursor);
}

void TextButton::SetHoverCursor(Onyx::Cursor* cursor, std::initializer_list<TextButton*> buttons)
{
    for (TextButton* btn : buttons) btn->setHoverCursor(cursor);
}

void TextButton::SetInputHandler(Onyx::InputHandler* handler, std::initializer_list<TextButton*> buttons)
{    
    for (TextButton* btn : buttons) btn->setInputHandler(handler);
}
