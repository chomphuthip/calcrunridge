#include <stdio.h>
#include <stdlib.h>
#include<string.h>

#define USER_INPUT_SZ 0x100
char user_input[USER_INPUT_SZ];
void get_user_input() {
	memset(user_input, 0, USER_INPUT_SZ);
	fgets(user_input, USER_INPUT_SZ, stdin);
	user_input[strcspn(user_input, "\r\n")] = '\0';
}

#define MAX_H 0x3
#define MAX_L 0x3

struct matrix {
	int h;
	int l;
	size_t buf[MAX_H * MAX_L];
	struct matrix* flink;
};

struct matrix* head;

struct matrix* get_last_matrix() {
	if (!head) return NULL;

	struct matrix* cur_matrix = head;
	size_t i = 0;
	while (cur_matrix->flink) {
		cur_matrix = cur_matrix->flink;
	}

	return cur_matrix;
}

struct matrix* get_matrix_at(size_t idx) {
	if (!head) return NULL;

	struct matrix* cur_matrix = head;
	size_t i = 0;
	while (cur_matrix->flink) {
		if (i++ == idx) return cur_matrix;
		cur_matrix = cur_matrix->flink;
	}

	if (i == idx) return cur_matrix;
	else return NULL;
}

int print_as_hex = 0;

void print_matrix(struct matrix* m) {
	for (int i = 0; i < m->h; i++) {
		for (int o = 0; o < m->l; o++) {
			if(!print_as_hex) printf("%lld, ", m->buf[(i * m->l) + o]);
			else printf("%p, ", (void*)m->buf[(i * m->l) + o]);
		}
		printf("\n");
	}
}

void set_matrix(struct matrix* m) {


	printf("Specify height and length as h,l (no spaces):");
	get_user_input();

	int new_l, new_h;
	sscanf(user_input,"%d,%d", &new_h, &new_l);
	if (new_h > MAX_H || new_l > MAX_L) {
		printf("invalid height/length!");
		return;
	}
	m->h = new_h;
	m->l = new_l;

	printf("Input numbers seperated by commas:");
	get_user_input();

	int nums_to_process = new_l * new_h;
	int idx = 0;
	int chars_travelled = 0;
	size_t cur_num = 0;
	char* cur_ptr = user_input;
	while (idx < nums_to_process && sscanf(cur_ptr, "%lld%n", &cur_num, &chars_travelled) == 1) {
		m->buf[idx++] = cur_num;

		cur_ptr += chars_travelled;
		while (*cur_ptr == ' ' || *cur_ptr == ',') cur_ptr++;
	}

	return;
}

void handle_create_matrix() {
	struct matrix* blink = get_last_matrix();

	struct matrix* cur = calloc(1, sizeof(*cur));
	if (!blink) {
		head = cur;
	}
	else {
		blink->flink = cur;
	}

	set_matrix(cur);
}

void handle_edit_matrix() {

	printf("Index of matrix to edit:");
	
	int idx = 0;
	while (!scanf("%d", &idx)) {
		printf("invalid input!");
	}

	struct matrix* cur = get_matrix_at(idx);

	if (!cur) {
		printf("No matrix #%d", idx);
	}
	else {
		set_matrix(cur);
	}
}

void handle_display_matrix() {

	printf("Index of matrix to display:");

	int idx = 0;
	while (!scanf("%d", &idx)) {
		printf("invalid input!");
	}

	struct matrix* cur = get_matrix_at(idx);

	if (!cur) {
		printf("No matrix #%d", idx);
	}
	else {
		print_matrix(cur);
	}
}

void handle_set_display_mode() {
	print_as_hex = print_as_hex == 0 ? 1 : 0;
	printf("printing as %s\n", print_as_hex == 0 ? "decimal" : "hex");
}

int main() {
	setvbuf(stdout, NULL, _IONBF, 0);

	while (1) {
		printf("Select an option:\n1: Create matrix\n2: Edit matrix\n3: Display matrix\n4: Set Display Mode\n5: Exit\n>>");

		get_user_input();

		switch (user_input[0]) {
		case '1':
			handle_create_matrix();
			break;
		case '2':
			handle_edit_matrix();
			break;
		case '3':
			handle_display_matrix();
			break;
		case '4':
			handle_set_display_mode();
			break;
		case '5':
			return 0;
		default:
			printf("Unknown choice\n\n");
			break;
		}
	}
}