#include <iostream>

#include <Garnet.h>

#include "ui/TextButton.h"
#include "ui/TextBox.h"

using Onyx::Math::IVec2, Onyx::Math::Vec2, Onyx::Math::Vec3, Onyx::Math::Vec4;

const int SCR_WIDTH = 1280, SCR_HEIGHT = 720;

namespace Color
{
    const Vec4 GRAY_0(0.14f, 0.155f, 0.18f, 1.0f);
    const Vec4 GRAY_1 = GRAY_0 * 1.25f;
    const Vec4 GRAY_2 = GRAY_1 * 1.25f;
    const Vec4 RED(1.0f, 0.3f, 0.4f, 1.0f);
    const Vec4 GREEN(0.4f, 0.8f, 0.4f, 1.0f);
    const Vec4 BLUE(0.2f, 0.6f, 1.0f, 1.0f);
}

int main()
{
    Onyx::ErrorHandler errorHandler(true, true, Onyx::Warning::Severity::Med);
    Onyx::Init(errorHandler);
    Onyx::SetResourcePath("../resources/");
    Garnet::Init(true);

    Onyx::Monitor monitor = Onyx::Monitor::GetPrimary();

    Onyx::Window window(
        Onyx::WindowProperties{
            .title = "Onyx Window",
            .width = SCR_WIDTH,
            .height = SCR_HEIGHT,
            .position = IVec2(monitor.getWidth() / 2 - SCR_WIDTH / 2, monitor.getHeight() / 2 - SCR_HEIGHT / 2),
            .backgroundColor = Vec3::White()
        }
    );

    window.init();
    
    Onyx::InputHandler input;
    window.linkInputHandler(input);

    Onyx::Camera cam(Onyx::Projection::Orthographic(SCR_WIDTH, SCR_HEIGHT));
    window.linkCamera(cam);

    Onyx::Renderer renderer(cam);
    window.linkRenderer(renderer);

    Onyx::Cursor arrowCursor = Onyx::Cursor::Standard(Onyx::CursorType::Arrow);
    Onyx::Cursor handCursor = Onyx::Cursor::Standard(Onyx::CursorType::Hand);
    Onyx::Cursor ibeamCursor = Onyx::Cursor::Standard(Onyx::CursorType::Ibeam);

    Onyx::Font roboto = Onyx::Font::Load(Onyx::Resources("fonts/Roboto/Roboto-Light.ttf"), 24);

    TextBox textBox(roboto, Align::TopLeft, Color::GRAY_0, Color::GRAY_1, Vec4::White(), Vec4::White(), Vec4::LightGray(), "Enter text here", 600, 100);
    textBox.setPosition(Vec2(SCR_WIDTH / 2, SCR_HEIGHT / 2));

    // TextButton::AddToRenderer(&renderer,        { &buttonA, &buttonB, &buttonC });
    // TextButton::SetWindow(&window,              { &buttonA, &buttonB, &buttonC });
    // TextButton::SetNormalCursor(&arrowCursor,   { &buttonA, &buttonB, &buttonC });
    // TextButton::SetHoverCursor(&handCursor,     { &buttonA, &buttonB, &buttonC });
    // TextButton::SetInputHandler(&input,         { &buttonA, &buttonB, &buttonC });

    TextBox::AddToRenderer(&renderer,           { &textBox });
    TextBox::SetWindow(&window,                 { &textBox });
    TextBox::SetNormalCursor(&arrowCursor,      { &textBox });
    TextBox::SetHoverCursor(&ibeamCursor,       { &textBox });
    TextBox::SetInputHandler(&input,            { &textBox });

    while (window.isOpen())
    {
        input.update();
        if (input.isKeyTapped(Onyx::Key::Escape)) window.close();

        // TextButton::Update({ &buttonA, &buttonB, &buttonC });
        TextBox::Update({ &textBox });

        cam.update();

        window.startRender();
        renderer.render();
        window.endRender();
    }

    window.dispose();
    renderer.dispose();

    Onyx::Terminate();
    Garnet::Terminate();

    return 0;
}
