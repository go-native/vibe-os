/**
 * kernel.c - Main kernel file for Vibe OS
 */

/* Define our own types since we can't use standard headers in freestanding environment */
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned long size_t;

// VGA text mode buffer address
#define VGA_BUFFER 0xB8000
// VGA text mode color attributes
#define VGA_COLOR_BLACK 0
#define VGA_COLOR_LIGHT_GREY 7
#define VGA_COLOR_WHITE 15
#define VGA_COLOR_CYAN 11

// Create a VGA color attribute
static inline uint8_t vga_entry_color(uint8_t fg, uint8_t bg) {
    return fg | (bg << 4);
}

// Create a VGA character entry
static inline uint16_t vga_entry(unsigned char c, uint8_t color) {
    return (uint16_t) c | ((uint16_t) color << 8);
}

// Calculate string length
static size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len]) {
        len++;
    }
    return len;
}

// Clear the screen
static void clear_screen() {
    uint16_t* vga_buffer = (uint16_t*) VGA_BUFFER;
    for (int i = 0; i < 80 * 25; i++) {
        vga_buffer[i] = vga_entry(' ', vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    }
}

// Print a string at a specific position
static void print_at(const char* str, int x, int y, uint8_t color) {
    uint16_t* vga_buffer = (uint16_t*) VGA_BUFFER;
    size_t len = strlen(str);
    
    for (size_t i = 0; i < len; i++) {
        vga_buffer[y * 80 + x + i] = vga_entry(str[i], color);
    }
}

// Kernel entry point
void kernel_main(void) {
    // Clear the screen
    clear_screen();
    
    // Print welcome message
    uint8_t title_color = vga_entry_color(VGA_COLOR_CYAN, VGA_COLOR_BLACK);
    print_at("Welcome to Vibe OS", 30, 10, title_color);
    print_at("A simple 32-bit operating system", 25, 12, vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
} 