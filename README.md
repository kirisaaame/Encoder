# Encoder
Assignment 1 of Computer Network course: Using OpenCV to encode binary files into videos and trying to decode back.

计网实验1：可见光传输信息的软件
使用OpenCV库将二进制字节流编码为视频，再将视频解码为二进制字节流。

由于编码为mp4文件存在压缩的问题总不能保证无损编码，因此编码为视频时采用了变为FFV1的avi格式。

食用方法：
1. 配置OpenCV库
2. 联合编译运行main.cpp
3. 命令行窗口输入encode进行编码或者decode进行解码，同时输入输入文件的相对路径名。


#彩蛋
将sample内的output.bin后缀改为.jpg