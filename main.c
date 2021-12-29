#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct node {
    float frequency;
    unsigned char symbol;
    char code[255];
    int code_len;
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

int huffman(char *filename, char *outname){
    FILE *f, *fbin, *fcomp;
    printf("HUFFMAN %s->%s\n", filename, outname);

    f = fopen(filename, "rb");
    int len = 0;

    if (f == NULL){
        printf("error");
        return 0;
    }
    rewind(f);
    int c;
    int cnt_overall = 0;
    int cnt_uniq_symb = 0;

    int symb_num[256] = {0};
    struct node symb_nodes[256] = {0};
    struct node *psymb[256];

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
    fcomp = fopen(outname, "wb");
    while((c = fgetc(fbin)) != EOF) bin_size++;

    mod = bin_size % 8;


    fprintf(fcomp, "%d %d %d ", cnt_uniq_symb, bin_size, mod);
    for(int i = 0; i < cnt_uniq_symb; i++){
        fprintf(fcomp, "%c %s ", symb_nodes[i].symbol, symb_nodes[i].code);
        printf("%c %s\n", symb_nodes[i].symbol, symb_nodes[i].code);
    }
    fprintf(fcomp, "\n");
    rewind(fbin);

    int b[9];
    union code code;
    int cnt = 0;
    printf("mod: %d\n", mod);
    printf("=========\n");
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
            //printf("c: %x\n", code.ch);
            //printf("%d  %d  %d  %d  %d  %d  %d  %d\n", code.bin.b0, code.bin.b1, code.bin.b2, code.bin.b3, code.bin.b4, code.bin.b5, code.bin.b6, code.bin.b7, code.ch);
            //printf("%d %d %d %d %d %d %d %d\n", b[0], b[1], b[2], b[3], b[4], b[5], b[6], b[7]);
            cnt = 0;
            continue;
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
            //printf("MOD: %d%d%d%d%d%d%d%d, %d\n", code.bin.b0, code.bin.b1, code.bin.b2, code.bin.b3, code.bin.b4, code.bin.b5, code.bin.b6, code.bin.b7, code.ch);
        }
        cnt++;
    }
    fcloseall();
    remove("temp.txt");
    return 0;
}

int huffman_decode(char *filename, char *outname){
    FILE *f;
    f = fopen(filename, "rb");
    int mod;
    int bin_size;
    int n_uniq_symb;
    fscanf(f, "%d %d %d", &n_uniq_symb, &bin_size, &mod);
    printf("uniq: %d, mod: %d\n", n_uniq_symb, mod);

    struct node nodes[256];
    char s[500];
    fseek(f, -2, SEEK_CUR);
    int cnt = 0;
    char *code = {0};
    //rewind(f);
    for(int i = 0; i < n_uniq_symb + 1; i++){
        char c = fgetc(f);
        nodes[i].symbol = c;
        c = fgetc(f);
        int code_cnt = 0;
        while((c = fgetc(f)) != ' '){
            nodes[i].code[code_cnt] = c;
            code_cnt++;
        }
        nodes[i].code[code_cnt] = '\0';
        nodes[i].code_len = code_cnt;

    }
    for (int i = 0; i < n_uniq_symb; i++) nodes[i] = nodes[i + 1];

    struct node root;
    root.left = NULL;
    root.right = NULL;

    struct node *curr = (struct node *) malloc(sizeof (struct node));
    curr = &root;
    for (int i = 0; i < n_uniq_symb; i++){
        for (int j = 0; j < nodes[i].code_len; j++){

            if(nodes[i].code[j] == '0') {
                //printf("%c", nodes[i].code[j]);
                if (curr->left == NULL) {
                    struct node *new = (struct node *) malloc(sizeof (struct node));
                    new->right = NULL;
                    new->left = NULL;

                    curr->left = new;
                    curr = new;
                }
                else{
                    curr = curr->left;
                }
                continue;
            }
            if(nodes[i].code[j] == '1') {
                //printf("%c", nodes[i].code[j]);
                if (curr->right == NULL) {
                    struct node *new = (struct node *) malloc(sizeof (struct node));
                    new->right = NULL;
                    new->left = NULL;
                    curr->right = new;
                    curr = new;
                }
                else{
                    curr = curr->right;
                }
            }
            continue;
        }
        //printf("\n");
        curr->symbol = nodes[i].symbol;
        curr = &root;
    }

    for (int i = 0; i < n_uniq_symb; i++){
        printf("%c %s %d\n", nodes[i].symbol, nodes[i].code, nodes[i].code_len);
    }
    printf("===========\n");
    char c;
    union code code_bin;

    FILE *f2, *fout;
    c = fgetc(f);

    f2 = fopen("temp2.txt", "wb");

    cnt = 0;
    for (int i = 0; i < bin_size; i+=8){
        code_bin.ch = fgetc(f);
        //printf("c2: %x\n", code_bin.ch);
        if (cnt < bin_size){
            fprintf(f2, "%d", code_bin.bin.b0);
            cnt++;
        }
        if (cnt < bin_size){
            fprintf(f2, "%d", code_bin.bin.b1);
            cnt++;
        }
        if (cnt < bin_size){
            fprintf(f2, "%d", code_bin.bin.b2);
            cnt++;
        }
        if (cnt < bin_size){
            fprintf(f2, "%d", code_bin.bin.b3);
            cnt++;
        }
        if (cnt < bin_size){
            fprintf(f2, "%d", code_bin.bin.b4);
            cnt++;
        }
        if (cnt < bin_size){
            fprintf(f2, "%d", code_bin.bin.b5);
            cnt++;
        }
        if (cnt < bin_size){
            fprintf(f2, "%d", code_bin.bin.b6);
            cnt++;
        }
        if (cnt < bin_size){
            fprintf(f2, "%d", code_bin.bin.b7);
            cnt++;
        }
        //printf("%d%d%d%d%d%d%d%d\n", code_bin.bin.b0, code_bin.bin.b1, code_bin.bin.b2, code_bin.bin.b3, code_bin.bin.b4, code_bin.bin.b5, code_bin.bin.b6, code_bin.bin.b7);
    }

    fclose(f2);
    f2 = fopen("temp2.txt", "rb");
    fout = fopen(outname, "wb");
    curr = &root;
    while ((c = fgetc(f2)) != EOF){
        if(c == '0'){
            if(curr->left != NULL){
                curr = curr->left;
            }
            else {
                fprintf(fout, "%c", curr->symbol);
                curr = &root;
                fseek(f2, -1, SEEK_CUR);
            }
        }
        if(c == '1'){
            if(curr->right != NULL){
                curr = curr->right;
            }
            else {
                fprintf(fout, "%c", curr->symbol);
                curr = &root;
                fseek(f2, -1, SEEK_CUR);
            }
        }
    }
    fprintf(fout, "%c", curr->symbol);
    fcloseall();
    remove("temp2.txt");


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

    while (fscanf(archive_file, "[%s %d]", filename, &size) != 0) {
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
    fcloseall();
    return 0;
}

int list(char* archive_name){
    FILE* archive_file = fopen(archive_name, "rb");
    char filename[128] = {0};
    int  filesize;
    while (fscanf(archive_file, "[%s %d]", filename, &filesize) != 0) {
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
        fprintf(archive_file, "[%s %d]", argv[i + 4], size);
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
    fcloseall();
    return 0;
}

int main(int argc, char* argv[]){
    char* archive_name;
    for (int i = 0; i < argc; i++){
        if(!strcmp("--file", argv[i])) archive_name = argv[i + 1];

        if(!strcmp("--create", argv[i])) {
            create("arch.txt", argc, argv);
            huffman("arch.txt", archive_name);
            //remove("arch.txt");
        }

        if(!strcmp("--extract", argv[i])){
            huffman_decode(archive_name, "arch.txt");
            extract("arch.txt");
            //remove("arch.txt");
        }

        if(!strcmp("--list", argv[i])) list(archive_name);
    }
    //int a = huffman("file2.txt", "fcomp.txt");
    //a = huffman_decode("fcomp.txt", "out.txt");
    return 0;
}