#include<string.h>
#include<stdio.h>
#include<stdlib.h>

enum object_type {
    STRING,
    OBJECT_ARRAY,
    SCALAR_ARRAY
};

struct object {
    enum object_type type;
    size_t len; //length by data type, not bytes
    void* backing_store;
};

enum entry_type {
    SCALAR,
    POINTER
};

struct namespace_entry {
    char* name;
    enum entry_type type;
    struct object* backing_obj; //if SCALAR, is used to hold 64 bit number
};

#define MAX_NAMESPACE 0x100

struct namespace_entry namespace[MAX_NAMESPACE];

#define ARRAY_PLACE_OOB -1
#define ARRAY_SUCCESS 0
int array_place_at(struct object* array, void* new_thing, size_t idx) {
    if (idx < 0 || idx >= array->len) return ARRAY_PLACE_OOB;
    void** backing = array->backing_store;
    backing[idx] = new_thing;
    return ARRAY_SUCCESS;
}

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

void print_string(struct object* obj) {
    printf(obj->backing_store);
    printf("\n");
}

void print_scalar_array(struct object* obj);

void print_object_array(struct object* obj) {
    struct object** arr = obj->backing_store;
    printf("[\n");
    for (size_t i = 0; i < obj->len; i++) {    
        printf("\t");
        if (!arr[i]) { printf("<empty>\n"); continue; }
        switch (arr[i]->type) {
        case STRING:
            print_string(arr[i]);
            break;
        case OBJECT_ARRAY:
            print_object_array(arr[i]);
            break;
        case SCALAR_ARRAY:
            print_scalar_array(arr[i]);
        }
    }
    printf("]\n");
}

void print_scalar_array(struct object* obj) {
    void** arr = obj->backing_store;
    for (size_t i = 0; i < obj->len; i++) {
        printf("%p, ", arr[i]);
    }
    printf("\n");
}

void print_obj(struct object* obj) {
    switch (obj->type) {
    case STRING:
        print_string(obj);
        break;
    case OBJECT_ARRAY:
        print_object_array(obj);
        break;
    case SCALAR_ARRAY:
        print_scalar_array(obj);
        break;
    }
}

void print_entry(struct namespace_entry* entry) {
    if (entry->type == SCALAR) printf("%p\n", entry->backing_obj);
    else print_obj(entry->backing_obj);
}

void print_namespace() {
    for (size_t i = 0; i < MAX_NAMESPACE; i++) {
        if (!namespace[i].name) continue;

        printf("%s: ", namespace[i].name);
        print_entry(&namespace[i]);
    }
}

#define USER_INPUT_SZ 0x100

size_t user_input_scalar() {
    printf("Scalar Value or Variable:\n");

    char* end_ptr = 0;
    size_t new_scalar = 0;
    int set = 0;
    char user_input[USER_INPUT_SZ] = { 0 };
    while (!set) {
        fgets(user_input, USER_INPUT_SZ, stdin);
        char* newline = memchr(user_input, '\n', sizeof(user_input));
        if (newline) *newline = '\0';

        struct namespace_entry* entry = entry_from_name(user_input);
        if (entry) { new_scalar = (size_t)entry->backing_obj; break; }

        //try reading as a base 10 number
        new_scalar = strtoll(user_input, &end_ptr, 10);
        if (*end_ptr == '\0') break;

        //try reading as a base 16
        new_scalar = strtoll(user_input, &end_ptr, 16);
        if (*end_ptr == '\0') break;

        printf("invalid input\n");
    }
    return new_scalar;
}

void set_string(struct object* obj) {
    char user_input[USER_INPUT_SZ] = { 0 };

    printf("New value:\n");

    fgets(user_input, USER_INPUT_SZ, stdin);
    char* newline = memchr(user_input, '\n', sizeof(user_input));
    if (newline) *newline = '\0';

    obj->backing_store = _strdup(user_input);
    obj->len = strlen(user_input);
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
    newline = memchr(user_input, '\n', sizeof(user_input));
    if (newline) *newline = '\0';

    struct namespace_entry* cur_entry = entry_from_name(user_input);
    if (!cur_entry) {
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

    array_place_at(obj, cur_entry->backing_obj, index);
}

void set_scalar_array(struct object* obj) {
    char* user_input[USER_INPUT_SZ] = { 0 };
    printf("Index:\n");

    fgets(user_input, USER_INPUT_SZ, stdin);
    char* newline = memchr(user_input, '\n', sizeof(user_input));
    if (newline) *newline = '\0';

    int index = atoi(user_input);

    size_t new_scalar = user_input_scalar();

    array_place_at(obj, new_scalar, index);
}



void set_scalar(struct namespace_entry* entry) {
    printf("Set %s value\n", entry->name);

    entry->backing_obj = (void*)user_input_scalar();
}

#define ARR_INIT_SZ 3
void set_variable() {
    char user_input[USER_INPUT_SZ] = { 0 };

    printf("Set variable name:\n");

    fgets(user_input, USER_INPUT_SZ, stdin);
    char* newline = memchr(user_input, '\n', sizeof(user_input));
    if (newline) *newline = '\0';

    struct namespace_entry* cur_entry = entry_from_name(user_input);
    struct object* cur_obj = NULL;

    if (!cur_entry) {
        char* new_var_name = strdup(user_input);
        printf("Select new var type:\n1 - scalar\n2 - string\n3 - object array\n4 - scalar array\n");

        fgets(user_input, USER_INPUT_SZ, stdin);
        newline = memchr(user_input, '\n', sizeof(user_input));
        if (newline) *newline = '\0';
        
        cur_entry = next_free_entry();
        if (!cur_entry) { printf("namespace full!\n"); return; }
        //if scalar, strdup name into entry slot, set type, and set value
        if (user_input[0] == '1') {


            cur_entry->name = new_var_name;
            cur_entry->type = SCALAR;

            set_scalar(cur_entry);
            return;
        }


        cur_obj = calloc(1, sizeof(*cur_obj));
        switch(user_input[0]) {
        case '2':
            cur_obj->type = STRING;
            cur_obj->len = 0;
            break;
        case '3':
            cur_obj->type = OBJECT_ARRAY;
            cur_obj->len = ARR_INIT_SZ;
            cur_obj->backing_store = calloc(ARR_INIT_SZ, sizeof(void*));
            break;
        case '4':
            cur_obj->type = SCALAR_ARRAY;
            cur_obj->len = ARR_INIT_SZ;
            cur_obj->backing_store = calloc(ARR_INIT_SZ, sizeof(size_t));
            break;
        default:
            printf("invalid choice!\n");
            free(new_var_name);
            free(cur_obj);
            return;
        }

        cur_entry->type = POINTER;
        cur_entry->backing_obj = cur_obj;
        cur_entry->name = new_var_name;
    }

    if (cur_entry->type == SCALAR) {
        set_scalar(cur_entry);
        return;
    }

    cur_obj = cur_entry->backing_obj;
    switch (cur_obj->type) {
    case STRING:
        set_string(cur_obj);
        break;
    case OBJECT_ARRAY:
        set_object_array(cur_obj);
        break;
    case SCALAR_ARRAY:
        set_scalar_array(cur_obj);
        break;
    }
}

int main() {
    char user_input[USER_INPUT_SZ] = { 0 };

    while (1) {
        printf("Select an option:\n1: set a variable\n2: list all variables\n>>");

        memset(user_input, 0, USER_INPUT_SZ);
        fgets(user_input, USER_INPUT_SZ, stdin);

        switch (user_input[0]) {
        case '1':
            set_variable();
            break;
        case '2':
            print_namespace();
            break;
        default:
            printf("Unknown choice\n\n");
            break;
        }
    }
}