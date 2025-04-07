#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

vector<Point2f> points;

void mouseCallback(int event, int x, int y, int, void*) {
    if (event == EVENT_LBUTTONDOWN && points.size() < 4) {
        points.emplace_back(x, y);
        cout << "Point " << points.size() << ": " << x << ", " << y << endl;
    }
}

vector<vector<int>> extractBoardState(Mat& image, int board_size = 12) {
    vector<vector<int>> board(board_size, vector<int>(board_size, 0));

    Mat gray, blurImg, thresh;
    cvtColor(image, gray, COLOR_BGR2GRAY);
    GaussianBlur(gray, blurImg, Size(5, 5), 0);
    threshold(blurImg, thresh, 90, 255, THRESH_BINARY_INV);

    int cell_h = thresh.rows / board_size;
    int cell_w = thresh.cols / board_size;

    for (int i = 0; i < board_size; i++) {
        for (int j = 0; j < board_size; j++) {
            int y1 = i * cell_h + 5, y2 = (i + 1) * cell_h - 5;
            int x1 = j * cell_w + 5, x2 = (j + 1) * cell_w - 5;

            Rect roi(x1, y1, x2 - x1, y2 - y1);
            Mat cell = thresh(roi);
            float ratio = countNonZero(cell) / (float)(cell.total());

            if (ratio > 0.6)
                board[i][j] = 1; // 黑子
            else if (ratio > 0.3)
                board[i][j] = 2; // 白子
        }
    }
    return board;
}

int main() {
    VideoCapture cap(1);  // 摄像头编号，根据实际情况修改
    if (!cap.isOpened()) {
        cerr << "无法打开摄像头" << endl;
        return -1;
    }

    Mat frame;
    cap.read(frame);
    if (frame.empty()) {
        cerr << "读取第一帧失败" << endl;
        return -1;
    }

    namedWindow("Click 4 Corners");
    setMouseCallback("Click 4 Corners", mouseCallback);

    while (points.size() < 4) {
        Mat temp = frame.clone();
        for (auto& p : points)
            circle(temp, p, 5, Scalar(0, 0, 255), -1);
        imshow("Click 4 Corners", temp);
        if (waitKey(1) == 'q') return 0;
    }

    destroyWindow("Click 4 Corners");

    // 计算透视矩阵
    Size boardSize(480, 480);
    vector<Point2f> dstPts = { {0,0}, {boardSize.width,0}, {boardSize.width,boardSize.height}, {0,boardSize.height} };
    Mat M = getPerspectiveTransform(points, dstPts);

    while (true) {
        Mat frame;
        cap >> frame;
        if (frame.empty()) break;

        Mat warped;
        warpPerspective(frame, warped, M, boardSize);

        auto board = extractBoardState(warped, 12);

        int cell = boardSize.width / 12;
        for (int i = 0; i < 12; ++i) {
            for (int j = 0; j < 12; ++j) {
                if (board[i][j] != 0) {
                    Point center(j * cell + cell / 2, i * cell + cell / 2);
                    Scalar color = board[i][j] == 1 ? Scalar(0, 0, 255) : Scalar(0, 255, 0);
                    circle(warped, center, 10, color, 2);
                }
            }
        }

        imshow("Warped Live", warped);
        if (waitKey(1) == 'q') break;
    }

    cap.release();
    destroyAllWindows();
    return 0;
}
