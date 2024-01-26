# Chythm Manual

## What is Chythm

Chythm是一款使用C语言开发的简单音乐游戏。

## About Environment

- 操作系统：Linux （本项目在Linux上开发，但是根据SDL库跨平台的特性，理论上可以在Windows上运行）
- C语言环境：gcc (Ubuntu 13.1.0-8ubuntu1~20.04.2) 13.1.0
- 编译选项：CMake
- 第三方库：SDL、SDL_image、SDL_mixer、SDL_ttf、SDL_gfx（此非SDL官方附属）、cJSON

## How to play

和市面上音乐游戏相同，玩家需要做的是在音符下落到判定点的时候按下对应按键，若时机正确则视为一次Hit，否则视为Miss。玩家需要在游玩过程中尽量获取更多Hit

## About performance

由于cJSON的解析速度较慢和游戏谱面文件较大，所以**加载速度较慢是正常的**，请耐心等待

## About the key sets

**本游戏不支持鼠标操作，全部使用键盘操作**

- 上一个选项：`W` / `UP`
- 下一个选项：`S` / `DOWN`
- 确认：`E` / `Enter`
- 返回：`Esc`
- 判定点按键组：[`S`, `D`, `F`, `G`, `H`, `J`, `K`, `L`] （编号从左到右为 [0, 1, 2, 3, 4, 5, 6, 7] ）

判定点的按键分布由判定点的数量决定，原则上保证按键居中分布：

```C
#define HIT_POINT_MAX_NUM 8 // 游戏最大支持的判定点数量
/**
 * \param hit_point_size 本谱面中判定点数量
 * \param i 本谱面第i个判定点，从0开始编号
 * \return 按键编号
*/
int AllocateKey(int hit_point_size, int i) {
    return HIT_POINT_MAX_NUM / 2 - hit_point_size / 2 + i;
}
```

## How to create a valid chart

**本部分介绍游戏谱面的编写，且信息较多，如只关注游戏内容，请跳过本部分！！！**

[How to create a valid chart](./How_to_make_a_chart.md)