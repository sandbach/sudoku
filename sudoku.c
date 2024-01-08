#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int cell_to_index(int row, int col) { return (row * 9 + col); }
void neighbours(int i, int ns[27]) {
    int row = i / 9;
    int col = i % 9;
    int t = 0;
    int c = 0;
    c = row * 9;
    while (t < 9) {
        ns[t++] = c;
        c += 1;
    }
    c = col;
    while (t < 18) {
        ns[t++] = c;
        c += 9;
    }
    int rstart = (row / 3) * 3;
    int cstart = (col / 3) * 3;
    for (int j = rstart; j < rstart + 3; j++) {
        for (int k = cstart; k < cstart + 3; k++) {
            ns[t++] = cell_to_index(j, k);
        }
    }
}
int valid_attempt(int sudoku[81],
                  int a, int c) {
    int ns[27];
    neighbours(c, ns);
    for (int i = 0; i < 27; i++) {
        if (sudoku[ns[i]] == a) {
            return 0;
        }
    }
    return 1;
}
int first_empty(int sudoku[81]) {
    for (int i = 0; i < 81; i++) {
        if (sudoku[i] == 0) {
            return i;
        }
    }
    return -1;
}
void copy_array(int *a, int *b, int n) {
    for (int i = 0; i < n; i++) {
        b[i] = a[i];
    }
}
void solve(int sudoku[81], int solved[81]) {
    static int is_solved = 0;
    if (is_solved) {
        return;
    }
    int c = first_empty(sudoku);
    if (c == -1) {
        is_solved = 1;
        copy_array(sudoku, solved, 81);
    }
    for (int i = 1; i <= 9; i++) {
        if (valid_attempt(sudoku, i, c)) {
            sudoku[c] = i;
            solve(sudoku, solved);
            sudoku[c] = 0;
        }
    }
}
void read_row(int sudoku[81],
              int row, char *row_string) {
    int index = cell_to_index(row, 0);
    for (int col = 0; col < 9; col++, index++) {
        if (*row_string == '\0') {
            sudoku[index] = 0;
        } else {
            char c = *row_string++;
            if (c >= '1' && c <= '9') {
                sudoku[index] = c - '0';
            } else {
                sudoku[index] = 0;
            }
        }
    }
}
void read_sudoku(int sudoku[81],
                 char *filename) {
    FILE *fp;
    char s[100];
    int row = 0;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("%s: file not found.\n", filename);
        exit(1);
    } else {
        while (fgets(s, sizeof(s), fp) != NULL && row < 9) {
            read_row(sudoku, row, s);
            row++;
        }
        fclose(fp);
    }
}
void print_sudoku(int sudoku[81], int tex) {
    int c;
    for (int i = 0; i < 81; i++) {
        c = sudoku[i];
        printf("%s", tex ? "|" : "");
        if (c == 0) {
            printf(" ");
        } else {
            printf("%d", sudoku[i]);
        }
        printf("%s", tex ? "" : " ");
        if (i % 9 == 8) {
            printf("%s", tex ? "|." : "");
            printf("\n");
        }
    }
}
void name_part(char *filename, char *result) {
    char *rs = result;
    char *i;
    for (i = filename; *i != '\0'; i++) {
        if (*i == '/') {
            result = rs;
        } else {
            *result++ = *i;
        }
    }
    *result = '\0';
}
int tex_print(char **args) {
    for (; *args != NULL; args++) {
        if (strcmp(*args, "-tex") == 0) {
            return 1;
        }
    }
    return 0;
}
void fib_weights(uint64_t w[81]) {
    uint64_t a = 0;
    uint64_t b = 1;
    for (int i = 80; i >= 0; i--) {
        w[i] = a + b;
        a = b;
        b = w[i];
    }
}
uint64_t difficulty(int sudoku[81],
                    uint64_t weights[81]) {
    uint64_t diff = 0;
    for (int i = 0; i < 81; i++) {
        if (sudoku[i] == 0) {
            diff += weights[i];
        }
    }
    return diff;
}
int rotate_cell(int cell) {
    int row = cell / 9;
    int col = cell % 9;
    int new_row = col;
    int new_col = 8 - row;
    return cell_to_index(new_row, new_col);
}
void rotate_times(int sudoku[81],
                  int times, int fixed[81]) {
    times = times % 4;
    for (int i = 0; i < 81; i++) {
        int cell = i;
        for (int j = times; j > 0; j--) {
            cell = rotate_cell(cell);
        }
        fixed[cell] = sudoku[i];
    }
}
int best_rotation(int sudoku[81],
                  int rotated[81]) {
    uint64_t weights[81];
    fib_weights(weights);
    int best = 0;
    uint64_t best_score = difficulty(sudoku, weights);
    copy_array(sudoku, rotated, 81);
    for (int i = 1; i < 4; i++) {
        int temp[81];
        rotate_times(sudoku, i, temp);
        uint64_t diff = difficulty(temp, weights);
        if (diff < best_score) {
            best_score = diff;
            best = i;
            copy_array(temp, rotated, 81);
        }
    }
    return best;
}
int main(int argc, char **argv) {
    int sudoku[81];
    int solved[81], tex;
    char exec_name[100]; /* the name of the executable can only be 100 characters long */
    name_part(argv[0], exec_name);
    read_sudoku(sudoku, argv[1]);
    tex = tex_print(&argv[2]);
    if (strcmp(exec_name, "solve") == 0) {
        int rotated[81];
        int rotations = best_rotation(sudoku, rotated);
        copy_array(rotated, sudoku, 81);
        solve(sudoku, solved);
        rotate_times(solved, 4 - rotations, sudoku);
        print_sudoku(sudoku, tex);
    } else {
        print_sudoku(sudoku, tex);
    }
}
