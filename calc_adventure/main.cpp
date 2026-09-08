#include<stdio.h>
#include<string.h>
#include<vector>

#define ASCII_NAME 0
#define WIDE_NAME 1
struct accessible_name {
    int ascii_or_wide; // 0 = ascii, 1 = wide
    union {
        char* ascii_name;
        wchar_t* wide_name;
    };
};

class Character {
private:
    char* internal_name;

public:
    struct accessible_name name;
    std::vector<Character*>* map;
    int hp;

    Character(char* new_name, std::vector<Character*>& characters) {
        name.ascii_or_wide = ASCII_NAME;
        name.ascii_name = strdup(new_name);
        
        internal_name = strdup(new_name);
        hp = 5;

        map = &characters;
        map->push_back(this);
    }

    virtual void special_move() {
        printf("HIYAAh!\n");
    }
    
    void set_ascii_name(char* new_name) {
        free(name.ascii_name);
        name.ascii_name = strdup(new_name);

        free(internal_name);
        internal_name = strdup(new_name);
    }

    void set_wide_name(wchar_t* new_name) {
        free(name.wide_name);
        name.wide_name = wcsdup(new_name);

        free(internal_name);
        size_t new_name_len = wcslen(new_name);
        char* new_internal_name = (char*)calloc(new_name_len+1, sizeof(char));

        wcstombs(new_internal_name, new_name, new_name_len);
        internal_name = new_internal_name;
    }

    void take_damage() {
        hp--;
        if (hp > 0) {
            if (name.ascii_or_wide == ASCII_NAME) printf("%s: AUGH!\n", name.ascii_name);
            else printf("%ls: AUGH!\n", name.wide_name);
        }
        else {
            if (name.ascii_or_wide == ASCII_NAME) printf("%s has died\n", name.ascii_name);
            else printf("%ls has died\n", name.wide_name);
            delete this;
        }
    }

    void set_to_wide() {
        size_t name_len = strlen(name.ascii_name);
        wchar_t* new_name = (wchar_t*)calloc(name_len + 1, sizeof(wchar_t));

        mbstowcs(new_name, name.ascii_name, name_len);

        name.ascii_or_wide = WIDE_NAME;
        name.wide_name = new_name;
    }

    void pretty_print() {
        if (name.ascii_or_wide == ASCII_NAME) {
            printf("Name: ");
            printf(name.ascii_name);
            printf("\n");
        }
        else {
            printf("Name: ");
            wprintf(name.wide_name);
            printf("\n");
        }
        printf("HP: %d\n", hp);
    }

    virtual ~Character() {
        for (size_t i = 0; i < map->size(); i++) {
            if (strcmp((*map)[i]->name.ascii_name, internal_name) == 0) (*map)[i] = NULL;
        }

        if (name.ascii_or_wide == ASCII_NAME) {
            if (name.ascii_name) free(name.ascii_name);
        }
        else {
            if (name.wide_name) free(name.wide_name);
        }
        
        free(internal_name);
    }
};

class Calc : Character {
public:
    Calc(char* new_name, std::vector<Character*>& characters) : Character(new_name, characters) {}

    virtual void special_move() override {
        printf("beep boop!\n");
    }
};

#define USER_INPUT_SZ 0x100

#define CALC_COUNT 0x3

const char* initial_names[CALC_COUNT] = {
    "Mr. TI",
    "Sir AlphaWolfram",
    "Paladin Desmos"
};

void init_characters(std::vector<Character*>& characters) {
    for (size_t i = 0; i < CALC_COUNT; i++) {
        new Calc((char*)initial_names[i], characters);
    }
}

void handle_list(std::vector<Character*> characters) {
    for (size_t i = 0; i < characters.size(); i++) {
        if (!characters[i]) continue;

        printf("Hero #%lld\n", i);
        characters[i]->pretty_print();
    }
}

void handle_edit(std::vector<Character*> characters) {
    char user_input[USER_INPUT_SZ] = { 0 };

    printf("Select a hero to edit:\n");
    //handle_list(characters);
    printf("\n>>");

    fgets(user_input, USER_INPUT_SZ, stdin);
    size_t idx = atoi(user_input);

    if (idx < 0 || idx >= CALC_COUNT) {
        printf("Outside of range!");
        return;
    }

    Character* cur_char = characters[idx];

    printf("New name: ");
    memset(user_input, 0, USER_INPUT_SZ);
    if (cur_char->name.ascii_or_wide == ASCII_NAME) {
        fgets(user_input, USER_INPUT_SZ, stdin);

        char* newline = (char*)memchr(user_input, '\n', USER_INPUT_SZ);
        if (newline) *newline = '\0';

        cur_char->set_ascii_name(user_input);
    }
    else {
        fgetws((wchar_t*)user_input, USER_INPUT_SZ/sizeof(wchar_t), stdin);

        wchar_t* newline = (wchar_t*)memchr(user_input, L'\n', USER_INPUT_SZ);
        if (newline) *newline = L'\0';

        cur_char->set_wide_name((wchar_t*)user_input);
    }

    printf("New HP: ");
    scanf("%d", &cur_char->hp);

    printf("Thank you!\n\n");
}

void handle_damage(std::vector<Character*> characters) {
    char user_input[USER_INPUT_SZ] = { 0 };

    printf("Select a hero to deal damage to:\n");
    //handle_list(characters);
    printf("\n>>");

    fgets(user_input, USER_INPUT_SZ, stdin);
    size_t idx = atoi(user_input);

    if (idx < 0 || idx >= CALC_COUNT) {
        printf("Outside of range!");
        return;
    }

    characters[idx]->take_damage();
}

void handle_widen(std::vector<Character*> characters) {
    char user_input[USER_INPUT_SZ] = { 0 };

    printf("Select a hero change name to wide:\n");
    //handle_list(characters);
    printf("\n>>");

    fgets(user_input, USER_INPUT_SZ, stdin);
    size_t idx = atoi(user_input);

    if (idx < 0 || idx >= CALC_COUNT) {
        printf("Outside of range!");
        return;
    }

    characters[idx]->set_to_wide();
}

void handle_special_move(std::vector<Character*> characters) {
    char user_input[USER_INPUT_SZ] = { 0 };

    printf("Select a hero to use a special move:\n");
    //handle_list(characters);
    printf("\n>>");

    fgets(user_input, USER_INPUT_SZ, stdin);
    size_t idx = atoi(user_input);

    if (idx < 0 || idx >= CALC_COUNT) {
        printf("Outside of range!");
        return;
    }

    characters[idx]->special_move();
}

#define CHEATCODE_NUMS 6
void handle_enter_cheat_code() {
    char user_input[USER_INPUT_SZ] = { 0 };
    size_t* block = (size_t*)calloc(6, sizeof(size_t));

    printf("Input numbers seperated by commas:");
    fgets(user_input, USER_INPUT_SZ, stdin);
    user_input[strcspn(user_input, "\n")] = '\0';

    int idx = 0;
    int chars_travelled = 0;
    size_t cur_num = 0;
    char* cur_ptr = user_input;
    while (idx < 6 && sscanf(cur_ptr, "%llx%n", &cur_num, &chars_travelled) == 1) {
        block[idx++] = cur_num;

        cur_ptr += chars_travelled;
        while (*cur_ptr == ' ' || *cur_ptr == ',') cur_ptr++;
    }
    printf("secret: %x\n", (int)block & 0xF);
}

int main() {
    setvbuf(stdout, NULL, _IONBF, 0);

    char user_input[USER_INPUT_SZ] = { 0 };

    std::vector<Character*> characters;
    init_characters(characters);

    while (1) {
        printf("Select an option:\n1: edit a character\n2: list all characters\n3: use special move!\n4: deal damage to a character\n"
        "5: widen character name\n6: quit\n>>");

        memset(user_input, 0, USER_INPUT_SZ);
        fgets(user_input, USER_INPUT_SZ, stdin);

        switch (user_input[0]) {
        case '1':
            handle_edit(characters);
            break;
        case '2':
            handle_list(characters);
            break;
        case '3':
            handle_special_move(characters);
            break;
        case '4':
            handle_damage(characters);
            break;
        case '5':
            handle_widen(characters);
            break;
        case '!':
            handle_enter_cheat_code();
            break;
        case '6':
            printf("See ya later!\n");
            return 0;
        default:
            printf("Unknown choice\n\n");
            break;
        }
    }
}