#
# This script is a utility for the ILI9488-Xilinx TFT display library.
# See https://github.com/viktor-nikolov/ILI9488-Xilinx
#
# The script reads an image file (any RGB image format supported by the "Image" library of the PIL package)
# and outputs to the standard output a constant in C++ (array of bytes), which contains image data in
# the color coding R:G:B 5b:6b:5b.
# This constant is meant to be used by the method ILI9488::drawImage565, which shows the image
# on the display.
#
# usage: python ImageTo565ColorConst.py <input_image_path>
#
# requires PIL package, which can be installed by the command: python -m pip install Pillow
#

from PIL import Image
import re
import os
import sys


def get_normalized_file_name(path):
    # Get image filename in the form, which can serve as a variable name in C++
    file_name = os.path.basename(path)
    file_name = file_name.replace(' ', '_')
    file_name = file_name.replace('.', '_')
    file_name = re.sub(r'[^a-zA-Z0-9_]', '', file_name)
    return file_name


def color565(r, g, b):
    # Pass 8-bit (each) R,G,B, get back 16-bit packed color as array of two bytes
    color = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)
    return [color >> 8, color & 0xFF]


if sys.version_info < (3, 8): # assignment expressions is supported only from Python 3.8
    print('ERROR: this script requires Python version at least 3.8')
    exit(1)

if len(sys.argv) == 1:
    print('usage: python ImageTo565ColorConst.py <input_image_path>')
    print('ERROR: input imapage path not specified')
    exit(1)

image_path = sys.argv[1]
image = Image.open(image_path)
width, height = image.size
image_name = get_normalized_file_name(image_path)

image_data = image.getdata()
if image_data.mode != 'RGB':
    print('ERROR: the image is not in RGB mode')
    exit(1)

image_data = list(image_data)

print(f"const uint16_t {image_name}_width  = {width};")
print(f"const uint16_t {image_name}_height = {height};")
print(f"const uint32_t {image_name}_pixels = {width * height};")
print(f"const uint32_t {image_name}_bytes  = {2 * width * height};\n")
print(f"const uint8_t {image_name}_image565[ {image_name}_bytes ] = {{")

i = iter(image_data)  # iterator through image data
d = next(i)           # getting first element of the data
color = color565(d[0], d[1], d[2])
print(f"{color[0]},{color[1]}", end="")

MAX_PIXELS_PER_LINE = 45
pixels_per_line_count = 1

while (d := next(i, None)) is not None: # going through data (starting with the second element) using iterator i
    print(",", end="")
    if pixels_per_line_count == MAX_PIXELS_PER_LINE:
        print("")
        pixels_per_line_count = 1
    else:
        pixels_per_line_count += 1
    color = color565(d[0], d[1], d[2])
    print(f"{color[0]},{color[1]}", end="")

print(" };")

image.close()
