# Texture_util

A tool for embedding image data into C source code.

Dependencies would be [`stb_image`](https://github.com/nothings/stb) and the C standard library.
Building is straightforward:

```ps1
> clang main.c -o bin\main.exe # Or just use the script .\build.ps1
> .\bin\main.exe
```
```txt
Usage: main.exe (-[ephdlsrgba]+)? ImageFile
   -e       (Default) Outputs ready to use code for Embedding into a C project.
   -p       Prints the pixel data directly in the forma "RRGGBB(AA)", using the hex
            values. Useful for debugging. Check option -l.
   -h       Display this help message and finish.

   -[rgba]  (Default) Change the desired channels. Defaults to all 4 (RGBA).
   -s       (Default) Silences some of the debug output.
   -d       Show all the debug output.
   -l       When in combination with option -p, Limits the ammount of pixels
            displayed to 30x30.

Example: main.exe ./test.png -plrgb.
   This command will display, in a more friendly format (-p) the RGB pixel data
   (-rgb) of the top left 30x30 pixels (-l) of the image test.png
```

## Example

```ps1
> .\bin\main.exe .\test.png
```

> Where test.png is a 2x2 pixels RGBA png file.

Outputs the following C code.

```c
// .\test.png
const int texture_width = 2;
const int texture_height = 2;
const int texture_channels = 4;
const int texture_data_size = 16;
const unsigned char texture_data[16] = {
    0x3f,0x48,0xcc,0xff,0xff,0x00,0x00,0xff,0x3f,0x48,0xcc,0xff,0x00,0x00,0x00,0xff,
};
```

Where `0x3f,0x48,0xcc,0xff` are the 4 bytes (RGBA) that form the first pixel.
