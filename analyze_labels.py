import cv2
import numpy as np

def analyze_labels(image_path):
    img = cv2.imread(image_path)
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    
    # Threshold to find black text on light background
    # Text is likely black/dark grey
    ret, thresh = cv2.threshold(gray, 100, 255, cv2.THRESH_BINARY_INV)
    
    # Define search areas below knobs
    # Threshold: x=206, y=436, r=83 -> Look around x=[150, 260], y=[520, 600]
    # Ratio: x=512, y=366, r=32 -> Look around x=[480, 540], y=[400, 450]
    # Attack: x=410, y=480, r=33 -> Look around x=[380, 440], y=[510, 560]
    # Gain: x=818
    # Release: x=614
    
    regions = {
        "Threshold": (150, 260, 520, 600),
        "Ratio": (480, 544, 400, 450),
        "Attack": (380, 440, 510, 560),
        "Release": (580, 640, 510, 560),
        "Gain": (760, 870, 520, 600)
    }
    
    print("--- DETECTED LABELS ---")
    for name, (x1, x2, y1, y2) in regions.items():
        roi = thresh[y1:y2, x1:x2]
        contours, _ = cv2.findContours(roi, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        
        # Find bounding box of all contours in ROI (text word)
        min_x, min_y = 9999, 9999
        max_x, max_y = 0, 0
        found = False
        
        for cnt in contours:
            x, y, w, h = cv2.boundingRect(cnt)
            if w > 5 and h > 5: # Filter noise
                min_x = min(min_x, x)
                min_y = min(min_y, y)
                max_x = max(max_x, x + w)
                max_y = max(max_y, y + h)
                found = True
        
        if found:
            # Shift back to global coords
            gx = x1 + min_x
            gy = y1 + min_y
            gw = max_x - min_x
            gh = max_y - min_y
            cx = gx + gw/2
            print(f"Label ({name}): x={gx}, y={gy}, w={gw}, h={gh}, center_x={cx}")

if __name__ == "__main__":
    analyze_labels("/Users/manato/.gemini/antigravity/brain/85765bd3-5e45-420e-b4a6-e3ee8b885a36/uploaded_media_1769505247676.png")
