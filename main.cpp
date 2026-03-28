#include <array>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

class TerminalSession {
public:
    TerminalSession() = default;

    ~TerminalSession() {
        disableRawMode();
    }

    void enableRawMode() {
        if (rawMode_) {
            return;
        }
        if (tcgetattr(STDIN_FILENO, &originalMode_) == -1) {
            return;
        }

        termios raw = originalMode_;
        raw.c_lflag &= ~(ECHO | ICANON | ISIG);
        raw.c_cc[VMIN] = 0;
        raw.c_cc[VTIME] = 1;
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

        rawMode_ = true;
        std::printf("\033[?25l");
        std::fflush(stdout);
    }

    void disableRawMode() {
        if (!rawMode_) {
            return;
        }
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &originalMode_);
        std::printf("\033[0m\033[?25h");
        std::fflush(stdout);
        rawMode_ = false;
    }

    void clearScreen() const {
        std::printf("\033[2J");
    }

    void moveCursor(int x, int y) const {
        std::printf("\033[%d;%dH", y, x);
    }

    void setColor(int code) const {
        if (code == 0) {
            std::printf("\033[0m");
            return;
        }

        struct ColorStyle {
            int code;
            const char* escape;
        };

        static constexpr std::array<ColorStyle, 8> kStyles = {{
            {43, "\033[1;33m"},
            {46, "\033[1;36m"},
            {44, "\033[1;34m"},
            {39, "\033[1;35m"},
            {11, "\033[1;36m"},
            {15, "\033[1;37m"},
            {12, "\033[1;34m"},
            {13, "\033[1;35m"},
        }};

        for (const auto& style : kStyles) {
            if (style.code == code) {
                std::printf("%s", style.escape);
                return;
            }
        }

        std::printf("\033[1;33m");
    }

    bool keyAvailable() const {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        timeval timeout = {0, 0};
        return select(STDIN_FILENO + 1, &readfds, nullptr, nullptr, &timeout) > 0;
    }

    int readKey() const {
        char ch = 0;
        if (read(STDIN_FILENO, &ch, 1) != 1) {
            return -1;
        }

        if (ch == '\033') {
            char seq[2] = {0, 0};
            if (read(STDIN_FILENO, &seq[0], 1) != 1) {
                return '\033';
            }
            if (read(STDIN_FILENO, &seq[1], 1) != 1) {
                return '\033';
            }
            if (seq[0] == '[') {
                if (seq[1] == 'A') {
                    return 0x48;
                }
                if (seq[1] == 'B') {
                    return 0x50;
                }
                if (seq[1] == 'C') {
                    return 0x4d;
                }
                if (seq[1] == 'D') {
                    return 0x4b;
                }
            }
            return '\033';
        }

        return ch;
    }

    void sleepMs(int milliseconds) const {
        usleep(static_cast<useconds_t>(milliseconds * 1000));
    }

private:
    termios originalMode_{};
    bool rawMode_ = false;
};

class tetrisGame {
public:
    void run() {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        terminal_.enableRawMode();
        terminal_.clearScreen();
        terminal_.moveCursor(1, 1);

        bestScore_ = loadBestScore();
        resetRoundState();
        initializeBoard();
        drawFrame();
        redrawBoard();
        updateHud();

        currentType_ = randomTetromino();
        while (!shouldQuit_) {
            nextType_ = randomTetromino();
            drawNextPiece(nextType_);

            activePiece_ = makeSpawnPiece(currentType_);
            if (!canPlace(activePiece_)) {
                handleGameOver();
                currentType_ = randomTetromino();
                continue;
            }

            drawPiece(activePiece_, tetromino(activePiece_.type).colorCode);
            std::fflush(stdout);
            dropActivePiece();
            if (shouldQuit_) {
                break;
            }

            lockActivePiece();
            clearCompletedLines();
            currentType_ = nextType_;
        }
    }

private:
    static constexpr int kBackColor = 0;
    static constexpr int kBoardWidth = 15;
    static constexpr int kBoardHeight = 25;
    static constexpr int kBoardLeft = 4;
    static constexpr int kBoardTop = 3;
    static constexpr int kSpawnX = 7;
    static constexpr int kSpawnY = 0;
    static constexpr std::array<int, 6> kSpeed = {0, 12, 9, 6, 3, 1};
    static constexpr std::array<int, 5> kLineScores = {0, 1, 3, 6, 10};

    struct Point {
        int x;
        int y;
    };

    struct BoardCell {
        Point screen;
        int colorCode;
        bool occupied;
    };

    struct RotationState {
        std::array<Point, 4> blocks;
    };

    struct RotationKick {
        Point shift;
    };

    struct TetrominoDefinition {
        int colorCode;
        int baseRotationIndex;
        int rotationCount;
        int spawnOffsetX;
        int previewOffsetX;
        std::array<RotationKick, 4> kicks;
    };

    struct Piece {
        int type;
        int rotationState;
        Point origin;
    };

    static constexpr std::array<RotationState, 19> kRotationStates = {{
        {{{{0, 0}, {1, 0}, {2, 0}, {3, 0}}}},
        {{{{0, 0}, {0, -1}, {0, -2}, {0, -3}}}},
        {{{{0, 0}, {0, -1}, {1, 0}, {1, -1}}}},
        {{{{0, 0}, {-1, 0}, {-1, -1}, {-2, -1}}}},
        {{{{0, 0}, {0, -1}, {1, -1}, {1, -2}}}},
        {{{{0, 0}, {1, 0}, {1, -1}, {2, -1}}}},
        {{{{0, 0}, {0, -1}, {-1, -1}, {-1, -2}}}},
        {{{{0, 0}, {1, -1}, {0, -1}, {0, -2}}}},
        {{{{0, 0}, {0, -1}, {0, -2}, {1, -2}}}},
        {{{{0, 0}, {0, -1}, {-1, -1}, {-2, -1}}}},
        {{{{0, 0}, {1, 0}, {1, -1}, {1, -2}}}},
        {{{{0, 0}, {1, 0}, {2, 0}, {2, -1}}}},
        {{{{0, 0}, {-1, 0}, {-1, -1}, {-1, -2}}}},
        {{{{0, 0}, {0, -1}, {1, -1}, {2, -1}}}},
        {{{{0, 0}, {0, -1}, {0, -2}, {-1, -2}}}},
        {{{{0, 0}, {1, 0}, {2, 0}, {1, -1}}}},
        {{{{0, 0}, {0, -1}, {1, -1}, {0, -2}}}},
        {{{{0, 0}, {-1, -1}, {0, -1}, {1, -1}}}},
        {{{{0, 0}, {0, -1}, {-1, -1}, {0, -2}}}},
    }};

    static constexpr std::array<TetrominoDefinition, 7> kTetrominoes = {{
        {11, 0, 2, -1, -3, {{{{-1, 0}}, {{1, 1}}, {{0, 0}}, {{0, 0}}}}},
        {15, 2, 1, 0, -1, {{{{0, 0}}, {{0, 0}}, {{0, 0}}, {{0, 0}}}}},
        {12, 3, 2, 1, 2, {{{{1, 0}}, {{-1, 0}}, {{0, 0}}, {{0, 0}}}}},
        {12, 5, 2, 0, -2, {{{{-1, 0}}, {{1, 0}}, {{0, 0}}, {{0, 0}}}}},
        {13, 7, 4, 0, -1, {{{{-1, 0}}, {{0, 0}}, {{2, -1}}, {{-1, 1}}}}},
        {13, 11, 4, 0, -1, {{{{-2, 0}}, {{1, 0}}, {{-1, -1}}, {{2, 1}}}}},
        {14, 15, 4, -1, -1, {{{{-1, 0}}, {{1, 0}}, {{0, 0}}, {{0, 0}}}}},
    }};

    TerminalSession terminal_;
    std::array<std::array<BoardCell, kBoardHeight>, kBoardWidth> board_{};
    Piece activePiece_{};
    int currentType_ = 0;
    int nextType_ = 0;
    int score_ = 0;
    int bestScore_ = 0;
    int medalRank_ = 1;
    bool shouldQuit_ = false;

    const TetrominoDefinition& tetromino(int type) const {
        return kTetrominoes[type];
    }

    const RotationState& rotationState(const Piece& piece) const {
        const auto& definition = tetromino(piece.type);
        return kRotationStates[definition.baseRotationIndex + piece.rotationState];
    }

    std::array<Point, 4> pieceCells(const Piece& piece) const {
        std::array<Point, 4> cells{};
        const auto& state = rotationState(piece);
        for (std::size_t index = 0; index < state.blocks.size(); ++index) {
            cells[index] = {
                piece.origin.x + state.blocks[index].x,
                piece.origin.y + state.blocks[index].y,
            };
        }
        return cells;
    }

    void initializeBoard() {
        for (int x = 0; x < kBoardWidth; ++x) {
            for (int y = 0; y < kBoardHeight; ++y) {
                board_[x][y].screen = {kBoardLeft + x * 2, kBoardTop + y};
                board_[x][y].colorCode = kBackColor;
                board_[x][y].occupied = false;
            }
        }
    }

    void resetRoundState() {
        score_ = 0;
        medalRank_ = 1;
        for (auto& column : board_) {
            for (auto& cell : column) {
                cell.colorCode = kBackColor;
                cell.occupied = false;
            }
        }
    }

    int loadBestScore() const {
        std::ifstream input("score.txt");
        int best = 0;
        if (input >> best) {
            return best;
        }

        std::ofstream output("score.txt");
        output << 0;
        return 0;
    }

    void saveBestScore() const {
        std::ofstream output("score.txt");
        if (output) {
            output << bestScore_;
        }
    }

    int randomTetromino() const {
        return std::rand() % static_cast<int>(kTetrominoes.size());
    }

    Piece makeSpawnPiece(int type) const {
        return Piece{
            type,
            0,
            {kSpawnX + tetromino(type).spawnOffsetX, kSpawnY},
        };
    }

    bool canPlace(const Piece& piece) const {
        for (const auto& cell : pieceCells(piece)) {
            if (cell.x < 0 || cell.x >= kBoardWidth || cell.y >= kBoardHeight) {
                return false;
            }
            if (cell.y >= 0 && board_[cell.x][cell.y].occupied) {
                return false;
            }
        }
        return true;
    }

    void drawBlockAtScreen(const Point& screen, int colorCode) const {
        terminal_.moveCursor(screen.x, screen.y);
        terminal_.setColor(colorCode);
        std::printf(colorCode == kBackColor ? "  " : "██");
    }

    void drawPiece(const Piece& piece, int colorCode) const {
        for (const auto& cell : pieceCells(piece)) {
            if (cell.y < 0 || cell.y >= kBoardHeight) {
                continue;
            }
            drawBlockAtScreen(board_[cell.x][cell.y].screen, colorCode);
        }
    }

    void erasePiece(const Piece& piece) const {
        drawPiece(piece, kBackColor);
    }

    void drawFrame() const {
        terminal_.clearScreen();
        terminal_.moveCursor(1, 1);
        terminal_.setColor(kBackColor);

        for (int column = 0; column < kBoardWidth; ++column) {
            std::printf("[]");
        }
        std::printf("[][][][][][][][][][][]\n");

        for (int row = 0; row < kBoardHeight + 1; ++row) {
            terminal_.moveCursor(1, 2 + row);
            std::printf("[]");
            terminal_.moveCursor(kBoardWidth * 2 + 7, 2 + row);
            if (row == 8 || row == 16) {
                std::printf("[][][][][][][][][][]");
            } else {
                std::printf("[]");
            }
            terminal_.moveCursor(kBoardWidth * 2 + 25, 2 + row);
            std::printf("[]");
        }

        terminal_.moveCursor(1, 3 + kBoardHeight);
        for (int column = 0; column < kBoardWidth; ++column) {
            std::printf("[]");
        }
        std::printf("[][][][][][][][][][][]\n");

        terminal_.setColor(43);
        terminal_.moveCursor(22 + kBoardWidth, 12);
        std::printf("BEST      %d", bestScore_);
        terminal_.setColor(46);
        terminal_.moveCursor(22 + kBoardWidth, 14);
        std::printf("SCORE");
        terminal_.setColor(44);
        terminal_.moveCursor(22 + kBoardWidth, 16);
        std::printf("MEDAL");
        terminal_.setColor(39);
        terminal_.moveCursor(22 + kBoardWidth, 20);
        std::printf("↑ ↓ ←  →");
        terminal_.moveCursor(22 + kBoardWidth, 21);
        std::printf("W S A  D");
        terminal_.moveCursor(22 + kBoardWidth, 22);
        std::printf("[SPACE] pause");
        terminal_.moveCursor(22 + kBoardWidth, 23);
        std::printf("[X] hard drop");
        terminal_.moveCursor(22 + kBoardWidth, 24);
        std::printf("[Q/ESC] exit");
        terminal_.moveCursor(22 + kBoardWidth, 3);
        std::printf("NEXT");

        terminal_.setColor(kBackColor);
        std::fflush(stdout);
    }

    void redrawBoard() const {
        for (int x = 0; x < kBoardWidth; ++x) {
            for (int y = 0; y < kBoardHeight; ++y) {
                const auto& cell = board_[x][y];
                drawBlockAtScreen(cell.screen, cell.occupied ? cell.colorCode : kBackColor);
            }
        }
        std::fflush(stdout);
    }

    void updateHud() {
        terminal_.setColor(46);
        terminal_.moveCursor(47, 14);
        std::printf("%-5d", score_);

        terminal_.setColor(44);
        terminal_.moveCursor(43, 16);
        std::printf("      ");
        if (score_ > 0) {
            terminal_.moveCursor(43, 16);
            for (int count = 0; count < medalRank_; ++count) {
                std::printf("★");
            }
        }

        if (score_ > bestScore_) {
            bestScore_ = score_;
            saveBestScore();
        }

        terminal_.setColor(43);
        terminal_.moveCursor(47, 12);
        std::printf("%-5d", bestScore_);
        std::fflush(stdout);
    }

    void drawNextPiece(int type) const {
        const auto& definition = tetromino(type);
        const auto& previewState = kRotationStates[definition.baseRotationIndex];
        const int previewX = 43 + definition.previewOffsetX;
        const int previewY = 6;

        terminal_.setColor(kBackColor);
        for (int row = 0; row < 4; ++row) {
            terminal_.moveCursor(40, previewY + row - 2);
            std::printf("          ");
        }

        terminal_.setColor(definition.colorCode);
        for (const auto& block : previewState.blocks) {
            terminal_.moveCursor(previewX + block.x * 2, previewY + block.y);
            std::printf("██");
        }
        std::fflush(stdout);
    }

    void movePieceIfPossible(int dx, int dy) {
        Piece candidate = activePiece_;
        candidate.origin.x += dx;
        candidate.origin.y += dy;

        if (!canPlace(candidate)) {
            return;
        }

        erasePiece(activePiece_);
        activePiece_ = candidate;
        drawPiece(activePiece_, tetromino(activePiece_.type).colorCode);
        std::fflush(stdout);
    }

    void rotatePieceIfPossible() {
        const auto& definition = tetromino(activePiece_.type);
        if (definition.rotationCount <= 1) {
            return;
        }

        Piece candidate = activePiece_;
        candidate.rotationState = (candidate.rotationState + 1) % definition.rotationCount;
        const auto& kick = definition.kicks[candidate.rotationState].shift;
        candidate.origin.x += kick.x;
        candidate.origin.y += kick.y;

        if (!canPlace(candidate)) {
            return;
        }

        erasePiece(activePiece_);
        activePiece_ = candidate;
        drawPiece(activePiece_, definition.colorCode);
        std::fflush(stdout);
    }

    void hardDrop() {
        Piece target = activePiece_;
        while (true) {
            Piece next = target;
            next.origin.y += 1;
            if (!canPlace(next)) {
                break;
            }
            target = next;
        }

        if (target.origin.x == activePiece_.origin.x && target.origin.y == activePiece_.origin.y) {
            return;
        }

        erasePiece(activePiece_);
        activePiece_ = target;
        drawPiece(activePiece_, tetromino(activePiece_.type).colorCode);
        std::fflush(stdout);
    }

    void pauseGame() {
        while (!shouldQuit_) {
            if (!terminal_.keyAvailable()) {
                terminal_.sleepMs(30);
                continue;
            }

            int key = terminal_.readKey();
            if (key == ' ' || key == '\n' || key == '\r') {
                return;
            }
            if (key == 'q' || key == 'Q' || key == 27) {
                shouldQuit_ = true;
                return;
            }
        }
    }

    void handleInputWindow() {
        const int slices = 50 / medalRank_;

        for (int index = 0; index < slices && !shouldQuit_; ++index) {
            if (terminal_.keyAvailable()) {
                const int key = terminal_.readKey();
                switch (key) {
                    case 0x48:
                    case 'w':
                    case 'W':
                        rotatePieceIfPossible();
                        break;
                    case 0x50:
                    case 's':
                    case 'S':
                        return;
                    case 0x4b:
                    case 'a':
                    case 'A':
                        movePieceIfPossible(-1, 0);
                        break;
                    case 0x4d:
                    case 'd':
                    case 'D':
                        movePieceIfPossible(1, 0);
                        break;
                    case 'x':
                    case 'X':
                        hardDrop();
                        return;
                    case ' ':
                        pauseGame();
                        break;
                    case 'q':
                    case 'Q':
                    case 27:
                        shouldQuit_ = true;
                        break;
                    default:
                        break;
                }
            }

            terminal_.sleepMs(kSpeed[medalRank_]);
        }
    }

    void dropActivePiece() {
        while (!shouldQuit_) {
            handleInputWindow();
            if (shouldQuit_) {
                return;
            }

            Piece next = activePiece_;
            next.origin.y += 1;
            if (!canPlace(next)) {
                break;
            }

            erasePiece(activePiece_);
            activePiece_ = next;
            drawPiece(activePiece_, tetromino(activePiece_.type).colorCode);
            std::fflush(stdout);
        }
    }

    void lockActivePiece() {
        const int colorCode = tetromino(activePiece_.type).colorCode;
        for (const auto& cell : pieceCells(activePiece_)) {
            if (cell.y < 0 || cell.y >= kBoardHeight) {
                continue;
            }
            board_[cell.x][cell.y].occupied = true;
            board_[cell.x][cell.y].colorCode = colorCode;
        }
    }

    void clearCompletedLines() {
        int clearedLines = 0;

        for (int y = 0; y < kBoardHeight; ++y) {
            bool fullRow = true;
            for (int x = 0; x < kBoardWidth; ++x) {
                if (!board_[x][y].occupied) {
                    fullRow = false;
                    break;
                }
            }

            if (!fullRow) {
                continue;
            }

            ++clearedLines;
            for (int row = y; row > 0; --row) {
                for (int x = 0; x < kBoardWidth; ++x) {
                    board_[x][row].occupied = board_[x][row - 1].occupied;
                    board_[x][row].colorCode = board_[x][row - 1].colorCode;
                }
            }
            for (int x = 0; x < kBoardWidth; ++x) {
                board_[x][0].occupied = false;
                board_[x][0].colorCode = kBackColor;
            }

            --y;
        }

        score_ += kLineScores[clearedLines];
        updateMedalRank();
        redrawBoard();
        updateHud();
    }

    void updateMedalRank() {
        if (score_ >= 140) {
            medalRank_ = 5;
        } else if (score_ >= 90) {
            medalRank_ = 4;
        } else if (score_ >= 50) {
            medalRank_ = 3;
        } else if (score_ >= 20) {
            medalRank_ = 2;
        } else if (score_ >= 1) {
            medalRank_ = 1;
        } else {
            medalRank_ = 1;
        }
    }

    void handleGameOver() {
        terminal_.setColor(46);
        terminal_.moveCursor(47, 14);
        std::printf("0    ");

        terminal_.moveCursor(42, 16);
        for (int count = 0; count < medalRank_; ++count) {
            std::printf("  ");
        }

        terminal_.setColor(kBackColor);
        for (int y = kBoardHeight - 1; y >= 0; --y) {
            for (int x = 0; x < kBoardWidth; ++x) {
                terminal_.moveCursor(kBoardLeft + x * 2, kBoardTop + y);
                std::printf("██");
            }
            terminal_.sleepMs(1);
        }

        resetRoundState();
        redrawBoard();
        updateHud();
    }
};

int main() {
    tetrisGame game;
    game.run();
    return 0;
}
