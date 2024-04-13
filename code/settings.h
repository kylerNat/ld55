#ifndef SETTINGS
#define SETTINGS

struct settings_t
{
    real effects_volume;
    real music_volume;

    union
    {
        struct
        {
            int use_button;
            int right_button;
            int up_button;
            int left_button;
            int down_button;
            int reset_button;
        };
        int buttons[6];
    };

    uint show_fps;
    uint fullscreen;

    int window_x;
    int window_y;
    int resolution_x;
    int resolution_y;
    int gif_resolution_x;
    int gif_resolution_y;

    uint level_editing;
    uint hub_unlocked;
};

settings_t settings;

void load_settings()
{
    char* filename = "settings.txt";
    FILE* file = fopen(filename, "r");
    if(!file)
    {
        log_output("error: could not open ", filename, "\n");
        return;
    }

    char keyname[1024];
    while(fscanf(file, "%s = ", keyname) != EOF)
    {
        #define do_setting(setting, format) if(strcmp(keyname, #setting)==0) fscanf(file, format"\n", &settings.setting);
        do_setting(effects_volume, "%f");
        do_setting(music_volume, "%f");
        do_setting(use_button, "%d");
        do_setting(right_button, "%d");
        do_setting(up_button, "%d");
        do_setting(left_button, "%d");
        do_setting(down_button, "%d");
        do_setting(reset_button, "%d");
        do_setting(show_fps, "%u");
        do_setting(fullscreen, "%u");
        do_setting(window_x, "%d");
        do_setting(window_y, "%d");
        do_setting(resolution_x, "%d");
        do_setting(resolution_y, "%d");
        do_setting(gif_resolution_x, "%d");
        do_setting(gif_resolution_y, "%d");
        do_setting(level_editing, "%u");
        do_setting(hub_unlocked, "%u");
        #undef do_setting
    }
    fclose(file);
}

void save_settings()
{
    char* filename = "settings.txt";
    FILE* file = fopen(filename, "w");
    if(!file)
    {
        log_output("error: could not open ", filename, "\n");
        return;
    }

    #define do_setting(setting, format) fprintf(file, #setting " = " format "\n", settings.setting);
    do_setting(effects_volume, "%f");
    do_setting(music_volume, "%f");
    do_setting(use_button, "%d");
    do_setting(right_button, "%d");
    do_setting(up_button, "%d");
    do_setting(left_button, "%d");
    do_setting(down_button, "%d");
    do_setting(reset_button, "%d");
    do_setting(show_fps, "%u");
    do_setting(fullscreen, "%u");
    do_setting(window_x, "%d");
    do_setting(window_y, "%d");
    do_setting(resolution_x, "%d");
    do_setting(resolution_y, "%d");
    do_setting(gif_resolution_x, "%d");
    do_setting(gif_resolution_y, "%d");
    do_setting(level_editing, "%u");
    do_setting(hub_unlocked, "%u");
    #undef do_setting
    fclose(file);
}

#endif //SETTINGS
