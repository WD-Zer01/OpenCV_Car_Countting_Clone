// Blob.h

#ifndef MY_BLOB // 如果 MY_BLOB 没有被定义过，则继续编译
#define MY_BLOB

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
class Blob {
public:
    // member variables ///////////////////////////////////////////////////////////////////////////
    std::vector<cv::Point> currentContour;

    cv::Rect currentBoundingRect;   // 一个 Blob 的外接矩形

    std::vector<cv::Point> centerPositions;   // 一个 Blob 中心点的位置向量

    double dblCurrentDiagonalSize;   // 当前 Blob 的对角线长度

    double dblCurrentAspectRatio;   // 当前 Blob 的长宽比

    bool blnCurrentMatchFoundOrNewBlob;   // 表示当前 Blob 是否与之前的 Blob 匹配

    bool blnStillBeingTracked;   // 表示当前 Blob 是否仍然被跟踪

    int intNumOfConsecutiveFramesWithoutAMatch;   // 一系列连续帧中，未匹配的 Blob 的数量

    cv::Point predictedNextPosition;   // 预测当前 Blob 的下一个位置

    // function prototypes ////////////////////////////////////////////////////////////////////////
    Blob(std::vector<cv::Point> _contour);   // Blob 类的构造函数

    void predictNextPosition(void);   // 预测当前 Blob 的下一个位置，并将其存储在 predictedNextPosition 中
};

#endif // MY_BLOB

#pragma once // 指示编译器只需要在一个文件中包含这个头文件
