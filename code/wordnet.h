#ifndef WORDNET
#define WORDNET

#include <utils/str.h>

using levenshteinSSE::levenshtein;

const int MAX_SYNONYMS = 200000;

struct wordnet_node {
    char word[256];
    wordnet_node** synonyms;
    int n_synonyms;
    int visit_id;
};

int current_visit_id = 1;

define_printer(wordnet_node n, ("%s", n.word));

#define hashmapname wordmap_t
#define key_type char*
#define value_type wordnet_node
#include <utils/hashmap.h>

wordmap_t wordmap;

void strip(char* str)
{
    int j = 0;
    for(int i = 0; i < 256; i++) {
        if(str[i] == ' ' || str[i] == '_') continue;
        if(str[i] == '"' || str[i] == 0) {
            str[j] = 0;
            break;
        }
        char c = str[i];
        if(is_uppercase(c)) c += 'a'-'A';
        str[j++] = c;
    }
    str[j] = 0;
}

bool check_word(char* word)
{
    int j = 0;
    for(int i = 0; word[i] && i < 256; i++) {
        if(!is_letter(word[i])) return false;
    }
    if(!in_map(wordmap, word)) {
        uint index = add_entry(wordmap, word, (wordnet_node){});
        memcpy(wordmap.values[index].word, word, 256);
        wordmap.keys[index] = wordmap.values[index].word;
        wordmap.values[index].synonyms = dynamic_alloc_typed(32, wordnet_node*);
    }
    return true;
}

void create_wordnet()
{
    char* filename = "data/en_thesaurus.jsonl";
    FILE* file = fopen(filename, "r");
    if(!file)
    {
        log_output("error: could not open ", filename, "\n");
        return;
    }

    //allocate a pool of memory for all synonym lists, don't need to free this until the game exits
    // wordnet_node** synonyms = dynamic_alloc_typed(MAX_SYNONYMS, wordnet_node*);

    char pos[256];
    int wordnet_id;
    char word[256];
    char key[256];
    char synonym[256];

    int error = 0;

    for(int k = 0; k < wordmap.max_entries; k++) {
        error = fscanf(file, "{\"pos\": \"%255s \"wordnet_id\": \"%d\", \"word\": \"%255s \"key\": \"%255s \"synonyms\": [", pos, &wordnet_id, word, key);
        if(error == EOF) break;
        strip(pos);
        strip(word);
        strip(key);
        for(int i = 0; word[i] && i < len(word); i++) {
            if(!is_letter(word[i])) goto next_word;
            if(is_uppercase(word[i])) word[i] += 'a'-'A';
        }

        if(check_word(word)) {
            wordnet_node& node = wordmap[word];

            log_output(pos, ", ", wordnet_id, ", ", word, ", ", key, "\n");
            bool last_syn = false;
            for(;;) {
                int n = fscanf(file, "\"%255s", synonym);
                if(n == 0) break;
                for(int i = 0; i < len(synonym); i++) {
                    if(synonym[i] == '\n') {
                        last_syn = true;
                        break;
                    }
                    if(synonym[i] == '"') {
                        synonym[i] = 0;
                        if(synonym[i+1] == ']') last_syn = true;
                        break;
                    }
                }
                if(strcmp(word, synonym) != 0 && check_word(synonym)) {
                    wordnet_node* snode = &(wordmap[synonym]);
                    node.synonyms[node.n_synonyms++] = snode;
                    snode->synonyms[snode->n_synonyms++] = &node;

                    log_output("    ", synonym, "\n");
                }
                if(last_syn) break;
            }
        }
    next_word:
        while(fgetc(file) != '\n');
    }

    fclose(file);
}

void cache_wordnet()
{
    char* filename = "data/thesaurus_cache.bin";
    FILE* file = fopen(filename, "wb");
    if(!file) {
        log_output("error: could not open ", filename, "\n");
        return;
    }
    int n_keys = 0;
    for(int i = 0; i < wordmap.max_entries; i++) {
        if(wordmap.keys[i] != 0) n_keys++;
    }
    fwrite(&n_keys, sizeof(n_keys), 1, file);
    for(int i = 0; i < wordmap.max_entries; i++) {
        if(wordmap.keys[i] != 0) {
            fwrite(&i, sizeof(i), 1, file);
            int length = strlen(wordmap.keys[i]);
            fwrite(&length, sizeof(length), 1, file);
            fwrite(wordmap.keys[i], 1, length, file);
            fwrite(&wordmap.values[i].n_synonyms, sizeof(int), 1, file);
            for(int j = 0; j < wordmap.values[i].n_synonyms; j++) {
                int32 index = (int) (wordmap.values[i].synonyms[j] - wordmap.values);
                fwrite(&index, sizeof(index), 1, file);
            }
        }
    }
    fclose(file);
}

bool load_wordnet_cache()
{
    int error = 0;
    char* filename = "data/thesaurus_cache.bin";
    FILE* file = fopen(filename, "rb");
    if(!file) {
        log_output("error: could not open ", filename, "\n");
        return false;
    }

    wordnet_node** synonyms = dynamic_alloc_typed(MAX_SYNONYMS, wordnet_node*);

    int max_synonyms = 0;
    int i = 0;
    int synonym_index = 0;
    int n_keys = 0;
    int32 syndex = 0;
    fread(&n_keys, sizeof(n_keys), 1, file);
    for(int k = 0; k < n_keys; k++) {
        error = fread(&i, sizeof(i), 1, file);
        int length;
        fread(&length, sizeof(length), 1, file);
        fread(wordmap.values[i].word, 1, length, file);
        wordmap.keys[i] = wordmap.values[i].word;
        fread(&wordmap.values[i].n_synonyms, sizeof(int), 1, file);
        max_synonyms = max(max_synonyms, wordmap.values[i].n_synonyms);
        wordmap.values[i].synonyms = synonyms;
        wordmap.values[i].visit_id = 0;
        for(int j = 0; j < wordmap.values[i].n_synonyms; j++) {
            fread(&syndex, sizeof(syndex), 1, file);
            *(synonyms++) = wordmap.values + syndex;
        }
    }
    log_output("max_synonyms: ", max_synonyms, "\n");
    fclose(file);
    return true;
}

int gematria(char* a)
{
    int sum = 0;
    while(*a) {
        int v = *a-'a'+1;
        a++;
        if(v < 1 || v > 26) {
#ifdef DEBUG
            if(*(a-1) != ' ') log_warning("character outside of [a-z ] encountered");
#endif
            continue;
        }
        sum += v;
    }
    return sum;
}

real get_pair_resonance(char* a, char* b)
{
    int a_len = strlen(a);
    int b_len = strlen(b);
    int distance = levenshtein(std::string(a), std::string(b));
    // size_t length = max(a_len, b_len);
    real score = 5.0*max(((real) (max(b_len, a_len)-distance)/(2+min(a_len, b_len))), 0.0f);
    int a_sum = gematria(a);
    int b_sum = gematria(b);
    if(a_sum == b_sum && strcmp(a, b) != 0) {
        score *= 1.5;
    }
    // log_output("    ", a, ", ", b, " : ", score, "\n");
    return score;
}

real_2 get_complete_resonance(char* a, wordnet_node* node, real weight, int depth)
{
    if(depth >= 2) return {};
    if(node->visit_id == current_visit_id) return {}; //prevent double counting
    node->visit_id = current_visit_id;
    real value = get_pair_resonance(a, node->word);
    real w = weight*exp(-1.0f/value);
    real_2 score = {value*w, w};
    for(int i = 0; i < node->n_synonyms; i++) {
        score += get_complete_resonance(a, node->synonyms[i], 0.9f*weight, depth+1);
    }
    return score;
}

real get_resonance(char* a, char* b)
{
    char a_stripped[256];
    char b_stripped[256];
    memcpy(a_stripped, a, 256);
    memcpy(b_stripped, b, 256);
    strip(a_stripped);
    strip(b_stripped);
    wordnet_node* node = &wordmap[b_stripped];
    if(!node->word[0]) {
        current_visit_id++;
        return get_pair_resonance(a_stripped, b_stripped);
    }
    real_2 score = get_complete_resonance(a_stripped, node, 1.0, 0);
    current_visit_id++;
    if(score.y == 0.0f) return 0.0f;
    real out = score.x/score.y;
    // log_output(a_stripped,", ", b_stripped, " = ", out, "\n");
    return out;
}

void load_wordnet()
{
    init_hashmap(&wordmap, 200000);
    if(!load_wordnet_cache()) {
        create_wordnet();
        cache_wordnet();
    }
}

#endif //WORDNET
