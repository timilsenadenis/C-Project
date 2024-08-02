#include <iostream>
#include <vector>
#include <fstream>
#include <bitset>

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

// Function to save the QR code matrix as a PGM image
void saveAsPGM(const std::vector<std::vector<bool>> &matrix, const std::string &filename) {
    int size = matrix.size();
    int imageSize = size * MODULE_SIZE;
    std::ofstream file(filename, std::ios::binary);

    file << "P5\n" << imageSize << " " << imageSize << "\n255\n";
    for (int y = 0; y < size; ++y) {
        for (int i = 0; i < MODULE_SIZE; ++i) {
            for (int x = 0; x < size; ++x) {
                unsigned char color = matrix[y][x] ? 0 : 255;
                for (int j = 0; j < MODULE_SIZE; ++j) {
                    file.write(reinterpret_cast<char*>(&color), sizeof(color));
                }
            }
        }
    }

    file.close();
}

int main() {
    std::string text = "Hello, World!";
    std::string binaryData = encodeToBinary(text);
    std::vector<std::vector<bool>> qrCodeMatrix = createQRCodeMatrix(binaryData);
    saveAsPGM(qrCodeMatrix, "qrcode.pgm");

    std::cout << "QR code saved to qrcode.pgm" << std::endl;
    return 0;
}
