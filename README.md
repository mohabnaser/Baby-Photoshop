=============================================
PicMagic - Ultimate Image Studio
=============================================

Project for: CS213 Object-Oriented Programming
Faculty of Computers and Artificial Intelligence (FCAI), Cairo University
Supervised by: Dr. Mohammad El-Ramly
Developed by: [Mohab Nasr Abdelmonem]
Date: October 17, 2025

---------------------------------------------
Setup and Run Instructions
---------------------------------------------

**What you need:**

1.  **Qt Framework:** Version [Qt Version Used, e.g., 6.2 LTS or 5.15 LTS] needs to be installed. Make sure you have the right C++ compiler kit set up in Qt (like MinGW or MSVC).
2.  **C++ Compiler:** One that supports C++11 or newer (like the g++ that comes with MinGW, or MSVC).
3.  **CMake:** This build tool is needed. Qt Creator usually includes it, or you can install it separately.
4.  **Qt Creator IDE (Recommended):** I set up the project so it's super easy to open and build using Qt Creator.

**How to Run:**

1.  **Get the Code:** Download or clone the project files.
2.  **Open in Qt Creator:**
    * Open Qt Creator.
    * Go to "File" > "Open File or Project...".
    * Find the project folder and choose the `CMakeLists.txt` file.
    * Follow the steps to configure the project. Make sure to pick the Qt Kit you installed.
3.  **Build it:**
    * Go to "Build" > "Build Project '[Your Project Name]'".
    * Or, just click the little hammer icon (usually bottom-left).
4.  **Run it:**
    * After it builds successfully, go to "Build" > "Run" or click the green play button.
    * The PicMagic window should pop up! 

---------------------------------------------
Tools and Versions Used
---------------------------------------------

* **Language:** C++ ([Specify Standard if known, e.g., C++11/14/17])
* **Framework:** Qt Framework Version [Qt Version Used]
* **IDE:** Qt Creator Version [Qt Creator Version if known]
* **Build System:** CMake
* **Compiler:** [Compiler Name and Version, e.g., MinGW g++ 11.2 or MSVC 2019]
* **Image Library:** stb_image & stb_image_write (Included via Image_Class.h - provided)

---------------------------------------------
List of Filters I Made
---------------------------------------------

**Color & Tone:**
* Grayscale
* Black & White
* Invert Colors
* Brightness (Dialog popup)
* Solarize (New )
* Infrared
* Night Purple

**Effects & Styles:**
* Edge Detect
* Oil Painting
* Old TV Effect
* Neon Effect (New  - choose color via Dialog)

**Transformations & Stuff:**
* Flip (Horizontal/Vertical via Dialog)
* Rotate (90° CW, 90° CCW)
* Blur (Gaussian Blur via Dialog)
* Skew (Horizontal Shear via Dialog)
* Frame (Simple, Double, Fancy via Dialog)
* Resize (Dialog popups)
* Crop (Select with mouse)
* Merge Images (Choose second image via File Dialog)

**Other Buttons:**
* Load Image
* Save Image
* Delete Current Image
* Reset Filters (Go back to original)
* Undo/Redo (Up to 10 steps)

---------------------------------------------
Explanation of New Filters
---------------------------------------------

1.  **Neon Effect:**
    * **Goal:** Make the edges of the image look like glowing neon lights. 
    * **How I did it:** First, I run an edge detection filter on a copy of the image. This gives me an image where the edges are bright (white) and everything else is dark (black). Then, the user picks a neon color they like. I go through each pixel of the original image. For each pixel, I look at how bright the corresponding edge pixel is (from 0 to 255). I use this brightness as a multiplier (from 0.0 to 1.0) for the chosen neon color. So, if the edge pixel is bright white (255), the output pixel becomes the full neon color. If the edge pixel is black (0), the output pixel is black. Pixels in between get a faded version of the neon color. This makes only the edges glow!

2.  **Solarize:**
    * **Goal:** Create that weird, half-negative photo effect you sometimes see in photography. 
    * **How I did it:** This one's pretty simple! I picked a brightness level (the middle value, 128). Then, for every pixel in the image, I check each color channel (Red, Green, Blue). If the channel's value is *less than* 128 (meaning it's on the darker side), I flip it by subtracting its value from 255 (so 0 becomes 255, 50 becomes 205, etc.). If the channel's value is 128 or *more* (on the brighter side), I just leave it alone. This inverts the dark parts but keeps the bright parts the same.

---------------------------------------------
Demo Video
---------------------------------------------

https://youtu.be/A3c8rdjUnyE?si=26zCSztq4LtrLISu

=============================================
