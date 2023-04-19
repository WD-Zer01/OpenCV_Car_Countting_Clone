// Blob.h

#ifndef MY_BLOB
#define MY_BLOB

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
class Blob {
public:
    // member variables ///////////////////////////////////////////////////////////////////////////
    // 一个 cv::Point 对象的向量，表示当前帧中 blob 的轮廓。
    std::vector<cv::Point> currentContour;

    // 一个 cv::Rect 对象，表示当前帧中 blob 的边界矩形。
    cv::Rect currentBoundingRect;

    // 一个 cv::Point 对象的向量，表示在检测到 blob 的每个帧中的中心位置。
    std::vector<cv::Point> centerPositions;

    // 一个 double 类型的变量，表示当前帧中 blob 的对角线大小。
    double dblCurrentDiagonalSize;

    // 一个 double 类型的变量，表示当前帧中 blob 的纵横比。 
    double dblCurrentAspectRatio;

    // 一个布尔值，表示 blob 是否与现有 blob 匹配或为新的 blob。
    bool blnCurrentMatchFoundOrNewBlob;

    // 一个布尔值，表示 blob 是否仍在被跟踪
    bool blnStillBeingTracked;

    // 一个整数，表示 blob 在连续的帧中没有与现有 blob 匹配的帧数。
    int intNumOfConsecutiveFramesWithoutAMatch;

    // 一个 cv::Point 对象，表示下一帧中 blob 的预测中心位置。
    cv::Point predictedNextPosition;

    // function prototypes //////////////////////////////////////////////////////////////////////
    // Blob 类有一个构造函数，它以一个 cv::Point 对象的向量作为参数。
    // 该构造函数使用提供的向量初始化 currentContour 成员变量。
    Blob(std::vector<cv::Point> _contour);

    // 该函数使用预测算法更新 predictedNextPosition 成员变量，
    // 表示下一帧中 blob 的预测中心位置。

    void predictNextPosition(void);

};

#endif    // MY_BLOB

