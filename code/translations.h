#ifndef TRANSLATIONS
#define TRANSLATIONS

struct translation_list
{
    char** text;
};

define_printer(translation_list s, ("%s", s.text[0]));

#define hashmapname translation_map
#define key_type char*
#define value_type translation_list
#include <utils/hashmap.h>

translation_map translations;
int n_languages = 0;

int language_index = 0;

char unknown_string[] = "missing translation";

char* get_translation(char* name)
{
    char** text = translations[name].text;
    if(!text || !text[language_index]) return unknown_string;
    return text[language_index];
}

void load_translations(char* filename)
{
    FILE* file = fopen(filename, "r");
    if(!file)
    {
        log_warning("could not open ", filename, "\n");
        return;
    }

    n_languages = 1024;

    char** tl = 0;

    bool counted_languages = false;

    char* s = (char*) stalloc_clear(65536);
    char* key = 0;
    int i = 0;
    int read = 1;
    bool stop = false;
    while(!stop)
    {
        if(!tl) tl = (char**) dynamic_alloc_clear(n_languages*sizeof(char*));

        bool read_entry = fscanf(file, "%[^\t\n]", s);
        bool end_line = (read=getc(file)) == '\n';
        if(read == EOF || read == 0)
        {
            end_line = true;
            stop = true;
        }
        else if(read_entry)
        {
            int length = strlen(s);
            char* text = (char*) dynamic_alloc(length+1);
            strcpy(text, s);
            if(!key) key = text;
            else tl[i++] = text;
        }
        if(end_line && key)
        {
            if(!counted_languages)
            {
                n_languages = i;
                counted_languages = true;
            }
            add_entry(translations, key, {tl});
            key = 0;
            tl = 0;
            i = 0;
        }
    }
    stunalloc(s);
    fclose(file);
}

#endif //TRANSLATIONS
