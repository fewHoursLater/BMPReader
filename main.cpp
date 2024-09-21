#include <windows.h>
#include <fstream>
#include <iostream>

class BMPReader {
 private:
  BITMAPFILEHEADER fileHeader;
  BITMAPINFOHEADER infoHeader;
  char* pixelData{nullptr};
  int width, height, bitCount;

 public:
  bool openBMP(const std::string& fileName) {
    std::ifstream file(fileName, std::ios::binary);
    if (!file) {
      std::cerr << "Error: Couldn't open file: " << fileName << std::endl;
      return false;
    }
    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(BITMAPFILEHEADER));
    file.read(reinterpret_cast<char*>(&infoHeader), sizeof(BITMAPINFOHEADER));
    if (fileHeader.bfType != 0x4D42) {
      std::cerr << "Error: Wrong file format" << std::endl;
      file.close();
      return false;
    }
    width = infoHeader.biWidth;
    height = infoHeader.biHeight;
    bitCount = infoHeader.biBitCount;
    if (bitCount != 24 && bitCount != 32) {
      std::cerr << "Error: Only 24 or 32-bit BMP are supported" << std::endl;
      file.close();
      return false;
    }
    const auto rowSize = ((bitCount * width + 31) / 32) * 4;
    const auto dataSize = rowSize * height;
    pixelData = new char[dataSize];
    file.seekg(fileHeader.bfOffBits, std::ios::beg);
    file.read(pixelData, dataSize);
    file.close();
    return true;
  }
  void displayBMP() const {
    if (!pixelData) {
      std::cerr << "Error: No data was loaded." << std::endl;
      return;
    }
    const auto rowSize = ((bitCount * width + 31) / 32) * 4;
    for (int y = height - 1; y >= 0; --y) {
      for (int x = 0; x < width; ++x) {
        const auto index = y * rowSize + x * (bitCount / 8);
        const unsigned char blue = pixelData[index];
        const unsigned char green = pixelData[index + 1];
        const unsigned char red = pixelData[index + 2];
        const unsigned char whiteChannel{255};
        const unsigned char blackChannel{0};
        if (blue == whiteChannel && green == whiteChannel &&
            red == whiteChannel) {
          std::cout << " ";
        } else if (blue == blackChannel && green == blackChannel &&
                   red == blackChannel) {
          std::cout << "#";
        } else {
          std::cerr << "\nError: Unsupported color in the image" << std::endl;
          return;
        }
      }
      std::cout << std::endl;
    }
  }
  void closeBMP() {
    delete[] pixelData;
    pixelData = nullptr;
  }
};

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <path to BMP>\n";
    return 1;
  }
  const auto fileName{argv[1]};
  BMPReader bmpReader;
  if (!bmpReader.openBMP(fileName)) {
    return 1;
  }
  bmpReader.displayBMP();
  bmpReader.closeBMP();
  return 0;
}