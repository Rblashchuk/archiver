#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct node {
    float frequency;
    unsigned char symbol;
    char code[255];
    struct node *left;
    struct node *right;
};

union code{
    unsigned char ch;

    struct binary{
        unsigned b0: 1;
        unsigned b1: 1;
        unsigned b2: 1;
        unsigned b3: 1;
        unsigned b4: 1;
        unsigned b5: 1;
        unsigned b6: 1;
        unsigned b7: 1;
    } bin;
};

struct node *gen_tree(struct node *symbols[], int len)
{
   struct node *temp;
    temp = (struct node*)malloc(sizeof(struct node));

    temp->frequency = symbols[len - 1]->frequency +
                        symbols[len - 2]->frequency;
    temp->code[0] = 0;
    temp->left = symbols[len - 1];
    temp->right = symbols[len - 2];

    if (len == 2) return temp;

    else
    {
        for (int i = 0; i < len; i++) {
            if (temp->frequency > symbols[i]->frequency){
                for (int j = len; j > i; j--){
                    symbols[j] = symbols[j - 1];
                }
                symbols[i] = temp;
                break;
            }
        }
    return gen_tree(symbols, len - 1);
    }
}

void gen_code(struct node *root){
    if(root->left){
        strcpy(root->left->code, root->code);
        strcat(root->left->code, "0");
        gen_code(root->left);
    }

    if(root->right){
        strcpy(root->right->code, root->code);
        strcat(root->right->code, "1");
        gen_code(root->right);
    }
}

int huffman(char *filename){
    FILE *f, *fbin, *fcomp;

    f = fopen(filename, "rb");

    int len = 0;

    if (f == NULL){
        printf("error");
        return 0;
    }
    struct node *dsymb_nodes = (struct node *) malloc(len * (sizeof(struct node))); //symbols
    struct node **dpsymb_i = (struct node **) malloc(len * sizeof (struct node *)); //psum

    int c;  //chh
    int cnt_overall = 0;    //kk
    int cnt_uniq_symb = 0;  //k

    int symb_num[256] = {0};    //kolvo
    struct node symb_nodes[256] = {0};  //simbols
    struct node *psymb[256];    //psim

    while((c = fgetc(f)) != EOF){
        for (int i = 0; i < 256; i++){

            if(symb_nodes[i].symbol == 0){
                symb_nodes[i].symbol = (unsigned char) c;

                symb_num[i]++;
                cnt_overall++;
                cnt_uniq_symb++;
                break;
            }

            if (c == symb_nodes[i].symbol){
                symb_num[i]++;
                cnt_overall++;
                break;
            }
        }
    }

    for(int i = 0; i < cnt_uniq_symb; i++){
        symb_nodes[i].frequency = (float) symb_num[i] / cnt_overall;
        psymb[i] = &symb_nodes[i];
    }

    struct node temp;

    for (int i = 0; i < cnt_uniq_symb; i++){
        for (int j = i + 1; j < cnt_uniq_symb; j++){
            if (symb_nodes[i].frequency < symb_nodes[j].frequency){
                temp = symb_nodes[i];
                symb_nodes[i] = symb_nodes[j];
                symb_nodes[j] = temp;
            }
        }
    }

    float sum_freq = 0;
    for(int i = 0; i < cnt_uniq_symb; i++){
        sum_freq += symb_nodes[i].frequency;
    }

    struct node *root = gen_tree(psymb, cnt_uniq_symb);
    gen_code(root);

    rewind(f);
    fbin = fopen("temp.txt", "wb");
    while((c = fgetc(f))!= EOF){
        for (int i = 0; i < cnt_uniq_symb; i++){
            if(c == symb_nodes[i].symbol){
                fprintf(fbin, "%s", symb_nodes[i].code);
            }
        }
    }

    int bin_size = 0;
    int mod = 0;
    fclose(fbin);
    fbin = fopen("temp.txt", "rb");
    fcomp = fopen("fcomp.txt", "wb");
    while((c = fgetc(fbin)) != EOF) bin_size++;

    mod = bin_size % 8;


    fprintf(fcomp, "%d %d ", cnt_uniq_symb, mod);
    for(int i = 0; i < cnt_uniq_symb; i++){
        fprintf(fcomp, "%c %s ", symb_nodes[i].symbol, symb_nodes[i].code);
        printf("ch: %c, code: %s\n", symb_nodes[i].symbol, symb_nodes[i].code);
    }
    fprintf(fcomp, "\n");
    rewind(fbin);

    int b[8];
    union code code;
    int cnt = 0;
    printf("mod: %d\n", mod);
    for (int i = 0; i < bin_size - mod; i++){
        b[cnt] = fgetc(fbin);
        if (cnt == 7){
            code.bin.b0 = b[0] - '0';
            code.bin.b1 = b[1] - '0';
            code.bin.b2 = b[2] - '0';
            code.bin.b3 = b[3] - '0';
            code.bin.b4 = b[4] - '0';
            code.bin.b5 = b[5] - '0';
            code.bin.b6 = b[6] - '0';
            code.bin.b7 = b[7] - '0';
            fputc(code.ch, fcomp);
            printf("%d %d %d %d %d %d %d %d, %d\n", code.bin.b0, code.bin.b1, code.bin.b2, code.bin.b3, code.bin.b4, code.bin.b5, code.bin.b6, code.bin.b7, code.ch);
            cnt = 0;
        }
        cnt++;
    }



    cnt = 0;
    for (int i = 0; i < 8; i++){
        b[i] = 0;
    }

    for (int i = 0; i < mod; i++){
        b[i] = fgetc(fbin);

        if(cnt == mod - 1){
            code.bin.b0 = b[0] - '0';
            code.bin.b1 = b[1] - '0';
            code.bin.b2 = b[2] - '0';
            code.bin.b3 = b[3] - '0';
            code.bin.b4 = b[4] - '0';
            code.bin.b5 = b[5] - '0';
            code.bin.b6 = b[6] - '0';
            code.bin.b7 = b[7] - '0';
            fputc(code.ch, fcomp);
            printf("MOD: %d %d %d %d %d %d %d %d, %d\n", code.bin.b0, code.bin.b1, code.bin.b2, code.bin.b3, code.bin.b4, code.bin.b5, code.bin.b6, code.bin.b7, code.ch);
        }
        cnt++;
    }
    fcloseall();
    //remove("temp.txt");
    return 0;
}

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

    while (fscanf(archive_file, "%s %d", filename, &size) != 0) {
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
    /*for (int i = 0; i < argc; i++){
        if(!strcmp("--file", argv[i])) archive_name = argv[i + 1];

        if(!strcmp("--create", argv[i])) create(archive_name, argc, argv);

        if(!strcmp("--extract", argv[i])) extract(archive_name);

        if(!strcmp("--list", argv[i])) list(archive_name);
    }*/
    int a = huffman("file2.txt");
    return 0;
}