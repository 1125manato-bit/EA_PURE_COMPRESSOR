import cv2
import numpy as np

def analyze_ui(image_path):
    img = cv2.imread(image_path)
    if img is None:
        print("Failed to load image")
        return

    height, width = img.shape[:2]
    print(f"Resolution: {width} x {height}")

    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    
    # 1. Find Circles (Knobs)
    # Blur to reduce noise
    gray_blurred = cv2.medianBlur(gray, 5)
    
    # Detect circles
    # param1: Canny higher threshold
    # param2: Accumulator threshold (lower = more circles)
    # minRadius/maxRadius: tuned based on expected size (Large ~180px, Small ~120px) but let's be wider
    circles = cv2.HoughCircles(gray_blurred, cv2.HOUGH_GRADIENT, dp=1, minDist=50,
                               param1=50, param2=30, minRadius=20, maxRadius=150)

    if circles is not None:
        circles = np.round(circles[0, :]).astype("int")
        print("\n--- Detected Knobs ---")
        # Identify knobs based on position and size
        # Threshold: Large, Left
        # Gain: Large, Right
        # Ratio: Small, Center Top
        # Attack: Small, Center Left
        # Release: Small, Center Right
        
        for (x, y, r) in circles:
            d = r * 2
            knob_type = "Unknown"
            
            # Simple heuristic classification
            if d > 120: # Large
                if x < width / 3: knob_type = "Threshold"
                elif x > width * 2 / 3: knob_type = "Gain"
            else: # Small
                if abs(x - width/2) < 50: knob_type = "Ratio"
                elif x < width/2: knob_type = "Attack"
                else: knob_type = "Release"
                
            print(f"Knob ({knob_type}): Center=({x}, {y}), Diameter={d}, Radius={r}")

    # 2. Find Meter (Rectangle)
    # Threshold to find dark screen
    ret, thresh = cv2.threshold(gray, 50, 255, cv2.THRESH_BINARY_INV)
    contours, _ = cv2.findContours(thresh, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    
    print("\n--- Detected Meter ---")
    for cnt in contours:
        x, y, w, h = cv2.boundingRect(cnt)
        if w > 200 and h > 100: # Filter small noise
            center_x = x + w/2
            if abs(center_x - width/2) < 50: # Must be central
                print(f"Meter Screen: x={x}, y={y}, w={w}, h={h}")

if __name__ == "__main__":
    analyze_ui("/Users/manato/.gemini/antigravity/brain/85765bd3-5e45-420e-b4a6-e3ee8b885a36/uploaded_media_1769504593975.png")
