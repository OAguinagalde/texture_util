#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define stb_
#define std_
#define app_

#define byte_isBitSet(byte, bit) ((1 << bit) & byte)
#define bit_flag(bitIndex) 1 << bitIndex
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define bool int
#define true ((int)1)
#define false ((int)0)

typedef enum application_action {
    do_embed,
    do_printDebug,
    do_showHelp,
    do_merge
} application_action;

typedef struct configuration_t {
    char* image_target1;
    char* image_target2;
    char target_channels;
    int desired_width, desired_height;
    bool custom_channels;
    bool debug_output;
    application_action action;
} configuration_t;

typedef struct image_t {
    unsigned char* data;
    int width, height, channels, data_size;
} image_t;

typedef enum image_mergeMode {
    // Image A left of image B
    // ·-··-·
    // |A||B|
    // ·-··-·
    image_mergeMode1,
    // Image A on top of Image B
    // ·-·
    // |A|
    // ·-·
    // ·-·
    // |B|
    // ·-·
    image_mergeMode2
} image_mergeMode;

typedef enum pixel_channel {
    pixel_channel_red = bit_flag(0),
    pixel_channel_green = bit_flag(1),
    pixel_channel_blue = bit_flag(2),
    pixel_channel_alpha = bit_flag(3)
} pixel_channel;

void image_printDebug(unsigned char* image_data, int image_width, int image_height, int image_channels, char desired_channels, int desired_width, int desired_height) {
    int image_pixelCount = image_width*image_height;
    for(int pixel_index = 0; pixel_index < image_pixelCount; pixel_index++) {
        int pixel_index_x = pixel_index % image_width;
        int pixel_index_y = pixel_index / image_width;
        
        if (pixel_index_x >= desired_width) { continue; }
        if (pixel_index_y >= desired_height) { break; }

        if (pixel_index_x == 0 && pixel_index != 0) {
            std_ printf("\n");
        }

        unsigned char pixel_r, pixel_g, pixel_b, pixel_a;
        if (image_channels >= 1) { pixel_r = image_data[(pixel_index*image_channels) + 0]; }
        if (image_channels >= 2) { pixel_g = image_data[(pixel_index*image_channels) + 1]; }
        if (image_channels >= 3) { pixel_b = image_data[(pixel_index*image_channels) + 2]; }
        if (image_channels >= 4) { pixel_a = image_data[(pixel_index*image_channels) + 3]; }

        if ((image_channels >= 1) && desired_channels & pixel_channel_red   ) { std_ printf("%02x",   pixel_r); }
        if ((image_channels >= 2) && desired_channels & pixel_channel_green ) { std_ printf("%02x",   pixel_g); }
        if ((image_channels >= 3) && desired_channels & pixel_channel_blue  ) { std_ printf("%02x",   pixel_b); }
        if ((image_channels >= 4) && desired_channels & pixel_channel_alpha ) { std_ printf("(%02x)", pixel_a); }
        std_ printf(" ");
    }
}

void image_embedToC(unsigned char* image_data, int image_width, int image_height, int image_channels, char desired_channels, char* image_name) {
    // Since the output is constructed on a char array, I have to calculate how big it is first...
    int channels_count = 0;
    if ((image_channels >= 1) && desired_channels & pixel_channel_red  ) { channels_count++; }
    if ((image_channels >= 2) && desired_channels & pixel_channel_green) { channels_count++; }
    if ((image_channels >= 3) && desired_channels & pixel_channel_blue ) { channels_count++; }
    if ((image_channels >= 4) && desired_channels & pixel_channel_alpha) { channels_count++; }
    
    unsigned long long output_size = 0;
    output_size += sizeof(char) * (image_height*image_width) * channels_count; // Count of bytes: pixels * channels (1 byte per channel of a pixel)
    output_size *= 5; // Each byte will be represented by 5 characters "0x00,"
    output_size++; // For the null terminator
    
    char* output = std_ calloc(1, output_size);    
    
    char hex_value[3]; // 2 characters representing the a given byte and a null terminator
    int image_pixelCount = image_width*image_height;
    for(int pixel_index = 0; pixel_index < image_pixelCount; pixel_index++) {
        int pixel_index_x = pixel_index % image_width;
        int pixel_index_y = pixel_index / image_width;
        int channels_written = 0;

        unsigned char r, g, b, a;
        if (image_channels >= 1) { r = image_data[(pixel_index*image_channels) + 0]; }
        if (image_channels >= 2) { g = image_data[(pixel_index*image_channels) + 1]; }
        if (image_channels >= 3) { b = image_data[(pixel_index*image_channels) + 2]; }
        if (image_channels >= 4) { a = image_data[(pixel_index*image_channels) + 3]; }

        if ((image_channels >= 1) && desired_channels & pixel_channel_red) {
            output[(pixel_index*channels_count*5)+0+(5*channels_written)] = '0';
            output[(pixel_index*channels_count*5)+1+(5*channels_written)] = 'x';
            std_ sprintf(hex_value, "%02x", r);
            std_ strcpy(&output[(pixel_index*channels_count*5)+2+(5*channels_written)],(const char*)&hex_value);
            output[(pixel_index*channels_count*5)+4+(5*channels_written)] = ',';
            channels_written++;
        }
        if ((image_channels >= 2) && desired_channels & pixel_channel_green) {
            output[(pixel_index*channels_count*5)+0+(5*channels_written)] = '0';
            output[(pixel_index*channels_count*5)+1+(5*channels_written)] = 'x';
            std_ sprintf(hex_value, "%02x", g);
            std_ strcpy(&output[(pixel_index*channels_count*5)+2+(5*channels_written)],(const char*)&hex_value);
            output[(pixel_index*channels_count*5)+4+(5*channels_written)] = ',';
            channels_written++;
        }
        if ((image_channels >= 3) && desired_channels & pixel_channel_blue) {
            output[(pixel_index*channels_count*5)+0+(5*channels_written)] = '0';
            output[(pixel_index*channels_count*5)+1+(5*channels_written)] = 'x';
            std_ sprintf(hex_value, "%02x", b);
            std_ strcpy(&output[(pixel_index*channels_count*5)+2+(5*channels_written)],(const char*)&hex_value);
            output[(pixel_index*channels_count*5)+4+(5*channels_written)] = ',';
            channels_written++;
        }
        if ((image_channels >= 4) && desired_channels & pixel_channel_alpha) {
            output[(pixel_index*channels_count*5)+0+(5*channels_written)] = '0';
            output[(pixel_index*channels_count*5)+1+(5*channels_written)] = 'x';
            std_ sprintf(hex_value, "%02x", a);
            std_ strcpy(&output[(pixel_index*channels_count*5)+2+(5*channels_written)],(const char*)&hex_value);
            output[(pixel_index*channels_count*5)+4+(5*channels_written)] = ',';
            channels_written++;
        }
    }
    output[image_pixelCount*channels_count*5] = '\0';
    std_ printf("// %s\n", image_name);  
    std_ printf("const int texture_width = %d;\n", image_width);
    std_ printf("const int texture_height = %d;\n", image_height);
    std_ printf("const int texture_channels = %d;\n", channels_count);
    std_ printf("const int texture_data_size = %d;\n", image_pixelCount*channels_count);
    std_ printf("const unsigned char texture_data[%d] = {\n    %s\n};\n", image_pixelCount*channels_count, output);
    std_ free(output);
}

int main(int argc, char** argv) {
    configuration_t configuration = (configuration_t) { 0 };
    // Defaults
    configuration.desired_width = 0;
    configuration.desired_height = 0;
    configuration.debug_output = false;
    configuration.custom_channels = false;
    configuration.action = do_embed;
    configuration.image_target1 = NULL;
    configuration.image_target2 = NULL;
    configuration.target_channels = pixel_channel_red | pixel_channel_green | pixel_channel_blue | pixel_channel_alpha;

    // parse the input
    for(int argument_index = 1; argument_index < argc; argument_index++) {
        char* argument = argv[argument_index];
        if (argument[0] == '-') {
            // It's an option
            int option_len = std_ strlen(argument);
            if (option_len > 1) {
                for (int option_index = 0; option_index < option_len-1; option_index++) {
                    char option = argument[1+option_index];
                    switch (option) {
                        case 'e': {
                            configuration.action = do_embed;
                        } break;
                        case 'p': {
                            configuration.action = do_printDebug;
                        } break;
                        case 'h': {
                            configuration.action = do_showHelp;
                        } break;
                        case 'd': {
                            configuration.debug_output = true;
                        } break;
                        case 'l': {
                            // For debugging purposes, when an image is very big, use option -l
                            // to display only a 30x30 piece of the image
                            configuration.desired_width = 30;
                            configuration.desired_height = 30;
                        } break;
                        case 's': {
                            configuration.debug_output = false;
                        } break;
                        case 'r': {
                            if (configuration.custom_channels == false) {
                                configuration.custom_channels = true;
                                configuration.target_channels = 0;
                            }
                            configuration.target_channels |= pixel_channel_red;
                        } break;
                        case 'g': {
                            if (configuration.custom_channels == false) {
                                configuration.custom_channels = true;
                                configuration.target_channels = 0;
                            }
                            configuration.target_channels |= pixel_channel_green;
                        } break;
                        case 'b': {
                            if (configuration.custom_channels == false) {
                                configuration.custom_channels = true;
                                configuration.target_channels = 0;
                            }
                            configuration.target_channels |= pixel_channel_blue;
                        } break;
                        case 'a': {
                            if (configuration.custom_channels == false) {
                                configuration.custom_channels = true;
                                configuration.target_channels = 0;
                            }
                            configuration.target_channels |= pixel_channel_alpha;
                        } break;
                    }
                }
            }
        }
        else {
            // If the argument is not an option then it has to be an image file
            // the application works maximum with 2 images
            if (configuration.image_target1 == NULL) {
                configuration.image_target1 = argument;
            }
            else {
                configuration.image_target2 = argument;
            }
        }
    }

    if (configuration.action == do_showHelp || configuration.image_target1 == NULL) {
        //          "....................................................................................." 85 character limit for the help message
        std_ printf("Usage: main.exe (-[ephdlsrgba]+)? ImageFile\n");
        std_ printf("   -e       (Default) Outputs ready to use code for Embedding into a C project.\n");
        std_ printf("   -p       Prints the pixel data directly in the forma \"RRGGBB(AA)\", using the hex\n            values. Useful for debugging. Check option -l.\n");
        std_ printf("   -h       Display this help message and finish.\n\n");
        std_ printf("   -[rgba]  (Default) Change the desired channels. Defaults to all 4 (RGBA).\n");
        std_ printf("   -s       (Default) Silences some of the debug output.\n");
        std_ printf("   -d       Show all the debug output.\n");
        std_ printf("   -l       When in combination with option -p, Limits the ammount of pixels\n            displayed to 30x30.\n\n");
        std_ printf("Example: main.exe ./test.png -plrgb.\n");
        std_ printf("   This command will display, in a more friendly format (-p) the RGB pixel data\n   (-rgb) of the top left 30x30 pixels (-l) of the image test.png\n");
        return -1;
    }

    switch (configuration.action) {
        case do_embed:
        case do_printDebug: {
            if (configuration.debug_output) std_ printf("Texture name \"%s\"\n", configuration.image_target1);
            image_t image = (image_t) {0};
            image.data = stb_ stbi_load(configuration.image_target1, &image.width, &image.height, &image.channels, 0);
            if (image.data) {
                if (configuration.debug_output) std_ printf("\t.Width = %d\n\t.Height = %d\n\t.Channels = %d\n", image.width, image.height, image.channels);
                
                if (configuration.desired_width <= 0) {
                    configuration.desired_width = image.width;
                }
                else {
                    configuration.desired_width = min(image.width, configuration.desired_width);
                }

                if (configuration.desired_height <= 0) {
                    configuration.desired_height = image.height;
                }
                else {
                    configuration.desired_height = min(image.height, configuration.desired_height);
                }

                // if (!silent_output) printf("Data as hex values...\n");
                // tu_embeddTexture(texture.data, texture.width, texture.height, texture.channels, channels, !silent_output);
                if (configuration.debug_output) std_ printf("Image values values...\n");
                if (configuration.action == do_embed) {              
                    app_ image_embedToC(image.data, image.width, image.height, image.channels, configuration.target_channels, configuration.image_target1);
                }
                if (configuration.action == do_printDebug) {
                    app_ image_printDebug(image.data, image.width, image.height, image.channels, configuration.target_channels, configuration.desired_width, configuration.desired_height);
                }
                stb_ stbi_image_free(image.data);
            } else {
                std_ printf("Couldn't read the image file!\n");
            }
        } break;

        case do_merge: {
            // handle merging images for atlases and stuff
            std_ printf("Two images supplied\n");
        } break;
        default: {
            std_ printf("Something went wrong!\n");
            return -1;
        }
    }
    return 1;
}