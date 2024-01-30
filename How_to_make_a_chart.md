# How to create a valid chart

本游戏的谱面文件夹结构如下

```
chart \
    chart.json 谱面及其信息的储存文件（固定名称chart.json）
    preview.png / preview.jpg 谱面的预览图片，支持JPG与PNG
    background.png / background.jpg 谱面的背景图片，支持JPG与PNG
    audio.mp3 谱面的音乐，只支持mp3
    score.txt 储存历史最佳成绩的文件
```

本游戏的谱面采用json文件保存，以下是该json文件的一般结构

```json
{
    "info": {
        "title": "This is a title",
        "artist": "Who make this chart",
        "preview": "preview.png" // example: cat.jpg
    },
    "chart": {
        "audio": "audio.mp3", // example: never_gonna_give_you_up.mp3
        "bpm": 60, // the bpm of the chart
        "offset": , // this function is under development
        "background": "background.png", // example: pure.png
        "lane_size": 4, // 判定点数量（在本游戏中lane与hit_point的数量是一一对应的）
        "notes": [], // 音符信息
        "lanes": [], // 判定点信息
        "events": [] // 事件信息
    }
}
```

## General

> **info**

- title：谱面的名称
- artist：谱面作者
- preview：谱面预览图片文件，支持JPG与PNG

> **chart**

- audio：谱面音乐文件，仅支持MP3
- bpm：谱面的beats per minute
- offset：谱面显示与音乐播放的偏差
- background：谱面背景图片文件，支持JPG与PNG
- lane_size：谱面的判定点个数（在本游戏中，lane与hit_point是一一对应的）

## Lanes

以下是一个一般的hit_point信息：

```json
{
    ...
    "lanes": [
        {
            "lane": 0,
            "position": {
                "x": 400,
                "y": 600
            }
        },
        ...
    ]
}
```

**lanes中的数量一定要和lane_size相同！！！**

- lane：判定点编号
- position：判定点初始位置信息
- x：判定点的x坐标
- y：判定点的y坐标

## Note

以下是一个一般的音符note信息：

```json
{
    ...
    "notes": [
        {
            "type": 0, // note的类型
            "move": [
                {
                    "lane": 0, // 判定点的编号，从0开始
                    "x": 400,
                    "y": -40,
                    "reach_time": 1000
                },
                ...
            ]
        },
        ...
    ],
}
```

- type：note的类型，目前有`SINGLE(0)`、`LONG(1)`、`MULTI(2)`
- move：该note的移动信息
- lane：note对应的判定点
- x：note的初始x坐标
- y：note的初始y坐标
- reach_time：note到达判定点的时间

### SINGLE

- move：只有一条信息，对应一个note

### LONG

- move：有两条信息，**按先后顺序**对应该note的首尾

### MULTI

- move：可以有2~4条信息，**按先后顺序**前后连接

## Event

以下是一个一般的事件Event信息：

```json
{
    ...
    "events": [
        {
            "type": 16, // event类型
            "lane": 0, // （可选）当object为Lane的时候需要此条
            "time": 2000, // event开始时间
            "lasting_time": 4000, // （可选）event持续时间
            "data": {
                ... // event所需要的数据，由类型确定
            }
        },
        ...
    ]
}
```

- type：event的类型：`保留类型(0~15)`，`游戏界面(16~31)`：[`输出文本(16)`, `改变bpm(17)`, `绘制特效(18)`, `改变背景(19)`]，`判定点(32~47)`：[`给予速度(32)`, `移动到(33)`, `停止运动(34)`]
- lane：event的作用判定点编号（从0开始）
- time：event的开始时间
- lasting_time：event的持续时间
- data：包含用于event的数据

### TEXT(16)：输出文本

- 功能：向游戏界面输出文本

```json
{
    "type": 16, // event类型
    "time": 2000, // event开始时间
    "lasting_time": 4000, // text持续时间
    "data": {
        "x": 840, // 显示位置
        "y": 0,
        "text": "Hello Chythm" // 显示文本
    }
}
```

### BPM(17)：改变bpm

- 功能：改变谱面的bpm

```json
{
    "type": 17, // event类型
    "time": 2000, // 触发时间
    "data": {
        "bpm": 60 // bpm的值
    }
}
```

### EFFECT(18)：绘制特效

- 功能：在屏幕上绘制特效

```json
{
    "type": 18, // event类型
    "time": 5000, // 触发时间
    "lasting_time": 2000, // 持续时间
    "data": {
        "x": 960, // 特效位置
        "y": 400,
        "radius": 60, // 特效大小
        "angle": 60.0, // 特效顺时针旋转角度（角度制）
        "type": 4, // 特效类型
        "repeat": 1 // 特效是否重复播放，为0则特效只播放一次
    }
}
```

### BACKGROUND(19)：改变背景

- 功能：改变谱面背景

```json
{
    "type": 19, // eventleix
    "time": 5000, // 触发时间
    "data": {
        "background": "background2.png" // 背景图片文件名（必须在chart文件夹下）
    }
}
```

### MOVE(32)：给予速度

- 功能：给予判定点速度

```json
{
    "type": 32, // event类型
    "lane": 0, // 判定点编号
    "time": 2000, // 运动开始时间
    "lasting_time": 4000, // 运动时间
    "data": {
        "x": 0, // x方向速度
        "y": 4 // y方向速度
    }
}
```

### MOVETO(33)：移动到

- 功能：使判定点在lasting_time内移动到目标位置

```json
{
    "type": 33, // event类型
    "lane": 0, // 判定点编号
    "time": 2000, // 运动开始时间
    "lasting_time": 4000, // 运动时间
    "data": {
        "x": 1200, // 坐标信息
        "y": 200
    }
}
```

### STOP(34): 停止运动

**此事件会自动添加，请不要在以上运动事件后添加该事件**

- 功能：停止判定点的一切运动