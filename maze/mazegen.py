#!/usr/bin/env python3

from PIL import Image
import numpy as np

# Open the maze image and make greyscale, and get its dimensions
im = Image.open('maze.png').convert('L')
w, h = im.size

# Ensure all black pixels are 0 and all white pixels are 1
binary = im.point(lambda p: p > 128 and 1)

# Resize to half its height and width so we can fit on Stack Overflow, get new dimensions
binary = binary.resize((w//4,h//4),Image.NEAREST)
w, h = binary.size

# Convert to Numpy array - because that's how images are best stored and processed in Python
nim = np.array(binary)
maze_array = nim.tolist()
print(maze_array)


