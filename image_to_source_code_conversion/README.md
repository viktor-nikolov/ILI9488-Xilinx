# Conversion of images into C++ source code

The two Python scripts in this folder read an image file (any RGB image format supported by the "Image" module of [the Pillow library](https://pillow.readthedocs.io/en/stable/)) and write to the standard output definition of a constant in C++ (an array of bytes), which contains image data in the color coding R:G:B 5b:6b:5b or 8b:8b:8b.

This constant is meant to be used by the method ILI9488::drawImage565 or ILI9488::drawImage888, which shows the image on the display.

Requires Python version at least 3.8 and the Pillow library, which can be installed by the command `python -m pip install Pillow`.

Usage:

```
python ImageTo565ColorConst.py <input_image_path>
python ImageTo888ColorConst.py <input_image_path>
```