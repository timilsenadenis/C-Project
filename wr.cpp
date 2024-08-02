#include <iostream>
#include <vector>
#include <bitset>
#include <SDL2/SDL.h>

const int QR_VERSION = 1; // Version 1 (21x21)
const int MODULE_SIZE = 10; // Size of each module in the final image

// Function to encode a string to a binary representation
std::string encodeToBinary(const std::string &text) {
    std::string binary;
    for (char c : text) {
        binary += std::bitset<8>(c).to_string();
    }
    return binary;
}

// Function to create a QR code matrix (simplified version)
std::vector<std::vector<bool>> createQRCodeMatrix(const std::string &binaryData) {
    int size = 21; // QR code version 1 size
    std::vector<std::vector<bool>> matrix(size, std::vector<bool>(size, false));

    // Place the binary data into the matrix (simplified, no error correction)
    int index = 0;
    for (int y = 0; y < size && index < binaryData.size(); ++y) {
        for (int x = 0; x < size && index < binaryData.size(); ++x) {
            if (binaryData[index] == '1') {
                matrix[y][x] = true;
            }
            ++index;
        }
    }

    return matrix;
}

// Function to render the QR code using SDL2
void renderQRCode(const std::vector<std::vector<bool>> &matrix) {
    int size = matrix.size();
    int windowSize = size * MODULE_SIZE;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return;
    }

    // Create a window
    SDL_Window *win = SDL_CreateWindow("QR Code", 100, 100, windowSize, windowSize, SDL_WINDOW_SHOWN);
    if (win == nullptr) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    // Create a renderer
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == nullptr) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(win);
        SDL_Quit();
        return;
    }

    // Set the draw color to white (background)
    SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
    SDL_RenderClear(ren);

    // Set the draw color to black (QR code modules)
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);

    // Draw the QR code modules
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            if (matrix[y][x]) {
                SDL_Rect rect;
                rect.x = x * MODULE_SIZE;
                rect.y = y * MODULE_SIZE;
                rect.w = MODULE_SIZE;
                rect.h = MODULE_SIZE;
                SDL_RenderFillRect(ren, &rect);
            }
        }
    }

    // Present the rendered image
    SDL_RenderPresent(ren);

    // Wait for a few seconds before quitting
    SDL_Delay(5000);

    // Clean up
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
}

int main() {
    std::string text = "Hello, World!";
    std::string binaryData = encodeToBinary(text);
    std::vector<std::vector<bool>> qrCodeMatrix = createQRCodeMatrix(binaryData);
    renderQRCode(qrCodeMatrix);

    return 0;
}
