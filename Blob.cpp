// Blob.cpp

#include "Blob.h"


Blob::Blob(std::vector<cv::Point> _contour) {  // 构造函数，传入包含运动物体的轮廓点的vector对象

    currentContour = _contour;  // 保存传入的轮廓点

    currentBoundingRect = cv::boundingRect(currentContour);  // 用轮廓点创建边框矩形，保存

    cv::Point currentCenter;  // 创建一个保存当前中心点坐标的Point对象

    currentCenter.x = (currentBoundingRect.x + currentBoundingRect.x + currentBoundingRect.width) / 2;  // 计算当前中心点x坐标
    currentCenter.y = (currentBoundingRect.y + currentBoundingRect.y + currentBoundingRect.height) / 2;  // 计算当前中心点y坐标

    centerPositions.push_back(currentCenter);  // 将当前中心点保存到vector对象中

    dblCurrentDiagonalSize = sqrt(pow(currentBoundingRect.width, 2) + pow(currentBoundingRect.height, 2));  // 计算当前边框矩形的对角线大小

    dblCurrentAspectRatio = (float)currentBoundingRect.width / (float)currentBoundingRect.height;  // 计算当前边框矩形的宽高比

    blnStillBeingTracked = true;  // 设置当前Blob对象还在被追踪

    blnCurrentMatchFoundOrNewBlob = true;  // 设置当前Blob对象是新的还是已经匹配到

    intNumOfConsecutiveFramesWithoutAMatch = 0;  // 初始化当前Blob对象未匹配到的帧数
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Blob::predictNextPosition(void) {

    int numPositions = (int)centerPositions.size();
    // 中心位置向量的长度

    if (numPositions == 1) {
        // 如果中心位置向量的长度为1

        predictedNextPosition.x = centerPositions.back().x;
        predictedNextPosition.y = centerPositions.back().y;
        // 预测的下一个位置等于当前的位置

    }
    else if (numPositions == 2) {
        // 如果中心位置向量的长度为2

        int deltaX = centerPositions[1].x - centerPositions[0].x;
        int deltaY = centerPositions[1].y - centerPositions[0].y;
        // 计算x和y方向上的变化量

        predictedNextPosition.x = centerPositions.back().x + deltaX;
        predictedNextPosition.y = centerPositions.back().y + deltaY;
        // 预测下一个位置等于当前位置加上变化量

    }
    else if (numPositions == 3) {
        // 如果中心位置向量的长度为3

        int sumOfXChanges = ((centerPositions[2].x - centerPositions[1].x) * 2) +
            ((centerPositions[1].x - centerPositions[0].x) * 1);
        // 计算x方向上的变化量之和

        int deltaX = (int)std::round((float)sumOfXChanges / 3.0);
        // 计算x方向上的平均变化量

        int sumOfYChanges = ((centerPositions[2].y - centerPositions[1].y) * 2) +
            ((centerPositions[1].y - centerPositions[0].y) * 1);
        // 计算y方向上的变化量之和

        int deltaY = (int)std::round((float)sumOfYChanges / 3.0);
        // 计算y方向上的平均变化量

        predictedNextPosition.x = centerPositions.back().x + deltaX;
        predictedNextPosition.y = centerPositions.back().y + deltaY;
        // 预测下一个位置等于当前位置加上平均变化量
    }
    // 如果目标物体的中心点坐标历史记录长度为4
    else if (numPositions == 4) {
        // 计算目标物体在水平方向上的变化量，权重分别为3、2、1
        int sumOfXChanges = ((centerPositions[3].x - centerPositions[2].x) * 3) +
            ((centerPositions[2].x - centerPositions[1].x) * 2) +
            ((centerPositions[1].x - centerPositions[0].x) * 1);

        // 计算水平方向上的变化量平均值
        int deltaX = (int)std::round((float)sumOfXChanges / 6.0);

        // 计算目标物体在竖直方向上的变化量，权重分别为3、2、1
        int sumOfYChanges = ((centerPositions[3].y - centerPositions[2].y) * 3) +
            ((centerPositions[2].y - centerPositions[1].y) * 2) +
            ((centerPositions[1].y - centerPositions[0].y) * 1);

        // 计算竖直方向上的变化量平均值
        int deltaY = (int)std::round((float)sumOfYChanges / 6.0);

        // 预测目标物体下一次的中心点坐标
        predictedNextPosition.x = centerPositions.back().x + deltaX;
        predictedNextPosition.y = centerPositions.back().y + deltaY;


    }
    else if (numPositions >= 5) {

        // 计算目标物体在水平方向上的变化量，权重分别为3、2、1
        int sumOfXChanges = ((centerPositions[3].x - centerPositions[2].x) * 3) +
            ((centerPositions[2].x - centerPositions[1].x) * 2) +
            ((centerPositions[1].x - centerPositions[0].x) * 1);

        // 计算水平方向上的变化量平均值
        int deltaX = (int)std::round((float)sumOfXChanges / 6.0);

        // 计算目标物体在竖直方向上的变化量，权重分别为3、2、1
        int sumOfYChanges = ((centerPositions[3].y - centerPositions[2].y) * 3) +
            ((centerPositions[2].y - centerPositions[1].y) * 2) +
            ((centerPositions[1].y - centerPositions[0].y) * 1);

        // 计算竖直方向上的变化量平均值
        int deltaY = (int)std::round((float)sumOfYChanges / 6.0);

        // 预测目标物体下一次的中心点坐标
        predictedNextPosition.x = centerPositions.back().x + deltaX;
        predictedNextPosition.y = centerPositions.back().y + deltaY;

    }
    else {
        // should never get here
    }

}

/*
    # 计算 X 轴位置变化量之和，其中最后一个位置变化量乘以3，倒数第二个位置变化量乘以2，倒数第三个位置变化量乘以1
    int sumOfXChanges = ((centerPositions[3].x - centerPositions[2].x) * 3) +
        ((centerPositions[2].x - centerPositions[1].x) * 2) +
        ((centerPositions[1].x - centerPositions[0].x) * 1);

    # 计算 X 轴变化量的平均值，向最近的整数四舍五入
    int deltaX = (int)std::round((float)sumOfXChanges / 6.0);

    # 计算 Y 轴位置变化量之和，其中最后一个位置变化量乘以3，倒数第二个位置变化量乘以2，倒数第三个位置变化量乘以1
    int sumOfYChanges = ((centerPositions[3].y - centerPositions[2].y) * 3) +
        ((centerPositions[2].y - centerPositions[1].y) * 2) +
        ((centerPositions[1].y - centerPositions[0].y) * 1);

    # 计算 Y 轴变化量的平均值，向最近的整数四舍五入
    int deltaY = (int)std::round((float)sumOfYChanges / 6.0);

    # 预测对象下一个位置的 X、Y 坐标值
    predictedNextPosition.x = centerPositions.back().x + deltaX;
    predictedNextPosition.y = centerPositions.back().y + deltaY;

}
# 如果检测到的中心位置数量大于等于 5
else if (numPositions >= 5) {

    # 计算 X 轴位置变化量之和，其中最后一个位置变化量乘以4，倒数第二个位置变化量乘以3，倒数第三个位置变化量乘以2，倒数第四个位置变化量乘以1
    int sumOfXChanges = ((centerPositions[numPositions - 1].x - centerPositions[numPositions - 2].x) * 4) +
        ((centerPositions[numPositions - 2].x - centerPositions[numPositions - 3].x) * 3) +
        ((centerPositions[numPositions - 3].x - centerPositions[numPositions - 4].x) * 2) +
        ((centerPositions[numPositions - 4].x - centerPositions[numPositions - 5].x) * 1);

    # 计算 X 轴变化量的平均值，向最近的整数四舍五入
    int deltaX = (int)std::round((float)sumOfXChanges / 10.0);

    # 计算 Y 轴位置变化量之和，其中最后一个位置变化量乘以4，倒数第二个位置变化量乘以3，倒数第三个位置
*/