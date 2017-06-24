#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *fin = NULL;
FILE *output = NULL;

char *vars;

int asm_cnt = 0, // счётчик для асм-инструкций
    bas_cnt = 0, // тож, ток для басика
    *count_cnt = 0,
    *var_value = 0;

const char command_id[][7] = {"REM", "INPUT", "PRINT", "GOTO", "IF", "LET", "END"}; 

void load_file(const char *filename)
{
    if ((fin = fopen(filename, "r")) == NULL) {
        fprintf(stderr, "Cannot open file: no such file\n");
        exit(EXIT_FAILURE);
    }
    return;
}

int check_var(char *var, char var_tmp)
{
    int is_var = 0;
    for (int i = 0; i < (int)strlen(var); i++) {
        if (var_tmp == var[i]) {
            is_var++;
            break;
        }
    }
    return is_var;
}

void add_var(char *vars, char var_tmp)
{
    vars = strcat(vars, &var_tmp);
}

int get_var_pos(char *vars, char var)
{
    int cnt = 0;
    for (int i = 0; i < (int)strlen(vars); i++, cnt++) {
        if (var == vars[i]) 
            break;
        else if (vars[i] == '\177')
            cnt--;
    }
    return 99 - cnt;
}

char *epx_to_rpn(char *calc)
{
    char *res = (char *) malloc(sizeof(*res) * 1000);
    char stack[50];
    char *tmp = strtok(calc, " ");
    while (tmp != NULL) {
        if (check_var(vars, tmp[0])) {
            strcat(res, &tmp[0]);
            strcat(res, " ");
        }
        else if (tmp[0] == '+' || tmp[0] == '-') {
            int flg = 0;
            for (int i = strlen(stack); i >= 0 && !flg; i--) {
                if (stack[i] == '+' || stack[i] == '-') {
                    char sign = stack[i];
                    stack[i] = tmp[0];
                    strcat(res, &sign);
                    flg++;
                }
            }
            if (!flg)
                strcat(stack, &tmp[0]);
        }
        else if (atoi(tmp)) {
            char new_var = 'A' + strlen(vars);
            add_var(vars, new_var);
            var_value[strlen(vars) - 1] = atoi(tmp);

        }
        tmp = strtok(NULL, " ");
    }
    return res;
}

void iff()
{
    char tmp[255];
    fgets(tmp, 254, fin);
}

void rem()
{
    char tmp[255];
    fgets(tmp, 254, fin);
}

void input()
{
    char var_tmp;
    fscanf(fin, "%c", &var_tmp);
    fscanf(fin, "%c", &var_tmp);
    if (!check_var(vars, var_tmp))
        add_var(vars, var_tmp);
    int pos = get_var_pos(vars, var_tmp); // позиция для переменной в озу
    fprintf(output, "%d READ %d\n", asm_cnt, pos);
    count_cnt[bas_cnt] = asm_cnt;
    asm_cnt++;
}

void print()
{
    char var_tmp;
    fscanf(fin, "%c", &var_tmp);
    fscanf(fin, "%c", &var_tmp);
    if (!check_var(vars, var_tmp)) {
        fprintf(stderr, "Error on line: %d. No such variable.\n", bas_cnt);
        exit(EXIT_FAILURE);
    }
    int pos = get_var_pos(vars, var_tmp);
    fprintf(output, "%d WRITE %d\n", asm_cnt, pos);
    count_cnt[bas_cnt] = asm_cnt;
    asm_cnt++;
}

void got()
{
    char tmp[255];
    fgets(tmp, 254, fin);
}

void let()
{
    char var_tmp;
    fscanf(fin, "%c", &var_tmp);
    fscanf(fin, "%c", &var_tmp);
    if (!check_var(vars, var_tmp))
        add_var(vars, var_tmp);
    fscanf(fin, "%c", &var_tmp);
    fscanf(fin, "%c", &var_tmp);
    char calc[255] = "\0";
    fgets(calc, 254, fin);
    epx_to_rpn(calc);
}

void end()
{
    fprintf(output, "%d HALT 00", asm_cnt);
}

void translating(const char *filename)
{
    int cnt = 0; // счётчик инструкций
    for (int i = 0; ; i++) {
        char tmp[255];
        fgets(tmp, 254, fin);
        if (feof(fin))
            break;
        cnt++;
    }

    count_cnt = (int *) malloc(sizeof(*count_cnt) * cnt); // индекс - номер строки, содержимое - начало счётчика инстр
    vars = (char *) malloc(sizeof(*vars) * 51);
    var_value = (int *) malloc(sizeof(*var_value) * 50);
    for (int k = 0; k < 50; k++)
        var_value[k] = 0;

    rewind(fin);
    output = fopen(filename, "w");
    int i = 0;
    for (i = 0; !feof(fin); i++) {
        int trsh; // муср
        if (!fscanf(fin, "%d", &trsh)) {
            fprintf(stderr, "Error on line: %d. Missing num of line (may be its %d?).\n", bas_cnt, bas_cnt);
        }
        char command[10];
        fscanf(fin, "%s", command);
        if (feof(fin))
            break;
        int j = 0;
        for (j = 0; j < 7; j++) {
            if (!strcmp(command, command_id[j])) {
                break;
            }
        }
        if (j == 0)
            rem();
        else if (j == 1)
            input();
        else if (j == 2)
            print();
        else if (j == 3)
            got();
        else if (j == 4)
            iff();
        else if (j == 5)
            let();
        else if (j == 6)
            end();
        else {
            fprintf(stderr, "Error on line: %d. Wrong command.\n", bas_cnt);
            exit(EXIT_FAILURE);
        }
        bas_cnt++;
    }
    // добавить переменные в конец озу
    // переписать лейблы !n на асмопонятный (n считаем во время выполнения, ! - тип лейбл)
}

int main(int argc, const char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s input.sb output.o\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    load_file(argv[1]);
    translating("tmp.sa");
    return 0;
}
