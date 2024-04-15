#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#if 1
// #define _X86_
#define _AMD64_
// #include <minwindef.h>
// #include <winnt.h>
#include <windef.h>
#include <processenv.h>
#include <fileapi.h>

#define FILE_FLAG_WRITE_THROUGH         0x80000000
#define FILE_FLAG_OVERLAPPED            0x40000000
#define FILE_FLAG_NO_BUFFERING          0x20000000
#define FILE_FLAG_RANDOM_ACCESS         0x10000000
#define FILE_FLAG_SEQUENTIAL_SCAN       0x08000000
#define FILE_FLAG_DELETE_ON_CLOSE       0x04000000
#define FILE_FLAG_BACKUP_SEMANTICS      0x02000000
#define FILE_FLAG_POSIX_SEMANTICS       0x01000000
#define FILE_FLAG_SESSION_AWARE         0x00800000
#define FILE_FLAG_OPEN_REPARSE_POINT    0x00200000
#define FILE_FLAG_OPEN_NO_RECALL        0x00100000
#define FILE_FLAG_FIRST_PIPE_INSTANCE   0x00080000

#include <handleapi.h>
#include <errhandlingapi.h>
#include <memoryapi.h>
#include <profileapi.h>
#include <wingdi.h>
#include <winuser.h>
#include <synchapi.h>
#include <intrin.h>
#include <winbase.h>
#include <commdlg.h>
#include <io.h>
#else
#include <windows.h>
#endif
#include <windowsx.h>

#include <xaudio2.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define error vorbis_error
#include <stb/stb_vorbis.c>
void STB_assert(int x) {assert(x);}
#define STBTT_assert(x) STB_assert(x)
#define STBRP_ASSERT(x) STB_assert(x)
#define STB_RECT_PACK_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb/stb_rect_pack.h"
#include "stb/stb_truetype.h"
#undef error

#undef L
#undef R
#undef C

//eww, I got std's
#include <string>
#include "levenshtein-sse.hpp"

#undef abs
#undef swap
#undef assert

#include <maths/maths.h>
#include <utils/misc.h>

FILE* logfile = 0;
int logprintf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    if(!logfile) return 0;
    int out = vfprintf(logfile, format, args);
    vprintf(format, args);
    va_end(args);
    return out;
}
#define PRINTFN logprintf
#define FLUSHFN() fflush(logfile); fflush(stdout)
#include <utils/logging.h>

// #define platform_big_alloc(memory_size) VirtualAlloc(0, memory_size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
void* platform_big_alloc(size_t memory_size)
{
    void* out = VirtualAlloc(0, memory_size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    assert(out, "VirtualAlloc failed allocating ", memory_size, " bytes, error code = ", (int) GetLastError(),"\n");
    return out;
}
// #define platform_big_alloc(memory_size) malloc(memory_size);

#include "memory.cpp"
#include "context.cpp"

#include "win32_work_system.h"

struct file_t {
    char filename[256];
    HANDLE handle;
};

define_printer(DWORD a, ("%li", a));

file_t open_file(char* filename, DWORD disposition)
{
    file_t file;
    memcpy(file.filename, filename, strlen(filename));
    file.handle = CreateFile(filename, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, 0, disposition, FILE_FLAG_SEQUENTIAL_SCAN, 0);
    assert(file.handle != INVALID_HANDLE_VALUE, GetLastError(), ", could not open file: ", filename);
    return file;
}

void close_file(file_t file)
{
    CloseHandle(file.handle);
}

size_t sizeof_file(file_t file)
{
    LARGE_INTEGER file_size;
    auto error = GetFileSizeEx(file.handle, &file_size);
    assert(error, GetLastError(), " getting file size of ", file.filename, "\n");

    return file_size.QuadPart;
}

void read_from_disk(void* data, file_t file, size_t offset, size_t size)
{
    OVERLAPPED overlapped = {
        .Offset = offset,
        .OffsetHigh = offset>>32,
    };
    int error = ReadFile(file.handle, data, size, 0, &overlapped);
}

void write_to_disk(file_t file, size_t offset, void* data, size_t size)
{
    OVERLAPPED overlapped = {
        .Offset = offset,
        .OffsetHigh = offset>>32,
    };
    int error = WriteFile(file.handle, data, size, 0, &overlapped);
}

HWND main_hwnd;

__inline uint get_cd(char* output)
{
    return GetCurrentDirectory(GetCurrentDirectory(0,0), output);
}

__inline void set_cd(char* directory_name)
{
    SetCurrentDirectory(directory_name);
}

char* load_file_0_terminated(char* filename)
{
    HANDLE file = CreateFile(filename,
                             GENERIC_READ,
                             FILE_SHARE_READ, 0,
                             OPEN_EXISTING,
                             FILE_FLAG_SEQUENTIAL_SCAN, 0);

    if(file == INVALID_HANDLE_VALUE)
    {
        log_error("windows code ", GetLastError(), ", file ", filename, " could not be found\n");
    }

    union
    {
        uint64 file_size;
        struct
        {
            DWORD file_size_low;
            DWORD file_size_high;
        };
    };
    file_size_low = GetFileSize(file, &file_size_high);
    if(file_size_low == INVALID_FILE_SIZE)
    {
        log_error(GetLastError(), " opening file\n");
    }

    char* output = (char*) dynamic_alloc(file_size+1);

    DWORD bytes_read;
    int error = ReadFile(file,
                         output,
                         file_size,
                         &bytes_read,
                         0);
    if(!error)
    {
        log_error("error reading file\n");
    }
    CloseHandle(file);

    output[file_size+1] = 0;
    return output;
}

#include "vk_graphics.h"
#include "xaudio2_audio.h"

#include "settings.h"

#include "game.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_SIZE: {
            vkon.framebuffer_resized = true;
            break;
        }
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

window_t create_window(char* window_title, char* class_name, int width, int height, int x, int y, HINSTANCE hinstance)
{
    int error;
    WNDCLASSEX wc;
    {//init the window class
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wc.lpfnWndProc = WndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = hinstance;
        wc.hIcon = LoadIcon(hinstance, MAKEINTRESOURCE(123));
        wc.hCursor = LoadCursor(0, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
        wc.lpszMenuName = 0;
        wc.lpszClassName = class_name;
        wc.hIconSm = LoadIcon(hinstance, MAKEINTRESOURCE(123));

        error = RegisterClassEx(&wc);
        assert(error, "window registration failed");
    }

    HWND hwnd = CreateWindowEx(
        WS_EX_APPWINDOW, //extended window style
        class_name, //the class name
        window_title, //The window title
        WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, //window style
        x, y, //window_position
        width, height, //window dimensions
        0, //handle to the parent window, this has no parents
        0, //menu handle
        hinstance, //duh
        0 //lparam
        );
    assert(hwnd, "window creation failed");

    // TITLEBARINFOEX title_info;
    // title_info.cbSize = sizeof(TITLEBARINFOEX);
    // SendMessage(hwnd, WM_GETTITLEBARINFOEX,0, (LPARAM) &title_info);
    // int title_height = title_info.rcTitleBar.bottom-title_info.rcTitleBar.top;
    // int title_width  = title_info.rcTitleBar.right -title_info.rcTitleBar.left;
    // log_output("title_height = ", title_height, "\n");
    // SetWindowPos(hwnd, HWND_TOP, x, y, width, height+2*title_height, 0);

    RECT rect;
    GetClientRect(hwnd, &rect);
    SetWindowPos(hwnd, HWND_TOP, x, y, 2*width-(rect.right-rect.left), 2*height-(rect.bottom-rect.top), 0);

    { //create raw input device
        RAWINPUTDEVICE rid[2];
        size_t n_rid = 0;

        //mouse
        rid[n_rid++] = {.usUsagePage = 0x01,
                        .usUsage = 0x02,
                        // .dwFlags = RIDEV_NOLEGACY,
                        .hwndTarget = hwnd};

        //keyboard
        rid[n_rid++] = {.usUsagePage = 0x01,
                        .usUsage = 0x06,
                        // .dwFlags = RIDEV_NOLEGACY,
                        .hwndTarget = hwnd};

        assert(RegisterRawInputDevices(rid, n_rid, sizeof(rid[0])), "error reading rawinput device");
    }

    LARGE_INTEGER timer_frequency;
    LARGE_INTEGER this_time;
    QueryPerformanceFrequency(&timer_frequency);
    QueryPerformanceCounter(&this_time);

    init_vulkan(hwnd, hinstance);

    main_hwnd = hwnd;
    return {.hwnd = hwnd,
            .timer_frequency = timer_frequency,
            .last_time = this_time,
            .this_time = this_time};
}

window_t create_window(char* window_title, char* class_name, HINSTANCE hinstance)
{
    return create_window(window_title, class_name, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hinstance);
}

void show_window(window_t wnd)
{
    ShowWindow(wnd.hwnd, SW_NORMAL);
    // UpdateWindow(wnd.hwnd);
}

int update_window(window_t* wnd)
{
    HDC dc = GetDC(wnd->hwnd);
    SwapBuffers(dc);

    POINT cursor_point;
    GetCursorPos(&cursor_point);

    RECT window_rect;
    GetWindowRect(wnd->hwnd, &window_rect);
    RECT client_rect;
    GetClientRect(wnd->hwnd, &client_rect);
    wnd->size = {(client_rect.right-client_rect.left),
        (client_rect.bottom-client_rect.top)};
    int window_width = wnd->size.x;
    int window_height = wnd->size.y;

    if(GetActiveWindow() == wnd->hwnd)
    {
        // ClipCursor(&window_rect);
    }
    else
    {
        clear_input_state(&wnd->input);
    }

    #ifdef IMGUI_VERSION
    ImGuiIO &io = ImGui::GetIO();
    #endif

    MSG msg;
    while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        switch(msg.message)
        {
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
                break;
            case WM_INPUT:
            {
                RAWINPUT raw;
                uint size = sizeof(raw);
                GetRawInputData((HRAWINPUT) msg.lParam, RID_INPUT, &raw, &size, sizeof(RAWINPUTHEADER));
                switch(raw.header.dwType)
                {
                    case RIM_TYPEMOUSE:
                    {
                        RAWMOUSE& ms = raw.data.mouse;

                        // if(ms.usButtonFlags & RI_MOUSE_WHEEL)
                        //     printf("0x%x, %d, %d, %d\n",
                        //            ms.usButtonFlags, ms.lLastX, ms.lLastY, ms.usButtonData);

                        if(ms.usFlags == MOUSE_MOVE_RELATIVE)
                        {
                            wnd->input.dmouse += {(4.0*ms.lLastX)/window_height,
                                    (4.0*ms.lLastY)/window_height};
                        }
                        // if(ms.usFlags&0b11)
                        // {
                        //     wnd->mouse = {(2.0*ms.lLastX-(window_rect.left+window_rect.right))/window_height,
                        //                   (-2.0*ms.lLastY+(window_rect.bottom+window_rect.top))/window_height};
                        // }

                        //usButtonFlags can have more than one input at a time

                        #define update_numbered_button(N)               \
                            if(ms.usButtonFlags & RI_MOUSE_BUTTON_##N##_DOWN) \
                            { wnd->input.pressed_buttons[0] |= 1<<M##N; wnd->input.buttons[0] |= 1<<M##N; } \
                            else if(ms.usButtonFlags & RI_MOUSE_BUTTON_##N##_UP) \
                            { wnd->input.released_buttons[0] |= 1<<M##N; wnd->input.buttons[0] &= ~(1<<M##N); }
                        update_numbered_button(1);
                        update_numbered_button(2);
                        update_numbered_button(3);
                        update_numbered_button(4);
                        update_numbered_button(5);

                        if(ms.usButtonFlags & RI_MOUSE_WHEEL)
                        {
                            wnd->input.mouse_wheel = (short) ms.usButtonData/WHEEL_DELTA;
                            if((short) ms.usButtonData > 0)
                            {
                                wnd->input.buttons[1] |= 1<<(M_WHEEL_UP-8);
                                wnd->input.pressed_buttons[1] |= 1<<(M_WHEEL_UP-8);
                            }
                            else
                            {
                                wnd->input.buttons[1] |= 1<<(M_WHEEL_DOWN-8);
                                wnd->input.pressed_buttons[1] |= 1<<(M_WHEEL_DOWN-8);
                            }
                            #ifdef IMGUI_VERSION
                            io.AddMouseWheelEvent(0.0f, ms.usButtonData);
                            #endif
                        }

                        if(ms.usButtonFlags & RI_MOUSE_HWHEEL)
                        {
                            wnd->input.mouse_hwheel = (short) ms.usButtonData/WHEEL_DELTA;
                            if((short) ms.usButtonData > 0)
                            {
                                wnd->input.buttons[1] |= 1<<(M_WHEEL_RIGHT-8);
                                wnd->input.pressed_buttons[1] |= 1<<(M_WHEEL_RIGHT-8);
                            }
                            else
                            {
                                wnd->input.buttons[1] |= 1<<(M_WHEEL_LEFT-8);
                                wnd->input.pressed_buttons[1] |= 1<<(M_WHEEL_LEFT-8);
                            }
                            #ifdef IMGUI_VERSION
                            io.AddMouseWheelEvent(ms.usButtonData/WHEEL_DELTA, 0.0);
                            #endif
                        }

                        break;
                    }
                    case RIM_TYPEKEYBOARD:
                    {
                        RAWKEYBOARD& kb = raw.data.keyboard;
                        bool keyup = kb.Flags&RI_KEY_BREAK;
                        if(keyup) set_key_up(kb.VKey, wnd->input);
                        else    set_key_down(kb.VKey, wnd->input);

                        if(wnd->input.n_typed < len(wnd->input.typed)) {
                            if(is_pressed(kb.VKey, &wnd->input)) wnd->input.typed[wnd->input.n_typed++] = kb.VKey;
                        }
                        break;
                    }
                }
                break;
            }
            case WM_QUIT:
            {
                return 0;
            }
            default:
                DispatchMessage(&msg);
        }
    }
    // RECT wnd_rect;
    // GetWindowRect(wnd.hwnd, &wnd_rect);

    // auto window_width = wnd_rect.right-wnd_rect.left;
    // auto window_height = wnd_rect.bottom-wnd_rect.top;
    // glViewport(0.5*(window_width-window_height), 0, window_height, window_height);

    // wnd->input.mouse += wnd->input.dmouse;

    POINT center_point = {window_width/2, window_height/2};
    MapWindowPoints(wnd->hwnd, 0, &center_point, 1);

    wnd->input.mouse = { 2.0*(cursor_point.x-center_point.x)/window_height,
        2.0*(cursor_point.y-center_point.y)/window_height};

    // if(io.WantCaptureKeyboard) memset(wnd->input.buttons, 0, sizeof(wnd->input.buttons));
    // if(io.WantCaptureMouse)
    // {
    //     wnd->input.dmouse = {0,0};
    //     ShowCursor(1);
    // }
    // else
    // {
    //     ShowCursor(0);
    // }

    CURSORINFO ci;
    GetCursorInfo(&ci);
    static HCURSOR old_cursor = 0;
    // HCURSOR last_cursor = SetCursor(0);
    // if(last_cursor) old_cursor = last_cursor;

    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return 1;
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    main_context = create_context(0);
    memory_manager* manager = get_context()->manager;

    #ifdef DEBUG
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    #endif
    logfile = fopen("log.txt", "w");

    load_wordnet();

    settings = (settings_t) {
        .effects_volume = 0.2,
        .music_volume = 0.2,
        .show_fps = false,
        .fullscreen = false,
        .window_x = 1280,
        .window_y = 720,
        .resolution_x = 640,
        .resolution_y = 360,
        .gif_resolution_x = 640,
        .gif_resolution_y = 360,
    };
    load_settings();

    vkon.render_resolution = {settings.resolution_x, settings.resolution_y};
    vkon.ui_resolution = {settings.window_x, settings.window_y};
    vkon.replay_resolution = {settings.gif_resolution_x, settings.gif_resolution_y};
    vkon.max_replay_frames = 600;
    vkon.replay_centiseconds = 3;

    window_t& wnd = window;
    wnd = create_window("Game", "mygame", settings.window_x, settings.window_y, 20, 20, hinstance);
    if(settings.fullscreen) fullscreen();
    show_window(wnd);

    int next_id = 1;

    // CreateDirectory(w.tim.savedir, 0);
    // w.entity_savefile = open_file("entities.dat");


    vkon.fov = pi*30.0f/180.0f;

    gui = {
        .background_color = {0.001,0.001,0.001,1},
        .foreground_color = {1,1,1,1},
        .highlight_color = {0.1,0.1,0.1,0.5},
    };

    replay_buffer rb = {};
    init_replay_buffer(&rb, {640, 360}, 1200);

    // load_sprites();

    init_audio_context();
    load_sounds();

    work_stack = (work_task*) stalloc(1024*sizeof(work_task));
    n_work_entries = 0;
    n_remaining_tasks = 0;
    work_semephore = CreateSemaphoreA(0, 0, n_max_workers, NULL);

    context_list[0] = main_context;

    int thread_numbers[n_max_workers] = {};

    n_ready_threads = 1;
    _ReadWriteBarrier();
    for(int i = 0; i < n_max_workers; i++)
    {
        DWORD thread_id;
        thread_numbers[i] = i+1;
        HANDLE thread_handle = CreateThread(0, 0, thread_proc, thread_numbers+i, 0, &thread_id);
        CloseHandle(thread_handle);
    }

    while(n_ready_threads < n_threads);
    _ReadWriteBarrier();

    // waterfall = load_audio_file(manager, "data/sounds/water_flow.wav");
    // jump_sound = load_audio_file(manager, "data/sounds/Meat_impacts_0.wav");
    // // play_sound(&ac, &waterfall, 1.0);

    create_world();

    for(int i = 0; i < len(context_list); i++)
    {
        if(!context_list[i]) continue;
        context_list[i]->seed = randui(&game.seed)+i;
        randui(&context_list[i]->seed);
        randui(&context_list[i]->seed);
        randui(&context_list[i]->seed);
    }
    _ReadWriteBarrier();

    init_hashmap(&translations, 65536);
    load_translations("data/translations.tsv");

    HANDLE shader_dir = CreateFileA("data/shaders/",
                                    GENERIC_READ,
                                    FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
                                    0,
                                    OPEN_EXISTING,
                                    FILE_FLAG_BACKUP_SEMANTICS,
                                    0);
    if(shader_dir == INVALID_HANDLE_VALUE) log_warning("could not open handle for shader directory");
    FILETIME last_filetime = {};
    GetFileTime(shader_dir, 0, 0, &last_filetime);

    bool step_mode = false;

    real Deltat = 0;
    //TODO: make render loop evenly sample inputs when vsynced
    while(update_window(&wnd))
    {
        wnd.last_time = wnd.this_time;
        QueryPerformanceCounter(&wnd.this_time);

        vkon.frame_time = ((real) (wnd.this_time.QuadPart - wnd.last_time.QuadPart))/(wnd.timer_frequency.QuadPart);
        Deltat += vkon.frame_time;
        bool did_game_update = false;

        if(is_pressed('4', &wnd.input)) play_sound(&explosion_sound, 1.0f);

        FILETIME filetime;
        GetFileTime(shader_dir, 0, 0, &filetime);

        if(CompareFileTime(&last_filetime, &filetime) != 0 || is_pressed(VK_F5, &wnd.input))
        {
            log_output("reloading shaders\n");
            recreate_pipelines();
            last_filetime = filetime;
            reset_input_state(&wnd.input);
        }

        int n_game_updates = 0;
        game.paused = game.replay_mode || game.pause_menu;

        if(game.pause_menu) vkon.ui_clear_color = {0,0,0,0.7f};
        else                vkon.ui_clear_color = {0,0,0,0};
        if(!game.paused)
        {
            if(Deltat > game_dt)
            {
                game.replay_mode = game.replay_mode!=is_pressed(VK_F11, &wnd.input);
                game.pause_menu = game.pause_menu!=is_pressed(VK_ESCAPE, &wnd.input);
                if(is_pressed(VK_F1, &wnd.input)) settings.show_fps = !settings.show_fps;

                Deltat -= game_dt;

                if(game.speed_multiplier <= 0) game.speed_multiplier = 1;
                for(int i = 0; i < game.speed_multiplier; i++) {
                    update_game(&wnd.input);
                    reset_input_state(&wnd.input);
                }

                n_game_updates++;
                did_game_update = true;

                // if(n_game_updates > 4) break;
            }

            if(did_game_update) update_camera_matrix();

            vkon.replay_mode = game.replay_mode;
            draw_frame(&wnd.input);
        }
        else
        {
            if(is_pressed(VK_F1, &wnd.input)) settings.show_fps = !settings.show_fps;
            if(is_pressed(VK_ESCAPE, &wnd.input) || is_pressed(VK_F11, &wnd.input)) {
                game.pause_menu = false;
                game.replay_mode = false;
            }

            vkon.replay_mode = game.replay_mode;
            draw_frame(&wnd.input);

            reset_input_state(&wnd.input);
        }
        if(Deltat > game_dt*2) Deltat = 2*game_dt;
    }

    cleanup_vulkan();

    fclose(logfile);
    return 0;
}
