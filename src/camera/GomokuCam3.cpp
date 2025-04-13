#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>

using namespace cv;
using namespace std;

// Sort the corner points: top-left, top-right, bottom-right, bottom-left
vector<Point2f> order_points(vector<Point> pts) {
    vector<Point2f> rect(4);
    vector<Point2f> floatPts(pts.begin(), pts.end());

    sort(floatPts.begin(), floatPts.end(), [](Point2f a, Point2f b) {
        return a.y < b.y || (a.y == b.y && a.x < b.x);
    });

    if (floatPts[0].x < floatPts[1].x) {
        rect[0] = floatPts[0]; // top-left
        rect[1] = floatPts[1]; // top-right
    } else {
        rect[0] = floatPts[1];
        rect[1] = floatPts[0];
    }

    if (floatPts[2].x < floatPts[3].x) {
        rect[3] = floatPts[2]; // bottom-left
        rect[2] = floatPts[3]; // bottom-right
    } else {
        rect[3] = floatPts[3];
        rect[2] = floatPts[2];
    }

    return rect;
}

// Determine the color of the piece based on average brightness
string detect_piece_color(Mat& gray, int x, int y, int r) {
    Mat mask = Mat::zeros(gray.size(), CV_8UC1);
    circle(mask, Point(x, y), r - 2, Scalar(255), -1);
    Scalar meanVal = mean(gray, mask);
    double val = meanVal[0];
    if (val < 70) return "black";
    else if (val > 180) return "white";
    else return "unknown";
}

int main() {
    const int grid_lines = 13;         // 13 lines = 12x12 grid intersections
    const int board_size = 600;        // Size of the warped board image
    const float spacing = board_size / float(grid_lines - 1);

    VideoCapture cap(1);               // Open camera with device ID 1
    if (!cap.isOpened()) {
        cerr << "Cannot open camera" << endl;
        return -1;
    }

    while (true) {
        Mat frame, gray, blur, edges;
        cap >> frame;
        if (frame.empty()) break;

        frame.copyTo(gray);
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        GaussianBlur(gray, blur, Size(5, 5), 0);
        Canny(blur, edges, 50, 150);   // Edge detection

        vector<vector<Point>> contours;
        findContours(edges, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        sort(contours.begin(), contours.end(), [](const vector<Point>& a, const vector<Point>& b) {
            return contourArea(a, false) > contourArea(b, false);
        });

        if (!contours.empty()) {
            vector<Point> approx;
            approxPolyDP(contours[0], approx, 0.02 * arcLength(contours[0], true), true);
            if (approx.size() == 4) {
                vector<Point2f> src_pts = order_points(approx);
                vector<Point2f> dst_pts = {
                    Point2f(0, 0),
                    Point2f(board_size - 1, 0),
                    Point2f(board_size - 1, board_size - 1),
                    Point2f(0, board_size - 1)
                };

                Mat M = getPerspectiveTransform(src_pts, dst_pts);
                Mat warped;
                warpPerspective(frame, warped, M, Size(board_size, board_size));

                Mat gray_warped;
                cvtColor(warped, gray_warped, COLOR_BGR2GRAY);
                GaussianBlur(gray_warped, gray_warped, Size(9, 9), 0);

                // Draw grid intersections
                vector<Point> grid_points;
                for (int i = 0; i < grid_lines; i++) {
                    for (int j = 0; j < grid_lines; j++) {
                        int x = int(j * spacing);
                        int y = int(i * spacing);
                        grid_points.push_back(Point(x, y));
                        circle(warped, Point(x, y), 2, Scalar(0, 255, 0), -1);
                    }
                }

                // Detect circular pieces
                vector<Vec3f> circles;
                HoughCircles(gray_warped, circles, HOUGH_GRADIENT, 1.2, spacing * 0.8,
                             100, 15, 18, 20);

                for (size_t i = 0; i < circles.size(); i++) {
                    int x = cvRound(circles[i][0]);
                    int y = cvRound(circles[i][1]);
                    int r = cvRound(circles[i][2]);

                    // Match to nearest grid point
                    double min_dist = DBL_MAX;
                    Point nearest;
                    for (const auto& gp : grid_points) {
                        double dist = pow(gp.x - x, 2) + pow(gp.y - y, 2);
                        if (dist < min_dist) {
                            min_dist = dist;
                            nearest = gp;
                        }
                    }

                    // If near a grid point, consider valid
                    if (min_dist < pow(spacing * 0.4, 2)) {
                        string color = detect_piece_color(gray_warped, x, y, r);
                        if (color != "unknown") {
                            circle(warped, Point(x, y), r, Scalar(0, 0, 255), 2);
                            circle(warped, Point(x, y), 2, Scalar
