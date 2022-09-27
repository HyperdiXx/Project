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

    struct VertexPosColor
    {
        float p1, p2, p3;
        float c1, c2, c3;

        VertexPosColor() {};
        VertexPosColor(const glm::vec3& pos, const glm::vec3& color) : 
            p1(pos.x), p2(pos.y), p3(pos.z),
            c1(color.x), c2(color.y), c3(color.z) {}
    };
    
    GameWindow::GameWindow(std::wstring wnd_name) :
        Wnd(L"RenderWin", wnd_name, true),
        m_camera2d(nullptr)
    {
        m_device = std::make_shared<GDevice>(getHandle(), false);

        auto currentPath = std::filesystem::current_path();
        auto dataDir = currentPath.parent_path() / +"Data";
        auto shadersDir = dataDir / "Shaders";

        m_triangle = m_device->createShaderProgram();

        ShaderInput vsInputTri = {};

        vsInputTri.filePath = shadersDir / "triangle.hlsl";
        vsInputTri.entyPoint = "vs_main";
        vsInputTri.target = "vs_5_0";
        vsInputTri.type = ShaderType::Vertex;

        ShaderInput psInputTri = {};

        psInputTri.filePath = shadersDir / "triangle.hlsl";
        psInputTri.entyPoint = "ps_main";
        psInputTri.target = "ps_5_0";
        psInputTri.type = ShaderType::Pixel;

        m_triangle->compileFromFile(vsInputTri);
        m_triangle->compileFromFile(psInputTri);
        m_triangle->create();

        auto vbLayoutPosColor = getLayoutSelector()->add("POS", LayoutType::Float, 3)
            ->add("COL", LayoutType::Float, 3)
            ->end();

        auto vbLayoutPos = getLayoutSelector()->add("POS", LayoutType::Float, 3)
            ->end();

        float vertexIndexDataQuadArray[] =
        {
            -0.5f, -0.5f, // point at top-right
             0.5f, -0.5f, // point at bottom-right
             0.5f,  0.5f, // point at bottom-left          
            -0.5f,  0.5f // point at top-left      
        };

        float vertexDataArrayPos[] =
        {
             0.0f,  0.5f,  0.0f,
             0.5f, -0.5f,  0.0f,
            -0.5f, -0.5f,  0.0f
        };

        float vertexDataArray[] =
        {
               15.0f,  15.0f,  0.0f, 1.0f, 0.0f, 0.0f,  // point at top
               15.0f, -15.0f,  0.0f, 0.0f, 1.0f, 0.0f,  // point at bottom-right
              -15.0f, -15.0f,  0.0f, 0.0f, 0.0f, 1.0f,  // point at bottom-left

              -15.0f,  -15.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // point at bottom-left               
               15.0f,  15.0f,  0.0f, 1.0f, 0.0f, 0.0f,   // point at bottom-left               
              -15.0f,  15.0f,  0.0f, 0.0f, 1.0f, 0.0f   // point at bottom-left               
        };

        std::vector<int> index_data = { 0, 1, 2, 2, 3, 0 };

        static const size_t batchCount = 1024;
        int sizeofPosColor = sizeof(VertexPosColor);
        std::vector<VertexPosColor> vertexQuadBatch = {};
        vertexQuadBatch.reserve(batchCount);
        
        unsigned int* indexQuadBatch = new unsigned int[batchCount * 6];

        size_t offset = 0;
        for (size_t i = 0; i < batchCount * 6; i += 6)
        {
            indexQuadBatch[i] = offset + 0;
            indexQuadBatch[i + 1] = offset + 1;
            indexQuadBatch[i + 2] = offset + 2;

            indexQuadBatch[i + 3] = offset + 2;
            indexQuadBatch[i + 4] = offset + 3;
            indexQuadBatch[i + 5] = offset + 0;

            offset += 4;
        }

        glm::vec3 redColor = { 1.0f, 0.0f, 0.0f };
        glm::vec3 blueColor = { 0.0f, 0.0f, 1.0f };
        glm::vec3 greenColor = { 0.0f, 1.0f, 0.0f };

        glm::vec3 position = { 0.0f, 0.0f, 0.0f };
        glm::vec3 scale = { 5.0f, 5.0f, 0.0f };

        //for (int i = 0; i < 3; i++)
        //{
            glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { scale.x, scale.y, 1.0f });

            glm::vec4 qVert1 = transform * glm::vec4(vertexIndexDataQuadArray[0], vertexIndexDataQuadArray[1], 0.0f, 1.0f);
            glm::vec4 qVert2 = transform * glm::vec4(vertexIndexDataQuadArray[2], vertexIndexDataQuadArray[3], 0.0f, 1.0f);
            glm::vec4 qVert3 = transform * glm::vec4(vertexIndexDataQuadArray[4], vertexIndexDataQuadArray[5], 0.0f, 1.0f);
            glm::vec4 qVert4 = transform * glm::vec4(vertexIndexDataQuadArray[6], vertexIndexDataQuadArray[7], 0.0f, 1.0f);

            vertexQuadBatch.emplace_back(qVert1.xyz(), redColor);
            vertexQuadBatch.emplace_back(qVert2.xyz(), redColor);
            vertexQuadBatch.emplace_back(qVert3.xyz(), redColor);
            vertexQuadBatch.emplace_back(qVert4.xyz(), redColor);

            position.x += 10.0f;
        // }

            transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { scale.x, scale.y, 1.0f });

            qVert1 = transform * glm::vec4(vertexIndexDataQuadArray[0], vertexIndexDataQuadArray[1], 0.0f, 1.0f);
            qVert2 = transform * glm::vec4(vertexIndexDataQuadArray[2], vertexIndexDataQuadArray[3], 0.0f, 1.0f);
            qVert3 = transform * glm::vec4(vertexIndexDataQuadArray[4], vertexIndexDataQuadArray[5], 0.0f, 1.0f);
            qVert4 = transform * glm::vec4(vertexIndexDataQuadArray[6], vertexIndexDataQuadArray[7], 0.0f, 1.0f);

            vertexQuadBatch.emplace_back(qVert1.xyz(), blueColor);
            vertexQuadBatch.emplace_back(qVert2.xyz(), blueColor);
            vertexQuadBatch.emplace_back(qVert3.xyz(), blueColor);
            vertexQuadBatch.emplace_back(qVert4.xyz(), blueColor);

            position.x += 10.0f;

            transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { scale.x, scale.y, 1.0f });

            qVert1 = transform * glm::vec4(vertexIndexDataQuadArray[0], vertexIndexDataQuadArray[1], 0.0f, 1.0f);
            qVert2 = transform * glm::vec4(vertexIndexDataQuadArray[2], vertexIndexDataQuadArray[3], 0.0f, 1.0f);
            qVert3 = transform * glm::vec4(vertexIndexDataQuadArray[4], vertexIndexDataQuadArray[5], 0.0f, 1.0f);
            qVert4 = transform * glm::vec4(vertexIndexDataQuadArray[6], vertexIndexDataQuadArray[7], 0.0f, 1.0f);

            vertexQuadBatch.emplace_back(qVert1.xyz(), greenColor);
            vertexQuadBatch.emplace_back(qVert2.xyz(), greenColor);
            vertexQuadBatch.emplace_back(qVert3.xyz(), greenColor);
            vertexQuadBatch.emplace_back(qVert4.xyz(), greenColor);

            position.x += 10.0f;

        m_vb = m_device->createVertexBuffer();
        m_vb->setState(vbLayoutPosColor, batchCount * 4, vertexQuadBatch.data());
       
        m_ib = m_device->createIndexBuffer();
        m_ib->setState(static_cast<int>(batchCount * index_data.size()), indexQuadBatch);

        static const char* projectionMatrix = "projection";
        
        m_camera2d = Camera2D(m_device);
        m_camera2d.updateScreen();

        m_triangle->setValue(projectionMatrix, m_camera2d.getProj());        
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
        }
    }

    void GameWindow::mouseUp(int btn, const glm::ivec2& crd, const ShiftState& ss)
    {
    }

    void GameWindow::mouseWheel(const glm::ivec2& crd, int delta, const ShiftState& ss)
    {
    }

    void GameWindow::mouseDblClick(int btn, const glm::ivec2& crd, const ShiftState& ss)
    {
    }

    void GameWindow::keyDown(uint32_t vKey, bool duplicate)
    {
    }

    void GameWindow::keyUp(uint32_t vKey, bool duplicate)
    {
    }

    void GameWindow::paint(bool* processed)
    {
        if (!processed)
        {
            return;
        }

        RECT rct;
        GetClientRect(getHandle(), &rct);
        if (rct.right - rct.left <= 0) return;
        if (rct.bottom - rct.top <= 0) return;

        m_device->beginFrame();
        render();
        m_device->endFrame();
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
    }

    void GameWindow::render()
    {           
        m_device->getStates()->push();
        
        m_triangle->setInputBuffers(m_vb, m_ib, {}, 0);  

        m_device->getStates()->setBlend(true, Blend::Src_Alpha, Blend::Inv_Src_Alpha);
        m_device->getStates()->setCull(CullMode::None);

        m_triangle->drawIndexed(PrimTopology::Triangle, 0, m_ib->getIndexCount());
        
        m_device->getStates()->pop();
    }
}