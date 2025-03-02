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
#define VGA_COLOR_GREEN 2

// Input buffer size
#define INPUT_BUFFER_SIZE 256
#define TERMINAL_WIDTH 80
#define TERMINAL_HEIGHT 25

// Global variables for terminal
static int cursor_x = 0;
static int cursor_y = 0;
static uint8_t terminal_color;

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
    for (int i = 0; i < TERMINAL_WIDTH * TERMINAL_HEIGHT; i++) {
        vga_buffer[i] = vga_entry(' ', vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    }
    cursor_x = 0;
    cursor_y = 0;
}

// Print a string at a specific position
static void print_at(const char* str, int x, int y, uint8_t color) {
    uint16_t* vga_buffer = (uint16_t*) VGA_BUFFER;
    size_t len = strlen(str);
    
    for (size_t i = 0; i < len; i++) {
        vga_buffer[y * TERMINAL_WIDTH + x + i] = vga_entry(str[i], color);
    }
}

// Print a character at a specific position
static void print_char_at(char c, int x, int y, uint8_t color) {
    uint16_t* vga_buffer = (uint16_t*) VGA_BUFFER;
    vga_buffer[y * TERMINAL_WIDTH + x] = vga_entry(c, color);
}

// Print a string at the current cursor position
static void print(const char* str) {
    for (size_t i = 0; i < strlen(str); i++) {
        if (str[i] == '\n') {
            cursor_x = 0;
            cursor_y++;
            if (cursor_y >= TERMINAL_HEIGHT) {
                // Simple scrolling - move everything up one line
                uint16_t* vga_buffer = (uint16_t*) VGA_BUFFER;
                for (int y = 0; y < TERMINAL_HEIGHT - 1; y++) {
                    for (int x = 0; x < TERMINAL_WIDTH; x++) {
                        vga_buffer[y * TERMINAL_WIDTH + x] = vga_buffer[(y + 1) * TERMINAL_WIDTH + x];
                    }
                }
                // Clear the last line
                for (int x = 0; x < TERMINAL_WIDTH; x++) {
                    vga_buffer[(TERMINAL_HEIGHT - 1) * TERMINAL_WIDTH + x] = vga_entry(' ', terminal_color);
                }
                cursor_y = TERMINAL_HEIGHT - 1;
            }
        } else {
            print_char_at(str[i], cursor_x, cursor_y, terminal_color);
            cursor_x++;
            if (cursor_x >= TERMINAL_WIDTH) {
                cursor_x = 0;
                cursor_y++;
                if (cursor_y >= TERMINAL_HEIGHT) {
                    // Simple scrolling - move everything up one line
                    uint16_t* vga_buffer = (uint16_t*) VGA_BUFFER;
                    for (int y = 0; y < TERMINAL_HEIGHT - 1; y++) {
                        for (int x = 0; x < TERMINAL_WIDTH; x++) {
                            vga_buffer[y * TERMINAL_WIDTH + x] = vga_buffer[(y + 1) * TERMINAL_WIDTH + x];
                        }
                    }
                    // Clear the last line
                    for (int x = 0; x < TERMINAL_WIDTH; x++) {
                        vga_buffer[(TERMINAL_HEIGHT - 1) * TERMINAL_WIDTH + x] = vga_entry(' ', terminal_color);
                    }
                    cursor_y = TERMINAL_HEIGHT - 1;
                }
            }
        }
    }
}

// Simple keyboard driver based on OSDev wiki
// https://wiki.osdev.org/PS/2_Keyboard

// Keyboard I/O ports
#define KBD_DATA_PORT   0x60
#define KBD_STATUS_PORT 0x64
#define KBD_COMMAND_PORT 0x64

// Read from an I/O port
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Write to an I/O port
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

// Wait for a short amount of time
static inline void io_wait(void) {
    outb(0x80, 0);
}

// Simple delay function (busy wait)
static void delay(int count) {
    // Use a more reliable delay mechanism
    // Each iteration is roughly 1 second on modern hardware in QEMU
    for (int i = 0; i < count; i++) {
        // Approximately 1 second delay - increased for much longer delay
        for (int j = 0; j < 500000000; j++) {
            asm volatile("nop");
        }
        
        // Update the countdown on screen
        char countdown[3];
        if (count - i >= 10) {
            countdown[0] = '0' + ((count - i) / 10);
            countdown[1] = '0' + ((count - i) % 10);
            countdown[2] = 0;
        } else {
            countdown[0] = '0' + (count - i);
            countdown[1] = 0;
        }
        print_at(countdown, 60, 14, vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    }
}

// Initialize the keyboard
static void keyboard_init() {
    // Wait for the keyboard controller to be ready
    while ((inb(KBD_STATUS_PORT) & 2) != 0);
    
    // Disable scanning
    outb(KBD_COMMAND_PORT, 0xAD);
    io_wait();
    
    // Flush the output buffer
    if ((inb(KBD_STATUS_PORT) & 1) != 0) {
        inb(KBD_DATA_PORT);
    }
    
    // Set the controller configuration byte
    outb(KBD_COMMAND_PORT, 0x20);
    io_wait();
    uint8_t status = inb(KBD_DATA_PORT);
    status |= 1;      // Enable IRQ1 (keyboard interrupt)
    status &= ~(1<<4); // Enable keyboard interface
    outb(KBD_COMMAND_PORT, 0x60);
    io_wait();
    outb(KBD_DATA_PORT, status);
    
    // Enable scanning
    outb(KBD_COMMAND_PORT, 0xAE);
    io_wait();
    
    // Reset the keyboard
    outb(KBD_DATA_PORT, 0xFF);
    io_wait();
    
    // Wait for ACK
    while (inb(KBD_DATA_PORT) != 0xFA);
}

// Scancode set 1 (US QWERTY layout)
static const char kbd_us[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, '<', 0, 0
};

// Get a character from the keyboard (blocking)
static char keyboard_getchar() {
    uint8_t scancode;
    
    // Wait for a key to be pressed
    while (1) {
        // Check if scan code is available
        if ((inb(KBD_STATUS_PORT) & 1) == 0) {
            // Small delay to reduce CPU usage
            for (int i = 0; i < 10000; i++) {
                asm volatile("nop");
            }
            continue;
        }
        
        // Get the scan code
        scancode = inb(KBD_DATA_PORT);
        
        // If the top bit of the scan code is set, a key was released
        if (scancode & 0x80) continue;
        
        // Convert scan code to ASCII
        if (scancode < 128 && kbd_us[scancode] != 0) {
            return kbd_us[scancode];
        }
    }
}

// Run the terminal interface
static void run_terminal() {
    clear_screen();
    terminal_color = vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
    
    print("Vibe OS Terminal v1.0\n");
    print("Type anything you want...\n\n");
    print("> ");
    
    // Main terminal loop
    while (1) {
        char c = keyboard_getchar();
        
        if (c == '\n') {
            // Enter key pressed, start a new line
            print("\n> ");
        } else if (c == '\b' && cursor_x > 2) {
            // Backspace key pressed, delete last character
            cursor_x--;
            print_char_at(' ', cursor_x, cursor_y, terminal_color);
        } else if (c >= ' ' && c <= '~') {
            // Printable character, display it
            print_char_at(c, cursor_x, cursor_y, terminal_color);
            cursor_x++;
            
            // Handle line wrapping
            if (cursor_x >= TERMINAL_WIDTH) {
                cursor_x = 0;
                cursor_y++;
                if (cursor_y >= TERMINAL_HEIGHT) {
                    // Scroll the screen
                    uint16_t* vga_buffer = (uint16_t*) VGA_BUFFER;
                    for (int y = 0; y < TERMINAL_HEIGHT - 1; y++) {
                        for (int x = 0; x < TERMINAL_WIDTH; x++) {
                            vga_buffer[y * TERMINAL_WIDTH + x] = vga_buffer[(y + 1) * TERMINAL_WIDTH + x];
                        }
                    }
                    // Clear the last line
                    for (int x = 0; x < TERMINAL_WIDTH; x++) {
                        vga_buffer[(TERMINAL_HEIGHT - 1) * TERMINAL_WIDTH + x] = vga_entry(' ', terminal_color);
                    }
                    cursor_y = TERMINAL_HEIGHT - 1;
                }
            }
        }
    }
}

// Kernel entry point
void kernel_main(void) {
    // Clear the screen
    clear_screen();
    
    // Initialize keyboard
    keyboard_init();
    
    // Print welcome message
    uint8_t title_color = vga_entry_color(VGA_COLOR_CYAN, VGA_COLOR_BLACK);
    uint8_t text_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    
    print_at("Welcome to Vibe OS", 30, 10, title_color);
    print_at("A simple 32-bit operating system", 25, 12, text_color);
    print_at("Terminal will start in 30 seconds...", 25, 14, text_color);
    
    // Wait for 30 seconds with countdown
    delay(30);
    
    // Start the terminal interface
    run_terminal();
    
    // Halt the CPU (should never reach here)
    while (1) {
        asm volatile("hlt");
    }
}