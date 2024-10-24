// Clears the entire screen with the given color
internal void clear_screen(u32 color) {
    unsigned int* pixel = (u32*)render_state.memory;  // Pointer to the screen's pixel memory
    // Loop through every pixel in the screen (row-major order)
    for (int y = 0; y < render_state.height; y++) {
        for (int x = 0; x < render_state.width; x++) {
            *pixel++ = color;  // Set the pixel to the given color
        }
    }
}

// Draws a rectangle in pixel coordinates (x0, y0) to (x1, y1) with the given color
internal void draw_rect_in_pixels(int x0, int y0, int x1, int y1, u32 color) {
    // Clamp the coordinates to ensure they stay within the screen bounds
    x0 = clamp(0, x0, render_state.width);
    x1 = clamp(0, x1, render_state.width);
    y0 = clamp(0, y0, render_state.height);
    y1 = clamp(0, y1, render_state.height);

    // Loop through the specified rectangular area and color the pixels
    for (int y = y0; y < y1; y++) {
        u32* pixel = (u32*)render_state.memory + x0 + y * render_state.width;  // Start of each row
        for (int x = x0; x < x1; x++) {
            *pixel++ = color;  // Set pixel to the color
        }
    }
}

// Draws arena borders by using multiple rectangles
internal void draw_arena_borders(float arena_x, float arena_y, u32 color) {
    // Scale the arena size relative to the render state height
    arena_x *= render_state.height * render_scale;
    arena_y *= render_state.height * render_scale;

    // Calculate the pixel coordinates for the arena
    int x0 = (int)((float)render_state.width * 0.5f - arena_x);
    int x1 = (int)((float)render_state.width * 0.5f + arena_x);
    int y0 = (int)((float)render_state.height * 0.5f - arena_y);
    int y1 = (int)((float)render_state.height * 0.5f + arena_y);

    // Draw four rectangles to create the arena borders
    draw_rect_in_pixels(0, 0, render_state.width, y0, color);      // Top border
    draw_rect_in_pixels(0, y1, x1, render_state.height, color);    // Bottom border
    draw_rect_in_pixels(0, y0, x0, y1, color);                     // Left border
    draw_rect_in_pixels(x1, y0, render_state.width, render_state.height, color);  // Right border
}

// Draws a rectangle centered at (x, y) with given size and color
internal void draw_rect(float x, float y, float half_size_x, float half_size_y, u32 color) {
    // Scale the coordinates based on the render scale
    x *= render_state.height * render_scale;
    y *= render_state.height * render_scale;
    half_size_x *= render_state.height * render_scale;
    half_size_y *= render_state.height * render_scale;

    // Move the coordinates to be centered on the screen
    x += render_state.width / 2.f;
    y += render_state.height / 2.f;

    // Calculate pixel coordinates for the rectangle
    int x0 = x - half_size_x;
    int x1 = x + half_size_x;
    int y0 = y - half_size_y;
    int y1 = y + half_size_y;

    // Draw the rectangle in pixel space
    draw_rect_in_pixels(x0, y0, x1, y1, color);
}

// Array of character patterns used for rendering text as rectangles
const char* letters[][7] = {
    // Character 'A' pattern
    " 00",
    "0  0",
    "0  0",
    "0000",
    "0  0",
    "0  0",
    "0  0",

    // Character 'B' pattern
    "000",
    "0  0",
    "0  0",
    "000",
    "0  0",
    "0  0",
    "000",

    // Other character patterns follow the same structure...
};

// Draws text on the screen by rendering each character as rectangles
internal void draw_text(const char *text, float x, float y, float size, u32 color) {
    float half_size = size * 0.5f;  // Half of the character size for centering
    float original_y = y;           // Save original y-coordinate to reset after each character

    while (*text) {  // Iterate through the input text
        if (*text != 32) {  // Skip spaces (ASCII 32)
            const char** letter;
            // Handle special characters (e.g., '/' and '.')
            if (*text == 47) letter = letters[27];  
            else if (*text == 46) letter = letters[26];
            else letter = letters[*text - 'A'];  // Map to corresponding letter pattern
            
            float original_x = x;  // Save original x-coordinate

            // Render the character pattern row by row
            for (int i = 0; i < 7; i++) {
                const char* row = letter[i];
                while (*row) {
                    if (*row == '0') {  // Render filled cells as rectangles
                        draw_rect(x, y, half_size, half_size, color);
                    }
                    x += size;  // Move to the next cell in the row
                    row++;
                }
                y -= size;  // Move to the next row
                x = original_x;
            }
        }
        text++;  // Move to the next character in the text
        x += size * 6.f;  // Adjust spacing between characters
        y = original_y;
    }
}

// Draws a number on the screen, digit by digit
internal void draw_number(int number, float x, float y, float size, u32 color) {
    float half_size = size * 0.5f;  // Half size for centering digits
    bool drew_number = false;  // Track if a number has been drawn

    // Process each digit from least significant to most significant
    while (number || !drew_number) {
        drew_number = true;

        int digit = number % 10;  // Extract the last digit
        number = number / 10;     // Remove the last digit

        // Render each digit by drawing appropriate rectangles
        switch (digit) {
            case 0:
                draw_rect(x - size, y, half_size, 2.5f * size, color);
                draw_rect(x + size, y, half_size, 2.5f * size, color);
                draw_rect(x, y + size * 2.f, half_size, half_size, color);
                draw_rect(x, y - size * 2.f, half_size, half_size, color);
                x -= size * 4.f;
                break;
            case 1:
                draw_rect(x + size, y, half_size, 2.5f * size, color);
                x -= size * 2.f;
                break;
            // Other digit cases follow the same structure...
        }
    }
}
