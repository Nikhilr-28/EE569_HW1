import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path

def plot_hist(csv_path, out_png, title):
    df = pd.read_csv(csv_path)

    x = df["value"].to_numpy()
    r = df["R"].to_numpy()
    g = df["G"].to_numpy()
    b = df["B"].to_numpy()

    plt.figure()
    plt.plot(x, r, label="R")
    plt.plot(x, g, label="G")
    plt.plot(x, b, label="B")
    plt.xlim(0, 255)
    plt.xlabel("Intensity (0-255)")
    plt.ylabel("Pixel count")
    plt.title(title)
    plt.legend()
    plt.tight_layout()
    plt.savefig(out_png, dpi=200)
    plt.close()

if __name__ == "__main__":
    out_dir = Path(r"A:\DIP\HW_1\EE569_HW1\images\3")

    before_csv = out_dir / "hist_before.csv"
    after_csv  = out_dir / "hist_after.csv"

    plot_hist(before_csv, out_dir / "hist_before.png", "Histogram Before AWB (Grey World)")
    plot_hist(after_csv,  out_dir / "hist_after.png",  "Histogram After AWB (Grey World)")

    print("Saved:")
    print(out_dir / "hist_before.png")
    print(out_dir / "hist_after.png")
