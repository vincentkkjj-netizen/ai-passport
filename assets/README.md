<p align="right">
  <a href="README.zh_CN.md">简体中文</a> · <strong>English</strong>
</p>

# Assets

This directory stores reusable fonts, images, music, and sound effects, organized by asset type.

Keep each asset in the matching subdirectory and document its destination, naming, integration method, and source/license. Do not mix binary assets with Markdown documentation.

## Fonts

Store reusable font files and generated font sources in `fonts/`.

- Use descriptive names that include the family, weight, size, and format when relevant.
- Document the source, license, character range, conversion command, and expected destination.
- Check Flash and internal-RAM impact before adding a font; the ESP32-C3 has no PSRAM.
- Do not commit fonts whose license does not permit redistribution.

### Jianshan CJK subsets

The four `fonts/jianshan_{sans,serif}_*.c` files are generated LVGL fonts for
the Jianshan demo. They contain printable ASCII plus the Chinese characters in
`fonts/jianshan-glyphs.txt`. The source typefaces are Noto Sans SC and Noto
Serif SC from Google Fonts, licensed under the SIL Open Font License 1.1; the
license text is stored as `fonts/OFL-1.1.txt`.

Generate the fonts with `lv_font_conv` 1.5.3 using 4 bits per pixel, sizes 14
and 18 for sans-serif and 20 and 26 for serif, printable ASCII range
`0x20-0x7E`, and the symbols in the glyph-list file. The generated C sources
are compiled directly by `main/CMakeLists.txt`; the multi-megabyte source fonts
are intentionally not committed.

## Images

Store reusable source images and generated display assets in `images/`.

- Use descriptive names and document dimensions, pixel format, conversion steps, and destination.
- Prefer formats suitable for the 240 × 320 RGB565 display and account for Flash and internal RAM.
- Preserve editable sources where licensing permits, and record the source and license.
- Never commit device QR secrets, credentials, or personal data in images.

## Music and sound effects

Store reusable music and sound-effect sources in `music/`.

- Document the source, license, sample rate, bit depth, channels, conversion command, and destination.
- Prefer 16 kHz, 16-bit mono PCM when it matches the current BSP audio path.
- Check Flash and internal-RAM cost before embedding audio; stream or chunk long recordings.
- Do not commit media without redistribution permission.
