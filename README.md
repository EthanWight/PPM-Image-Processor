# PPM Image Processor

## Overview

The PPM Image Processor is a C program designed to read, manipulate, and save PPM (Portable Pixmap) images in binary format (P6). It supports various image processing operations, including creating a negative image, color rotation, converting to grayscale, and rotating the image by 180 degrees. 

## Features

- **Read PPM P6 Format**: Supports reading PPM images with the P6 format, including comments in the header.
- **Image Processing Options**:
  - Negative Image
  - Color Rotation (Red -> Green, Green -> Blue, Blue -> Red)
  - Grayscale Conversion
  - 180-Degree Image Rotation
- **Write Processed Images**: Saves the processed image to a new PPM file.

## Requirements

- A C compiler (e.g., GCC)
- Standard C libraries
