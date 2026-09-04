#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define CALC_COUNT 3
#define USER_INPUT_SZ 0x100

struct calc {
    int cost;
    int buttons;
    char name[0x20];
    size_t (*calculate)(size_t, size_t);
};

size_t calculate_method(size_t a, size_t b) {
    return a + b;
}

void init_calc(struct calc* c, int buttons, char* name) {
    c->cost = buttons * 2;
    c->buttons = buttons;
    strcpy(&c->name, name);
    c->calculate = calculate_method;
}


char* initial_names[CALC_COUNT] = {
    "TI-108 Solar Power",
    "calc.exe",
    //"Desmos",
    //"WolframAlpha",
    "TI-84 SUPER PLUS ULTRA" 
};

struct calc* gen_calcs() {
    struct calc* to_ret = calloc(CALC_COUNT, sizeof(*to_ret));
    if (!to_ret) return NULL;

    for (size_t i = 0; i < CALC_COUNT; i++) {
        init_calc(&to_ret[i], i * 23, initial_names[i]);
    }
    
    return to_ret;
}

void pretty_print(struct calc* c) {
    printf(c->name);
    printf("\ncost: %d\nbuttons: %d\n\n", c->cost, c->buttons);
}

void handle_list(struct calc* calcs) {
    for (size_t i = 0; i < CALC_COUNT; i++) {
        printf("Calc #%lld\n", i);
        pretty_print(&calcs[i]);
    }
}

void handle_edit(struct calc* calcs) {
    char user_input[USER_INPUT_SZ] = { 0 };

    printf("Select calc:\n");
    handle_list(calcs);
    printf("\n>>");

    fgets(user_input, USER_INPUT_SZ, stdin);
    size_t idx = atoi(user_input);

    if (idx < 0 || idx >= CALC_COUNT) {
        printf("Outside of range!");
        return;
    }

    struct calc* selected_calc = &calcs[idx];

    printf("New name: ");
    fgets(selected_calc->name, USER_INPUT_SZ, stdin);
    char* newline = memchr(selected_calc->name, '\n', sizeof(selected_calc->name));
    if (newline) *newline = '\0';

    printf("New cost: ");
    scanf("%d", &selected_calc->cost);

    printf("New button count: ");
    scanf("%d", &selected_calc->buttons);

    printf("Thank you!\n\n");
}

void handle_calc(struct calc* calcs) {
    char user_input[USER_INPUT_SZ] = { 0 };

    printf("Select calc:\n");
    handle_list(calcs);
    printf("\n>>");

    fgets(user_input, USER_INPUT_SZ, stdin);
    size_t idx = atoi(user_input);

    if (idx < 0 || idx >= CALC_COUNT) {
        printf("Outside of range!\n\n");
        return;
    }

    struct calc* selected_calc = &calcs[idx];

    size_t a, b;

    printf("Provide value A: ");
    scanf("%lld", &a);

    printf("Provide value B: ");
    scanf("%lld", &b);

    printf("Result: %d\n\n", selected_calc->calculate(a, b));
}

int main() {
    char user_input[USER_INPUT_SZ] = { 0 };

    struct calc* calcs = gen_calcs();
    
    if (!calcs) {
        printf("couldn't allocate calcs!");
        return -1;
    }

    while (1) {
        printf("Select an option:\n1: edit a calc\n2: list all calcs\n3: run calculation\n4: quit\n>>");
        
        memset(user_input, 0, USER_INPUT_SZ);
        fgets(user_input, USER_INPUT_SZ, stdin);

        switch (user_input[0]) {
        case '1':
            handle_edit(calcs);
            break;
        case '2':
            handle_list(calcs);
            break;
        case '3':
            handle_calc(calcs);
            break;
        case '4':
            printf("See ya later!\n");
            return 0;
        default:
            printf("Unknown choice\n\n");
            break;
        }
    }
}