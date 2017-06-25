#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *fin = NULL;
FILE *output = NULL;

char *vars;

int asm_cnt = 0, // счётчик для асм-инструкций
    bas_cnt = 0, // тож, ток для басика
    tmp_cnt = 0, // счётчик временных переменных (константы)
    *count_cnt = 0, // массив позиций (индекс - позиция в басике, значение - позиция в асме)
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
    char tmp[] = { var_tmp, '\0' };
    vars = strcat(vars, tmp);
}

int get_var_pos(char *vars, char var)
{
    int cnt = 0;
    for (int i = 0; i < (int)strlen(vars); i++, cnt++) {
        if (var == vars[i]) 
            break;
    }
    return 99 - cnt;
}

void add_accum(char var) 
{
    int pos = get_var_pos(vars, var);
    fprintf(output, "%d LOAD %d\n", asm_cnt, pos);
    asm_cnt++;
}

char *epx_to_rpn(char *exp)
{
    char *res = (char *) calloc(100, sizeof(*res));
    char stack[50] = "\0";
    char *tmp = strtok(exp, " =\n");
    while (tmp != NULL) {

        if (check_var(vars, tmp[0])) 
            strcat(res, tmp);

        else if (tmp[0] == '+' || tmp[0] == '-' || tmp[0] == '*' 
                || tmp[0] == '/' || tmp[0] == '(' || tmp[0] == ')') {

            if (tmp[0] == '+' || tmp[0] == '-') {
                for (int i = strlen(stack) - 1; i >= 0; i--) {
                    if (stack[i] == '(')
                        break;
                    if (stack[i] == '*' || stack[i] == '/' ||
                            stack[i] == '+' || stack[i] == '-') {
                        char sign[] = { stack[i], '\0' };
                        stack[i] = '\0';
                        strcat(res, sign);
                    }
                }
            } 

            else if (tmp[0] == '*' || tmp[0] == '/') {
                for (int i = strlen(stack) - 1; i >= 0; i--) {
                    if (stack[i] == '(')
                        break;
                    if (stack[i] == '*' || stack[i] == '/') {
                        char sign[] = { stack[i], '\0' };
                        stack[i] = '\0';
                        strcat(res, sign);
                    }
                }
            }

            else if (tmp[0] == ')') {
                for (int i = strlen(stack) - 1; i >= 0; i--) {
                    if (stack[i] == '(') {
                        stack[i] = '\0';
                        tmp[0] = '\0';
                        break;
                    }
                    char sign[] = { stack[i], '\0' };
                    stack[i] = '\0';
                    strcat(res, sign);
                }
            }

            strcat(stack, &tmp[0]);
        }

        else if (atoi(tmp)) {
            char new_var = 'z' - tmp_cnt;
            add_var(vars, new_var);
            var_value[strlen(vars) - 1] = atoi(tmp); // -2, тк добавляется какойт шлак /177
            char fix_var[] = { new_var, '\0' };
            strcat(res, fix_var);
        }
        tmp = strtok(NULL, " =\n");
    }
    for (int i = strlen(stack) - 1; i >= 0; i--) 
        strcat(res, &stack[i]);
    printf("%s\n", res);
    return res;
}

void calculating(char *rpn)
{
    char stack[100] = "\0";
    int pos = 0,
        flg = 0; // добавили ли первое значение в акум
    for (int i = 0; rpn[i]; i++) {
        if (rpn[i] == '+') {
            if (!flg) {
                add_accum(stack[pos - 2]);
                flg++;
                for (int i = pos - 2; i < (int)strlen(stack); i++)
                    stack[i] = stack[i + 1];
            }
            int var_pos = get_var_pos(vars, stack[pos - 2]);
            fprintf(output, "%d ADD %d\n", asm_cnt, var_pos);
            asm_cnt++;
        }

        else if (rpn[i] == '-') {
            if (!flg) {
                add_accum(stack[pos - 2]);
                flg++;
                for (int i = pos - 2; i < (int)strlen(stack); i++)
                    stack[i] = stack[i + 1];
            }
            int var_pos = get_var_pos(vars, stack[pos - 2]);
            fprintf(output, "%d SUB %d\n", asm_cnt, var_pos);
            asm_cnt++;
        }

        else if (rpn[i] == '*') {
            if (!flg) {
                add_accum(stack[pos - 2]);
                flg++;
                for (int i = pos - 2; i < (int)strlen(stack); i++)
                    stack[i] = stack[i + 1];
            }
            int var_pos = get_var_pos(vars, stack[pos - 2]);
            fprintf(output, "%d MUL %d\n", asm_cnt, var_pos);
            asm_cnt++;
        }

        else if (rpn[i] == '/') {
            if (!flg) {
                add_accum(stack[pos - 2]);
                flg++;
                for (int i = pos - 2; i < (int)strlen(stack); i++)
                    stack[i] = stack[i + 1];
            }
            int var_pos = get_var_pos(vars, stack[pos - 2]);
            fprintf(output, "%d DIVIDE %d\n", asm_cnt, var_pos);
            asm_cnt++;
        }

        else {
            char fix_var[] = { rpn[i], '\0' };
            strcat(stack, fix_var);
            pos++;
        }
    }
}

void iff()
{
    char var_a,
         var_b,
         tmp_val[10], // на случай, если будет число
         sign;
    fscanf(fin, "%c", &var_a);
    fscanf(fin, "%c", &var_a);
    if (var_a < 'A') {
        char tmp[] = { var_a, '\0' };
        while (var_a != ' ') {
            strcat(tmp_val, tmp);
            fscanf(fin, "%c", &var_a);
            tmp[0] = var_a;
        }
        char new_var = 'z' - tmp_cnt;
        add_var(vars, new_var);
        var_value[strlen(vars) - 1] = atoi(tmp_val);
        var_a = new_var;
    } else
        fscanf(fin, "%c", &sign);
    fscanf(fin, "%c", &sign);
    fscanf(fin, "%c", &var_b);
    fscanf(fin, "%c", &var_b);
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
    char exp[255] = "\0";
    fgets(exp, 254, fin);
    char *rpn = epx_to_rpn(exp);
    calculating(rpn);
    int pos = get_var_pos(vars, var_tmp);
    fprintf(output, "%d STORE %d\n", asm_cnt, pos);
    asm_cnt++;
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

        count_cnt[bas_cnt] = asm_cnt;

        int j = 0;

        for (j = 0; j < 7; j++) 
            if (!strcmp(command, command_id[j])) 
                break;

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
