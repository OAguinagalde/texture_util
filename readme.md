# A tool for doing things with images

So far I use it to embed textures in my C projects.

```ps1
> .\bin\main.exe .\test.png
```

> Where test.png is a 2x2 pixels RGBA png file.

Generates the following C code.

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

Dependencies would be [`stb_image`](https://github.com/nothings/stb) and the C standard library.