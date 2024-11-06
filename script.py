import time
import pyautogui

while True:
    # Move the mouse cursor slightly
    pyautogui.moveRel(5, 0, duration=0.25)
    pyautogui.moveRel(-5, 0, duration=0.25)
    
    # Wait for 1 minute before moving the mouse again
    time.sleep(60)
