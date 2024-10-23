#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Function declarations
void process_negative(unsigned char *image_data, int width, int height, int max_val);
void process_color_rotate(unsigned char *image_data, int width, int height);
void process_grayscale(unsigned char *image_data, int width, int height);
void process_flip_180(unsigned char *image_data, int width, int height);

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

// Function to read the header and handle errors
int read_header(FILE *fp, int *width, int *height, int *max_val) {
    char format[3];

    // Read format identifier (P6)
    if (fscanf(fp, "%2s", format) != 1 || strcmp(format, "P6") != 0) {
        fprintf(stderr, "Error: Unsupported file format\n");
        return 0;
    }
    printf("Format: %s\n", format);  // Debug: Print format

    // Skip comments and read width
    skip_comments(fp);
    if (fscanf(fp, "%d", width) != 1 || *width <= 0) {
        fprintf(stderr, "Error: Invalid width. Last read value: '%d'\n", *width);
        return 0;
    }
    printf("Width: %d\n", *width);  // Debug: Print width

    // Skip comments and read height
    skip_comments(fp);
    if (fscanf(fp, "%d", height) != 1 || *height <= 0) {
        fprintf(stderr, "Error: Invalid height. Last read value: '%d'\n", *height);
        return 0;
    }
    printf("Height: %d\n", *height);  // Debug: Print height

    // Skip comments and read max_val
    skip_comments(fp);
    if (fscanf(fp, "%d", max_val) != 1 || *max_val <= 0 || *max_val > 255) {
        fprintf(stderr, "Error: Invalid max pixel value. Last read value: '%d'\n", *max_val);
        return 0;
    }
    printf("Max value: %d\n", *max_val);  // Debug: Print max_val

    // Skip one last newline character
    while (fgetc(fp) != '\n'); // Loop to skip any trailing newlines or spaces

    return 1;
}

int main(int argc, char *argv[]) {
    char src_filename[256], dst_filename[256];
    FILE *fp_in, *fp_out;
    int width, height, max_val;
    unsigned char *image_data;

    // Handle command-line arguments or prompt for filenames
    if (argc < 3) {
        printf("Enter source filename: ");
        scanf("%s", src_filename);
        printf("Enter destination filename: ");
        scanf("%s", dst_filename);
    } else {
        strcpy(src_filename, argv[1]);
        strcpy(dst_filename, argv[2]);
    }

    // Open the source file
    fp_in = fopen(src_filename, "rb");
    if (fp_in == NULL) {
        fprintf(stderr, "Error: Unable to open source file %s\n", src_filename);
        return 1;
    }

    // Read the header
    if (!read_header(fp_in, &width, &height, &max_val)) {
        fclose(fp_in);
        return 1;
    }

    // Allocate memory for image data
    image_data = (unsigned char *)malloc(3 * width * height * sizeof(unsigned char));
    if (image_data == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory\n");
        fclose(fp_in);
        return 1;
    }

    // Read the image data (binary format)
    if (fread(image_data, sizeof(unsigned char), 3 * width * height, fp_in) != 3 * width * height) {
        fprintf(stderr, "Error: Failed to read image data\n");
        free(image_data);
        fclose(fp_in);
        return 1;
    }
    fclose(fp_in);

    // Ask user which processing option to apply
    int option;
    printf("Choose an image processing option:\n");
    printf("1. Negative\n");
    printf("2. Color rotate\n");
    printf("3. Grayscale\n");
    printf("4. Rotate 180 degrees\n");
    printf("Enter your choice (1-4): ");
    scanf("%d", &option);

    switch (option) {
        case 1:
            process_negative(image_data, width, height, max_val);
            break;
        case 2:
            process_color_rotate(image_data, width, height);
            break;
        case 3:
            process_grayscale(image_data, width, height);
            break;
        case 4:
            process_flip_180(image_data, width, height);
            break;
        default:
            fprintf(stderr, "Error: Invalid option\n");
            free(image_data);
            return 1;
    }

    // Open the destination file
    fp_out = fopen(dst_filename, "wb");
    if (fp_out == NULL) {
        fprintf(stderr, "Error: Unable to open destination file %s\n", dst_filename);
        free(image_data);
        return 1;
    }

    // Write the PPM header
    fprintf(fp_out, "P6\n%d %d\n%d\n", width, height, max_val);

    // Write the processed image data
    fwrite(image_data, sizeof(unsigned char), 3 * width * height, fp_out);
    fclose(fp_out);

    // Print the header after processing is complete
    printf("Header: P6 %d %d %d\n", width, height, max_val);

    // Free the allocated memory
    free(image_data);

    printf("Image processed and saved to %s\n", dst_filename);
    return 0;
}

// Function to process the image into a negative
void process_negative(unsigned char *image_data, int width, int height, int max_val) {
    for (int i = 0; i < 3 * width * height; i++) {
        image_data[i] = max_val - image_data[i];
    }
}

// Function to rotate colors (Red -> Green, Green -> Blue, Blue -> Red)
void process_color_rotate(unsigned char *image_data, int width, int height) {
    for (int i = 0; i < 3 * width * height; i += 3) {
        unsigned char temp = image_data[i];  // Red
        image_data[i] = image_data[i + 1];   // Red <- Green
        image_data[i + 1] = image_data[i + 2];  // Green <- Blue
        image_data[i + 2] = temp;  // Blue <- Red
    }
}

// Function to process the image into grayscale
void process_grayscale(unsigned char *image_data, int width, int height) {
    for (int i = 0; i < 3 * width * height; i += 3) {
        unsigned char gray = (image_data[i] + image_data[i + 1] + image_data[i + 2]) / 3;
        image_data[i] = gray;
        image_data[i + 1] = gray;
        image_data[i + 2] = gray;
    }
}

// Function to rotate the image by 180 degrees
void process_flip_180(unsigned char *image_data, int width, int height) {
    for (int i = 0, j = 3 * width * height - 3; i < j; i += 3, j -= 3) {
        // Swap pixels
        for (int k = 0; k < 3; k++) {
            unsigned char temp = image_data[i + k];
            image_data[i + k] = image_data[j + k];
            image_data[j + k] = temp;
        }
    }
}