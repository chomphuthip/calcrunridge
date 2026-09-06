#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

int main(int argc, char** argv) {
    char buf[0x10];

    FILE* f = fopen(argv[1], "rb");
    struct stat sb;
    stat(argv[1], &sb);

    printf("Uh let me run the numbers...\n");
    fgets(buf, sb.st_size, f);
    int a = atoi(buf);
    int b = atoi(buf[5]);
    printf("numbers came back! here they are: %x\n", a * b + 0xDEADBEEF + 0xC0DE);
}