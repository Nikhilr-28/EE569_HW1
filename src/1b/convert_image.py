'''
/*
Name: Nikhil Ravichandran
ID: 8515860149
Email: nikhilr@usc.edu
Date: February 1, 2026
*/

'''
import os
import numpy as np
import matplotlib.pyplot as plt

BASE = r"A:\DIP\HW_1\EE569_HW1\images\1b"
out_dir = os.path.join(BASE, "plots")
os.makedirs(out_dir, exist_ok=True)

WIDTH, HEIGHT = 1024, 1024

def read_u8_raw(path, w, h):
    data = np.fromfile(path, dtype=np.uint8)
    if data.size != w*h:
        raise ValueError(f"Size mismatch for {path}")
    return data.reshape((h, w))

orig = read_u8_raw(r"A:\DIP\HW_1\EE569_HW1\images\airplane.raw", WIDTH, HEIGHT)
imgA = read_u8_raw(os.path.join(BASE, "airplane_A.raw"), WIDTH, HEIGHT)
imgB = read_u8_raw(os.path.join(BASE, "airplane_B.raw"), WIDTH, HEIGHT)

plt.figure()
plt.imshow(orig, cmap="gray", vmin=0, vmax=255)
plt.axis("off")
plt.title("Original Image")
plt.tight_layout()
plt.savefig(os.path.join(out_dir, "orig_airplane.png"), dpi=200)

plt.figure()
plt.imshow(imgA, cmap="gray", vmin=0, vmax=255)
plt.axis("off")
plt.title("Enhanced Image - Method A")
plt.tight_layout()
plt.savefig(os.path.join(out_dir, "airplane_method_A.png"), dpi=200)

plt.figure()
plt.imshow(imgB, cmap="gray", vmin=0, vmax=255)
plt.axis("off")
plt.title("Enhanced Image - Method B")
plt.tight_layout()
plt.savefig(os.path.join(out_dir, "airplane_method_B.png"), dpi=200)

print("Saved image figures to:", out_dir)
