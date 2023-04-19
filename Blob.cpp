#include "Blob.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Blob 类的构造函数，传入参数为cv::Point 对象的向量
Blob::Blob(std::vector<cv::Point> _contour) {
    // 当前 Blob 对象的轮廓点设置为传入的参数
    currentContour = _contour;

    // 获取当前 Blob 对象的边界矩形
    currentBoundingRect = cv::boundingRect(currentContour);

    // 定义一个 Point 对象表示当前 Blob 对象的中心点
    cv::Point currentCenter;

    // 计算中心点的 x 坐标
    currentCenter.x = (currentBoundingRect.x + currentBoundingRect.x + currentBoundingRect.width) / 2;

    // 计算中心点的 y 坐标
    currentCenter.y = (currentBoundingRect.y + currentBoundingRect.y + currentBoundingRect.height) / 2;

    // 将中心点坐标添加到 centerPositions 中
    centerPositions.push_back(currentCenter);

    // 计算当前 Blob 对象的对角线长度
    dblCurrentDiagonalSize = sqrt(pow(currentBoundingRect.width, 2) + pow(currentBoundingRect.height, 2));

    // 计算当前 Blob 对象的宽高比
    dblCurrentAspectRatio = (float)currentBoundingRect.width / (float)currentBoundingRect.height;

    // 设置当前 Blob 对象仍在被跟踪
    blnStillBeingTracked = true;

    // 当前 Blob 对象是新的或者已经被匹配
    blnCurrentMatchFoundOrNewBlob = true;

    // 当前 Blob 对象在连续的帧中没有被匹配的数量为 0
    intNumOfConsecutiveFramesWithoutAMatch = 0;
}

/*
这是一段 C++ 代码，其目的是根据物体的移动轨迹，预测出物体下一步的位置。代码中定义了一个叫做 Blob 的类，
并在该类中实现了一个叫做 predictNextPosition 的成员函数，用于预测物体的下一个位置。

代码中，首先获取了物体移动轨迹的长度，即 centerPositions 的大小，然后通过不同的分支结构，对不同长度的轨迹采用不同的预测方法。

当轨迹长度为 1 时，直接将最后一个位置作为下一个位置的预测值；
当轨迹长度为 2 时，通过计算两个位置之间的距离，来预测下一个位置的位置；
当轨迹长度为 3 时，采用加权平均法来预测下一个位置的位置；
当轨迹长度为 4 时，同样采用加权平均法来预测下一个位置的位置；
当轨迹长度大于等于 5 时，同样采用加权平均法来预测下一个位置的位置，不同的是，此时需要考虑前 5 个位置之间的距离，即使用了更多的历史数据进行预测。
*/
LYJ分支修改测试void Blob::predictNextPosition(void) {

    // 获取当前Blob的历史中心点位置数量
    int numPositions = (int)centerPositions.size();

    // 如果历史中心点位置数量为1，即当前Blob只有一个中心点
    if (numPositions == 1) {
        // 则预测下一步的位置为当前中心点的位置
        predictedNextPosition.x = centerPositions.back().x;
        predictedNextPosition.y = centerPositions.back().y;

    }
    // 如果历史中心点位置数量为2，即当前Blob有两个中心点
    else if (numPositions == 2) {

        // 计算中心点位置变化量
        int deltaX = centerPositions[1].x - centerPositions[0].x;
        int deltaY = centerPositions[1].y - centerPositions[0].y;

        // 则预测下一步的位置为当前中心点的位置再加上中心点位置变化量
        predictedNextPosition.x = centerPositions.back().x + deltaX;
        predictedNextPosition.y = centerPositions.back().y + deltaY;

    }
    // 如果历史中心点位置数量为3，即当前Blob有三个中心点
    else if (numPositions == 3) {

        // 计算中心点位置变化量之和（x轴）
        int sumOfXChanges = ((centerPositions[2].x - centerPositions[1].x) * 2) +
            ((centerPositions[1].x - centerPositions[0].x) * 1);

        // 计算中心点位置变化量之和的平均值（x轴）
        int deltaX = (int)std::round((float)sumOfXChanges / 3.0);

        // 计算中心点位置变化量之和（y轴）
        int sumOfYChanges = ((centerPositions[2].y - centerPositions[1].y) * 2) +
            ((centerPositions[1].y - centerPositions[0].y) * 1);

        // 计算中心点位置变化量之和的平均值（y轴）
        int deltaY = (int)std::round((float)sumOfYChanges / 3.0);

        // 预测下一步的位置为当前中心点的位置再加上中心点位置变化量的平均值
        predictedNextPosition.x = centerPositions.back().x + deltaX;
        predictedNextPosition.y = centerPositions.back().y + deltaY;

    }
    /*
    这段代码通过计算运动物体在前四帧中在x和y方向上的移动距离，来预测下一帧中物体的位置。
    首先，代码将前四帧中物体在x和y方向上的移动距离加权求和，权重分别为3、2、1，得到物体在x和y方向上的总移动距离。
    然后，代码将总移动距离除以6，取整后得到平均每帧的移动距离。
    最后，代码通过将上一帧物体的位置加上平均移动距离，得到预测的下一帧物体的位置。
    */
    // 如果观察到车辆连续移动了4帧
    else if (numPositions == 4) {

        // 计算X方向的变化总和
        int sumOfXChanges = ((centerPositions[3].x - centerPositions[2].x) * 3) +
            ((centerPositions[2].x - centerPositions[1].x) * 2) +
            ((centerPositions[1].x - centerPositions[0].x) * 1);

        // 计算X方向的变化平均值
        int deltaX = (int)std::round((float)sumOfXChanges / 6.0);

        // 计算Y方向的变化总和
        int sumOfYChanges = ((centerPositions[3].y - centerPositions[2].y) * 3) +
            ((centerPositions[2].y - centerPositions[1].y) * 2) +
            ((centerPositions[1].y - centerPositions[0].y) * 1);

        // 计算Y方向的变化平均值
        int deltaY = (int)std::round((float)sumOfYChanges / 6.0);

        // 预测下一步的位置为当前中心点的位置再加上中心点位置变化量的平均值
        predictedNextPosition.x = centerPositions.back().x + deltaX;
        predictedNextPosition.y = centerPositions.back().y + deltaY;

    }
    // 如果观察到车辆连续移动了5帧及以上
    else if (numPositions >= 5) {

        // 计算x轴方向移动的总和
        int sumOfXChanges = ((centerPositions[numPositions - 1].x - centerPositions[numPositions - 2].x) * 4) +
            ((centerPositions[numPositions - 2].x - centerPositions[numPositions - 3].x) * 3) +
            ((centerPositions[numPositions - 3].x - centerPositions[numPositions - 4].x) * 2) +
            ((centerPositions[numPositions - 4].x - centerPositions[numPositions - 5].x) * 1);

        // 计算X方向的变化平均值
        int deltaX = (int)std::round((float)sumOfXChanges / 10.0);

        // 计算y轴方向移动的总和
        int sumOfYChanges = ((centerPositions[numPositions - 1].y - centerPositions[numPositions - 2].y) * 4) +
            ((centerPositions[numPositions - 2].y - centerPositions[numPositions - 3].y) * 3) +
            ((centerPositions[numPositions - 3].y - centerPositions[numPositions - 4].y) * 2) +
            ((centerPositions[numPositions - 4].y - centerPositions[numPositions - 5].y) * 1);

        // 计算y方向的变化平均值
        int deltaY = (int)std::round((float)sumOfYChanges / 10.0);

        // 预测下一步的位置为当前中心点的位置再加上中心点位置变化量的平均值
        predictedNextPosition.x = centerPositions.back().x + deltaX;
        predictedNextPosition.y = centerPositions.back().y + deltaY;

    }
    else {
        // should never get here
    }

}