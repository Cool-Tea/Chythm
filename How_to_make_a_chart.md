# How to create a valid chart

本游戏的谱面文件夹结构如下

```
chart \
    chart.json 谱面及其信息的储存文件
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
    "chart":{
        "audio": "audio.mp3", // example: never_gonna_give_you_up.mp3
        "bpm": 60, // the bpm of the chart
        "offset": , // this function is under developing
        "background": "background.png", // example: pure.png
        "lane_size": 4, // 判定点数量（在本游戏中lane与hit_point的数量是一一对应的）
        "notes": [], // 音符信息
        "hit_points": [], // 判定点信息
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

## Hit Point

以下是一个一般的hit_point信息：

```json
{
    ...
    "hit_points": [
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

**hit_points中的数量一定要和lane_size相同！！！**

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
            "type": 0, // note的类型，目前有SINGLE与LONG
            "lane": 0, // 判定点的编号，从0开始
            "move": [
                {
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

- type：note的类型，目前有`SINGLE(0)`、`LONG(1)`
- lane：note对应的判定点
- move：该note的移动信息
- x：note的初始x坐标
- y：note的初始y坐标
- reach_time：note到达判定点的时间

### SINGLE

- move：只有一条信息，对应一个note

### LONG

- move：有两条信息，**按先后顺序**对应该note的首尾

## Event

以下是一个一般的事件Event信息：

```json
{
    ...
    "events": [
        {
            "object": 0, // event操作的对象，目前有GameScene与Lane
            "type": 16, // event类型
            "time": 2000, // event开始时间
            "lasting_time": 4000, // event持续时间
            "data": {
                ... // event所需要的数据，由类型确定
            }
        },
        ...
    ]
}
```

- object：event操作的对象，目前有`游戏界面(0)`与`判定点(1)`
- type：event的类型：`保留类型(0~15)`，`游戏界面(16~31)`：[`输出文本(16)`]，`判定点(32~47)`：[`给予速度(32)`, `移动到(33)`, `停止运动(34)`]
- time：event的开始时间
- lasting_time：event的持续时间
- data：包含用于event的数据

### TEXT(16)：输出文本

- 功能：向游戏界面输出文本
- data：

```json
{
    "data": {
        "x": 840, // 显示位置
        "y": 0,
        "text": "Hello Chythm" // 显示文本
    }
}
```

### MOVE(32)：给予速度

- 功能：给予判定点速度
- data：

```json
{
    "data": {
        "x": 0, // x方向速度
        "y": 4 // y方向速度
    }
}
```

### MOVETO(33)：移动到

- 功能：使判定点在lasting_time内移动到目标位置
- data：

```json
{
    "data": {
        "x": 1200, // 坐标信息
        "y": 200
    }
}
```

### STOP(34): 停止运动

**此事件会自动添加，请不要在以上运动事件后添加该事件**

- 功能：停止判定点的一切运动
- data：NULL