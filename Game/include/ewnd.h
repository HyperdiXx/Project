#pragma once

#include "eutils.h"
#include "egraphics.h"
#include "world/eworld.h"

#define NOMINMAX
#include <windows.h>
#include <eheader.h>

namespace EProject
{
    struct ShiftState
    {
        bool ctrl;
        bool shift;
        bool mouse_btn[5];
    };

    class Wnd
    {
    public:
        Wnd(const std::wstring& class_name, std::wstring wnd_caption, bool isMainWindow);
        virtual ~Wnd();

        virtual void mouseMove(const glm::ivec2& crd, const ShiftState& ss);
        virtual void mouseDown(int btn, const glm::ivec2& crd, const ShiftState& ss);
        virtual void mouseUp(int btn, const glm::ivec2& crd, const ShiftState& ss);
        virtual void mouseWheel(const glm::ivec2& crd, int delta, const ShiftState& ss);
        virtual void mouseDblClick(int btn, const glm::ivec2& crd, const ShiftState& ss);
        virtual void keyDown(uint32_t vKey, bool duplicate);
        virtual void keyUp(uint32_t vKey, bool duplicate);
        virtual void paint(bool* processed);
        virtual void windowResized(const glm::ivec2& new_size);
        virtual LRESULT wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

        HWND getHandle() const;
        void invalidate();
    protected:
        bool m_isMainWindow;
    private:
        HWND m_hwnd;
        ATOM m_wndclass;
        std::wstring m_class_name;
    };

    class GameWindow : public Wnd
    {
    public:
        GameWindow(std::wstring wnd_name);
        ~GameWindow();

        virtual void mouseMove(const glm::ivec2& crd, const ShiftState& ss);
        virtual void mouseDown(int btn, const glm::ivec2& crd, const ShiftState& ss);
        virtual void mouseUp(int btn, const glm::ivec2& crd, const ShiftState& ss);
        virtual void mouseWheel(const glm::ivec2& crd, int delta, const ShiftState& ss);
        virtual void mouseDblClick(int btn, const glm::ivec2& crd, const ShiftState& ss);
        virtual void keyDown(uint32_t vKey, bool duplicate);
        virtual void keyUp(uint32_t vKey, bool duplicate);
        virtual void execute(bool* processed);
        virtual void windowResized(const glm::ivec2& new_size);
    
    protected:
        virtual void fixedUpdate(float _ts);
        virtual void update(float _ts);
        virtual void render();

    private:

        std::shared_ptr<GDevice> m_device;       
        std::shared_ptr<AssetManager> m_manager;

        Camera2DPtr m_camera2d;
        Render2D m_canvas;

        Camera3DPtr m_camera3d;
        Render3D m_render3d;
        
        World m_world;

        float m_zoom = 60.0f;
    };

    void MessageLoop(const std::function<void()> idle_proc);
    void MessageLoop();
}
