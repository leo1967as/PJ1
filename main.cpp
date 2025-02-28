#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// ตัวแปรเก็บค่าสำหรับ Trackbars
int green_h_min = 35, green_h_max = 85;
int yellow_h_min = 20, yellow_h_max = 30;
int red_h_min = 0, red_h_max = 10, red_h_max2 = 180, red_h_min2 = 170;
int blue_h_min = 100, blue_h_max = 140;
int gaussian_blur_size = 5; // ค่าขนาด Gaussian Blur
int canny_threshold1 = 23, canny_threshold2 = 30; // ค่า Canny thresholds
int morph_size = 3; // ขนาด kernel สำหรับ morphological operations

// Debug controls
bool show_trackbar = true; // ควบคุมการแสดง Trackbar window
bool show_input = true;     // ควบคุมการแสดงหน้าต่าง Input
bool show_masking = true;   // ควบคุมการแสดงหน้าต่าง Masking
bool show_output = true;    // ควบคุมการแสดงหน้าต่าง Output
bool show_edges = true;     // ควบคุมการแสดงหน้าต่าง Edges
bool show_contours = true;  // ควบคุมการแสดงหน้าต่าง Contours

void processFrame(Mat &frame) {
    Mat hsv, mask_green, mask_yellow, mask_red1, mask_red2, mask_red, mask_blue, final_mask, output;
    
    // ปรับขนาดของ Gaussian Blur ให้เป็นเลขคี่
    int ksize = (gaussian_blur_size % 2 == 0) ? gaussian_blur_size + 1 : gaussian_blur_size;
    
    // ลดแสงสะท้อนด้วย GaussianBlur
    GaussianBlur(frame, frame, Size(ksize, ksize), 0);
    
    // แปลงภาพเป็น HSV
    cvtColor(frame, hsv, COLOR_BGR2HSV);
    
    // ลดแสงจ้าโดยปรับค่า V (Brightness) ให้ต่ำลงในภาพ HSV
    vector<Mat> hsv_channels;
    split(hsv, hsv_channels);
    hsv_channels[2] = hsv_channels[2] * 0.8; // ลดค่าความสว่างลง 20%
    merge(hsv_channels, hsv);
    
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
    
    // แปลงภาพ Output เป็น grayscale สำหรับ Canny edge detection
    Mat gray;
    cvtColor(output, gray, COLOR_BGR2GRAY);
    
    // Canny edge detection
    Mat edges;
    Canny(gray, edges, canny_threshold1, canny_threshold2);
    
    // Morphological operations (Closing) เพื่อเชื่อมขอบที่ขาด
    Mat kernel = getStructuringElement(MORPH_RECT, Size(morph_size, morph_size));
    morphologyEx(edges, edges, MORPH_CLOSE, kernel);
    
    // หา Contours จาก edges
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(edges, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
    
    // วาด Contours บนภาพต้นฉบับ
    Mat contour_image = frame.clone();
    drawContours(contour_image, contours, -1, Scalar(0, 255, 0), 2);
    
    // แสดงผลตาม Debug Controls
    if (show_input) imshow("Input", frame);
    if (show_masking) imshow("Masking", final_mask);
    if (show_output) imshow("Output", output);
    if (show_edges) imshow("Edges", edges);
    if (show_contours) imshow("Contours", contour_image);
}

int main() {
    VideoCapture cap(1);
    if (!cap.isOpened()) {
        cout << "ไม่สามารถเปิดกล้องได้!" << endl;
        return -1;
    }

    // สร้างหน้าต่างควบคุม
    if (show_trackbar) {
        namedWindow("Color Controls", WINDOW_AUTOSIZE);
        resizeWindow("Color Controls", 500, 600);
        
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
        createTrackbar("Gaussian Blur Size", "Color Controls", &gaussian_blur_size, 20);
        createTrackbar("Canny Threshold 1", "Color Controls", &canny_threshold1, 255);
        createTrackbar("Canny Threshold 2", "Color Controls", &canny_threshold2, 255);
        createTrackbar("Morph Size", "Color Controls", &morph_size, 20);
    }
    
    while (true) {
        Mat frame;
        cap >> frame;
        if (frame.empty()) break;
        
        processFrame(frame);
        
        // ควบคุม Debug ด้วยคีย์บอร์ด
        char key = waitKey(30);
        if (key == 'q') break;
        else if (key == 't') show_trackbar = !show_trackbar; // Toggle Trackbar window
        else if (key == '1') show_input = !show_input;      // Toggle Input window
        else if (key == '2') show_masking = !show_masking; // Toggle Masking window
        else if (key == '3') show_output = !show_output;   // Toggle Output window
        else if (key == '4') show_edges = !show_edges;     // Toggle Edges window
        else if (key == '5') show_contours = !show_contours; // Toggle Contours window
    }

    cap.release();
    destroyAllWindows();
    return 0;
}