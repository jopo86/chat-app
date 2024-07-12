#include <iostream>
#include <vector>

#include <Garnet.h>

#include "ui/TextButton.h"
#include "ui/TextBox.h"

using Onyx::Math::IVec2, Onyx::Math::Vec2, Onyx::Math::Vec3, Onyx::Math::Vec4;
using Garnet::ServerTCP, Garnet::ClientTCP, Garnet::Address;

const int SCR_WIDTH = 600, SCR_HEIGHT = 400;

namespace Color3
{
    const Vec3 GRAY_0(0.1f, 0.12f, 0.14f);
    const Vec3 GRAY_1 = GRAY_0 * 1.5f;
    const Vec3 GRAY_2 = GRAY_1 * 1.5f;
    const Vec3 GRAY_3 = GRAY_2 * 1.5f;
    const Vec3 GRAY_4 = GRAY_3 * 1.5f;
    const Vec3 RED = Vec3(0.7f, GRAY_3.yz());
    const Vec3 GREEN = Vec3(GRAY_3.getX(), 0.7f, GRAY_3.getZ());
    const Vec3 BLUE = Vec3(GRAY_3.xy(), 0.7f);

    const Vec3 RECV = GRAY_1;
    const Vec3 SENT = Vec3(GRAY_3.getX(), 0.5f, 0.8f);
}

namespace Color4
{
    const Vec4 GRAY_0 = Vec4(Color3::GRAY_0.xyz(), 1.0f);
    const Vec4 GRAY_1 = Vec4(Color3::GRAY_1.xyz(), 1.0f);
    const Vec4 GRAY_2 = Vec4(Color3::GRAY_2.xyz(), 1.0f);
    const Vec4 GRAY_3 = Vec4(Color3::GRAY_3.xyz(), 1.0f);
    const Vec4 GRAY_4 = Vec4(Color3::GRAY_4.xyz(), 1.0f);
    const Vec4 RED = Vec4(Color3::RED, 1.0f);
    const Vec4 GREEN = Vec4(Color3::GREEN, 1.0f);
    const Vec4 BLUE = Vec4(Color3::BLUE, 1.0f);

    const Vec4 RECV = Vec4(Color3::RECV, 1.0f);
    const Vec4 SENT = Vec4(Color3::SENT, 1.0f);
}

enum class Mode
{
    Start,
    Host,
    Join,
    Server,
    Client
};

const int MSG_SPACING = 5;

std::vector<TextButton*> sentMsgs;
std::vector<TextButton*> recvMsgs;
int getNextMsgY(int msgHeight)
{
    if (sentMsgs.empty() && recvMsgs.empty()) return SCR_HEIGHT - 15 - msgHeight / 2;
    else if (sentMsgs.empty()) return recvMsgs.back()->getPosition().getY() - MSG_SPACING - recvMsgs.back()->getHeight();
    else if (recvMsgs.empty()) return sentMsgs.back()->getPosition().getY() - MSG_SPACING - sentMsgs.back()->getHeight();
    else return std::min(sentMsgs.back()->getPosition().getY(), recvMsgs.back()->getPosition().getY()) - MSG_SPACING - sentMsgs.back()->getHeight();
}

Onyx::Font font18x, font24x;

Onyx::Projection orthoStatic;
Onyx::Projection orthoDynamic;

ServerTCP server;
ClientTCP client;

std::string recvMsg;

void serverRecvCallback(void* buffer, int size, int actualSize, Address from);
void serverClientConnectCallback(Address addr);
void serverClientDisconnectCallback(Address addr);

void clientRecvCallback(void* buffer, int size, int actualSize);

int main()
{
    Mode mode = Mode::Start;

    Onyx::ErrorHandler errorHandler(true, true, Onyx::Warning::Severity::Med);
    Onyx::Init(errorHandler);
    Onyx::SetResourcePath("../resources/");
    Garnet::Init(true);

    Onyx::Monitor monitor = Onyx::Monitor::GetPrimary();

    Onyx::Window window(
        Onyx::WindowProperties{
            .title = "Chat App",
            .width = SCR_WIDTH,
            .height = SCR_HEIGHT,
            .position = IVec2(monitor.getWidth() / 2 - SCR_WIDTH / 2, monitor.getHeight() / 2 - SCR_HEIGHT / 2),
            .topmost = true,
            .backgroundColor = Color3::GRAY_0
        }
    );
    window.init();

    Onyx::WindowIcon icon = Onyx::WindowIcon::Load({
        Onyx::Resources("icons/icon-16x.png"),
        Onyx::Resources("icons/icon-24x.png"),
        Onyx::Resources("icons/icon-32x.png"),
        Onyx::Resources("icons/icon-48x.png"),
        Onyx::Resources("icons/icon-256x.png"),
    });
    window.setIcon(icon);
    icon.dispose();
    
    Onyx::InputHandler input;
    window.linkInputHandler(input);

    orthoStatic = Onyx::Projection::Orthographic(SCR_WIDTH, SCR_HEIGHT);
    orthoDynamic = Onyx::Projection::Orthographic(SCR_WIDTH, SCR_HEIGHT);

    Onyx::Cursor arrowCursor = Onyx::Cursor::Standard(Onyx::CursorType::Arrow);
    Onyx::Cursor handCursor = Onyx::Cursor::Standard(Onyx::CursorType::Hand);
    Onyx::Cursor ibeamCursor = Onyx::Cursor::Standard(Onyx::CursorType::Ibeam);

    font18x = Onyx::Font::Load(Onyx::Resources("fonts/Roboto/Roboto-Light.ttf"), 18);
    font24x = Onyx::Font::Load(Onyx::Resources("fonts/Roboto/Roboto-Light.ttf"), 24);

    TextBox tbHost(font24x, Align::Center, Color4::GRAY_1, Color4::GRAY_2, Vec4::White(), Vec4::White(), Color4::GRAY_4, "IP / Hostname", 350, 50, 10);
    TextBox tbPort(font24x, Align::Center, Color4::GRAY_1, Color4::GRAY_2, Vec4::White(), Vec4::White(), Color4::GRAY_4, "Port #", 350, 50, 10);
    tbHost.setPosition(Vec2(SCR_WIDTH / 2, SCR_HEIGHT / 2 + 60));
    tbPort.setPosition(Vec2(SCR_WIDTH / 2, SCR_HEIGHT / 2));
    tbHost.hide();
    tbPort.hide();

    TextButton btnHostChat("Host Chat", font24x, Align::Center, Color4::GRAY_1, Color4::GRAY_2, Vec4::White(), 200, 50, 10);
    TextButton btnJoinChat("Join Chat", font24x, Align::Center, Color4::GRAY_1, Color4::GRAY_2, Vec4::White(), 200, 50, 10);
    TextButton btnExit("Exit", font24x, Align::Center, Color4::GRAY_1, Color4::GRAY_2, Color4::RED, 200, 50, 10);
    TextButton btnHost("Host", font24x, Align::Center, Color4::GRAY_1, Color4::GRAY_2, Vec4::White(), 200, 50, 10);
    TextButton btnConnect("Connect", font24x, Align::Center, Color4::GRAY_1, Color4::GRAY_2, Vec4::White(), 200, 50, 10);
    btnHostChat.setPosition(Vec2(SCR_WIDTH / 2, SCR_HEIGHT / 2 + 60));
    btnJoinChat.setPosition(Vec2(SCR_WIDTH / 2, SCR_HEIGHT / 2));
    btnExit.setPosition(Vec2(SCR_WIDTH / 2, SCR_HEIGHT / 2 - 60));
    btnHost.setPosition(Vec2(SCR_WIDTH / 2 - btnHost.getWidth() / 2.0f - 7, SCR_HEIGHT / 2 - 60));
    btnConnect.setPosition(Vec2(SCR_WIDTH / 2 - btnConnect.getWidth() / 2.0f - 5, SCR_HEIGHT / 2 - 60));

    btnHost.hide();
    btnConnect.hide();

    int FUTURE_BTN_EXIT_WIDTH = 70;
    int TB_MSG_WIDTH = SCR_WIDTH - 45 - FUTURE_BTN_EXIT_WIDTH;
    int TB_MSG_HEIGHT = 45;
    TextBox tbMsg = TextBox(font18x, Align::Left, Color4::GRAY_1, Color4::GRAY_2, Vec4::White(), Vec4::White(), Color4::GRAY_4, 
        "Type a message...", TB_MSG_WIDTH, TB_MSG_HEIGHT);
    tbMsg.setPosition(Vec2(SCR_WIDTH / 2 - btnExit.getWidth() / 2 - 8, TB_MSG_HEIGHT / 2 + 15));
    tbMsg.hide();

    TextButton::SetAllPtrs(&window, &arrowCursor, &handCursor, &input,  { &btnHostChat, &btnJoinChat, &btnExit, &btnHost, &btnConnect });
    TextBox::SetAllPtrs(&window, &arrowCursor, &ibeamCursor, &input,    { &tbHost, &tbPort, &tbMsg });

    auto validHost = [&]() -> bool
    {
        if (tbHost.getText().empty()) return false;
        // can be a hostname, so can't say false bc of no number or dot
        else return true;
    };

    auto validPort = [&]() -> bool
    {
        if (tbPort.getText().empty()) return false;

        for (char c : tbPort.getText())
        {
            if (!std::isdigit(c))
            {
                return false;
            }
        }

        int port = std::stoi(tbPort.getText());
        if (port < 0 || port > 65535)
        {
            return false;
        }

        return true;
    };

    auto showInvalidHost = [&]()
    {
        tbHost.setText("");
        tbHost.unfocus();
        tbHost.setPlaceholderText("Invalid IP / Hostname");
        tbHost.setPlaceholderTextColor(Color4::RED);
    };

    auto showInvalidPort = [&]()
    {
        tbPort.setText("");
        tbPort.unfocus();
        tbPort.setPlaceholderText("Invalid Port");
        tbPort.setPlaceholderTextColor(Color4::RED);
    };

    auto resetHostPlaceholder = [&]()
    {
        tbHost.setPlaceholderText("IP / Hostname");
        tbHost.setPlaceholderTextColor(Color4::GRAY_4);
    };

    auto resetPortPlaceholder = [&]()
    {
        tbPort.setPlaceholderText("Port #");
        tbPort.setPlaceholderTextColor(Color4::GRAY_4);
    };

    auto showConnecting = [&]()
    {
        tbHost.setText("");
        tbPort.setText("");
        tbHost.unfocus();
        tbPort.unfocus();
        tbHost.setPlaceholderText("Connecting...");
        tbPort.setPlaceholderText("Please Wait");
        tbHost.setPlaceholderTextColor(Color4::BLUE);
        tbPort.setPlaceholderTextColor(Color4::BLUE);
    };

    auto showFailedToHost = [&]()
    {
        tbHost.setText("");
        tbPort.setText("");
        tbHost.unfocus();
        tbPort.unfocus();
        tbHost.setPlaceholderText("Failed to Host");
        tbPort.setPlaceholderText("Please Try Again");
        tbHost.setPlaceholderTextColor(Color4::RED);
        tbPort.setPlaceholderTextColor(Color4::RED);
    };

    auto showFailedToConnect = [&]()
    {
        std::cout << "Failed to connect\n";

        tbHost.setText("");
        tbPort.setText("");
        tbHost.unfocus();
        tbPort.unfocus();
        tbHost.setPlaceholderText("Error");
        tbPort.setPlaceholderText("Failed to Connect");
        tbHost.setPlaceholderTextColor(Color4::RED);
        tbPort.setPlaceholderTextColor(Color4::RED);
    };

    const short NOT_CONNECTED = 0;
    const short CONNECTING = 1;
    const short CONNECTED = 2;
    const short FAILED_CONNECT = -1;
    short connectStatus = NOT_CONNECTED;

    std::string host;
    ushort port;

    float tbHostResetPlaceholderTimer = -1.0f;
    float tbPortResetPlaceholderTimer = -1.0f;

    std::string msg;

    while (window.isOpen())
    {
        input.update();

        if (tbHostResetPlaceholderTimer > 0.0f)
        {
            tbHostResetPlaceholderTimer -= window.getDeltaTime();
            if (tbHostResetPlaceholderTimer <= 0.0f && tbHostResetPlaceholderTimer > -1.0f)
            {
                tbHostResetPlaceholderTimer = -1.0f;
                resetHostPlaceholder();
            }
        }
        if (tbPortResetPlaceholderTimer > 0.0f)
        {
            tbPortResetPlaceholderTimer -= window.getDeltaTime();
            if (tbPortResetPlaceholderTimer <= 0.0f && tbPortResetPlaceholderTimer > -1.0f)
            {
                tbPortResetPlaceholderTimer = -1.0f;
                resetPortPlaceholder();
            }
        }

        if (font18x.getStringWidth(tbMsg.getText()) > tbMsg.getWidth() - 20) tbMsg.setAlign(Align::Right);
        else tbMsg.setAlign(Align::Left);

        TextButton::Update({ &btnHostChat, &btnJoinChat, &btnExit, &btnHost, &btnConnect });
        TextBox::Update({ &tbHost, &tbPort, &tbMsg });

        bool esc = input.isKeyTapped(Onyx::Key::Escape);
        bool click = input.isMouseButtonTapped(Onyx::MouseButton::Left);
        bool tab = input.isKeyTapped(Onyx::Key::Tab);
        bool enter = input.isKeyTapped(Onyx::Key::Enter);

        switch (mode)
        {
        case Mode::Start:

            if (click)
            {
                if (btnHostChat.isHovered())
                {
                    mode = Mode::Host;
                    btnHostChat.hide();
                    btnJoinChat.hide();
                    tbHost.show();
                    tbPort.show();
                    btnHost.show();
                    btnExit.setPosition(Vec2(SCR_WIDTH / 2 + btnExit.getWidth() / 2 + 5, SCR_HEIGHT / 2 - 60));
                }
                if (btnJoinChat.isHovered())
                {
                    mode = Mode::Join;
                    btnHostChat.hide();
                    btnJoinChat.hide();
                    tbHost.show();
                    tbPort.show();
                    btnConnect.show();
                    btnExit.setPosition(Vec2(SCR_WIDTH / 2 + btnExit.getWidth() / 2 + 5, SCR_HEIGHT / 2 - 60));
                }
                if (btnExit.isHovered()) window.close();
            }
            break;

        case Mode::Host:
            if (esc)
            {
                tbHost.unfocus();
                tbPort.unfocus();
            }

            if (btnHost.isHovered() && click || enter)
            {
                bool valid = true;

                if (!validHost())
                {
                    showInvalidHost();
                    tbHostResetPlaceholderTimer = 3.0f;
                    valid = false;
                }
                else resetHostPlaceholder();

                if (!validPort())
                {
                    showInvalidPort();
                    tbPortResetPlaceholderTimer = 3.0f;
                    valid = false;
                }
                else resetPortPlaceholder();

                if (!valid) break;

                bool successA, successB;
                host = tbHost.getText();
                port = (ushort)std::stoi(tbPort.getText());
                new (&server) ServerTCP(Address{
                    .host = host,
                    .port = port
                }, &successA);
                server.open(10, &successB);

                if (!successA || !successB)
                {
                    showFailedToHost();
                    tbHostResetPlaceholderTimer = 3.0f;
                    tbPortResetPlaceholderTimer = 3.0f;
                }
                else
                {
                    mode = Mode::Server;
                    window.setTitle("Chat App - Host (" + host + ":" + std::to_string(port) + ")");
                    tbHost.hide();
                    tbPort.hide();
                    btnHost.hide();
                    tbMsg.show();
                    btnExit = TextButton("Exit", font18x, Align::Center, Color4::GRAY_1, Color4::GRAY_2, Color4::RED, FUTURE_BTN_EXIT_WIDTH, TB_MSG_HEIGHT, 10);
                    btnExit.setAllPtrs(&window, &arrowCursor, &arrowCursor, &input);
                    tbMsg.setPosition(Vec2(SCR_WIDTH / 2 - btnExit.getWidth() / 2 - 8, TB_MSG_HEIGHT / 2 + 15));
                    btnExit.setPosition(Vec2(SCR_WIDTH / 2 + TB_MSG_WIDTH / 2 + 7, TB_MSG_HEIGHT / 2 + 15));

                    server.setReceiveCallback(serverRecvCallback);
                    server.setClientConnectCallback(serverClientConnectCallback);
                    server.setClientDisconnectCallback(serverClientDisconnectCallback);
                }
            }

            if (tab)
            {
                if (tbHost.isFocused())
                {
                    tbHost.unfocus();
                    tbPort.focus();
                }
                else if (tbPort.isFocused())
                {
                    tbPort.unfocus();
                }
            }

            if (btnExit.isHovered() && click) window.close();
            break;

        case Mode::Join:
            if (connectStatus == FAILED_CONNECT)
            {
                showFailedToConnect();
                connectStatus = NOT_CONNECTED;
                tbHostResetPlaceholderTimer = 3.0f;
                tbPortResetPlaceholderTimer = 3.0f;
            }
            else if (connectStatus == CONNECTED)
            {
                mode = Mode::Client;
                window.setTitle("Chat App - Client");
                tbHost.hide();
                tbPort.hide();
                btnConnect.hide();
                tbMsg.show();
                btnExit = TextButton("Exit", font18x, Align::Center, Color4::GRAY_1, Color4::GRAY_2, Color4::RED, FUTURE_BTN_EXIT_WIDTH, TB_MSG_HEIGHT, 10);
                btnExit.setAllPtrs(&window, &arrowCursor, &arrowCursor, &input);
                tbMsg.setPosition(Vec2(SCR_WIDTH / 2 - btnExit.getWidth() / 2 - 8, TB_MSG_HEIGHT / 2 + 15));
                btnExit.setPosition(Vec2(SCR_WIDTH / 2 + TB_MSG_WIDTH / 2 + 7, TB_MSG_HEIGHT / 2 + 15));

                client.setReceiveCallback(clientRecvCallback);
            }

            if (esc)
            {
                tbHost.unfocus();
                tbPort.unfocus();
            }

            if (btnConnect.isHovered() && click || enter)
            {
                bool valid = true;

                if (!validHost())
                {
                    showInvalidHost();
                    tbHostResetPlaceholderTimer = 3.0f;
                    valid = false;
                }
                else resetHostPlaceholder();

                if (!validPort())
                {
                    showInvalidPort();
                    tbPortResetPlaceholderTimer = 3.0f;
                    valid = false;
                }
                else resetPortPlaceholder();

                if (!valid) break;

                host = tbHost.getText();
                port = (ushort)std::stoi(tbPort.getText());
                showConnecting();

                auto connect = [&]()
                {
                    connectStatus = CONNECTING;
                    bool successA, successB;
                    new (&client) ClientTCP('c', &successA);

                    client.connect(Address{
                        .host = host,
                        .port = port
                    }, &successB);
                    if (!successA || !successB) connectStatus = FAILED_CONNECT;
                    else connectStatus = CONNECTED;
                };
                
                std::thread t(connect);
                t.detach();
            }

            if (tab)
            {
                if (tbHost.isFocused())
                {
                    tbHost.unfocus();
                    tbPort.focus();
                }
                else if (tbPort.isFocused())
                {
                    tbPort.unfocus();
                }
            }

            if (btnExit.isHovered() && click) window.close();
            break;

        case Mode::Server:
        {
            if (esc) tbMsg.unfocus();
            if (btnExit.isHovered() && click) window.close();

            float scroll = (float)input.getScrollDeltas().getY() * 50.0f;
            if (orthoDynamic.getTop() + scroll > SCR_HEIGHT)
            {
                orthoDynamic.setTop(SCR_HEIGHT);
                orthoDynamic.setBottom(0);
            }
            else if (orthoDynamic.getBottom() + scroll <= getNextMsgY(30) - MSG_SPACING - tbMsg.getHeight())
            {
                if (getNextMsgY(30) - MSG_SPACING - 15 < tbMsg.getHeight())
                {                   
                    orthoDynamic.setBottom(getNextMsgY(30) - MSG_SPACING - tbMsg.getHeight());
                    orthoDynamic.setTop(orthoDynamic.getBottom() + SCR_HEIGHT);
                }
            }
            else
            {
                orthoDynamic.setTop(orthoDynamic.getTop() + scroll);
                orthoDynamic.setBottom(orthoDynamic.getBottom() + scroll);
            }

            if (enter)
            {
                msg = tbMsg.getText();
                if (!msg.empty())
                {
                    TextButton* pBtn = new TextButton(msg, font18x, Align::Right, Color4::SENT, Color4::SENT, Vec4::White(), 10, -2);
                    pBtn->setAllPtrs(&window, &arrowCursor, &arrowCursor, &input);
                    pBtn->setPosition(Vec2(SCR_WIDTH - 15 - pBtn->getWidth() / 2, getNextMsgY(pBtn->getHeight())));
                    sentMsgs.push_back(pBtn);

                    if (getNextMsgY(30) - MSG_SPACING - 15 < tbMsg.getHeight())
                    {
                        orthoDynamic.setBottom(getNextMsgY(30) - MSG_SPACING - tbMsg.getHeight());
                        orthoDynamic.setTop(orthoDynamic.getBottom() + SCR_HEIGHT);
                    }

                    msg = "Host: " + msg;
                    for (const Address& addr : server.getClientAddresses())
                    {
                        server.send((void*)(msg.c_str()), msg.length(), addr);
                    }


                    tbMsg.setText("");
                }
            }

            if (!recvMsg.empty())
            {
                TextButton* pBtn = new TextButton(recvMsg, font18x, Align::Center, Color4::RECV, Color4::RECV, Vec4::White(), 10, -2);
                pBtn->setAllPtrs(&window, &arrowCursor, &arrowCursor, &input);
                pBtn->setPosition(Vec2(15 + pBtn->getWidth() / 2, getNextMsgY(pBtn->getHeight())));
                recvMsgs.push_back(pBtn);

                if (getNextMsgY(30) - MSG_SPACING - 15 < tbMsg.getHeight())
                {
                    orthoDynamic.setBottom(getNextMsgY(30) - MSG_SPACING - tbMsg.getHeight());
                    orthoDynamic.setTop(orthoDynamic.getBottom() + SCR_HEIGHT);
                }

                recvMsg = "";
            }

            break;
        }
        case Mode::Client:
        {
            if (esc) tbMsg.unfocus();
            if (btnExit.isHovered() && click) window.close();

            float scroll = (float)input.getScrollDeltas().getY() * 50.0f;
            if (orthoDynamic.getTop() + scroll > SCR_HEIGHT)
            {
                orthoDynamic.setTop(SCR_HEIGHT);
                orthoDynamic.setBottom(0);
            }
            else if (orthoDynamic.getBottom() + scroll <= getNextMsgY(30) - MSG_SPACING - tbMsg.getHeight())
            {
                if (getNextMsgY(30) - MSG_SPACING - 15 < tbMsg.getHeight())
                {                   
                    orthoDynamic.setBottom(getNextMsgY(30) - MSG_SPACING - tbMsg.getHeight());
                    orthoDynamic.setTop(orthoDynamic.getBottom() + SCR_HEIGHT);
                }
            }
            else
            {
                orthoDynamic.setTop(orthoDynamic.getTop() + scroll);
                orthoDynamic.setBottom(orthoDynamic.getBottom() + scroll);
            }

            if (enter)
            {
                msg = tbMsg.getText();
                if (!msg.empty())
                {
                    client.send((void*)(msg.c_str()), msg.length());

                    TextButton* pBtn = new TextButton(msg, font18x, Align::Right, Color4::SENT, Color4::SENT, Vec4::White(), 10, -2);
                    pBtn->setAllPtrs(&window, &arrowCursor, &arrowCursor, &input);
                    pBtn->setPosition(Vec2(SCR_WIDTH - 15 - pBtn->getWidth() / 2, getNextMsgY(pBtn->getHeight())));
                    sentMsgs.push_back(pBtn);

                    if (getNextMsgY(30) - MSG_SPACING - 15 < tbMsg.getHeight())
                    {
                        orthoDynamic.setBottom(getNextMsgY(30) - MSG_SPACING - tbMsg.getHeight());
                        orthoDynamic.setTop(orthoDynamic.getBottom() + SCR_HEIGHT);
                    }

                    tbMsg.setText("");
                }
            }

            if (!recvMsg.empty())
            {
                TextButton* pBtn = new TextButton(recvMsg, font18x, Align::Center, Color4::RECV, Color4::RECV, Vec4::White(), 10, -2);
                pBtn->setAllPtrs(&window, &arrowCursor, &arrowCursor, &input);
                pBtn->setPosition(Vec2(15 + pBtn->getWidth() / 2, getNextMsgY(pBtn->getHeight())));
                recvMsgs.push_back(pBtn);

                if (getNextMsgY(30) - MSG_SPACING - 15 < tbMsg.getHeight())
                {
                    orthoDynamic.setBottom(getNextMsgY(30) - MSG_SPACING - tbMsg.getHeight());
                    orthoDynamic.setTop(orthoDynamic.getBottom() + SCR_HEIGHT);
                }

                recvMsg = "";
            }

            break;
        }
        }

        window.startRender();
        
        tbHost.render(orthoStatic.getMatrix());
        tbPort.render(orthoStatic.getMatrix());
        btnHostChat.render(orthoStatic.getMatrix());
        btnJoinChat.render(orthoStatic.getMatrix());
        btnExit.render(orthoStatic.getMatrix());
        btnHost.render(orthoStatic.getMatrix());
        btnConnect.render(orthoStatic.getMatrix());
        tbMsg.render(orthoStatic.getMatrix());
        for (TextButton* pBtn : sentMsgs) pBtn->render(orthoDynamic.getMatrix());
        for (TextButton* pBtn : recvMsgs) pBtn->render(orthoDynamic.getMatrix());

        window.endRender();
    }

    window.dispose();
    font18x.dispose();
    font24x.dispose();
    for (TextButton* pBtn : sentMsgs) delete pBtn;
    for (TextButton* pBtn : recvMsgs) delete pBtn;

    if (server.isOpen()) server.close();
    if (client.isConnected()) client.disconnect();

    Onyx::Terminate();
    Garnet::Terminate();

    return 0;
}

void serverRecvCallback(void* buffer, int size, int actualSize, Address from)
{
    std::string msg((char*)buffer, actualSize);
    msg = "Client (" + from.host + ":" + std::to_string(from.port) + "): " + msg;
    recvMsg = msg;

    for (const Address& addr : server.getClientAddresses())
    {
        if (addr != from) server.send((void*)(msg.c_str()), msg.length(), addr);
    }

    delete buffer;
}

void serverClientConnectCallback(Address addr)
{
    std::string msg = "Client (" + addr.host + ":" + std::to_string(addr.port) + ") connected.";
    recvMsg = msg;

    for (const Address& _addr : server.getClientAddresses())
    {
        if (_addr != addr) server.send((void*)(msg.c_str()), msg.length(), _addr);
    }
}

void serverClientDisconnectCallback(Address addr)
{
    std::string msg = "Client (" + addr.host + ":" + std::to_string(addr.port) + ") disconnected.";
    recvMsg = msg;

    for (const Address& _addr : server.getClientAddresses())
    {
        if (_addr != addr) server.send((void*)(msg.c_str()), msg.length(), _addr);
    }
}

void clientRecvCallback(void* buffer, int size, int actualSize)
{
    std::string msg((char*)buffer, actualSize);
    recvMsg = msg;

    delete buffer;
}
