<p align="right">
  <strong>简体中文</strong> · <a href="README.md">English</a>
</p>

# 资源目录（Assets）

本目录集中存放可复用的资源（字库、图片、音乐等），按资源类型分子目录管理。每个资源放在其类型对应的子目录，并记录放置路径、命名方式、集成方式与来源/许可。二进制资源（字体、图片、音频）不属于纯 markdown 文档，请勿与文档混放。涉及版权/授权的资源需注明来源与许可。

## 字库（fonts）

可复用的字库文件与生成的字库源码放在 `fonts/`。

- 命名要能反映字族、字重、字级与格式。
- 记录来源、许可、字符范围、转换命令与目标放置路径。
- 添加字库前评估 Flash 与内部 RAM 影响；ESP32-C3 无 PSRAM。
- 不提交许可不允许分发的字库。

### “见山”中文子集字库

`fonts/jianshan_font_16.c` 与 `fonts/jianshan_font_22.c` 是“见山”demo 使用的
LVGL 生成字库，覆盖可打印 ASCII 及 `fonts/jianshan-glyphs.txt` 中的中文字符。
源字体为 Google Fonts 的
[Noto Sans SC Regular](https://github.com/google/fonts/tree/main/ofl/notosanssc)，
采用 SIL Open Font License 1.1；许可全文保存在 `fonts/OFL-1.1.txt`。

两份文件由 `lv_font_conv` 1.5.3 生成，参数分别为 16/22 px、4 bpp、可打印
ASCII 范围 `0x20-0x7E`，以及字符清单中的 symbols。生成的 C 源码由
`main/CMakeLists.txt` 直接编译；体积较大的源字体不提交到仓库。

## 图片（images）

可复用的源图与生成的显示资产放在 `images/`。

- 使用描述性命名，并记录尺寸、像素格式、转换步骤与目标路径。
- 优先采用适合 240 × 320 RGB565 显示的格式，并纳入 Flash 与内部 RAM 考量。
- 许可允许时保留可编辑源文件，并记录来源与许可。
- 图片中不得包含设备二维码秘密、凭证或个人数据。

## 音乐与音效（music）

可复用的音乐与音效源码放在 `music/`。

- 记录来源、许可、采样率、位深、声道、转换命令与目标路径。
- 与当前 BSP 音频路径匹配时优先采用 16 kHz、16 位单声道 PCM。
- 嵌入音频前评估 Flash 与内部 RAM 成本；长录音应流式或分块。
- 无再分发许可不提交媒体文件。
