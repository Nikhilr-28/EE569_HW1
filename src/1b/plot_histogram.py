\
import os
import pandas as pd
import matplotlib.pyplot as plt

BASE = r"A:\DIP\HW_1\EE569_HW1\images\1b"
out_dir = os.path.join(BASE, "plots")
os.makedirs(out_dir, exist_ok=True)

# -------- (1) Histogram of original image (BAR PLOT) --------
hist = pd.read_csv(os.path.join(BASE, "hist_original.csv"))

plt.figure()
plt.bar(hist["intensity"], hist["count"], width=1.0)
plt.xlabel("Intensity value")
plt.ylabel("Number of pixels")
plt.title("Histogram of Original Image (airplane.raw)")
plt.xlim(0, 255)
plt.tight_layout()
plt.savefig(os.path.join(out_dir, "1_hist_original.png"), dpi=200)

# -------- (2) Transfer function (Method A) --------
tf = pd.read_csv(os.path.join(BASE, "transfer_A.csv"))

plt.figure()
plt.plot(tf["input"], tf["output"])
plt.xlabel("Input intensity")
plt.ylabel("Output intensity")
plt.title("Transfer Function (Method A)")
plt.xlim(0, 255)
plt.ylim(0, 255)
plt.tight_layout()
plt.savefig(os.path.join(out_dir, "2_transfer_method_A.png"), dpi=200)

# -------- (3) CDF before/after (Method B) --------
cdf = pd.read_csv(os.path.join(BASE, "cdf_B.csv"))

plt.figure()
plt.plot(cdf["intensity"], cdf["cdf_before"], label="Before")
plt.plot(cdf["intensity"], cdf["cdf_after"], label="After")
plt.xlabel("Intensity value")
plt.ylabel("Cumulative probability")
plt.title("Cumulative Histograms Before and After (Method B)")
plt.xlim(0, 255)
plt.legend()
plt.tight_layout()
plt.savefig(os.path.join(out_dir, "3_cdf_before_after_method_B.png"), dpi=200)

print("Saved required plots to:", out_dir)

# -------- (4) Histogram of enhanced image (Method B) --------
# Paths
import numpy as np
BASE = r"A:\DIP\HW_1\EE569_HW1\images\1b"
raw_path = os.path.join(BASE, "airplane_B.raw")

# Image size (given in assignment)
WIDTH, HEIGHT = 1024, 1024

# Read RAW image
img = np.fromfile(raw_path, dtype=np.uint8)
if img.size != WIDTH * HEIGHT:
    raise ValueError("Size mismatch for airplane_B.raw")

# Compute histogram
hist = np.bincount(img, minlength=256)

# Plot BAR histogram
plt.figure()
plt.bar(range(256), hist, width=1.0)
plt.xlabel("Intensity value")
plt.ylabel("Number of pixels")
plt.title("Histogram of Enhanced Image (Method B – Bucket Filling)")
plt.xlim(0, 255)
plt.tight_layout()

# Save
plt.savefig(os.path.join(out_dir, "bucket_fill.png"), dpi=200)

print("Saved required plots to:", out_dir)