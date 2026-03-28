#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/select.h>

const int BACK = 0;
const int SPEED[] = { 0, 12, 9, 6, 3, 1 };
const int NUMBER[] = { 0, 2, 3, 5, 7, 11, 15 };
const int COL[] = { 11, 15, 12, 12, 13, 13, 14 };

const int WIDTH = 15;
const int HEIGHT = 25;

int rank = 1;
int level = 0;  // 0表示既不是向右也不是向左，1表示向左，2表示向右
int kind;
int pri_kind;   // 7大类，颜色标记，调用COL[col_kind];
int revolve = 0; // 旋转状态标记

struct {
    int x;
    int y;
} current, before;

struct {
    int x;
    int y;
    int color;
    bool having;
} board[WIDTH][HEIGHT];

struct {
    int vary_x[4];
    int vary_y[4];
} vary[] = {
    {{0, 2, 4, 6},{0, 0, 0, 0}},{{0, 0, 0, 0},{0,-1,-2,-3}},
    {{0, 2, 2, 0},{0, 0,-1,-1}},{{0,-2,-2,-4},{0 ,0,-1,-1}},
    {{0, 0, 2, 2},{0,-1,-1,-2}},{{0, 2, 2, 4},{0, 0,-1,-1}},
    {{0, 0,-2,-2},{0,-1,-1,-2}},{{0, 0, 2, 4},{0,-1, 0, 0}},
    {{0, 0, 0, 2},{0,-1,-2,-2}},{{0, 0,-2,-4},{0,-1,-1,-1}},
    {{0, 2, 2, 2},{0, 0,-1,-2}},{{0, 2, 4, 4},{0, 0, 0,-1}},
    {{0,-2,-2,-2},{0, 0,-1,-2}},{{0, 0, 2, 4},{0,-1,-1,-1}},
    {{0, 0, 0,-2},{0,-1,-2,-2}},{{0 ,2, 4, 2},{0, 0, 0,-1}},
    {{0, 0, 2, 0},{0,-1,-1,-2}},{{0,-2, 0, 2},{0,-1,-1,-1}},
    {{0, 0,-2, 0},{0,-1,-1,-2}},
};

struct {
    int sum;
    int connection_x[5];
    int connection_y[5];
} connection[] = {
    {2,{-2,2},{0,1}},
    {1,{0},{0}},
    {2,{2,-2},{0,0}},
    {2,{-2,2},{0,0}},
    {4,{-2,0,4,-2},{0,0,-1,1}},
    {4,{-4,2,-2,4},{0,0,-1,1}},
    {4,{-2,2,0,0},{0,0,0,0}},
};

static struct termios orig_termios;
static bool raw_mode = false;

void disableRawMode() {
    if (!raw_mode) return;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    printf("\033[0m\033[?25h");
    fflush(stdout);
    raw_mode = false;
}

void enableRawMode() {
    if (raw_mode) return;
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) return;
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    raw_mode = true;
    atexit(disableRawMode);
    printf("\033[?25l");
    fflush(stdout);
}

void gotoxy(int x, int y) {
    printf("\033[%d;%dH", y, x);
}

void color(int code) {
    if (code == BACK) {
        printf("\033[0m");
        return;
    }
    switch (code) {
        case 43: printf("\033[1;33m"); break;
        case 46: printf("\033[1;36m"); break;
        case 44: printf("\033[1;34m"); break;
        case 39: printf("\033[1;35m"); break;
        case 11: printf("\033[1;36m"); break;
        case 15: printf("\033[1;37m"); break;
        case 12: printf("\033[1;34m"); break;
        case 13: printf("\033[1;35m"); break;
        case 14: printf("\033[1;33m"); break;
        default: printf("\033[0m"); break;
    }
}

void clearScreen() {
    printf("\033[2J");
}

bool kbhit() {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    struct timeval tv = {0, 0};
    return select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv) > 0;
}

int readKey() {
    char c;
    if (read(STDIN_FILENO, &c, 1) != 1) return -1;
    if (c == '\033') {
        char seq[2];
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\033';
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\033';
        if (seq[0] == '[') {
            if (seq[1] == 'A') return 0x48;
            if (seq[1] == 'B') return 0x50;
            if (seq[1] == 'C') return 0x4d;
            if (seq[1] == 'D') return 0x4b;
        }
        return '\033';
    }
    return c;
}

void sleep_ms(int ms) {
    usleep(ms * 1000);
}

void PrintGarphy() {
    int i, j;
    clearScreen();
    gotoxy(1, 1);
    color(BACK);
    for (i = 0; i < WIDTH; ++i) {
        printf("[]");
    }
    printf("[]" "[]" "[]" "[]" "[]" "[]" "[]" "[]" "[]" "[]" "[]\n");

    for (i = 0; i < HEIGHT + 1; i++) {
        gotoxy(1, 2 + i);
        printf("[]");
        gotoxy(WIDTH * 2 + 7, 2 + i);
        if (i == 8 || i == 16)
            printf("[]" "[]" "[]" "[]" "[]" "[]" "[]" "[]" "[]" "[]");
        else
            printf("[]");
        gotoxy(WIDTH * 2 + 25, 2 + i);
        printf("[]");
    }

    gotoxy(1, 3 + HEIGHT);
    for (i = 0; i < WIDTH; ++i) {
        printf("[]");
    }
    printf("[]" "[]" "[]" "[]" "[]" "[]" "[]" "[]" "[]" "[]" "[]\n");

    int t = 0;
    FILE* fp = fopen("score.txt", "r");
    if (fp) {
        fscanf(fp, "%d", &t);
        fclose(fp);
    }

    color(43);
    gotoxy(22 + WIDTH, 12);
    printf("BEST      %d", t);
    color(46);
    gotoxy(22 + WIDTH, 14);
    printf("SCORE");
    color(44);
    gotoxy(22 + WIDTH, 16);
    printf("MEDAL");
    color(39);
    gotoxy(22 + WIDTH, 20);
    printf("↑ ↓ ←  →");
    gotoxy(22 + WIDTH, 22);
    printf("[SPACE] pause");
    gotoxy(22 + WIDTH, 24);
    printf("[ESC] exit");
    gotoxy(22 + WIDTH, 3);
    printf("NEXT");

    color(BACK);
    for (i = 0; i < WIDTH; ++i) {
        for (j = 0; j < HEIGHT; ++j) {
            board[i][j].x = 4 + 2 * i;
            board[i][j].y = 3 + j;
            board[i][j].having = false;
            gotoxy(board[i][j].x, board[i][j].y);
            printf("  ");
        }
    }
    fflush(stdout);
}

void GameOver() {
    color(46);
    gotoxy(47, 14);
    printf("0    ");

    gotoxy(42, 16);
    while (rank--)
        printf("  ");

    color(BACK);
    for (int i = HEIGHT - 1; i >= 0; i--) {
        for (int j = 0; j < WIDTH; j++) {
            gotoxy(32 - j * 2, 3 + i);
            printf("██");
        }
        sleep_ms(1);
    }
    color(BACK);
    for (int j = 0; j < HEIGHT; j++) {
        for (int i = 0; i < WIDTH; i++) {
            board[i][j].having = false;
            gotoxy(4 + 2 * i, j + 3);
            printf("  ");
        }
        sleep_ms(1);
    }
    fflush(stdout);
}

void GetDepth(int* pdepth) {
    (*pdepth) = 24;
    for (int j = 0; j < 4; j++) {
        int sum = 0;
        int n = (current.x + vary[kind].vary_x[j] - 4) / 2;
        int m = current.y + vary[kind].vary_y[j] - 3;
        if (m < 0)
            m = 0;
        for (++m; m < HEIGHT; m++) {
            if (n >= 0 && n < WIDTH && m >= 0 && m < HEIGHT && board[n][m].having == false)
                sum++;
            else
                break;
        }
        if (sum < (*pdepth))
            (*pdepth) = sum;
    }
}

void CheckFull(int* pscore) {
    int i, j;
    int sum = 0;
    int s[] = {0, 1, 3, 6, 10};
    for (i = 0; i < HEIGHT; i++) {
        bool mark = true;
        for (j = 0; j < WIDTH; j++) {
            if (!board[j][i].having) {
                mark = false;
                board[j][i].color = BACK;
            }
        }
        if (mark) {
            sum++;
            for (int t = i; t > 0; t--) {
                gotoxy(4, 3 + t);
                for (j = 0; j < WIDTH; j++) {
                    if (board[j][t - 1].having) {
                        color(board[j][t - 1].color);
                        printf("██");
                        board[j][t].color = board[j][t - 1].color;
                        board[j][t].having = true;
                        board[j][t - 1].having = false;
                    } else {
                        color(BACK);
                        printf("  ");
                        board[j][t].color = BACK;
                        board[j][t].having = false;
                        board[j][t - 1].having = false;
                    }
                }
            }
        }
    }
    (*pscore) += s[sum];
    color(46);
    gotoxy(47, 14);
    printf("%d", *pscore);
    if (*pscore >= 140)
        rank = 5;
    else if (*pscore >= 90)
        rank = 4;
    else if (*pscore >= 50)
        rank = 3;
    else if (*pscore >= 20)
        rank = 2;
    else if (*pscore >= 1)
        rank = 1;
    color(44);
    gotoxy(43, 16);
    for (i = 0; i < rank && *pscore; i++)
        printf("★");

    int t = 0;
    FILE* fp = fopen("score.txt", "r");
    if (fp) {
        fscanf(fp, "%d", &t);
        fclose(fp);
    }
    if (*pscore > t) {
        fp = fopen("score.txt", "w");
        if (fp) {
            fprintf(fp, "%d", *pscore);
            fclose(fp);
        }
        color(43);
        gotoxy(47, 12);
        printf("%d", *pscore);
    }
    fflush(stdout);
}

void PrintNext(int pkind) {
    int i;
    int x = 43, y = 6;
    color(34);
    for (i = 0; i < 4; i++) {
        gotoxy(x - 3, y + i - 2);
        printf("        ");
    }
    pri_kind = pkind;
    if (pkind == 6)
        pkind = 15, x--;
    else if (pkind == 5)
        pkind = 11, x--;
    else if (pkind == 4)
        pkind = 7, x--;
    else if (pkind == 3)
        pkind = 5, x -= 2;
    else if (pkind == 2)
        pkind = 3, x += 2;
    else if (pkind == 1)
        pkind = 2, x--;
    else
        pkind = 0, x -= 3;
    color(COL[pri_kind]);
    for (i = 0; i < 4; i++) {
        gotoxy(x + vary[pkind].vary_x[i], y + vary[pkind].vary_y[i]);
        printf("██");
    }
    fflush(stdout);
}

void CheckBoundary() {
    int i, n, m;
    for (i = 0; i < 4; i++) {
        m = current.x + vary[kind].vary_x[i];
        if (m < 4 || m > 32) {
            current.x += (m < 4 ? 2 : -2);
            i = 0;
        }
    }
    for (i = 0; i < 4; i++) {
        m = current.x + vary[kind].vary_x[i];
        n = current.y + vary[kind].vary_y[i];
        m = (m - 4) / 2;
        n -= 3;
        if (n < 0 || n > 24 || board[m][n].having) {
            if (level == 1)
                current.x += 2;
            else if (level == 2)
                current.x -= 2;
            level = 0;
            break;
        }
    }
}

void LevelMove() {
    CheckBoundary();
    if (level == 0) return;
    int i;
    color(BACK);
    for (i = 0; i < 4; i++) {
        if (current.y + vary[kind].vary_y[i] > 2) {
            gotoxy(before.x + vary[kind].vary_x[i], before.y + vary[kind].vary_y[i]);
            printf("  ");
        }
    }
    color(COL[pri_kind]);
    for (i = 0; i < 4; i++) {
        if (current.y + vary[kind].vary_y[i] > 2) {
            gotoxy(current.x + vary[kind].vary_x[i], current.y + vary[kind].vary_y[i]);
            printf("██");
        }
    }
    before.x = current.x;
}

void Revolve(int** prev_count) {
    int i;
    before.x = current.x;
    before.y = current.y;
    (**prev_count)++;
    (**prev_count) %= connection[pri_kind].sum;
    current.x = current.x + connection[pri_kind].connection_x[**prev_count];
    current.y = current.y + connection[pri_kind].connection_y[**prev_count];
    CheckBoundary();
    for (i = 0; i < 4; i++) {
        int m = (current.x - 4 + vary[NUMBER[pri_kind] + **prev_count].vary_x[i]) / 2;
        int n = current.y + vary[NUMBER[pri_kind] + **prev_count].vary_y[i] - 3;
        if (m < 0 || m >= WIDTH || n < 0 || n >= HEIGHT || board[m][n].having) {
            current.x = current.x - connection[pri_kind].connection_x[**prev_count];
            current.y = current.y - connection[pri_kind].connection_y[**prev_count];
            revolve = 0;
            (**prev_count)--;
            before.x = current.x;
            before.y = current.y;
            return;
        }
    }
    color(BACK);
    for (i = 0; i < 4; i++) {
        if (before.y + vary[kind].vary_y[i] > 2) {
            gotoxy(before.x + vary[kind].vary_x[i], before.y + vary[kind].vary_y[i]);
            printf("  ");
        }
    }
    kind = NUMBER[pri_kind] + **prev_count;
    color(COL[pri_kind]);
    for (i = 0; i < 4; i++) {
        if (current.y + vary[kind].vary_y[i] > 2) {
            gotoxy(current.x + vary[kind].vary_x[i], current.y + vary[kind].vary_y[i]);
            printf("██");
        }
    }
    sleep_ms(SPEED[rank]);
    before.x = current.x;
    before.y = current.y;
    fflush(stdout);
}

void ChoiceDirection(int* prev_count) {
    int t = 50 / rank;
    while (t--) {
        if (kbhit()) {
            int c = readKey();
            switch (c) {
                case 0x48: // up
                    revolve = 1;
                    Revolve(&prev_count);
                    break;
                case 0x50: // down
                    level = 0;
                    return;
                case 0x4b: // left
                    before.x = current.x;
                    current.x -= 2;
                    level = 1;
                    LevelMove();
                    break;
                case 0x4d: // right
                    before.x = current.x;
                    current.x += 2;
                    level = 2;
                    LevelMove();
                    break;
                case ' ':
                    while (!kbhit()) sleep_ms(30);
                    readKey();
                    break;
                case 27:
                    disableRawMode();
                    exit(0);
                default: break;
            }
        }
        sleep_ms(SPEED[rank]);
    }
}

void StraightFall(int depth) {
    int i;
    int rev_count = 0;
    before.x = current.x;
    before.y = current.y;
    while (GetDepth(&depth), depth--) {
        ChoiceDirection(&rev_count);
        GetDepth(&depth);
        if (depth == 0) {
            revolve = 0;
            before.x = current.x;
            before.y = current.y;
            break;
        }
        color(BACK);
        for (i = 0; i < 4; i++) {
            if (before.y + vary[kind].vary_y[i] > 2) {
                gotoxy(before.x + vary[kind].vary_x[i], before.y + vary[kind].vary_y[i]);
                printf("  ");
            }
        }
        current.y++;
        color(COL[pri_kind]);
        for (i = 0; i < 4; i++) {
            if (current.y + vary[kind].vary_y[i] > 2) {
                gotoxy(current.x + vary[kind].vary_x[i], current.y + vary[kind].vary_y[i]);
                printf("██");
            }
        }
        revolve = 0;
        before.x = current.x;
        before.y = current.y;
        fflush(stdout);
    }
    ChoiceDirection(&rev_count);
    if (level || revolve) {
        level = 0;
        revolve = 0;
        ChoiceDirection(&rev_count);
        StraightFall(depth);
    }
}

int main() {
    enableRawMode();
    clearScreen();
    gotoxy(1, 1);

    srand((unsigned long)time(NULL));

    color(46);
    gotoxy(47, 14);
    printf("0");

    int score = 0;
    FILE* fp = fopen("score.txt", "r");
    if (fp == NULL) {
        fp = fopen("score.txt", "w");
        if (fp) {
            fprintf(fp, "%d", score);
            fclose(fp);
        }
    }
    PrintGarphy();
    kind = rand() % 7;
    int depth;
    while (true) {
        int pkind = rand() % 7;
        PrintNext(pkind);
        current.x = 18;
        current.y = 2;
        GetDepth(&depth);
        if (depth == 0) {
            GameOver();
            score = 0;
            rank = 1;
        }
        pri_kind = kind;
        if (kind == 6)
            kind = 15, current.x -= 2;
        else if (kind == 5)
            kind = 11;
        else if (kind == 4)
            kind = 7;
        else if (kind == 3)
            kind = 5;
        else if (kind == 2)
            kind = 3, current.x += 2;
        else if (kind == 1)
            kind = 2;
        else
            kind = 0, current.x -= 2;
        StraightFall(depth);
        for (int i = 0; i < 4; i++) {
            int m = (current.x - 4 + vary[kind].vary_x[i]) / 2;
            int n = current.y - 3 + vary[kind].vary_y[i];
            if (m >= 0 && m < WIDTH && n >= 0 && n < HEIGHT) {
                board[m][n].having = true;
                board[m][n].color = COL[pri_kind];
            }
        }
        CheckFull(&score);
        kind = pkind;
    }
    return 0;
}