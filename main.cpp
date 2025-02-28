#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// ตัวแปรเก็บค่าสำหรับ Trackbars
int green_h_min = 35, green_h_max = 85;
int yellow_h_min = 20, yellow_h_max = 30;
int red_h_min = 0, red_h_max = 10, red_h_max2 = 180, red_h_min2 = 170;
int blue_h_min = 100, blue_h_max = 140;

void processFrame(Mat &frame) {
    Mat hsv, mask_green, mask_yellow, mask_red1, mask_red2, mask_red, mask_blue, final_mask, output;
    
    // แปลงภาพเป็น HSV
    cvtColor(frame, hsv, COLOR_BGR2HSV);
    
    // Masking สี
    inRange(hsv, Scalar(green_h_min, 50, 50), Scalar(green_h_max, 255, 255), mask_green);
    inRange(hsv, Scalar(yellow_h_min, 100, 100), Scalar(yellow_h_max, 255, 255), mask_yellow);
    inRange(hsv, Scalar(red_h_min, 120, 70), Scalar(red_h_max, 255, 255), mask_red1);
    inRange(hsv, Scalar(red_h_min2, 120, 70), Scalar(red_h_max2, 255, 255), mask_red2);
    mask_red = mask_red1 | mask_red2;
    inRange(hsv, Scalar(blue_h_min, 150, 50), Scalar(blue_h_max, 255, 255), mask_blue);
    
    // รวม Mask ทั้งหมด
    final_mask = mask_green | mask_yellow | mask_red | mask_blue;
    
    // Apply mask กับภาพต้นฉบับ
    bitwise_and(frame, frame, output, final_mask);
    
    // แสดงผล
    imshow("Input", frame);
    imshow("Masking", final_mask);
    imshow("Output", output);
}

int main() {
    VideoCapture cap(1);
    if (!cap.isOpened()) {
        cout << "ไม่สามารถเปิดกล้องได้!" << endl;
        return -1;
    }

    // สร้างหน้าต่างควบคุมและกำหนดให้สามารถปรับขนาดได้
    namedWindow("Color Controls", WINDOW_NORMAL);
    resizeWindow("Color Controls", 400, 500); // กำหนดขนาดเริ่มต้นของหน้าต่าง
    
    // สร้าง Trackbars สำหรับแต่ละสี
    createTrackbar("Green H Min", "Color Controls", &green_h_min, 180);
    createTrackbar("Green H Max", "Color Controls", &green_h_max, 180);
    createTrackbar("Yellow H Min", "Color Controls", &yellow_h_min, 180);
    createTrackbar("Yellow H Max", "Color Controls", &yellow_h_max, 180);
    createTrackbar("Red H Min", "Color Controls", &red_h_min, 180);
    createTrackbar("Red H Max", "Color Controls", &red_h_max, 180);
    createTrackbar("Red H Min2", "Color Controls", &red_h_min2, 180);
    createTrackbar("Red H Max2", "Color Controls", &red_h_max2, 180);
    createTrackbar("Blue H Min", "Color Controls", &blue_h_min, 180);
    createTrackbar("Blue H Max", "Color Controls", &blue_h_max, 180);
    
    while (true) {
        Mat frame;
        cap >> frame;
        if (frame.empty()) break;
        
        processFrame(frame);
        
        if (waitKey(30) == 'q') break;
    }

    cap.release();
    destroyAllWindows();
    return 0;
}