#ifndef GUI
#define GUI

struct gui_style_t {
    real_4 background_color;
    real_4 foreground_color;
    real_4 highlight_color;
};
gui_style_t gui;

//hexagon rotations
real_2 hex_rot = {cos(pi/3), sin(pi/3)};
real_2 hex_rot_rev = {cos(pi/3), -sin(pi/3)};

real_2 hex_rots[6] = {};
int init_hex_rots()
{
    real_2 rot = {1,0};
    for(int i = 0; i < 6; i++)
    {
        hex_rots[i] = rot;
        rot = complexx(rot, hex_rot);
    }
    return 0;
}
int _ = init_hex_rots();

enum ui_type
{
    ui_none,
    ui_gene,
    ui_gizmo,
    ui_form,
    ui_form_voxel,
    ui_form_joint,
    ui_form_foot,
    ui_window,
    ui_button,
    n_ui_elements
};

struct user_input
{
    real_2 mouse;
    real_2 dmouse;
    int16 mouse_wheel;
    int16 mouse_hwheel;
    byte buttons[32];
    byte pressed_buttons[32];
    byte released_buttons[32];
    int gamepads_connected;
    int16 gamepad_buttons;
    int16 gamepad_prev_buttons;
    real_2 gamepad_left_stick;
    real_2 gamepad_right_stick;
    bool click_blocked;
    bool escape_blocked;
    void* active_ui_element;
    int hovered_ui_element;
    int old_hovered_ui_element;
};

struct window_t
{
    HWND hwnd;

    real_2 size;
    user_input input;

    LARGE_INTEGER timer_frequency;
    LARGE_INTEGER last_time;
    LARGE_INTEGER this_time;
};

#define M1 0x01
#define M2 0x02
#define M3 0x04
#define M4 0x05
#define M5 0x06
#define M_WHEEL_DOWN 0x0A
#define M_WHEEL_UP 0x0B
#define M_WHEEL_LEFT 0x0E
#define M_WHEEL_RIGHT 0x0F

#define LARROW 0x25
#define UARROW 0x26
#define RARROW 0x27
#define DARROW 0x28

#define is_down(key_code, input) ((((input)->buttons[(key_code)/8]>>((key_code)%8)) | ((input)->pressed_buttons[(key_code)/8]>>((key_code)%8)))&1) //check pressed_buttons in case there was a press and release within a single frame
#define is_pressed(key_code, input) ((input)->pressed_buttons[(key_code)/8]>>((key_code)%8)&1)
#define is_released(key_code, input) ((input)->released_buttons[(key_code)/8]>>((key_code)%8)&1)

// #define is_click_pressed(input) (!(input)->click_blocked && is_pressed(M1, input))
#define is_click_pressed(input) (!(input)->click_blocked && is_pressed(M1, input))
#define is_click_down(input) (!(input)->click_blocked && is_down(M1, input))

#define set_key_down(key_code, input) (((input).pressed_buttons[(key_code)/8] |= (~(input).buttons[(key_code)/8])&(1<<((key_code)%8))), ((input).buttons[(key_code)/8] |= 1<<((key_code)%8)))
#define set_key_up(key_code, input) (((input).buttons[(key_code)/8] &= ~(1<<((key_code)%8))), ((input).released_buttons[(key_code)/8] |= 1<<((key_code)%8)))

void reset_input_state(user_input* input)
{
    memset(input->pressed_buttons, 0, sizeof(input->pressed_buttons));
    memset(input->released_buttons, 0, sizeof(input->released_buttons));
    input->buttons[M_WHEEL_DOWN/8] &= ~(11<<(M_WHEEL_DOWN%8));
    input->mouse_wheel = 0;
    input->dmouse = zero_2;
    input->click_blocked &= is_down(M1, input);
}

void clear_input_state(user_input* input)
{
    memset(input->buttons, 0, sizeof(input->buttons));
    memset(input->pressed_buttons, 0, sizeof(input->pressed_buttons));
    memset(input->released_buttons, 0, sizeof(input->released_buttons));
    input->buttons[M_WHEEL_DOWN/8] &= ~(11<<(M_WHEEL_DOWN%8));
    input->mouse_wheel = 0;
    input->dmouse = zero_2;
    input->click_blocked = false;
}

WINDOWPLACEMENT g_wpPrev = { sizeof(g_wpPrev) };

window_t window;

void fullscreen()
{
    DWORD dwStyle = GetWindowLong(window.hwnd, GWL_STYLE);
    if (dwStyle & WS_OVERLAPPEDWINDOW) {
        MONITORINFO mi = { sizeof(mi) };
        if (GetWindowPlacement(window.hwnd, &g_wpPrev) &&
            GetMonitorInfo(MonitorFromWindow(window.hwnd,
                                             MONITOR_DEFAULTTOPRIMARY), &mi)) {
            SetWindowLong(window.hwnd, GWL_STYLE,
                          dwStyle & ~WS_OVERLAPPEDWINDOW);
            SetWindowPos(window.hwnd, HWND_TOP,
                         mi.rcMonitor.left, mi.rcMonitor.top,
                         mi.rcMonitor.right - mi.rcMonitor.left,
                         mi.rcMonitor.bottom - mi.rcMonitor.top,
                         SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
    } else {
        SetWindowLong(window.hwnd, GWL_STYLE,
                      dwStyle | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(window.hwnd, &g_wpPrev);
        SetWindowPos(window.hwnd, NULL, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                     SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
}

void get_keycode_name(int keycode, char* out, int max_size)
{
    if(keycode < 0) sprintf(out, "press button (esc to cancel)");
    else if('A' <= keycode && keycode <= 'Z')
    {
        sprintf(out, "%c", keycode-'A'+'a');
    }
    else if('0' <= keycode && keycode <= '9')
    {
        sprintf(out, "%c", keycode);
    }
    else
    {
        int error = GetKeyNameTextA(keycode, out, max_size);
        if(!error) sprintf(out, "keycode %d", keycode);
        switch(keycode)
        {
            case VK_LBUTTON: { sprintf(out, "left mouse button"); break; }
            case VK_RBUTTON: { sprintf(out, "right mouse button"); break; }
            case VK_CANCEL: { sprintf(out, "control-break processing"); break; }
            case VK_MBUTTON: { sprintf(out, "middle mouse button (three-button mouse)"); break; }
            case VK_XBUTTON1: { sprintf(out, "mouse button 4"); break; }
            case VK_XBUTTON2: { sprintf(out, "mouse button 5"); break; }

            case M_WHEEL_DOWN: { sprintf(out, "scroll wheel down"); break; }
            case M_WHEEL_UP: { sprintf(out, "scroll wheel up"); break; }
            case M_WHEEL_LEFT: { sprintf(out, "scroll wheel left"); break; }
            case M_WHEEL_RIGHT: { sprintf(out, "scroll wheel right"); break; }

            case VK_BACK: { sprintf(out, "backspace"); break; }
            case VK_TAB: { sprintf(out, "tab"); break; }

            case VK_CLEAR: { sprintf(out, "clear"); break; }
            case VK_RETURN: { sprintf(out, "enter"); break; }

            case VK_SHIFT: { sprintf(out, "shift"); break; }
            case VK_CONTROL: { sprintf(out, "ctrl"); break; }
            case VK_MENU: { sprintf(out, "alt"); break; }
            case VK_PAUSE: { sprintf(out, "pause"); break; }
            case VK_CAPITAL: { sprintf(out, "caps lock"); break; }
            case VK_KANA: { sprintf(out, "ime kana mode"); break; }
            case VK_JUNJA: { sprintf(out, "ime junja mode"); break; }
            case VK_FINAL: { sprintf(out, "ime final mode"); break; }
            case VK_HANJA: { sprintf(out, "ime hanja mode"); break; }
            case VK_ESCAPE: { sprintf(out, "esc"); break; }
            case VK_CONVERT: { sprintf(out, "ime convert"); break; }
            case VK_NONCONVERT: { sprintf(out, "ime nonconvert"); break; }
            case VK_ACCEPT: { sprintf(out, "ime accept"); break; }
            case VK_MODECHANGE: { sprintf(out, "ime mode change request"); break; }
            case VK_SPACE: { sprintf(out, "spacebar"); break; }
            case VK_PRIOR: { sprintf(out, "page up"); break; }
            case VK_NEXT: { sprintf(out, "page down"); break; }
            case VK_END: { sprintf(out, "end"); break; }
            case VK_HOME: { sprintf(out, "home"); break; }
            case VK_LEFT: { sprintf(out, "left arrow"); break; }
            case VK_UP: { sprintf(out, "up arrow"); break; }
            case VK_RIGHT: { sprintf(out, "right arrow"); break; }
            case VK_DOWN: { sprintf(out, "down arrow"); break; }
            case VK_SELECT: { sprintf(out, "select"); break; }
            case VK_PRINT: { sprintf(out, "print"); break; }
            case VK_EXECUTE: { sprintf(out, "execute"); break; }
            case VK_SNAPSHOT: { sprintf(out, "print screen"); break; }
            case VK_INSERT: { sprintf(out, "ins"); break; }
            case VK_DELETE: { sprintf(out, "del"); break; }
            case VK_HELP: { sprintf(out, "help"); break; }

            case VK_LWIN: { sprintf(out, "left windows key (natural keyboard)"); break; }
            case VK_RWIN: { sprintf(out, "right windows key (natural keyboard)"); break; }
            case VK_APPS: { sprintf(out, "applications key (natural keyboard)"); break; }

            case VK_SLEEP: { sprintf(out, "computer sleep"); break; }
            case VK_NUMPAD0: { sprintf(out, "numeric keypad 0"); break; }
            case VK_NUMPAD1: { sprintf(out, "numeric keypad 1"); break; }
            case VK_NUMPAD2: { sprintf(out, "numeric keypad 2"); break; }
            case VK_NUMPAD3: { sprintf(out, "numeric keypad 3"); break; }
            case VK_NUMPAD4: { sprintf(out, "numeric keypad 4"); break; }
            case VK_NUMPAD5: { sprintf(out, "numeric keypad 5"); break; }
            case VK_NUMPAD6: { sprintf(out, "numeric keypad 6"); break; }
            case VK_NUMPAD7: { sprintf(out, "numeric keypad 7"); break; }
            case VK_NUMPAD8: { sprintf(out, "numeric keypad 8"); break; }
            case VK_NUMPAD9: { sprintf(out, "numeric keypad 9"); break; }
            case VK_MULTIPLY: { sprintf(out, "multiply"); break; }
            case VK_ADD: { sprintf(out, "add"); break; }
            case VK_SEPARATOR: { sprintf(out, "separator"); break; }
            case VK_SUBTRACT: { sprintf(out, "subtract"); break; }
            case VK_DECIMAL: { sprintf(out, "decimal"); break; }
            case VK_DIVIDE: { sprintf(out, "divide"); break; }
            case VK_F1: { sprintf(out, "f1"); break; }
            case VK_F2: { sprintf(out, "f2"); break; }
            case VK_F3: { sprintf(out, "f3"); break; }
            case VK_F4: { sprintf(out, "f4"); break; }
            case VK_F5: { sprintf(out, "f5"); break; }
            case VK_F6: { sprintf(out, "f6"); break; }
            case VK_F7: { sprintf(out, "f7"); break; }
            case VK_F8: { sprintf(out, "f8"); break; }
            case VK_F9: { sprintf(out, "f9"); break; }
            case VK_F10: { sprintf(out, "f10"); break; }
            case VK_F11: { sprintf(out, "f11"); break; }
            case VK_F12: { sprintf(out, "f12"); break; }
            case VK_F13: { sprintf(out, "f13"); break; }
            case VK_F14: { sprintf(out, "f14"); break; }
            case VK_F15: { sprintf(out, "f15"); break; }
            case VK_F16: { sprintf(out, "f16"); break; }
            case VK_F17: { sprintf(out, "f17"); break; }
            case VK_F18: { sprintf(out, "f18"); break; }
            case VK_F19: { sprintf(out, "f19"); break; }
            case VK_F20: { sprintf(out, "f20"); break; }
            case VK_F21: { sprintf(out, "f21"); break; }
            case VK_F22: { sprintf(out, "f22"); break; }
            case VK_F23: { sprintf(out, "f23"); break; }
            case VK_F24: { sprintf(out, "f24"); break; }

            case VK_NUMLOCK: { sprintf(out, "num lock"); break; }
            case VK_SCROLL: { sprintf(out, "scroll lock"); break; }

            case VK_LSHIFT: { sprintf(out, "left shift"); break; }
            case VK_RSHIFT: { sprintf(out, "right shift"); break; }
            case VK_LCONTROL: { sprintf(out, "left control"); break; }
            case VK_RCONTROL: { sprintf(out, "right control"); break; }
            case VK_LMENU: { sprintf(out, "left alt"); break; }
            case VK_RMENU: { sprintf(out, "right alt"); break; }
            case VK_BROWSER_BACK: { sprintf(out, "browser back"); break; }
            case VK_BROWSER_FORWARD: { sprintf(out, "browser forward"); break; }
            case VK_BROWSER_REFRESH: { sprintf(out, "browser refresh"); break; }
            case VK_BROWSER_STOP: { sprintf(out, "browser stop"); break; }
            case VK_BROWSER_SEARCH: { sprintf(out, "browser search"); break; }
            case VK_BROWSER_FAVORITES: { sprintf(out, "browser favorites"); break; }
            case VK_BROWSER_HOME: { sprintf(out, "browser start and home"); break; }
            case VK_VOLUME_MUTE: { sprintf(out, "volume mute"); break; }
            case VK_VOLUME_DOWN: { sprintf(out, "volume down"); break; }
            case VK_VOLUME_UP: { sprintf(out, "volume up"); break; }
            case VK_MEDIA_NEXT_TRACK: { sprintf(out, "next track"); break; }
            case VK_MEDIA_PREV_TRACK: { sprintf(out, "previous track"); break; }
            case VK_MEDIA_STOP: { sprintf(out, "stop media"); break; }
            case VK_MEDIA_PLAY_PAUSE: { sprintf(out, "play/pause media"); break; }
            case VK_LAUNCH_MAIL: { sprintf(out, "start mail"); break; }
            case VK_LAUNCH_MEDIA_SELECT: { sprintf(out, "select media"); break; }
            case VK_LAUNCH_APP1: { sprintf(out, "start application 1"); break; }
            case VK_LAUNCH_APP2: { sprintf(out, "start application 2"); break; }

            case VK_OEM_PLUS: { sprintf(out, "+"); break; }
            case VK_OEM_COMMA: { sprintf(out, ","); break; }
            case VK_OEM_MINUS: { sprintf(out, "-"); break; }
            case VK_OEM_PERIOD: { sprintf(out, "."); break; }

            case VK_PROCESSKEY: { sprintf(out, "ime process"); break; }

            case VK_ATTN: { sprintf(out, "attn"); break; }
            case VK_CRSEL: { sprintf(out, "crsel"); break; }
            case VK_EXSEL: { sprintf(out, "exsel"); break; }
            case VK_EREOF: { sprintf(out, "erase eof"); break; }
            case VK_PLAY: { sprintf(out, "play"); break; }
            case VK_ZOOM: { sprintf(out, "zoom"); break; }
            case VK_NONAME: { sprintf(out, "reserved"); break; }
            case VK_PA1: { sprintf(out, "pa1"); break; }
            case VK_OEM_CLEAR: { sprintf(out, "clear"); break; }
            default: { sprintf(out, "keycode %d", keycode); break; }
        }
    }
};

/////////////////////////////////////

struct button_out
{
    bool clicked;
    bool hovered;
};

int next_gui_element = 1;

button_out do_text_button(user_input* input, real_3 pos, real_2 size, char* text)
{
    bool hovered = all_less_than_eq(abs_per_axis(input->mouse-pos.xy), size);
    bool clicked = hovered && is_click_pressed(input);
    input->click_blocked |= clicked;
    real_4 color = (hovered) ? gui.highlight_color : gui.background_color;
    draw_rectangle(pos, size, color);
    draw_text(text, pos.xy, gui.foreground_color, {0,0.5}, vkon.default_font);

    return {clicked, hovered};
}

button_out do_circle_button(user_input* input, real_3 pos, real size)
{
    //NOTE:square hitbox, to make it easier to click, might want to change to an actual circle
    bool hovered = all_less_than_eq(abs_per_axis(input->mouse-pos.xy), {size, size});
    bool clicked = hovered && is_click_pressed(input);
    input->click_blocked |= clicked;
    real_4 color = (hovered & !clicked) ? gui.highlight_color : gui.background_color;
    draw_circle(pos, size, color);

    return {clicked, hovered};
}

button_out do_circle_arrow_button(user_input* input, real_3 pos, real_2 d0, real_2 d1, real head_dir, real radius, real thickness, real head_scale)
{
    real_2 p = input->mouse-pos.xy;
    real_2 vert = normalize_or_zero(d0+d1);
    real_2 hor = -perp(vert);
    real_2x2 rot = (real_2x2){hor, vert};
    p = p*rot;
    p.x = abs(p.x);
    real_2 sc = d0*rot;
    real dist = p.y*sc.x > p.x*sc.y ? abs(norm(p)-radius) : norm(p-radius*sc);

    bool hovered = dist < thickness*head_scale;
    bool clicked = hovered && is_click_pressed(input);
    input->click_blocked |= clicked;
    real_4 color = (hovered & !clicked) ? gui.highlight_color : gui.background_color;

    draw_arc(pos, d0, d1, radius, thickness, color);
    real_2 tip_dir = head_dir > 0.0f ? d0 : d1;
    real_2 mid = radius*tip_dir;
    draw_triangle(pos+pad_3(mid-head_dir*head_scale*thickness*perp(tip_dir)),
                  pos+pad_3(mid+head_scale*thickness*tip_dir),
                  pos+pad_3(mid-head_scale*thickness*tip_dir),
                  color);

    return {clicked, hovered};
}

button_out do_slider(user_input* input, real_3 pos, real_2 size, real* value, real min_value, real max_value)
{
    bool hovered = all_less_than_eq(abs_per_axis(input->mouse-pos.xy), size);
    bool clicked = hovered && is_click_pressed(input);
    input->click_blocked |= clicked;
    real_4 color = (hovered & !clicked) ? gui.highlight_color : gui.background_color;

    int gui_id = next_gui_element++;
    if(hovered) input->hovered_ui_element = gui_id;

    if(clicked)
    {
        input->active_ui_element = value;
    }

    if(input->active_ui_element == value)
    {
        if(is_down(M1, input))
        {
            real t = (input->mouse.x-(pos.x-size.x))/(2.0f*size.x);
            *value = lerp(min_value, max_value, clamp(t, 0.0f, 1.0f));
        }
        else
        {
            input->active_ui_element = 0;
        }
    }
    real t = (*value-min_value)/(max_value-min_value);
    draw_rectangle(pos, size, color);
    draw_rectangle(pos+(real_3){-(1.0f-t)*size.x}, {t*size.x, 0.8*size.y}, gui.foreground_color);
    // draw_text(text, pos.x, pos.y, gui.foreground_color, {0,0}, vkon.default_font);

    return {clicked, hovered};
}

real_3 hsv_to_rgb(real_3 hsv)
{
    real_3 rgb = {};
    hsv.x = fmod(fmod(hsv.x, 2*pi)+4*pi, 2*pi); //make sure hsv.x is between 0 and 2pi
    int raw_M = floor(hsv.x*(1.5f/pi)+0.5f);
    int M = raw_M%3;
    int i = (M+1)%3;
    int j = (M+2)%3;
    real C = hsv.y*hsv.z;
    rgb[M] = hsv.z;
    real imj = (hsv.x*(3.0f/pi)-2*raw_M)*C;
    if(imj > 0.0f) {
        rgb[j] = hsv.z-C;
        rgb[i] = rgb[j]+imj;
    } else {
        rgb[i] = hsv.z-C;
        rgb[j] = rgb[i]-imj;
    }

    return rgb;
}

real_3 rgb_to_hsv(real_3 rgb)
{
    int M = 0;
    int m = 0;
    if(rgb[1] > rgb[M]) M = 1;
    if(rgb[2] > rgb[M]) M = 2;
    if(rgb[1] < rgb[m]) m = 1;
    if(rgb[2] < rgb[m]) m = 2;

    real_3 hsv = {};

    real C = rgb[M]-rgb[m];
    int i = (M+1)%3;
    int j = (M+2)%3;
    if(C > 0.0f)
    {
        hsv.x = ((rgb[i]-rgb[j])/C+2*M)*(pi/3.0f);
    }
    hsv.z = rgb[M];
    if(hsv.z > 0.0f)
        hsv.y = C/hsv.z;
    return hsv;
}

real_4 do_color_picker(user_input* input, real_3 pos, real size, real_3* hsv)
{
    real_2 r = input->mouse-pos.xy;
    real rsq = normsq(r);
    real inner_radius = size*0.8f;

    real_2 hue_axis = {cos(hsv->x), sin(hsv->x)};
    //t is scaled and rotated so that the fully saturated point is at (0,0), and the x-axis is aligned with the center line of the triangle
    real_2 t = (complexx(r, conjugate(hue_axis))+(real_2){-inner_radius,0.0f})/(1.5f*inner_radius);

    //barycentric coords
    real_3 lambda = {1.0f+t.x, t.y*(0.5f*sqrt(3.0f))-0.5f*t.x, 0};
    lambda.z = 1.0f-lambda.x-lambda.y;

    // bool triangle_hovered = all_less_than_eq({}, lambda);
    bool triangle_hovered = normsq(r) <= sq(inner_radius);
    bool triangle_clicked = triangle_hovered && is_click_pressed(input);
    bool ring_hovered =  rsq <= sq(size) && !triangle_hovered;
    bool ring_clicked = ring_hovered && is_click_pressed(input);
    input->click_blocked |= ring_clicked || triangle_clicked;

    int ring_gui_id = next_gui_element++;
    int triangle_gui_id = next_gui_element++;
    if(ring_hovered) input->hovered_ui_element = ring_gui_id;
    if(triangle_hovered) input->hovered_ui_element = triangle_gui_id;

    if(ring_clicked)
    {
        input->active_ui_element = &hsv->x;
    }
    if(triangle_clicked)
    {
        input->active_ui_element = &hsv->y;
    }

    if(input->active_ui_element == &hsv->x)
    {
        if(is_down(M1, input))
        {
            hsv->x = fmod((atan2(r.y, r.x)+2*pi), 2*pi);
        }
        else
        {
            input->active_ui_element = 0;
        }
    }

    if(input->active_ui_element == &hsv->y)
    {
        //clamp barycentric coords
        for(int i = 0; i < 3; i++)
        {
            int j = (i+1)%3;
            int k = (i+2)%3;
            if(lambda[i] < 0.0f && lambda[j] < 0.0f)
            {
                lambda[i] = 0.0f;
                lambda[j] = 0.0f;
                lambda[k] = 1.0f;
            }
        }
        for(int i = 0; i < 3; i++)
        {
            int j = (i+1)%3;
            int k = (i+2)%3;
            if(lambda[i] < 0.0f)
            {
                real jk_scale = 1.0f/(lambda[j]+lambda[k]);
                lambda[j] *= jk_scale;
                lambda[k] *= jk_scale;
                lambda[i] = 0.0f;
            }
        }

        if(is_down(M1, input))
        {
            hsv->y = 0.5f*(lambda.x-lambda.y)+0.5f;
            hsv->z = 1.0-lambda.z;
        }
        else
        {
            input->active_ui_element = 0;
        }
    }

    hue_axis = {cos(hsv->x), sin(hsv->x)};
    real_4 color = {0,0,0,1};
    color.xyz = pow_per_axis(hsv_to_rgb(*hsv), 2.2);
    // draw_circle(pos, size, {1,1,1,1});
    // draw_circle(pos, inner_radius, color);
    real_3 vertices[3] = {
        pos+pad_3(hue_axis*inner_radius),
        pos+pad_3(complexx(hue_axis*inner_radius, hex_rots[2])),
        pos+pad_3(complexx(hue_axis*inner_radius, hex_rots[4])),
    };
    real_4 colors[3] = {
        pad_4(hsv_to_rgb({hsv->x,1,1}), 1.0f),
        {1,1,1,1},
        {0,0,0,1},
    };
    // draw_triangles(vertices, colors, 1, gui.camera);

    draw_color_picker(pos, size, *hsv);

    real_3 selected_pos = vertices[0]*(hsv->y-0.5f+0.5f*hsv->z)+vertices[1]*(-hsv->y+0.5f+0.5f*hsv->z)+vertices[2]*(1.0f-hsv->z);
    draw_circle(selected_pos, size*0.055f, {1,1,1,1});
    draw_circle(selected_pos, size*0.05f, color);
    return color;
}

bool cancel_button_changes()
{
    bool canceled = false;
    for(int i = 0; i < len(settings.buttons); i++)
    {
        if(settings.buttons[i] < 0)
        {
            settings.buttons[i] = -settings.buttons[i];
            canceled = true;
        }
    }
    return canceled;
}

void do_keybind_setting(user_input* input, char* name, real_3 pos, real_2 size, int* key)
{
    char keyname[256] = {};
    get_keycode_name(*key, keyname, len(keyname));

    bool hovered = all_less_than_eq(abs_per_axis(input->mouse-pos.xy), size);
    bool clicked = hovered && is_click_pressed(input);
    input->click_blocked |= clicked;
    real_4 color = (hovered & !clicked) ? gui.highlight_color : gui.background_color;
    draw_rectangle(pos, size, color);
    draw_text(name, pos.xy-(real_2){0.25f,0.0f}, gui.foreground_color, {0,0}, vkon.default_font);

    if(hovered) input->hovered_ui_element = next_gui_element++;

    if(clicked)
    {
        cancel_button_changes();
        *key = -*key;
    }

    pos.x += 0.25;
    draw_text(keyname, pos.xy, gui.foreground_color, {0,0}, vkon.default_font);

    if(!clicked && *key < 0)
    {
        if(is_pressed(VK_ESCAPE, input))
        {
            *key = -*key;
            input->escape_blocked = true;
        }
        else
        {
            for(int i = 1; i < 0xFF; i++)
            {
                if(is_pressed(i, input)) *key = i;
            }
        }
    }
}

void do_replay_export_menu(user_input* input, real dt)
{
    int n_thumbnails = 10;
    real aspect_ratio = vkon.aspect_ratio;
    real scale = 1.0f/n_thumbnails;
    real_2 thumbnail_size = {scale*aspect_ratio, scale};
    real_3 seekbar_pos = {-aspect_ratio+thumbnail_size.x, 1.0f-thumbnail_size.y, 0};
    int first_frame = vkon.replay_index - vkon.n_replay_frames;
    int last_frame = vkon.replay_index-1;

    static int preview_frame = 0;
    static int export_start_frame = 0;
    static int export_end_frame = 0;

    static real Deltat = 0;
    Deltat += dt;
    real frame_time = 0.01f*vkon.replay_centiseconds;

    export_start_frame = clamp(export_start_frame, first_frame, last_frame);
    export_end_frame = clamp(export_end_frame, export_start_frame+1, last_frame);

    replay_frame_render_info* replay_frames = stalloc_typed(1024, replay_frame_render_info);
    int n_replay_frames = 0;
    rectangle_render_info* rectangles = stalloc_typed(1024, rectangle_render_info);
    int n_rectangles = 0;

    real_3 pos = seekbar_pos;
    for(int i = 0; i < n_thumbnails; i++) {
        int frame_index = clamp((int) lerp(first_frame, last_frame, (real) i/(n_thumbnails-1)), first_frame, last_frame);
        frame_index = (frame_index+vkon.max_replay_frames)%vkon.max_replay_frames;
        replay_frames[n_replay_frames++] = {pos.xy, scale, frame_index};
        pos.x += 2.0*thumbnail_size.x;
    }

    real seek_indicator_w = 0.005f;

    if(abs(input->mouse.y-seekbar_pos.y) <= thumbnail_size.y) {
        rectangles[n_rectangles++] = {{input->mouse.x, seekbar_pos.y, 0}, {seek_indicator_w, thumbnail_size.y}, {1,1,1,1}};
        preview_frame = clamp((int) lerp(first_frame, last_frame, input->mouse.x*0.5f/aspect_ratio+0.5f), first_frame, last_frame);

        if(is_down(M1, input)) export_start_frame = preview_frame;
        if(is_down(M2, input)) export_end_frame = preview_frame;
    } else {
        if(Deltat >= frame_time)
        {
            preview_frame++;
            Deltat -= frame_time;
        }
        preview_frame = (preview_frame-export_start_frame)%(export_end_frame-export_start_frame)+export_start_frame;
    }

    Deltat = min(Deltat, frame_time);

    real export_start_t = (real)(export_start_frame-first_frame)/(last_frame-first_frame);
    real export_end_t = (real)(export_end_frame-first_frame)/(last_frame-first_frame);
    rectangles[n_rectangles++] = {{aspect_ratio*2.0f*(export_start_t-0.5f), seekbar_pos.y, 0}, {seek_indicator_w, thumbnail_size.y}, {1,1,1,1}};
    rectangles[n_rectangles++] = {{aspect_ratio*2.0f*(export_end_t-0.5f), seekbar_pos.y, 0}, {seek_indicator_w, thumbnail_size.y}, {1,1,1,1}};

    pos = {0.0f, 0.0f-thumbnail_size.y};

    int preview_frame_index = (preview_frame+vkon.max_replay_frames)%vkon.max_replay_frames;

    replay_frames[n_replay_frames++] = {pos.xy, 1.0-thumbnail_size.y, preview_frame_index};

    draw_rectangle({0,0}, {aspect_ratio, 1.0f}, {0,0,0,0.9f});
    draw_replay_frames(replay_frames, n_replay_frames);
    draw_rectangles(rectangles, n_replay_frames);

    stunalloc(rectangles);
    stunalloc(replay_frames);

    pos = {aspect_ratio-0.2f, -1.0f+thumbnail_size.y+0.1f};

    if(do_text_button(input, pos, {0.15, 0.05}, "Save Gif").clicked) {
        int n_frames = export_end_frame-export_start_frame;

        char filename[1024]; //TODO: use date and time
        time_t epoch = time(0);
        tm* gmt = gmtime(&epoch);
        sprintf(filename, "replay_%d%d%d%d%d%d", gmt->tm_year+1900, gmt->tm_mon, gmt->tm_mday, gmt->tm_hour, gmt->tm_min, gmt->tm_sec);
        OPENFILENAME ofn = {};
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = main_hwnd;
        ofn.lpstrFile = filename;
        ofn.nMaxFile = sizeof(filename);
        ofn.lpstrFilter = "GIF\0*.gif\0All\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = "gif_recordings";
        ofn.lpstrDefExt = ".gif";
        ofn.Flags = OFN_NOTESTFILECREATE | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
        if(GetSaveFileNameA(&ofn))
        {
            export_gif(filename, export_start_frame, export_end_frame-export_start_frame);
        }

        clear_input_state(input);
    }
}

bool do_pause_menu(user_input* input, real dt)
{
    bool exit_menu = false;

    next_gui_element = 1;

    input->hovered_ui_element = 0;

    // real_3 pos = {-1.0f*window_width/window_height+0.5,0.8,0};
    real_3 pos = {0.0,-0.9,0};
    real center_spacing = 0.02;

    draw_text("Paused", pos.xy, gui.foreground_color, {0,0}, vkon.default_font);
    pos.y += 0.2f;

    pos.x -= 0.1f;

    real slider_width = 0.3;
    draw_text("Game Volume", pos.xy-(real_2){center_spacing,0.0f}, gui.foreground_color, {1,0}, vkon.default_font);
    do_slider(input, pos+(real_3){slider_width+center_spacing}, {slider_width, 0.03}, &settings.effects_volume, 0.0, 1.0);
    audio.effects_voice->SetVolume(settings.effects_volume);

    pos.y += 0.1f;

    draw_text("Music Volume", pos.xy-(real_2){center_spacing,0.0f}, gui.foreground_color, {1,0}, vkon.default_font);
    do_slider(input, pos+(real_3){slider_width+center_spacing}, {slider_width, 0.03}, &settings.music_volume, 0.0, 1.0);
    audio.music_voice->SetVolume(settings.music_volume);

    pos.x = 0.0f;

    real_4 old_background_color = gui.background_color;
    gui.background_color = {};
    pos.y += 0.1f;

    char show_fps_text[256];
    sprintf(show_fps_text, settings.show_fps ? "Show fps on" : "Show fps off");
    if(do_text_button(input, pos, {0.8, 0.05}, show_fps_text).clicked)
    {
        settings.show_fps = !settings.show_fps;
    }

    pos.y += 0.1f;

    char fullscreen_text[256];
    sprintf(fullscreen_text, settings.fullscreen ? "Fullscreen on" : "Fullscreen off");
    if(do_text_button(input, pos, {0.8, 0.05}, fullscreen_text).clicked)
    {
        settings.fullscreen = !settings.fullscreen;
        fullscreen();
    }
    pos.y += 0.15f;

    draw_text("Controls:", pos.xy, gui.foreground_color, {0,0}, vkon.default_font);
    // pos.x -= 0.25;

    pos.y += 0.1f;
    do_keybind_setting(input, "Interact", pos, {0.8, 0.05}, &settings.use_button);

    pos.y += 0.1f;
    do_keybind_setting(input, "Reset", pos, {0.8, 0.05}, &settings.reset_button);

    pos.y += 0.1f;
    do_keybind_setting(input, "Up", pos, {0.8, 0.05}, &settings.up_button);
    pos.y += 0.1f;
    do_keybind_setting(input, "Left", pos, {0.8, 0.05}, &settings.left_button);
    pos.y += 0.1f;
    do_keybind_setting(input, "Down", pos, {0.8, 0.05}, &settings.down_button);
    pos.y += 0.1f;
    do_keybind_setting(input, "Right", pos, {0.8, 0.05}, &settings.right_button);

    // pos.x = 0;

    pos.y += 0.1f;
    if(do_text_button(input, pos, {0.8, 0.05}, "Continue").clicked)
    {
        exit_menu = true;
    }

    pos.y += 0.15f;
    if(do_text_button(input, pos, {0.8, 0.05}, "Quit").clicked)
    {
        save_settings();
        exit(0);
    }

    // if(input->hovered_ui_element != input->old_hovered_ui_element)
    // {
    //     play_sound(ac, &menu_blip, 1.0);
    // }
    // if(is_pressed(M1, input) && input->click_blocked)
    // {
    //     play_sound(ac, &menu_click, 1.0);
    // }
    input->old_hovered_ui_element = input->hovered_ui_element;

    gui.background_color = old_background_color;

    return exit_menu;
}

#endif //GUI
