#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Add the PPM structure definition at the top
typedef struct {
    unsigned char *data;
    int width;
    int height;
    int max_val;
} PPMImage;

// Function declarations
void process_negative(PPMImage *img);
void process_color_rotate(PPMImage *img);
void process_grayscale(PPMImage *img);
void process_contrast(PPMImage *img, float scale_factor);
PPMImage* read_ppm(const char *filename);
void write_ppm(const char *filename, PPMImage *img);
void free_ppm(PPMImage *img);

// Function to skip comments in the header
void skip_comments(FILE *fp) {
    int ch;
    // Read until a non-comment character is found
    while (1) {
        ch = fgetc(fp);
        if (ch == '#') {
            // If we find a comment, read until the end of the line
            while ((ch = fgetc(fp)) != '\n' && ch != EOF);
        } else if (ch == EOF) {
            // Handle unexpected EOF
            fprintf(stderr, "Error: Unexpected EOF while skipping comments\n");
            exit(1);
        } else if (!isspace(ch)) {
            // If we find a non-whitespace character, push it back to the stream
            ungetc(ch, fp);
            break; // Exit the loop to continue processing
        }
    }
}

// Modified read_ppm function
PPMImage* read_ppm(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "Error: Unable to open file %s\n", filename);
        return NULL;
    }

    PPMImage *img = malloc(sizeof(PPMImage));
    if (!img) {
        fclose(fp);
        return NULL;
    }

    char format[3];
    if (fscanf(fp, "%2s", format) != 1 || strcmp(format, "P6") != 0) {
        fprintf(stderr, "Error: Unsupported file format\n");
        free(img);
        fclose(fp);
        return NULL;
    }

    skip_comments(fp);
    if (fscanf(fp, "%d", &img->width) != 1 || img->width <= 0) {
        fprintf(stderr, "Error: Invalid width\n");
        free(img);
        fclose(fp);
        return NULL;
    }

    skip_comments(fp);
    if (fscanf(fp, "%d", &img->height) != 1 || img->height <= 0) {
        fprintf(stderr, "Error: Invalid height\n");
        free(img);
        fclose(fp);
        return NULL;
    }

    skip_comments(fp);
    if (fscanf(fp, "%d", &img->max_val) != 1 || img->max_val <= 0 || img->max_val > 255) {
        fprintf(stderr, "Error: Invalid max value\n");
        free(img);
        fclose(fp);
        return NULL;
    }

    // Skip one whitespace character after max_val
    fgetc(fp);

    // Allocate memory for image data
    img->data = malloc(3 * img->width * img->height);
    if (!img->data) {
        fprintf(stderr, "Error: Unable to allocate memory\n");
        free(img);
        fclose(fp);
        return NULL;
    }

    // Read image data
    if (fread(img->data, 1, 3 * img->width * img->height, fp) != 3 * img->width * img->height) {
        fprintf(stderr, "Error: Failed to read image data\n");
        free(img->data);
        free(img);
        fclose(fp);
        return NULL;
    }

    fclose(fp);
    return img;
}

// New contrast scaling function
void process_contrast(PPMImage *img, float scale_factor) {
    unsigned char *ptr = img->data;
    int size = 3 * img->width * img->height;
    
    for (int i = 0; i < size; i++, ptr++) {
        float pixel = *ptr;
        // Center around zero, scale, then shift back
        pixel = (pixel - 128) * scale_factor + 128;
        // Clamp values
        if (pixel > img->max_val) pixel = img->max_val;
        if (pixel < 0) pixel = 0;
        *ptr = (unsigned char)pixel;
    }
}

// Modified main function
int main(int argc, char *argv[]) {
    char src_filename[256], dst_filename[256];

    // Handle command-line arguments or prompt for filenames
    if (argc < 3) {
        printf("Enter source filename: ");
        scanf(" %[^\n]%*c", src_filename);
        printf("Enter destination filename: ");
        scanf(" %[^\n]%*c", dst_filename);
    } else {
        strcpy(src_filename, argv[1]);
        strcpy(dst_filename, argv[2]);
    }

    PPMImage *img = read_ppm(src_filename);
    if (!img) {
        return 1;
    }

    printf("Image loaded: %dx%d pixels, max value: %d\n", 
           img->width, img->height, img->max_val);

    int option;
    printf("Choose an image processing option:\n");
    printf("1. Negative\n");
    printf("2. Color rotate\n");
    printf("3. Grayscale\n");
    printf("4. Contrast scaling\n");
    printf("Enter your choice (1-4): ");
    scanf("%d", &option);

    switch (option) {
        case 1:
            process_negative(img);
            break;
        case 2:
            process_color_rotate(img);
            break;
        case 3:
            process_grayscale(img);
            break;
        case 4: {
            float scale_factor;
            printf("Enter contrast scale factor (0.1-3.0): ");
            scanf("%f", &scale_factor);
            process_contrast(img, scale_factor);
            break;
        }
        default:
            fprintf(stderr, "Error: Invalid option\n");
            free_ppm(img);
            return 1;
    }

    // Write the processed image
    FILE *fp_out = fopen(dst_filename, "wb");
    if (!fp_out) {
        fprintf(stderr, "Error: Unable to open destination file\n");
        free_ppm(img);
        return 1;
    }

    fprintf(fp_out, "P6\n%d %d\n%d\n", img->width, img->height, img->max_val);
    fwrite(img->data, 1, 3 * img->width * img->height, fp_out);
    fclose(fp_out);

    printf("Image processed and saved to %s\n", dst_filename);
    free_ppm(img);
    return 0;
}

// Modified processing functions to use the PPM structure
void process_negative(PPMImage *img) {
    unsigned char *ptr = img->data;
    int size = 3 * img->width * img->height;
    
    for (int i = 0; i < size; i++, ptr++) {
        *ptr = img->max_val - *ptr;
    }
}

void process_color_rotate(PPMImage *img) {
    unsigned char *ptr = img->data;
    int size = img->width * img->height;
    
    for (int i = 0; i < size; i++, ptr += 3) {
        unsigned char temp = *ptr;
        *ptr = *(ptr + 1);
        *(ptr + 1) = *(ptr + 2);
        *(ptr + 2) = temp;
    }
}

void process_grayscale(PPMImage *img) {
    unsigned char *ptr = img->data;
    int size = img->width * img->height;
    
    for (int i = 0; i < size; i++, ptr += 3) {
        unsigned char gray = (ptr[0] + ptr[1] + ptr[2]) / 3;
        ptr[0] = ptr[1] = ptr[2] = gray;
    }
}

void free_ppm(PPMImage *img) {
    if (img) {
        free(img->data);
        free(img);
    }
}
