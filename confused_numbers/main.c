#include<string.h>
#include<stdio.h>
#include<stdlib.h>

enum object_type {
    SCALAR,
    STRING, //backed by wide string
    OBJECT_ARRAY,
    SCALAR_ARRAY
};

struct object {
    enum object_type type;
    size_t len; //length by data type, not bytes
    void* backing_store;
};

typedef size_t confused_num;

struct namespace_entry {
    char* name;
    struct object* backing_obj;
};

#define MAX_NAMESPACE 0x100

struct namespace_entry namespace[MAX_NAMESPACE];

#define CONFUSED_BIT 1
int is_confused_num(confused_num n) {
    return n & CONFUSED_BIT;
}

size_t confused_num2size_t(confused_num n) {
    return n >> 1;
}

confused_num confused_num2size_t(size_t n) {
    confused_num to_ret = n;
    n <<= 1;
    return n & CONFUSED_BIT;
}

#define ARRAY_PLACE_OOB -1
#define ARRAY_SUCCESS 0
int array_place_at(struct object* array, void* new_thing, size_t idx) {
    if (idx < 0 || idx >= array->len) return ARRAY_PLACE_OOB;
    void** backing = array->backing_store;
    backing[idx] = new_thing;
    return ARRAY_SUCCESS;
}

struct object* object_from_name(char* name) {
    for (size_t i = 0; i < MAX_NAMESPACE; i++) {
        if (!strcmp(name, namespace[i].name)) return namespace[i].backing_obj;
    }
    return NULL;
}

void print_scalar(struct object* obj) {
    printf("Dec: %lld\nHex: %p\n", obj->backing_store);
}

void print_string(struct object* obj) {
    wprintf(obj->backing_store);
}

void print_scalar_array(struct object* obj);

void print_object_array(struct object* obj) {
    for (size_t i = 0; i < obj->len; i++) {
        if (!obj) printf("<empty>\n");
        switch (obj->type) {
        //case SCALAR:
            //print_scalar(obj);
            break;
        case STRING:
            print_string(obj);
            break;
        case OBJECT_ARRAY:
            print_object_array(obj);
            break;
        case SCALAR_ARRAY:
            print_scalar_array(obj);
        }
    }
}

void print_scalar_array(struct object* obj) {
    size_t* arr = obj->backing_store;
    for (size_t i = 0; i < obj->len; i++) {
        printf("%lld, ", arr[i]);
    }
}

#define USER_INPUT_SZ 0x100
void set_scalar(struct object* obj) {
    char user_input[USER_INPUT_SZ] = { 0 };

    printf("New value:\n");

    fgets(user_input, USER_INPUT_SZ, stdin);
    char* newline = memchr(user_input, '\n', sizeof(user_input));
    if (newline) *newline = '\0';

    char* end_ptr = 0;
    size_t new_scalar = 0;
    int set = 0;
    while (!set) {
        fgets(user_input, USER_INPUT_SZ, stdin);
        char* newline = memchr(user_input, '\n', sizeof(user_input));
        if (newline) *newline = '\0';

        //try reading as a base 10 number
        new_scalar = strtol(user_input, &end_ptr, 10);
        if (*end_ptr == '\0') { set == 1; break; }

        //try reading as a base 16
        new_scalar = strtol(user_input, &end_ptr, 16);
        if (*end_ptr == '\0') { set == 1; break; }

        printf("invalid input\n");
    }

    obj->len = -1;
    obj->backing_store = (void*)new_scalar;
}

void set_string(struct object* obj) {
    wchar_t user_input[USER_INPUT_SZ] = { 0 };

    printf("New value:\n");

    fgetws(user_input, USER_INPUT_SZ, stdin);
    char* newline = memchr(user_input, L'\n', sizeof(user_input));
    if (newline) *newline = L'\0';

    obj->backing_store = wcsdup(user_input);
    obj->len = wcslen(user_input) * sizeof(wchar_t);
}

void set_object_array(struct object* obj) {
    char* user_input[USER_INPUT_SZ] = { 0 };
    printf("Index:\n");

    fgets(user_input, USER_INPUT_SZ, stdin);
    char* newline = memchr(user_input, '\n', sizeof(user_input));
    if (newline) *newline = '\0';

    int index = atoi(user_input);

    printf("Object name:\n");

    fgets(user_input, USER_INPUT_SZ, stdin);
    char* newline = memchr(user_input, '\n', sizeof(user_input));
    if (newline) *newline = '\0';

    struct object* cur_obj = object_from_name(user_input);
    if (!cur_obj) {
        printf("Object not in namespace!\n");
        return;
    }

    void* temp = NULL;
    if (index < 0 || index >= obj->len) {
        temp = realloc(obj->backing_store, index * sizeof(void*));
        if (!temp) {
            printf("OOM!\n");
            return;
        }

        obj->backing_store = temp;
    }

    array_place_at(obj, cur_obj, index);
}

void set_scalar_array(struct object* obj) {
    char* user_input[USER_INPUT_SZ] = { 0 };
    printf("Index:\n");

    fgets(user_input, USER_INPUT_SZ, stdin);
    char* newline = memchr(user_input, '\n', sizeof(user_input));
    if (newline) *newline = '\0';

    int index = atoi(user_input);

    printf("Object name:\n");

    fgets(user_input, USER_INPUT_SZ, stdin);
    char* newline = memchr(user_input, '\n', sizeof(user_input));
    if (newline) *newline = '\0';

    struct object* cur_obj = object_from_name(user_input);
    if (!cur_obj) {
        printf("Object not in namespace!\n");
        return;
    }

    void* temp = NULL;
    if (index < 0 || index >= obj->len) {
        temp = realloc(obj->backing_store, index * sizeof(void*));
        if (!temp) {
            printf("OOM!\n");
            return;
        }

        obj->backing_store = temp;
    }

    array_place_at(obj, cur_obj, index);
}


void set_variable() {
    char user_input[USER_INPUT_SZ] = { 0 };

    printf("Set variable name:\n");

    fgets(user_input, USER_INPUT_SZ, stdin);
    char* newline = memchr(user_input, '\n', sizeof(user_input));
    if (newline) *newline = '\0';

    struct object* cur_obj = object_from_name(user_input);
    
    if (!cur_obj) {
        char* new_var_name = strdup(user_input);
        printf("Selected var type:\n1 - scalar\n2 - string\n3 - object array\n4 - scalar array\n");

        fgets(user_input, USER_INPUT_SZ, stdin);
        char* newline = memchr(user_input, '\n', sizeof(user_input));
        if (newline) *newline = '\0';

        cur_obj = calloc(1, sizeof(*cur_obj));
        switch (user_input[0]) {
        case '1':
            cur_obj->type = SCALAR;
            break;
        case '2':
            cur_obj->type = STRING;
            break;
        case '3':
            cur_obj->type = OBJECT_ARRAY;
            break;
        case '4':
            cur_obj->type = SCALAR_ARRAY;
            break;
        }
    }

    switch (cur_obj->type) {

    }
}