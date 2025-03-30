#include "ewnd.h"

namespace EProject
{
    LRESULT CALLBACK DefWndProc(
        _In_ HWND   hwnd,
        _In_ UINT   uMsg,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam
    ) 
    {
        if (uMsg == WM_NCCREATE) {
            SetPropW(hwnd, L"WndPtr", ((tagCREATESTRUCTW*)lParam)->lpCreateParams);
            return true;
        }
        Wnd* win = (Wnd*)GetPropW(hwnd, L"WndPtr");
        if (win)
        {
            return win->wndProc(hwnd, uMsg, wParam, lParam);
        }
        else
        {
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);
        }
    }

    ShiftState toShiftState(WPARAM wParam)
    {
        ShiftState res;
        res.ctrl = wParam & MK_CONTROL;
        res.shift = wParam & MK_SHIFT;
        res.mouse_btn[0] = wParam & MK_LBUTTON;
        res.mouse_btn[1] = wParam & MK_RBUTTON;
        res.mouse_btn[2] = wParam & MK_MBUTTON;
        res.mouse_btn[3] = wParam & MK_XBUTTON1;
        res.mouse_btn[4] = wParam & MK_XBUTTON2;
        return res;
    }

    Wnd::Wnd(const std::wstring& class_name, std::wstring wnd_caption, bool isMainWindow)
    {
        m_isMainWindow = isMainWindow;
        m_class_name = class_name;
        
        WNDCLASSEXW wcex = {};

        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        wcex.lpfnWndProc = DefWndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = GetModuleHandle(nullptr);
        wcex.hIcon = LoadIcon(GetModuleHandle(nullptr), IDI_APPLICATION);
        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground = 0;
        wcex.lpszMenuName = NULL;
        wcex.lpszClassName = m_class_name.c_str();
        wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

        m_wndclass = RegisterClassExW(&wcex);

        m_hwnd = CreateWindowExW(
            WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW,
            m_class_name.c_str(),
            wnd_caption.c_str(),
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            0,
            0,
            GetModuleHandle(nullptr),
            this
        );
    }

    Wnd::~Wnd()
    {
        if (m_hwnd)
        {
            DestroyWindow(m_hwnd);
        }
            
        UnregisterClassW(m_class_name.c_str(), GetModuleHandle(nullptr));
    }

    void Wnd::mouseMove(const glm::ivec2& crd, const ShiftState& ss)
    {
    }

    void Wnd::mouseDown(int btn, const glm::ivec2& crd, const ShiftState& ss)
    {
    }

    void Wnd::mouseUp(int btn, const glm::ivec2& crd, const ShiftState& ss)
    {
    }

    void Wnd::mouseWheel(const glm::ivec2& crd, int delta, const ShiftState& ss)
    {
    }

    void Wnd::mouseDblClick(int btn, const glm::ivec2& crd, const ShiftState& ss)
    {
    }

    void Wnd::keyDown(uint32_t vKey, bool duplicate)
    {      
    }

    void Wnd::keyUp(uint32_t vKey, bool duplicate)
    {
    }

    void Wnd::paint(bool* processed)
    {
    }

    void Wnd::windowResized(const glm::ivec2& new_size)
    {
    }

    LRESULT Wnd::wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        auto toCoord = [](LPARAM lParam)->glm::ivec2 {
            glm::ivec2 p;
            p.x = int(lParam & 0xffff);
            p.y = int(lParam >> 16);
            return p;
        };

        switch (uMsg) {
        case WM_CLOSE:
            DestroyWindow(m_hwnd);
            m_hwnd = 0;
            return 0;
        case WM_DESTROY:
            if (m_isMainWindow)
                PostQuitMessage(0);
            return 0;
        case WM_LBUTTONDOWN:
            mouseDown(0, toCoord(lParam), toShiftState(wParam));
            return 0;
        case WM_RBUTTONDOWN:
            mouseDown(1, toCoord(lParam), toShiftState(wParam));
            return 0;
        case WM_MBUTTONDOWN:
            mouseDown(2, toCoord(lParam), toShiftState(wParam));
            return 0;
        case WM_XBUTTONDOWN:
            mouseDown(int(wParam >> 16) + 2, toCoord(lParam), toShiftState(wParam));
            return 0;
        case WM_LBUTTONUP:
            mouseUp(0, toCoord(lParam), toShiftState(wParam));
            return 0;
        case WM_RBUTTONUP:
            mouseUp(1, toCoord(lParam), toShiftState(wParam));
            return 0;
        case WM_MBUTTONUP:
            mouseUp(2, toCoord(lParam), toShiftState(wParam));
            return 0;
        case WM_XBUTTONUP:
            mouseUp(int(wParam >> 16) + 2, toCoord(lParam), toShiftState(wParam));
            return 0;
        case WM_MOUSEMOVE:
            mouseMove(toCoord(lParam), toShiftState(wParam));
            return 0;
        case WM_MOUSEWHEEL:
            glm::ivec2 crd = toCoord(lParam);
            POINT pt;
            pt.x = crd.x;
            pt.y = crd.y;
            ScreenToClient(hwnd, &pt);
            crd.x = pt.x;
            crd.y = pt.y;
            mouseWheel(crd, GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA, toShiftState(wParam));
            return 0;
        case WM_LBUTTONDBLCLK:
            mouseDblClick(0, toCoord(lParam), toShiftState(wParam));
            return 0;
        case WM_RBUTTONDBLCLK:
            mouseDblClick(1, toCoord(lParam), toShiftState(wParam));
            return 0;
        case WM_MBUTTONDBLCLK:
            mouseDblClick(2, toCoord(lParam), toShiftState(wParam));
            return 0;
        case WM_XBUTTONDBLCLK:
            mouseDblClick(int(wParam >> 16) + 2, toCoord(lParam), toShiftState(wParam));
            return 0;
        case WM_KEYDOWN:
            keyDown(uint32_t(wParam), (lParam & (1 << 30)) != 0);
            return 0;
        case WM_KEYUP:
            keyUp(uint32_t(wParam), (lParam & (1 << 30)) != 0);
            return 0;
        case WM_PAINT:
        {
            bool processed = false;
            paint(&processed);
            if (processed)
            {
                ValidateRect(hwnd, nullptr);
                return 0;
            }
            break;
        }
        case WM_SIZE:
            UINT width = LOWORD(lParam);
            UINT height = HIWORD(lParam);
            windowResized(glm::ivec2(width, height));
            return 0;
        }
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }

    HWND Wnd::getHandle() const
    {
        return m_hwnd;
    }

    void Wnd::invalidate()
    {
        InvalidateRect(m_hwnd, nullptr, false);
    }
    
    void MessageLoop(const std::function<void()> idle_proc)
    {
        MSG msg = {};
        while (true)
        {
            while (PeekMessageW(&msg, 0, 0, 0, PM_REMOVE))
            {
                if (msg.message == WM_QUIT) return;
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }            
            idle_proc();
        }
    }
    
    void MessageLoop()
    {
        MSG msg;
        while (GetMessageW(&msg, 0, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    GameWindow::GameWindow(std::wstring wnd_name) :
        Wnd(L"RenderWin", wnd_name, true),
        m_manager(nullptr),
        m_camera2d(nullptr),
        m_canvas(nullptr, m_camera2d),
        m_camera3d(nullptr),
        m_render3d(nullptr, m_camera3d)
    {                       
        m_device = std::make_shared<GDevice>(getHandle(), false);
        
        m_manager = std::make_shared<AssetManager>(m_device);

        m_camera2d = std::make_shared<Camera2D>(m_device);
        m_camera2d->updateScreen(m_zoom);

        m_canvas = Render2D(m_device, m_camera2d);
        m_canvas.init(m_manager);
       
        m_camera3d = std::make_shared<Camera3D>(m_device);

        m_camera3d->setPosition({ 0.0f, 0.0f, -10.0f });
        m_camera3d->lookAt(m_camera3d->getPosition(), MVEK3FORWARD, MVEK3UP);

        m_render3d = Render3D(m_device, m_camera3d);
        m_render3d.init(m_manager);

        m_world.init(m_manager, m_device);

        // Timer...
        m_current = std::chrono::high_resolution_clock::now();
        m_last = std::chrono::high_resolution_clock::now();
        m_deltaTime = std::chrono::duration<float>(0.0f);
    }

    GameWindow::~GameWindow()
    {
    }

    void GameWindow::mouseMove(const glm::ivec2& crd, const ShiftState& ss)
    {
    }

    void GameWindow::mouseDown(int btn, const glm::ivec2& crd, const ShiftState& ss)
    {
        if (ss.mouse_btn[0])
        {
            std::cout << "LMouse button pressed!\n";
            std::cout << "Mouse coord: " << crd.x << " " << crd.y << "\n";
        }
    }

    void GameWindow::mouseUp(int btn, const glm::ivec2& crd, const ShiftState& ss)
    {
    }

    void GameWindow::mouseWheel(const glm::ivec2& crd, int delta, const ShiftState& ss)
    {
        m_zoom += delta;

        glm::fclamp(m_zoom, 0.1f, 90.0f);

        std::cout << m_zoom << "\n";

        m_camera2d->updateScreen(m_zoom);
        m_canvas.markDirty();
    }

    void GameWindow::mouseDblClick(int btn, const glm::ivec2& crd, const ShiftState& ss)
    {
    }

    void GameWindow::keyDown(uint32_t vKey, bool duplicate)
    {
        constexpr float cameraSpeed = 0.2f;
        glm::vec3 newPos = m_camera3d->getPosition();

        glm::vec3 direction = {};
        direction.x = cos(glm::radians(45.0f)) * cos(glm::radians(45.0f));
        direction.y = sin(glm::radians(45.0f));
        direction.z = sin(glm::radians(45.0f)) * cos(glm::radians(45.0f));
        glm::vec3 cameraFront = glm::normalize(direction);

        if (vKey == 'W')
        {
            newPos += MVEK3FORWARD * cameraSpeed;
        }
        if (vKey == 'S')
        {
            newPos -= MVEK3FORWARD * cameraSpeed;
        }
        if (vKey == 'A')
        {
            newPos -= glm::normalize(glm::cross(MVEK3FORWARD, MVEK3UP)) * cameraSpeed;
        }
        if (vKey == 'D')
        {
            newPos += glm::normalize(glm::cross(MVEK3FORWARD, MVEK3UP)) * cameraSpeed;
        }

        m_camera3d->setPosition(newPos);
        m_camera3d->lookAt(newPos, newPos + MVEK3FORWARD, MVEK3UP);
    }

    void GameWindow::keyUp(uint32_t vKey, bool duplicate)
    {
    }

    void GameWindow::execute(bool* processed)
    { 
        if (!processed)
        {
            return;
        }

        RECT rct;
        GetClientRect(getHandle(), &rct);
        if (rct.right - rct.left <= 0) return;
        if (rct.bottom - rct.top <= 0) return;

        m_current = std::chrono::high_resolution_clock::now();
        m_deltaTime = m_current - m_last;

        fixedUpdate(m_deltaTime.count());
        update(m_deltaTime.count());

        m_device->beginFrame();
        render();
        m_device->endFrame();

        m_last = m_current;
        *processed = true;
    }

    void GameWindow::windowResized(const glm::ivec2& new_size)
    {
       
    }

    void GameWindow::fixedUpdate(float _ts)
    {
    }

    void GameWindow::update(float _ts)
    {
        FrameInfo frame;
        frame.dt = _ts;
        frame.render2DPtr = &m_canvas;
        frame.render3DPtr = &m_render3d;

        m_world.update(frame);
    }

    void GameWindow::render()
    {           
        FrameInfo frame;
        frame.render2DPtr = &m_canvas;
        frame.render3DPtr = &m_render3d;

        auto* states = m_device->getStates();
        
        states->push();
        m_world.draw(frame);
        states->pop();

        m_canvas.draw();
    }
}