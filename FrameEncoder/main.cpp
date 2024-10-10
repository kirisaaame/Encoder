#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <vector>
const uint16_t sourceAddress = 0x1234;
const uint16_t destinationAddress = 0x5678;

void Encode(const std::string& binaryFilePath, int maxBytes, const std::string& outputVideoFilename, int maxVideoLengthInMs) ;
//calculate CRC
uint16_t calculateCRC(const std::vector<uint8_t>& data) {
    uint16_t crc = 0xFFFF;

    for (size_t i = 0; i < data.size(); i++) {
        crc ^= static_cast<uint16_t>(data[i] << 8);
        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc = crc << 1;
            }
        }
    }

    return crc;
}
int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <input file> <max bytes> <output file> <video length in ms>" << std::endl;
        return 1;
    }

    std::string inputFile = argv[1];
    int maxBytes = std::stoi(argv[2]);
    std::string outputFile = argv[3];
    int maxVideoLengthInMs = std::stoi(argv[4]);

    Encode(inputFile, maxBytes, outputFile, maxVideoLengthInMs);

    std::cout << "Encoded successfully" << std::endl;

    return 0;
}
void Encode(const std::string& binaryFilePath, int maxBytes, const std::string& outputFile, int maxVideoLengthInMs) 
{
    std::ifstream inputFile(binaryFilePath, std::ios::binary);
    std::vector<uint8_t> data(maxBytes);

    if (inputFile) {
        inputFile.read(reinterpret_cast<char*>(data.data()), maxBytes);
    }

    // Add frame delimiter
    data.insert(data.begin(), 0x7E);

    // Add destination address
    data.insert(data.begin() + 1, destinationAddress >> 8);
    data.insert(data.begin() + 2, destinationAddress & 0xFF);

    // Add source address
    data.insert(data.begin() + 3, sourceAddress >> 8);
    data.insert(data.begin() + 4, sourceAddress & 0xFF);

    // Add length
    uint16_t length = maxBytes;
    data.insert(data.begin() + 5, length >> 8);
    data.insert(data.begin() + 6, length & 0xFF);

    // Calculate and add CRC
    uint16_t crc = calculateCRC(data);
    data.push_back(crc >> 8);
    data.push_back(crc & 0xFF);

    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(inputFile), {});

    int width = 640; // 设置你的视频帧的宽度
    int height = 480; // 设置你的视频帧的高度

    cv::VideoWriter writer(outputFile, cv::VideoWriter::fourcc('F', 'F', 'V', '1'), 30, cv::Size(width, height), false);

    size_t i = 0;
    while (i < buffer.size()) {
        cv::Mat frame(height, width, CV_8UC1, cv::Scalar(0)); // 创建一个新的帧，并将所有的像素初始化为0

        for (int y = 0; y < height && i < buffer.size(); ++y) {
            for (int x = 0; x < width && i < buffer.size(); ++x) {
                frame.at<unsigned char>(y, x) = buffer[i];
                ++i;
            }
        }

        writer.write(frame);
    }

   
}
