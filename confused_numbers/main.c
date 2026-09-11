#include<string.h>
#include<stdio.h>
#include<stdlib.h>

#define USER_INPUT_SZ 0x100
char user_input[USER_INPUT_SZ];
void get_user_input() {
    memset(user_input, 0, USER_INPUT_SZ);
    fgets(user_input, USER_INPUT_SZ, stdin);
    user_input[strcspn(user_input, "\r\n")] = '\0';
}

enum entry_type {
    SDHF,
    SMART,
    SCALAR
};

#define COLLECTION_LEN 3
struct namespace_entry {
    char* name;
    enum entry_type type;
    union {
        size_t* sdhf;
        char** smart_colors;
        size_t rgb;
    };
};

#define MAX_NAMESPACE 0x100

struct namespace_entry namespace[MAX_NAMESPACE];

struct namespace_entry* next_free_entry() {
    for (size_t i = 0; i < MAX_NAMESPACE; i++) {
        if (!namespace[i].name) return &namespace[i];
    }
    return NULL;
}

struct namespace_entry* entry_from_name(char* name) {
    for (size_t i = 0; i < MAX_NAMESPACE; i++) {
        if (!namespace[i].name) continue;

        if (!strcmp(name, namespace[i].name)) return &namespace[i];
    }
    return NULL;
}

void set_sdhf(struct namespace_entry* e) {
    printf("Input your values for your SUPER DUPER HIGH FIDELITY color (comma seperated, hexdecimal):");

    get_user_input();
    
    if(!e->sdhf) e->sdhf = calloc(COLLECTION_LEN, sizeof(size_t));

    int idx = 0;
    int chars_travelled = 0;
    size_t cur_num = 0;
    char* cur_ptr = user_input;
    while (idx < COLLECTION_LEN && sscanf(cur_ptr, "%llx%n", &cur_num, &chars_travelled) == 1) {
        e->sdhf[idx++] = cur_num;

        cur_ptr += chars_travelled;
        while (*cur_ptr == ' ' || *cur_ptr == ',') cur_ptr++;
    }
}

void set_smart(struct namespace_entry* e) {
    printf("Input your values for your Smart(tm) color (space seperated strings):");

    get_user_input();

    char** smart_colors = calloc(COLLECTION_LEN, sizeof(char*));

    int idx = 0;
    int chars_travelled = 0;
    char* cur_ptr = user_input;
    char cur_word[64] = { 0 };
    while (idx < COLLECTION_LEN && sscanf(cur_ptr, "%s%n", &cur_word, &chars_travelled) == 1) {
        smart_colors[idx++] = _strdup(cur_word);
        memset(cur_word, 0, 64);

        cur_ptr += chars_travelled;
        while (*cur_ptr == ' ' || *cur_ptr == ',') cur_ptr++;
    }

    e->smart_colors = smart_colors;
}

void set_rgb(struct namespace_entry* e) {

    printf("Input hex value for RGB:");

    get_user_input();
    sscanf(user_input, "%llx", &e->rgb);
    
}

void print_sdhf(struct namespace_entry* e) {
    printf("%s: %llx, %llx, %llx\n", e->name, e->sdhf[0], e->sdhf[1], e->sdhf[2]);
}

void print_smart(struct namespace_entry* e) {
    printf("%s: [\n", e->name);
    printf("\t"); printf(e->smart_colors[0]); printf("\n");
    printf("\t"); printf(e->smart_colors[1]); printf("\n");
    printf("\t"); printf(e->smart_colors[2]); printf("\n");
    printf("]\n");
}

void print_rgb(struct namespace_entry* e) {
    printf("%s: %llx\n", e->name, e->rgb);
}

void list_colors() {
    for (size_t i = 0; i < MAX_NAMESPACE; i++) {
        if (!namespace[i].name) continue;
        switch (namespace[i].type) {
        case SDHF:
            print_sdhf(&namespace[i]);
            break;
        case SMART:
            print_smart(&namespace[i]);
            break;
        case SCALAR:
            print_rgb(&namespace[i]);
            break;
        }
    }
}

void handle_convert_color() {
    printf("Which color would you like to convert?\n");
    list_colors();

    get_user_input();

    struct namespace_entry* e = entry_from_name(user_input);
    if (!e) { printf("color not reconized\n"); return; }

    printf("Which color type would you like to convert %s to?\n", e->name);
    printf("1: SUPER DUPER HIGH FIDELITY\n2: Smart(tm) color\n3: RGB\n");

    get_user_input();
    switch(user_input[0]) {
    case '1':
        e->type = SDHF;
        break;
    case '2':
        e->type = SMART;
        break;
    case '3':
        e->type = SCALAR;
        break;
    default:
        printf("choice not recognized\n");
        return;
    }

    printf("WARNING: CONVERTING COLOR WILL LIKELY RESULT IN DATA LOSS. TYPE 'AFFIRM' TO CONTINUE\n");
    get_user_input();;
    if (strcmp(user_input, "AFFIRM") != 0) {
        printf("ABORTED!\n");
        return;
    }

    switch (e->type) {
    case SDHF:
        set_sdhf(e);
        return;
    case SMART:
        set_smart(e);
        return;
    case SCALAR:
        set_rgb(e);
        return;
    }
}

void handle_new_color() {
    printf("Input name of new color:\n");

    get_user_input();

    struct namespace_entry* e = entry_from_name(user_input);
    if (e) { printf("color already exists!\n"); return; }

    char* new_name = strdup(user_input);

    e = next_free_entry();

    printf("Select new color type:\n");
    printf("1: SUPER DUPER HIGH FIDELITY\n2: Smart(tm) color\n3: RGB\n");

    get_user_input();
    switch (user_input[0]) {
    case '1':
        e->type = SDHF;
        set_sdhf(e);
        break;
    case '2':
        e->type = SMART;
        set_smart(e);
        break;
    case '3':
        e->type = SCALAR;
        set_rgb(e);
        break;
    default:
        printf("choice not recognized\n");
        free(new_name);
        return;
    }

    e->name = new_name;
}

void handle_edit_color() {
    printf("Input name of color to edit:\n");

    get_user_input();

    struct namespace_entry* e = entry_from_name(user_input);
    if (!e) { printf("color not reconized!\n"); return; }

    switch (e->type) {
    case SDHF:
        set_sdhf(e);
        break;
    case SMART:
        set_smart(e);
        break;
    case SCALAR:
        set_rgb(e);
        break;
    default:
        printf("choice not recognized\n");
        return;
    }
}

void handle_render() {
    printf("Input name of color to edit:\n");

    get_user_input();

    struct namespace_entry* e = entry_from_name(user_input);
    if (!e) { printf("color not reconized!\n"); return; }

    printf("Rendering %s...\n", e->name);

    printf("Wow! Its so beautiful!\n");
}


int main() {
    setvbuf(stdout, NULL, _IONBF, 0);

    while (1) {
        printf("Select an option:\n1: add a new color\n2: edit a color\n3: list all colors\n4: convert a color to a different format\n5: render a color\n6: exit\n>>");

        get_user_input();
        switch (user_input[0]) {
        case '1':
            handle_new_color();
            break;
        case '2':
            handle_edit_color();
            break;
        case '3':
            list_colors();
            break;
        case '4':
            handle_convert_color();
            break;
        case '5':
            handle_render();
            break;
        case '6':
            return 0;
        default:
            printf("Unknown choice\n\n");
            break;
        }
    }
}