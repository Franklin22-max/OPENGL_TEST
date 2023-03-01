#pragma once


#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class App
{
public:
    // Demonstrate the various window flags. Typically you would just use the default!
    static bool no_titlebar;
    static bool no_scrollbar;
    static bool no_menu;
    static bool no_move;
    static bool no_resize;
    static bool no_collapse;
    static bool no_close;
    static bool no_nav;
    static bool no_background;
    static bool no_bring_to_front;
    static bool unsaved_document;


    static ImGuiWindowFlags Begin(bool* p_open = NULL)
    {
        ImGuiWindowFlags window_flags = 0;
        if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
        if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
        if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
        if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
        if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
        if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
        if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
        if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
        if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
        if (unsaved_document)   window_flags |= ImGuiWindowFlags_UnsavedDocument;
        if (no_close)           p_open = NULL; // Don't pass our bool* to Begin

        // We specify a default position/size in case there's no data in the .ini file.
        // We only do it to make the demo applications a little more welcoming, but typically this isn't required.
        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

        return window_flags;
    }


   
    
};


bool App::no_titlebar = false;
bool App::no_scrollbar = false;
bool App::no_menu = false;
bool App::no_move = false;
bool App::no_resize = true;
bool App::no_collapse = false;
bool App::no_close = false;
bool App::no_nav = false;
bool App::no_background = false;
bool App::no_bring_to_front = false;
bool App::unsaved_document = false;