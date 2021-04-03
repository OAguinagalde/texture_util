#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define tu_log printf
#define tu_free free
#define tu_malloc malloc

#define tu_bit(byte, bit) ((1 << bit) & byte)
#define tu_flag(n) 1 << n

typedef struct tu_texture {
    unsigned char* data;
    int width, height, channels;
} tu_texture;

typedef enum tu_textureMergeMode {
    t1_isNextTo_t2,
    t1_isTopOf_t2
} tu_textureMergeMode;

typedef enum tu_channel {
    tu_channel_red = tu_flag(0),
    tu_channel_green = tu_flag(1),
    tu_channel_blue = tu_flag(2),
    tu_channel_alpha = tu_flag(3)
} tu_channel;

typedef enum tu_result {
    tu_success,
    tu_fail
} tu_result;

// Allocations: 1 x tu_texture, 1 x unsigned char* at tu_texture->data
tu_result tu_loadTextureFile(const char* textureFileName, tu_texture* out_texture) {
    out_texture->data = stbi_load(textureFileName, &out_texture->width, &out_texture->height, &out_texture->channels, 0);
    if (out_texture->data) return tu_success;
    return tu_fail;
}

void tu_unloadTexture(tu_texture texture) {
    stbi_image_free(texture.data);
}

#if 0
// Assuming that the pixel data is RGBA, 4 bytes per pixel.
// You can figure out the offsets since you choose the MergeMode and know the texture sizes
// there is an allocation on result.data, make sure to free it when you are done!
void ce_graphics_mergeTextures(ce_graphics_textureInMemory t1, ce_graphics_textureInMemory t2, ce_graphics_textureMergeMode mode, ce_graphics_textureInMemory* out_texture) {
    ce_profiling_start();
    ce_log_debug("Merging textures...");
    ce_assert(t1.pixel_size == 4 && t2.pixel_size == 4 && "RGBA pixel data expected");
    out_texture->pixel_size = 4;
    switch (mode) {
        case t1_isNextTo_t2: {
            out_texture->width = t1.width + t2.width;
            out_texture->height = ce_max(t1.height, t2.height);
        } break;
        case t1_isTopOf_t2: {
            out_texture->height = t1.height + t2.height;
            out_texture->width = ce_max(t1.width, t2.width);
        } break;
        default: {
            // TODO: manage this issue...
            ce_log_error("Invalid ce_graphics_textureMergeMode");
        } break;
    }
    out_texture->data = ce_calloc(1, out_texture->pixel_size * out_texture->width * out_texture->height);
    ce_assert(out_texture->data);

    for(int i = 0; i < out_texture->width * out_texture->height; i++) {
        int pixel_index = i;
        int col = pixel_index % out_texture->width;
        int row = pixel_index / out_texture->width;

        bool currentPixelIsT1 = true;
        unsigned char r,g,b,a;
        // Figure out the resulting pixel data
        switch (mode) {
            case t1_isNextTo_t2: {
                if (col < t1.width) {
                    if (row < t1.height) {
                        // Current pixel is in t1
                        int t1_row = row;
                        int t1_col = col;
                        int t1_pixel_index = (t1_row * t1.width) + t1_col;
                        r = t1.data[(t1_pixel_index*4) + 0];
                        g = t1.data[(t1_pixel_index*4) + 1];
                        b = t1.data[(t1_pixel_index*4) + 2];
                        a = t1.data[(t1_pixel_index*4) + 3];
                    } else {
                        // Empty area
                        r = 0;
                        g = 0;
                        b = 0;
                        a = 0;
                    }
                } else {
                    currentPixelIsT1 = false;
                    if (row < t2.height) {
                        // Current pixel is in t2
                        int t2_row = row;
                        int t2_col = col - t1.width;
                        int t2_pixel_index = (t2_row * t2.width) + t2_col;
                        r = t2.data[(t2_pixel_index*4) + 0];
                        g = t2.data[(t2_pixel_index*4) + 1];
                        b = t2.data[(t2_pixel_index*4) + 2];
                        a = t2.data[(t2_pixel_index*4) + 3];
                    } else {
                        // Empty area
                        r = 0;
                        g = 0;
                        b = 0;
                        a = 0;
                    }
                }
            } break;
            case t1_isTopOf_t2: {
                if (row < t1.height) {
                    if (col < t1.width) {
                        // Current pixel is in t1
                        int t1_row = row;
                        int t1_col = col;
                        int t1_pixel_index = (t1_row * t1.width) + t1_col;
                        r = t1.data[(t1_pixel_index*4) + 0];
                        g = t1.data[(t1_pixel_index*4) + 1];
                        b = t1.data[(t1_pixel_index*4) + 2];
                        a = t1.data[(t1_pixel_index*4) + 3];
                    } else {
                        // Empty area
                        r = 0;
                        g = 0;
                        b = 0;
                        a = 0;
                    }
                } else {
                    currentPixelIsT1 = false;
                    if (col < t2.width) {
                        // Current pixel is in t2
                        int t2_row = row - t1.height;
                        int t2_col = col;
                        int t2_pixel_index = (t2_row * t2.width) + t2_col;
                        r = t2.data[(t2_pixel_index*4) + 0];
                        g = t2.data[(t2_pixel_index*4) + 1];
                        b = t2.data[(t2_pixel_index*4) + 2];
                        a = t2.data[(t2_pixel_index*4) + 3];
                    } else {
                        // Empty area
                        r = 0;
                        g = 0;
                        b = 0;
                        a = 0;
                    }
                }
            } break;
            default: {
                // TODO: manage this issue...
                ce_log_error("Invalid ce_graphics_textureMergeMode");
            } break;
        }

        // write the pixel data to the new image
        out_texture->data[(pixel_index*4) + 0] = r;
        out_texture->data[(pixel_index*4) + 1] = g;
        out_texture->data[(pixel_index*4) + 2] = b;
        out_texture->data[(pixel_index*4) + 3] = a;
    }
    ce_log_debug("Textures merged!");
    ce_profiling_end();
}
#endif

// I use this mostly for debugging
void tu_printTextureData(unsigned char* data, int width, int height, int channels, char channel_to_print) {
    for(int i = 0; i < width*height; i++) {
        int pixel_index = i;
        unsigned char r, g, b, a;
        if (pixel_index % width == 0 && pixel_index != 0) { // First pixel of new "line"
            tu_log("\n");
        }
        if (channels >= 1) { r = data[(pixel_index*channels) + 0]; }
        if (channels >= 2) { g = data[(pixel_index*channels) + 1]; }
        if (channels >= 3) { b = data[(pixel_index*channels) + 2]; }
        if (channels >= 4) { a = data[(pixel_index*channels) + 3]; }
        if ((channels >= 1) && channel_to_print & tu_channel_red) { tu_log("%02x", r); }
        if ((channels >= 2) && channel_to_print & tu_channel_green) { tu_log("%02x", g); }
        if ((channels >= 3) && channel_to_print & tu_channel_blue) { tu_log("%02x", b); }
        if ((channels >= 4) && channel_to_print & tu_channel_alpha) { tu_log("(%02x)", a); }
        tu_log(" ");
    }
}

void tu_embeddTexture(unsigned char* data, int width, int height, int channels) {
    static char hexval[3];
    char* output = tu_malloc((sizeof(char) * height*width*channels*5)+1);
    memset(output,0,(sizeof(char) * height*width*channels*5)+1);
    for(int i = 0; i < width*height; i++) {
        int pixel_index = i;
        unsigned char r, g, b, a;
        // if (pixel_index % width == 0 && pixel_index != 0) { // First pixel of new "line"
        //     tu_log("\n");
        // }
        if (channels >= 1) { r = data[(pixel_index*channels) + 0]; }
        if (channels >= 2) { g = data[(pixel_index*channels) + 1]; }
        if (channels >= 3) { b = data[(pixel_index*channels) + 2]; }
        if (channels >= 4) { a = data[(pixel_index*channels) + 3]; }

        if (channels >= 1) {
            output[(pixel_index*channels*5)+0] = '0';
            output[(pixel_index*channels*5)+1] = 'x';
            sprintf(hexval, "%02x", r);
            strcpy(&output[(pixel_index*channels*5)+2],&hexval);
            output[(pixel_index*channels*5)+4] = ',';
        }
        if (channels >= 2) {
            output[(pixel_index*channels*5)+0+5] = '0';
            output[(pixel_index*channels*5)+1+5] = 'x';
            sprintf(hexval, "%02x", g);
            strcpy(&output[(pixel_index*channels*5)+2+5],&hexval);
            output[(pixel_index*channels*5)+4+5] = ',';
        }
        if (channels >= 3) {
            output[(pixel_index*channels*5)+0+10] = '0';
            output[(pixel_index*channels*5)+1+10] = 'x';
            sprintf(hexval, "%02x", b);
            strcpy(&output[(pixel_index*channels*5)+2+10],&hexval);
            output[(pixel_index*channels*5)+4+10] = ',';
        }
        if (channels >= 4) {
            output[(pixel_index*channels*5)+0+15] = '0';
            output[(pixel_index*channels*5)+1+15] = 'x';
            sprintf(hexval, "%02x", a);
            strcpy(&output[(pixel_index*channels*5)+2+15],&hexval);
            output[(pixel_index*channels*5)+4+15] = ',';
        }
    }
    output[height*width*channels*5] = '\0';
    tu_log("Data:\n%s\nBytes: %d", output, height*width*channels);
    tu_free(output);
    // unsigned char protocols_txt[] = {
    // 0x23, 0x20, 0x2f, 0x65, 0x74, 0x63, 0x2f, 0x70, 0x72, 0x6f, 0x74, 0x6f,
    // 0x63, 0x6f, 0x6c, 0x73, 0x3a, 0x0a, 0x23, 0x20, 0x24, 0x49, 0x64, 0x3a,
    // 0x20, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x63, 0x6f, 0x6c, 0x73, 0x2c, 0x76,
    // ... ...  ... ...  ... ...  ... ...  ... ...  ... ...   
    // 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x5b, 0x49, 0x41,
    // 0x4e, 0x41, 0x5d, 0x0a
    // };
    // unsigned int protocols_txt_len = 6568;
}

int main(int argc, char** argv) {
    tu_log("Texture name \"%s\"\n", argv[1]);
    tu_texture texture = (tu_texture) {0};
    tu_result result = tu_loadTextureFile(argv[1], &texture);
    if (result == tu_success) {
        tu_log("\t.Width = %d\n\t.Height = %d\n\t.Channels = %d\n", texture.width, texture.height, texture.channels);
        // tu_printTextureData(texture.data, texture.width, texture.height, texture.channels, tu_channel_red | tu_channel_green | tu_channel_blue | tu_channel_alpha);
        tu_embeddTexture(texture.data, texture.width, texture.height, texture.channels);
        tu_unloadTexture(texture);
    } else {
        tu_log("Something failed\n");
    }
    return 1;
}