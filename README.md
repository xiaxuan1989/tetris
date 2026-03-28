# tetris

macOS 终端版俄罗斯方块，单文件实现。

编译：

```bash
clang++ -std=c++17 -O2 main.cpp -o tetris
```

运行：

```bash
./tetris
```

操作：

- `←` `→` 或 `A` `D`：左右移动
- `↑` 或 `W`：旋转
- `↓` 或 `S`：加速下落
- `X`：直接落底
- `Space`：暂停/继续
- `Q`：退出
- `R`：游戏结束后重新开始

最高分会保存在当前目录下的 `score.txt`。
