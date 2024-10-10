#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <vector>
const uint16_t sourceAddress = 0x1234;
const uint16_t destinationAddress = 0x5678;
bool Decode(std::string inputFile, std::string outputFile);
uint16_t calculateCRC(const std::vector<uint8_t>& data)
{
    uint16_t crc = 0;
    for (const auto& byte : data) {
        crc ^= byte;
        for (int i = 0; i < 8; ++i) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc = crc >> 1;
            }
        }
    }
    return crc;
}
int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input file> <output file>" << std::endl;
        return 1;
    }

    std::string inputFile = argv[1];
    std::string outputFile = argv[2];

    if (Decode(inputFile, outputFile)) {
        std::cout << "Decoded successfully" << std::endl;
    } else {
        std::cerr << "Failed to decode" << std::endl;
        return 1;
    }

    return 0;
}
bool Decode(std::string inputFile, std::string outputFile)
{
    cv::VideoCapture cap(inputFile);

    if (!cap.isOpened()) {
        std::cerr << "Failed to open input file" << std::endl;
        return -1;
    }

    std::ofstream output(outputFile, std::ios::binary);

    cv::Mat frame;
    std::vector<uint8_t> data;
    while (cap.read(frame)) {
        cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY); // 将帧转换为灰度图像
        for (int y = 0; y < frame.rows; ++y) {
            for (int x = 0; x < frame.cols; ++x) {
                unsigned char pixel = frame.at<unsigned char>(y, x);
                data.push_back(pixel);
            }
        }
    }

    // 删除帧格式符
    if (data.size() > 0 && data[0] == 0x7E) {
        data.erase(data.begin());
    }

    // 提取并删除 CRC
    uint16_t receivedCRC = 0;
    if (data.size() >= 2) {
        receivedCRC = (data[data.size() - 2] << 8) | data[data.size() - 1];
        data.erase(data.end() - 2, data.end());
    }

    // 计算 CRC
    uint16_t calculatedCRC = calculateCRC(data);

    // 比较接收到的 CRC 和计算出的 CRC
    if (receivedCRC != calculatedCRC) {
        std::cerr << "CRC check failed" << std::endl;
        return -1;
    }

    // 写入输出文件
    output.write(reinterpret_cast<const char*>(data.data()), data.size());
    std::cout << "Decoded successfully" << std::endl;
    return 0;
}