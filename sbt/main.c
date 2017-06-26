#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void command_decode(const char *command);

FILE *fin = NULL;
FILE *output = NULL;

char *vars;

int asm_cnt = 0, // счётчик для асм-инструкций
    bas_cnt = 1, // тож, ток для басика
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

char add_tmpvar(char *vars, int val)
{
    char new_var = 'z' - tmp_cnt;
    tmp_cnt++;
    add_var(vars, new_var);
    var_value[strlen(vars) - 1] = val;
    return new_var;
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

void add_cmd(const char *cmd, int operand)
{
    if (!strcmp(cmd, "JNEG") || !strcmp(cmd, "JUMP")) 
        fprintf(output, "%d %s !%d\n", asm_cnt, cmd, operand);
    else
        fprintf(output, "%d %s %d\n", asm_cnt, cmd, operand);
    asm_cnt++;
}

void add_accum(char var) 
{
    int pos = get_var_pos(vars, var);
    add_cmd("LOAD", pos);
}

void stack_pull(char *res, char *stack, int i) 
{
    char sign[] = { stack[i], '\0' };
    stack[i] = '\0';
    strcat(res, sign);
}

char *epx_to_rpn(char *exp)
{
    char *res = (char *) calloc(100, sizeof(*res));
    char *stack = (char *) calloc(100, sizeof(*res));
    char *tmp = strtok(exp, " =\n");
    while (tmp != NULL) {

        if (check_var(vars, tmp[0])) 
            strcat(res, tmp);

        else if (tmp[0] == '+' || tmp[0] == '-' || tmp[0] == '*' 
                || tmp[0] == '/' || tmp[0] == '(' || tmp[0] == ')') {

			for (int i = strlen(stack) - 1; i >= 0; i--) {
                if (stack[i] == '(')
                    break;

				if (tmp[0] == '+' || tmp[0] == '-') {
                    if (stack[i] == '*' || stack[i] == '/' ||
                            stack[i] == '+' || stack[i] == '-')
                        stack_pull(res, stack, i);
                }

                else if (tmp[0] == '*' || tmp[0] == '/') {
                    if (stack[i] == '*' || stack[i] == '/') 
                        stack_pull(res, stack, i);
                }

                else if (tmp[0] == ')') {
                    for (int j = strlen(stack) - 1; j >= 0; j--) {
                        if (stack[j] == '(') {
                            stack[j] = '\0';
                            tmp[0] = '\0';
                            break;
                        }
                        stack_pull(res, stack, j);
                    }
                }
            }

            strcat(stack, &tmp[0]);
        }

        else if (atoi(tmp) || tmp[0] == '0') {
            char new_var = add_tmpvar(vars, atoi(tmp));
            char fix_var[] = { new_var, '\0' };
            strcat(res, fix_var);
        }
        tmp = strtok(NULL, " =\n");
    }
    for (int i = strlen(stack) - 1; i >= 0; i--) 
        strcat(res, &stack[i]);
    return res;
}

void calculating(char *rpn, char var)
{
    char stack[100] = "\0";
    int pos = 0, var_pos = 0,
        flg = 0;

    for (int i = 0; rpn[i]; i++) {
        if (pos > 1 && rpn[i] < 'A') {

            add_accum(stack[pos - 2]);
            if (!flg) {
                var_pos = get_var_pos(vars, stack[pos - 1]);
                flg++;
            } else
                var_pos = get_var_pos(vars, var);

            if (rpn[i] == '+') 
                add_cmd("ADD", var_pos);

            else if (rpn[i] == '-') 
                add_cmd("SUB", var_pos);

            else if (rpn[i] == '*')
                add_cmd("MUL", var_pos);

            else if (rpn[i] == '/') 
                add_cmd("DIVIDE", var_pos);

            var_pos = get_var_pos(vars, var);
            add_cmd("STORE", var_pos);
            pos--;

        } else {
            stack[pos] = rpn[i];
            pos++;
        }
    }
}

void iff()
{
    char var_a,
         var_b,
         tmp_val[10], // на случай, если будет число
         trsh,
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
        var_a = add_tmpvar(vars, atoi(tmp_val));

    } else
        fscanf(fin, "%c", &sign);

    fscanf(fin, "%c", &sign);

    fscanf(fin, "%c", &var_b);
    fscanf(fin, "%c", &var_b);
    if (var_b < 'A') {
        char tmp[] = { var_b, '\0' };
        while (var_b != ' ') {
            strcat(tmp_val, tmp);
            fscanf(fin, "%c", &var_b);
            tmp[0] = var_b;
        }
        var_b = add_tmpvar(vars, atoi(tmp_val));

    } else
        fscanf(fin, "%c", &trsh);

    if (sign == '>') {
        int pos = get_var_pos(vars, var_a);
        add_cmd("LOAD", pos);

        pos = get_var_pos(vars, var_b);
        add_cmd("SUB", pos);

        add_cmd("JNEG", bas_cnt + 1);
    }

    else if (sign == '<') {
        int pos = get_var_pos(vars, var_b);
        add_cmd("LOAD", pos);

        pos = get_var_pos(vars, var_a);
        add_cmd("SUB", pos);

        add_cmd("JNEG", bas_cnt + 1);
    }

    else if (sign == '=') {
        int pos = get_var_pos(vars, var_a);
        add_cmd("LOAD", pos);

        pos = get_var_pos(vars, var_b);
        add_cmd("SUB", pos);

        add_cmd("JZ", asm_cnt + 2);

        add_cmd("JUMP", bas_cnt + 1);
    }

    char command[10] = "\0";
    fscanf(fin, "%s", command);
    command_decode(command);
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
    add_cmd("READ", pos);
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
    add_cmd("WRITE", pos);
}

void got()
{
    int goto_pos;
    fscanf(fin, "%d", &goto_pos);
    goto_pos /= 10;
    add_cmd("JUMP", goto_pos);
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
    if (strlen(rpn) != 1) {
        calculating(rpn, var_tmp);
    } else {
        int pos = get_var_pos(vars, rpn[0]);
        add_cmd("LOAD", pos);
        pos = get_var_pos(vars, var_tmp);
        add_cmd("STORE", pos);
    }
}

void end()
{
    add_cmd("HALT", 0);
}

void command_decode(const char *command)
{
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
}

void fix_goto()
{
    rename("./tmp.sa", "./tmp1.sa");
    output = fopen("tmp.sa", "w"); 
    FILE *filer = fopen("tmp1.sa", "r");
    char mass[50] = "\0";
    while (1) {
		fscanf(filer, "%s", mass);
        if (feof(filer))
            break;
		if(mass[0] == '!') {
			for(int i = 0; i < (int)strlen(mass); i++)
				mass[i] = mass[i + 1];
			int temp = atoi(mass);
			sprintf(mass, "%d", count_cnt[temp]);			
		}			
        char trsh;
		fscanf(filer, "%c", &trsh);			
		fprintf(output, "%s%c", mass, trsh);
	}
    fclose(output);
    fclose(filer);
    system("rm tmp1.sa");
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

    count_cnt = (int *) malloc(sizeof(*count_cnt) * (cnt + 1)); // индекс - номер строки, содержимое - начало счётчика инстр
    vars = (char *) malloc(sizeof(*vars) * 50);
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

        command_decode(command);

        bas_cnt++;
    }
    
    for (int i = strlen(vars) - 1; i >= 0; i--) {
        int pos = get_var_pos(vars, vars[i]);
        fprintf(output, "%d %d\n", pos, var_value[i]);
    }

    fclose(output);
    fix_goto();
}

int main(int argc, const char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s input.sb output.o\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    load_file(argv[1]);
    translating("tmp.sa");
    system("./sat tmp.sa tmp");
    rename("./tmp", argv[2]);
    return 0;
}
