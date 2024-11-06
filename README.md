# PPM Image Processor

This program reads, processes, and writes PPM images in the P6 format. It offers options to apply various effects to an image, including generating negative, rotating colors, converting to grayscale, and adjusting contrast. The processed image is saved to a specified output file.

## Features

The program includes the following image processing options:

1. **Negative** - Inverts the colors of the image.
2. **Color Rotate** - Rotates the RGB channels of the image.
3. **Grayscale** - Converts the image to grayscale.
4. **Contrast Scaling** - Adjusts the contrast of the image by a specified scale factor.

## Structure

- **PPMImage struct**: Contains the width, height, max value, and pixel data of the PPM image.
- **Functions**:
  - `read_ppm()`: Reads a PPM file in P6 format.
  - `write_ppm()`: Writes the processed image to a PPM file.
  - `skip_comments()`: Skips comments in the PPM header.
  - `process_negative()`: Applies the negative effect.
  - `process_color_rotate()`: Rotates RGB colors.
  - `process_grayscale()`: Converts the image to grayscale.
  - `process_contrast()`: Adjusts contrast by a given scale factor.
  - `free_ppm()`: Frees the memory allocated for the PPM image.

## Usage

### Compilation

To compile the program, use:

`gcc ppm_processor.c -o ppm_processor`

### Run Program

You can run the program with the following options:

`./ppm_processor <input_file> <output_file>`

or

`./ppm_processor`
