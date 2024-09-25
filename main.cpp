#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <vector>


bool Encode(std::string inputFile, std::string outputFile);
bool Decode(std::string inputFile, std::string outputFile);
int main() {
    std::string inputFile;    
    std::string outputFile;  
    std::string operation;
    std::cout << "Enter command: (input q to exit) " << std::endl;
    while (std::cin >> operation)
    {
        if (operation == "encode")
        {
            std::cout << "Enter the name of input file: " << std::endl;
            std::cin >> inputFile;
            std::cout << "Enter the name of output file: " << std::endl;
            std::cin >> outputFile;
            if (Encode(inputFile, outputFile))
                std::cout << "Encoded successfully" << std::endl;
        }
        else if (operation == "decode")
        {
            std::cout << "Enter the name of input file: " << std::endl;
            std::cin >> inputFile;
            std::cout << "Enter the name of output file: " << std::endl;
            std::cin >> outputFile;
            if (Decode(inputFile, outputFile))
                std::cout << "Decoded successfully" << std::endl;
        }
        else
        {
            return 0;
        }
    }
    return 0;
}
bool Encode(std::string inputFile, std::string outputFile)
{
    std::ifstream input(inputFile, std::ios::binary);
    if (!input)
    {
        std::cerr << "Failed to open input file" << std::endl;
        return false;
    }

    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});

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
bool Decode(std::string inputFile, std::string outputFile)
{
    cv::VideoCapture cap(inputFile);

    if (!cap.isOpened()) {
        std::cerr << "Failed to open input file" << std::endl;
        return -1;
    }

    std::ofstream output(outputFile, std::ios::binary);

    cv::Mat frame;
    while (cap.read(frame)) {
        cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY); // 将帧转换为灰度图像
        for (int y = 0; y < frame.rows; ++y) {
            for (int x = 0; x < frame.cols; ++x) {
                unsigned char pixel = frame.at<unsigned char>(y, x);
                output.write(reinterpret_cast<const char*>(&pixel), sizeof(pixel));
            }
        }
    }

    std::cout << "Decoded successfully" << std::endl;
    return 0;
}