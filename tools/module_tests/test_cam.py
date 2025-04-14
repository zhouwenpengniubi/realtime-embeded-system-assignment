import cv2

cap = cv2.VideoCapture(0) # Arg "0" means camera0

# Reduce the resolution for better performance
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 320)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 240)

if not cap.isOpened():
    print("Can not access the camera")
    exit()

while True:
    ret, frame = cap.read()
    if not ret:
        print("Can not read frame")
        break

    cv2.imshow("Camera", frame)

    # Update the window
    cv2.waitKey(1)

    # Exit if user clicks the window close button (getWindowProperty returns -1 if the window is closed)
    if cv2.getWindowProperty("Camera", cv2.WND_PROP_VISIBLE) < 1:
        break

cap.release()
cv2.destroyAllWindows()