#include <stdio.h>
#include <string.h>

int extract(char* archive_name){
    FILE* archive_file = fopen(archive_name, "rb");
    int cnt_archive = 0;
    int cnt_header = 0;

    int chr;
    while ((chr = getc(archive_file)) != '\n') {
        cnt_archive++;
    }
    cnt_archive++;

    fseek(archive_file, 0, SEEK_SET);
    char filename[128];
    int size;
    FILE *file;

    while (fscanf(archive_file, "[%s : %d]", filename, &size) != 0) {
        file = fopen(filename, "wb");
        cnt_header = ftell(archive_file);
        fseek(archive_file, cnt_archive, SEEK_SET);
        cnt_archive += size;
        while (size-- > 0)
            putc((chr = getc(archive_file)), file);

        fseek(archive_file, cnt_header, SEEK_SET);
        fclose(file);
        printf("%s extracted\n", filename);
    }
    return 0;
}

int list(char* archive_name){
    FILE* archive_file = fopen(archive_name, "rb");
    char filename[128] = {0};
    int  filesize;
    while (fscanf(archive_file, "[%s : %d]", filename, &filesize) != 0) {
        printf("%s\n", filename);
    }
    fclose(archive_file);
    return 0;
}

int create(char* archive_name, int argc, char* argv[]){
    FILE* archive_file = fopen(archive_name, "wb");
    FILE* file;
    int size;
    int error_cnt = 0;

    for (int i = 0; i < argc - 4; i++) {
        file = fopen(argv[i + 4], "rb");
        if (file == NULL) continue;
        fseek(file, 0, SEEK_END);
        size = ftell(file);
        fclose(file);
        fprintf(archive_file, "[%s : %d]", argv[i + 4], size);
        fclose(file);
    }
    fprintf(archive_file, "\n");

    char chr;
    for (int i = 4; i < argc; i++){
        file = fopen(argv[i], "rb");
        if (file == NULL){
            printf("Error\n");
            error_cnt++;
            continue;
        }

        while ((chr = getc(file)) != EOF)
            putc(chr, archive_file);
        fclose(file);
        printf("%s was added to %s\n", argv[i], archive_name);
    }
    printf("\nSucsessfully archived: %d out of %d files.\n", argc - 4 - error_cnt, argc - 4);
    return 0;
}

int main(int argc, char* argv[]){
    char* archive_name;
    for (int i = 0; i < argc; i++){
        if(!strcmp("--file", argv[i])) archive_name = argv[i + 1];

        if(!strcmp("--create", argv[i])) create(archive_name, argc, argv);

        if(!strcmp("--extract", argv[i])) extract(archive_name);

        if(!strcmp("--list", argv[i])) list(archive_name);
    }
    return 0;
}