import cv2
import numpy as np

def analyze_ui(image_path):
    img = cv2.imread(image_path)
    if img is None:
        print("Failed to load image")
        return

    height, width = img.shape[:2]
    print(f"IMAGE_RES: {width} x {height}")

    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    gray_blurred = cv2.medianBlur(gray, 5)
    
    # Stricter detection
    circles = cv2.HoughCircles(gray_blurred, cv2.HOUGH_GRADIENT, dp=1, minDist=80,
                               param1=50, param2=30, minRadius=20, maxRadius=200)

    knobs = []
    if circles is not None:
        circles = np.round(circles[0, :]).astype("int")
        for (x, y, r) in circles:
            # Filter out top screws or unlikely positions
            if y < height * 0.3: continue 
            # Filter huge circles (likely background curves)
            if r > 100: continue
            knobs.append((x, y, r))

    # Sort knobs by X
    knobs.sort(key=lambda k: k[0])

    print("\n--- DETECTED KNOB CANDIDATES ---")
    for k in knobs:
        print(f"Knob: x={k[0]}, y={k[1]}, r={k[2]}, d={k[2]*2}")

if __name__ == "__main__":
    analyze_ui("/Users/manato/.gemini/antigravity/brain/85765bd3-5e45-420e-b4a6-e3ee8b885a36/uploaded_media_1769505247676.png")
