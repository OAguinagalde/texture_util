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

void tu_embeddTexture(unsigned char* data, int width, int height, int channels, char channel_to_print, int verbose) {
    int channels_I_care_about = 0;
    if ((channels >= 1) && channel_to_print & tu_channel_red) {
        channels_I_care_about++;
    }
    if ((channels >= 2) && channel_to_print & tu_channel_green) {
        channels_I_care_about++;
    }
    if ((channels >= 3) && channel_to_print & tu_channel_blue) {
        channels_I_care_about++;
    }
    if ((channels >= 4) && channel_to_print & tu_channel_alpha) {
        channels_I_care_about++;
    }
    static char hexval[3];
    char* output = tu_malloc((sizeof(char) * height*width*channels_I_care_about*5)+1);
    memset(output,0,(sizeof(char) * height*width*channels_I_care_about*5)+1);
    for(int i = 0; i < width*height; i++) {
        int pixel_index = i;
        int channels_written_so_far = 0;
        unsigned char r, g, b, a;
        // if (pixel_index % width == 0 && pixel_index != 0) { // First pixel of new "line"
        //     tu_log("\n");
        // }
        if (channels >= 1) { r = data[(pixel_index*channels) + 0]; }
        if (channels >= 2) { g = data[(pixel_index*channels) + 1]; }
        if (channels >= 3) { b = data[(pixel_index*channels) + 2]; }
        if (channels >= 4) { a = data[(pixel_index*channels) + 3]; }

        if ((channels >= 1) && channel_to_print & tu_channel_red) {
            output[(pixel_index*channels_I_care_about*5)+0+(5*channels_written_so_far)] = '0';
            output[(pixel_index*channels_I_care_about*5)+1+(5*channels_written_so_far)] = 'x';
            sprintf(hexval, "%02x", r);
            strcpy(&output[(pixel_index*channels_I_care_about*5)+2+(5*channels_written_so_far)],&hexval);
            output[(pixel_index*channels_I_care_about*5)+4+(5*channels_written_so_far)] = ',';
            channels_written_so_far++;
        }
        if ((channels >= 2) && channel_to_print & tu_channel_green) {
            output[(pixel_index*channels_I_care_about*5)+0+(5*channels_written_so_far)] = '0';
            output[(pixel_index*channels_I_care_about*5)+1+(5*channels_written_so_far)] = 'x';
            sprintf(hexval, "%02x", g);
            strcpy(&output[(pixel_index*channels_I_care_about*5)+2+(5*channels_written_so_far)],&hexval);
            output[(pixel_index*channels_I_care_about*5)+4+(5*channels_written_so_far)] = ',';
            channels_written_so_far++;
        }
        if ((channels >= 3) && channel_to_print & tu_channel_blue) {
            output[(pixel_index*channels_I_care_about*5)+0+(5*channels_written_so_far)] = '0';
            output[(pixel_index*channels_I_care_about*5)+1+(5*channels_written_so_far)] = 'x';
            sprintf(hexval, "%02x", b);
            strcpy(&output[(pixel_index*channels_I_care_about*5)+2+(5*channels_written_so_far)],&hexval);
            output[(pixel_index*channels_I_care_about*5)+4+(5*channels_written_so_far)] = ',';
            channels_written_so_far++;
        }
        if ((channels >= 4) && channel_to_print & tu_channel_alpha) {
            output[(pixel_index*channels_I_care_about*5)+0+(5*channels_written_so_far)] = '0';
            output[(pixel_index*channels_I_care_about*5)+1+(5*channels_written_so_far)] = 'x';
            sprintf(hexval, "%02x", a);
            strcpy(&output[(pixel_index*channels_I_care_about*5)+2+(5*channels_written_so_far)],&hexval);
            output[(pixel_index*channels_I_care_about*5)+4+(5*channels_written_so_far)] = ',';
            channels_written_so_far++;
        }
    }
    output[height*width*channels_I_care_about*5] = '\0';
    if (verbose) {
        tu_log("Data:\n%s\nBytes: %d", output, height*width*channels_I_care_about);
    } else {
        tu_log("%s", output);
    }
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

typedef enum {
    tu_action_asHex,
    tu_action_printChannel,
    tu_action_mergeHorizontal
} tu_action;

int main(int argc, char** argv) {
    // Default actions
    tu_action action_singleTexture = tu_action_asHex;
    tu_action action_doubleTexture = tu_action_mergeHorizontal;
    // Texture names
    char* texture1 = NULL;
    char* texture2 = NULL;
    // channels
    char channels = tu_channel_red | tu_channel_green | tu_channel_blue | tu_channel_alpha;
    int channels_is_modified = 0;
    // silent
    int silent_output = 0;

    // Figure out the input
    int input_pathcount = 0;
    char* tool = argv[0];
    // tu_log("argc: %d\n", argc);
    for(int i = 1; i < argc; i++) {
        char* argument = argv[i];
        // tu_log("argument %d: %s\n", i, argument);

        if (argument[0] == '-') {
            // It's an option
            int option_len = strlen(argument);
            if (option_len > 1) {
                // tu_log("option: %s\n", &argument[1]);
                for (int option_index = 0; option_index < option_len-1; option_index++) {
                    char option = argument[1+option_index];
                    // tu_log("option: %c", option);
                    switch (option) {
                        case 'h': {
                            action_singleTexture = tu_action_asHex;
                        } break;
                        case 'p': {
                            action_singleTexture = tu_action_printChannel;
                        } break;
                        case 's': {
                            silent_output = 1;
                        } break;
                        case 'r': {
                            if (channels_is_modified == 0) {
                                channels_is_modified = 1;
                                channels = 0;
                            }
                            channels |= tu_channel_red;
                        } break;
                        case 'g': {
                            if (channels_is_modified == 0) {
                                channels_is_modified = 1;
                                channels = 0;
                            }
                            channels |= tu_channel_green;
                        } break;
                        case 'b': {
                            if (channels_is_modified == 0) {
                                channels_is_modified = 1;
                                channels = 0;
                            }
                            channels |= tu_channel_blue;
                        } break;
                        case 'a': {
                            if (channels_is_modified == 0) {
                                channels_is_modified = 1;
                                channels = 0;
                            }
                            channels |= tu_channel_alpha;
                        } break;
                    }
                }
            }
        }
        else {
            // tu_log("path: %s\n", argument);
            if (input_pathcount == 0) {
                texture1 = argument;
            }
            else {
                texture2 = argument;
            }
            input_pathcount++;
        }
    }

    switch (input_pathcount) {
        case 0: {
            tu_log("No path supplied!\n");
        } break;
        case 1: {
            if (!silent_output) tu_log("Texture name \"%s\"\n", texture1);
            tu_texture texture = (tu_texture) {0};
            tu_result result = tu_loadTextureFile(texture1, &texture);

            if (result == tu_success) {
                if (!silent_output) tu_log("\t.Width = %d\n\t.Height = %d\n\t.Channels = %d\n", texture.width, texture.height, texture.channels);
                switch (action_singleTexture)
                {
                    case tu_action_asHex: {
                        if (!silent_output) tu_log("Data as hex values...\n");
                        tu_embeddTexture(texture.data, texture.width, texture.height, texture.channels, channels, !silent_output);
                    } break;
                    case tu_action_printChannel: {
                        if (!silent_output) tu_log("Image values values...\n");
                        tu_printTextureData(texture.data, texture.width, texture.height, texture.channels, channels);
                    } break;
                }
                tu_unloadTexture(texture);
            } else {
                tu_log("Something failed\n");
            }
        } break;
        case 2: {
            // handle merging images for atlases and stuff
            tu_log("Two images supplied\n");
        } break;
        default: {
            tu_log("Too many paths supplied\n");
            return -1;
        }
    }
    return 1;
}