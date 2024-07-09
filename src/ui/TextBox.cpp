#include "TextBox.h"
#include "charkeys.h"

using Onyx::Math::IVec2, Onyx::Math::Vec2, Onyx::Math::Vec4;

#define CURSOR_WIDTH 1
#define CURSOR_SPACING 2
#define CURSOR_BLINK_INTERVAL 0.5f

TextBox::TextBox()
{
    m_bgWidth = m_bgHeight = 0;
    m_cursorWidth = m_cursorHeight = 0;
    m_textWidth = m_textHeight = 0;
    m_plTextWidth = 0;
    m_padding = 0;
    m_hover = m_focus = false;
    m_cursorTimer = 0.0f;
    m_win = nullptr;
    m_normCursor = m_hoverCursor = nullptr;
    m_input = nullptr;
}

TextBox::TextBox(Onyx::Font& font, Align align, const Vec4& bgColor, const Vec4& bgHoverColor, 
    const Vec4& cursorColor, const Vec4& textColor, const Vec4& plTextColor, 
    const std::string& plText, int padding)
{
    m_bgColor = bgColor;
    m_bgHoverColor = bgHoverColor;

    m_align = align;
    m_padding = padding;

    m_text = Onyx::TextRenderable("", font, textColor);
    m_text.setZIndex(2);
    m_textWidth = m_text.getWidth();
    m_textHeight = font.getStringHeight("A");

    m_plText = Onyx::TextRenderable(plText, font, plTextColor);
    m_plText.setZIndex(2);
    m_plTextWidth = m_plText.getWidth();

    m_bgWidth = m_plTextWidth + padding * 2;
    m_bgHeight = m_textHeight + padding * 2;
    m_bg = Onyx::UiRenderable::ColoredQuad(m_bgWidth, m_bgHeight, bgColor);
    m_bg.setZIndex(1);

    m_cursorWidth = CURSOR_WIDTH;
    m_cursorHeight = m_textHeight * 1.3f;
    m_cursor = Onyx::UiRenderable::ColoredQuad(m_cursorWidth, m_cursorHeight, cursorColor);
    m_cursor.setZIndex(2);
    m_cursor.hide();

    updateTextPos();

    m_hover = m_focus = false;
    m_cursorTimer = 0.0f;
    m_win = nullptr;
    m_normCursor = m_hoverCursor = nullptr;
    m_input = nullptr;
}

TextBox::TextBox(Onyx::Font& font, Align align, const Vec4& bgColor, const Vec4& bgHoverColor, 
    const Vec4& cursorColor, const Vec4& textColor, const Vec4& plTextColor, 
    const std::string& plText, int bgWidth, int bgHeight, int padding)
{
    m_bgColor = bgColor;
    m_bgHoverColor = bgHoverColor;

    m_align = align;
    m_padding = padding;

    m_text = Onyx::TextRenderable("", font, textColor);
    m_text.setZIndex(2);
    m_textWidth = m_text.getWidth();
    m_textHeight = font.getStringHeight("A");

    m_plText = Onyx::TextRenderable(plText, font, plTextColor);
    m_plText.setZIndex(2);
    m_plTextWidth = m_plText.getWidth();

    m_bgWidth = bgWidth;
    m_bgHeight = bgHeight;
    m_bg = Onyx::UiRenderable::ColoredQuad(m_bgWidth, m_bgHeight, bgColor);
    m_bg.setZIndex(1);

    m_cursorWidth = CURSOR_WIDTH;
    m_cursorHeight = m_textHeight * 1.3f;
    m_cursor = Onyx::UiRenderable::ColoredQuad(m_cursorWidth, m_cursorHeight, cursorColor);
    m_cursor.setZIndex(2);
    m_cursor.hide();

    updateTextPos();

    m_hover = m_focus = false;
    m_cursorTimer = 0.0f;
    m_win = nullptr;
    m_normCursor = m_hoverCursor = nullptr;
    m_input = nullptr;
}

void TextBox::update()
{
    if (!m_input) return;

    double x = m_input->getMousePos().getX(), y = m_input->getMousePos().getY();
    if (x >= m_bg.getPosition().getX() - m_bgWidth / 2.0f &&
        x <= m_bg.getPosition().getX() + m_bgWidth / 2.0f &&
        y >= m_bg.getPosition().getY() - m_bgHeight / 2.0f &&
        y <= m_bg.getPosition().getY() + m_bgHeight / 2.0f) 
    {
        if (!m_hover)
        {
            m_hover = true;
            m_bg.setColor(m_bgHoverColor);
            if (m_win && m_hoverCursor) m_win->setCursor(*m_hoverCursor);
        }
    }
    else if (m_hover)
    {
        m_hover = false;
        m_bg.setColor(m_bgColor);
        if (m_win && m_normCursor) m_win->setCursor(*m_normCursor);
    }

    if (m_hover && m_input->isMouseButtonTapped(Onyx::MouseButton::Left)) focus();
    else if (!m_hover && m_input->isMouseButtonTapped(Onyx::MouseButton::Left)) unfocus();

    if (m_focus)
    {
        if (m_win)
        {
            m_cursorTimer += m_win->getDeltaTime();
            if (m_cursorTimer >= CURSOR_BLINK_INTERVAL)
            {
                m_cursor.toggleVisibility();
                m_cursorTimer = m_cursorTimer - CURSOR_BLINK_INTERVAL;
            }
        }

        for (char c : ck::all)
        {
            if (m_input->isKeyTapped(ck::ctok(c)) || m_input->isKeyRepeated(ck::ctok(c)))
            {
                m_cursor.show();
                bool shift = m_input->isKeyDown(Onyx::Key::LeftShift) || 
                    m_input->isKeyDown(Onyx::Key::RightShift) ||
                    m_input->IsCapsLockOn();
                if (shift) c = ck::shift(c);
                m_text.setText(m_text.getText() + c);
                updateTextDims();
                updateTextPos();
                updateCursorPos();
                updatePlTextPos();
            }
        }

        if (m_input->isKeyTapped(Onyx::Key::Backspace) || m_input->isKeyRepeated(Onyx::Key::Backspace))
        {
            m_cursor.show();
            if (m_text.getText().size() > 0)
            {
                m_text.setText(m_text.getText().substr(0, m_text.getText().size() - 1));
                updateTextDims();
                updateTextPos();
                updateCursorPos();
                updatePlTextPos();
            }
        }
    }
}

void TextBox::render(const Onyx::Math::Mat4& ortho)
{
    m_bg.render(ortho);
    m_text.render(ortho);
}

void TextBox::addToRenderer(Onyx::Renderer* renderer)
{
    renderer->add(m_bg);
    renderer->add(m_cursor);
    renderer->add(m_text);
    renderer->add(m_plText);
}

void TextBox::focus()
{
    if (m_focus) return;
    m_focus = true;
    updateCursorPos();
    m_cursor.show();
    m_plText.hide();
}

void TextBox::unfocus()
{
    if (!m_focus) return;
    m_focus = false;
    m_cursor.hide();
    if (m_text.getText().size() == 0) m_plText.show();
}

void TextBox::setPosition(const Vec2& pos)
{
    m_bg.setPosition(IVec2(pos));
    updateTextPos();
    updateCursorPos();
    updatePlTextPos();
}

void TextBox::setScale(float scale)
{
    m_bgWidth /= m_bg.getScale().getX();
    m_bgHeight /= m_bg.getScale().getX();
    m_cursorWidth /= m_bg.getScale().getX();
    m_cursorHeight /= m_bg.getScale().getX();
    m_textWidth /= m_bg.getScale().getX();
    m_textHeight /= m_bg.getScale().getX();
    m_plTextWidth /= m_bg.getScale().getX();

    m_bg.setScale(scale);
    m_cursor.setScale(scale);
    m_text.setScale(scale);
    m_plText.setScale(scale);

    updateTextDims();
    updateTextPos();
    updateCursorDims();
    updateCursorPos();
    updatePlTextDims();
    updatePlTextPos();

    m_bgWidth *= scale;
    m_bgHeight *= scale;
    m_cursorWidth *= scale;
    m_cursorHeight *= scale;
    m_textWidth *= scale;
    m_textHeight *= scale;
    m_plTextWidth *= scale;
}

void TextBox::setBackgroundColor(const Vec4& color)
{
    m_bg.setColor(color);
}

void TextBox::setTextColor(const Vec4& color)
{
    m_text.setColor(color);
}

void TextBox::setWindow(Onyx::Window* window)
{
    m_win = window;
}

void TextBox::setNormalCursor(Onyx::Cursor* cursor)
{
    m_normCursor = cursor;
}

void TextBox::setHoverCursor(Onyx::Cursor* cursor)
{
    m_hoverCursor = cursor;
}

void TextBox::setInputHandler(Onyx::InputHandler* input)
{
    m_input = input;
}

bool TextBox::isHovered() const
{
    return m_hover;
}

bool TextBox::isFocused() const
{
    return m_focus;
}

const std::string& TextBox::getText() const
{
    return m_text.getText();
}

int TextBox::getWidth() const
{
    return m_bgWidth;
}

int TextBox::getHeight() const
{
    return m_bgHeight;
}

const Vec2& TextBox::getPosition() const
{
    return m_bg.getPosition();
}

float TextBox::getScale() const
{
    return m_bg.getScale().getX();
}

void TextBox::updateTextDims()
{
    m_textWidth = m_text.getWidth();
    m_textHeight = m_text.getFont().getStringHeight("A") * m_text.getScale().getX();
}

void TextBox::updateTextPos()
{
    switch (m_align)
    {
        case Align::Center:
            m_text.setPosition(IVec2(m_bg.getPosition() + Vec2(-m_textWidth / 2.0f, -m_textHeight / 2.0f)));
            break;
        case Align::Left:
            m_text.setPosition(IVec2(m_bg.getPosition() + Vec2(-m_bgWidth / 2 + m_padding, -m_textHeight / 2.0f)));
            break;
        case Align::Right:
            m_text.setPosition(IVec2(m_bg.getPosition() + Vec2(m_bgWidth / 2 - m_padding - m_textWidth, -m_textHeight / 2.0f)));
            break;
        case Align::Top:
            m_text.setPosition(IVec2(m_bg.getPosition() + Vec2(-m_textWidth / 2.0f, m_bgHeight / 2 - m_padding - m_textHeight)));
            break;
        case Align::Bottom:
            m_text.setPosition(IVec2(m_bg.getPosition() + Vec2(-m_textWidth / 2.0f, -m_bgHeight / 2 + m_padding)));
            break;
        case Align::TopLeft:
            m_text.setPosition(IVec2(m_bg.getPosition() + Vec2(-m_bgWidth / 2 + m_padding, m_bgHeight / 2 - m_padding - m_textHeight)));
            break;
        case Align::TopRight:
            m_text.setPosition(IVec2(m_bg.getPosition() + Vec2(m_bgWidth / 2 - m_padding - m_textWidth, m_bgHeight / 2 - m_padding - m_textHeight)));
            break;
        case Align::BottomLeft:
            m_text.setPosition(IVec2(m_bg.getPosition() + Vec2(-m_bgWidth / 2 + m_padding, -m_bgHeight / 2 + m_padding)));
            break;
        case Align::BottomRight:
            m_text.setPosition(IVec2(m_bg.getPosition() + Vec2(m_bgWidth / 2 - m_padding - m_textWidth, -m_bgHeight / 2 + m_padding)));
            break;
    }
}

void TextBox::updateCursorDims()
{
    m_cursorWidth = CURSOR_WIDTH * m_text.getScale().getX();
    m_cursorHeight = m_textHeight * 1.3f * m_text.getScale().getX();
}

void TextBox::updateCursorPos()
{
    m_cursor.setPosition(m_text.getPosition() + Vec2(m_textWidth + CURSOR_SPACING, m_textHeight / 2));
}

void TextBox::updatePlTextDims()
{
    m_plTextWidth = m_plText.getWidth();
}

void TextBox::updatePlTextPos()
{
    switch (m_align)
    {
        case Align::Center:
            m_plText.setPosition(IVec2(m_bg.getPosition() + Vec2(-m_plTextWidth / 2.0f, -m_textHeight / 2.0f)));
            break;
        case Align::Left:
            m_plText.setPosition(IVec2(m_bg.getPosition() + Vec2(-m_bgWidth / 2 + m_padding, -m_textHeight / 2.0f)));
            break;
        case Align::Right:
            m_plText.setPosition(IVec2(m_bg.getPosition() + Vec2(m_bgWidth / 2 - m_padding - m_plTextWidth, -m_textHeight / 2.0f)));
            break;
        case Align::Top:
            m_plText.setPosition(IVec2(m_bg.getPosition() + Vec2(-m_plTextWidth / 2.0f, m_bgHeight / 2 - m_padding - m_textHeight)));
            break;
        case Align::Bottom:
            m_plText.setPosition(IVec2(m_bg.getPosition() + Vec2(-m_plTextWidth / 2.0f, -m_bgHeight / 2 + m_padding)));
            break;
        case Align::TopLeft:
            m_plText.setPosition(IVec2(m_bg.getPosition() + Vec2(-m_bgWidth / 2 + m_padding, m_bgHeight / 2 - m_padding - m_textHeight)));
            break;
        case Align::TopRight:
            m_plText.setPosition(IVec2(m_bg.getPosition() + Vec2(m_bgWidth / 2 - m_padding - m_plTextWidth, m_bgHeight / 2 - m_padding - m_textHeight)));
            break;
        case Align::BottomLeft:
            m_plText.setPosition(IVec2(m_bg.getPosition() + Vec2(-m_bgWidth / 2 + m_padding, -m_bgHeight / 2 + m_padding)));
            break;
        case Align::BottomRight:
            m_plText.setPosition(IVec2(m_bg.getPosition() + Vec2(m_bgWidth / 2 - m_padding - m_plTextWidth, -m_bgHeight / 2 + m_padding)));
            break;
    }
}

void TextBox::Update(std::initializer_list<TextBox*> textBoxes)
{
    for (TextBox* tb : textBoxes) tb->update();
}

void TextBox::AddToRenderer(Onyx::Renderer* renderer, std::initializer_list<TextBox*> textBoxes)
{
    for (TextBox* tb : textBoxes) tb->addToRenderer(renderer);
}

void TextBox::SetWindow(Onyx::Window* window, std::initializer_list<TextBox*> textBoxes)
{
    for (TextBox* tb : textBoxes) tb->setWindow(window);
}

void TextBox::SetNormalCursor(Onyx::Cursor* cursor, std::initializer_list<TextBox*> textBoxes)
{
    for (TextBox* tb : textBoxes) tb->setNormalCursor(cursor);
}

void TextBox::SetHoverCursor(Onyx::Cursor* cursor, std::initializer_list<TextBox*> textBoxes)
{
    for (TextBox* tb : textBoxes) tb->setHoverCursor(cursor);
}

void TextBox::SetInputHandler(Onyx::InputHandler* handler, std::initializer_list<TextBox*> textBoxes)
{
    for (TextBox* tb : textBoxes) tb->setInputHandler(handler);
}

#undef CURSOR_WIDTH
#undef CURSOR_SPACING
#undef CURSOR_BLINK_INTERVAL
