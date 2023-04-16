// main.cpp
// 引入OpenCV库中的核心、图形用户界面和图像处理模块的头文件
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

// 引入输入和输出操作所需的标准库
#include<iostream>
#include<conio.h>           // 如果不是使用Windows系统，可能需要更改或删除此行



#include "Blob.h"

// 定义宏来控制是否显示中间处理步骤
#define SHOW_STEPS            // 取消或注释掉此行来显示或隐藏步骤

// 全局变量 ///////////////////////////////////////////////////////////////////////////////
// 定义Scalar类型的常量，用于表示OpenCV中的颜色
const cv::Scalar SCALAR_BLACK = cv::Scalar(0.0, 0.0, 0.0);
const cv::Scalar SCALAR_WHITE = cv::Scalar(255.0, 255.0, 255.0);
const cv::Scalar SCALAR_YELLOW = cv::Scalar(0.0, 255.0, 255.0);
const cv::Scalar SCALAR_GREEN = cv::Scalar(0.0, 200.0, 0.0);
const cv::Scalar SCALAR_RED = cv::Scalar(0.0, 0.0, 255.0);

// 上述代码的功能是导入OpenCV库和标准库所需的头文件和宏，以及定义常量和变量。其中常量是表示颜色的常量，变量是用于Blob检测的全局变量。

// 函数原型 ////////////////////////////////////////////////////////////////////////////
// 将当前帧的Blob与已有的Blob进行匹配
void matchCurrentFrameBlobsToExistingBlobs(std::vector<Blob>& existingBlobs, std::vector<Blob>& currentFrameBlobs);
// 将当前帧的Blob加入已有的Blob中
void addBlobToExistingBlobs(Blob& currentFrameBlob, std::vector<Blob>& existingBlobs, int& intIndex);
// 将新的Blob加入已有的Blob中
void addNewBlob(Blob& currentFrameBlob, std::vector<Blob>& existingBlobs);
// 计算两个点之间的距离
double distanceBetweenPoints(cv::Point point1, cv::Point point2);
// 绘制并显示轮廓
void drawAndShowContours(cv::Size imageSize, std::vector<std::vector<cv::Point> > contours, std::string strImageName);
// 绘制并显示Blob
void drawAndShowContours(cv::Size imageSize, std::vector<Blob> blobs, std::string strImageName);
// 检查Blob是否穿过给定的线，并返回穿过线的Blob数量
bool checkIfBlobsCrossedTheLine(std::vector<Blob>& blobs, int& intHorizontalLinePosition, int& carCount);
// 在图像上绘制Blob信息
void drawBlobInfoOnImage(std::vector<Blob>& blobs, cv::Mat& imgFrame2Copy);
// 在图像上绘制汽车数量
void drawCarCountOnImage(int& carCount, cv::Mat& imgFrame2Copy);

// 上述代码的功能是声明函数原型，包括用于Blob匹配、Blob绘制和汽车穿过线的检测等。

///////////////////////////////////////////////////////////////////////////////////////////////////
int main(void) {

    // 打开名视频文件，并检查是否成功打开。如果无法打开视频文件，则显示错误消息并退出程序。
    // 定义了一个名为capVideo的cv::VideoCapture对象，用于从视频文件中读取帧。
    cv::VideoCapture capVideo;

    // 定义了一个名为imgFrame1的cv::Mat对象，用于存储视频的第一帧图像。
    cv::Mat imgFrame1;
    // 定义了一个名为imgFrame2的cv::Mat对象，用于存储视频的第二帧图像。
    cv::Mat imgFrame2;

    // 定义了一个名为blobs的std::vector对象，用于存储检测到的车辆。
    std::vector<Blob> blobs;

    // 定义了一个名为crossingLine的cv::Point数组，用于存储跨越的线段的起点和终点坐标。
    cv::Point crossingLine[2];

    // 定义了一个名为carCount的整数变量，用于存储检测到的车辆数量，并将其初始化为0。
    int carCount = 0;

    // 打开为视频文件，可以是绝对路径或者子目录文件夹内的视频文件，路径一致就行，并将其赋值给capVideo对象。
    capVideo.open("CarsDrivingUnderBridge.mp4");

    // 检查capVideo对象是否成功打开视频文件。如果打开失败，则显示错误消息，等待用户输入任何字符并退出程序。
    if (!capVideo.isOpened()) {                                                 // 如果打开视频文件失败
        std::cout << "error reading video file" << std::endl << std::endl;      // 显示错误信息
        _getch();                   // 如果不在Windows平台编译，可能需要更改或删除此行
        return(0);                                                              // 然后退出程序
    }

    if (capVideo.get(CV_CAP_PROP_FRAME_COUNT) < 2) {
        std::cout << "error: video file must have at least two frames";
        _getch();                   // 如果不在Windows平台编译，可能需要更改或删除此行
        return(0);                  // 然后退出程序
    }

    // 从视频文件中读取一帧图像到imgFrame1中
    capVideo.read(imgFrame1);
    // 从视频文件中读取一帧图像到imgFrame2中
    capVideo.read(imgFrame2);

    // 计算跨越线的位置// 计算通过图像中间的水平线的y坐标
    int intHorizontalLinePosition = (int)std::round((double)imgFrame1.rows * 0.35);

    // 设置水平线的左侧点的x坐标为0
    crossingLine[0].x = 0;
    // 设置水平线的左侧点的y坐标为计算得到的y坐标
    crossingLine[0].y = intHorizontalLinePosition;

    // 设置水平线的右侧点的x坐标为图像宽度减1
    crossingLine[1].x = imgFrame1.cols - 1;
    // 设置水平线的右侧点的y坐标为计算得到的y坐标
    crossingLine[1].y = intHorizontalLinePosition;

    // 定义一个名为chCheckForEscKey的字符变量，用于存储用户按键的值。// 初始化检查Esc键的变量为0
    char chCheckForEscKey = 0;

    // 初始化第一帧图像的变量为true
    bool blnFirstFrame = true;

    // 初始化帧计数器为2（因为已经读入了两帧图像）
    int frameCount = 2;

    // 当视频文件打开且没有按下Esc键时，执行循环
    while (capVideo.isOpened() && chCheckForEscKey != 27) {

        // 声明一个向量，用于存储当前帧中检测到的运动物体
        std::vector<Blob> currentFrameBlobs;

        // 下面的代码是对两帧图像进行处理，得到前景物体的二值化图像。
        // 首先定义一个currentFrameBlobs变量，用于存储当前帧中检测到的所有Blob（一组相邻像素组成的对象，表示图像中的一个物体）。
        // 然后将imgFrame1和imgFrame2分别复制到imgFrame1Copy和imgFrame2Copy中，以便对它们进行处理，而不影响原始图像。
        cv::Mat imgFrame1Copy = imgFrame1.clone();
        cv::Mat imgFrame2Copy = imgFrame2.clone();

        // 定义一个名为imgDifference的cv::Mat对象，用于存储两帧图像的差值。
        cv::Mat imgDifference;
        // 定义一个名为imgThresh的cv::Mat对象，用于存储二值化图像。
        cv::Mat imgThresh;

        // 将imgFrame1Copy和imgFrame2Copy转换为灰度图像。
        cv::cvtColor(imgFrame1Copy, imgFrame1Copy, CV_BGR2GRAY);
        cv::cvtColor(imgFrame2Copy, imgFrame2Copy, CV_BGR2GRAY);

        // 使用高斯模糊对imgFrame1Copy和imgFrame2Copy进行平滑处理，以去除噪声和细节。
        cv::GaussianBlur(imgFrame1Copy, imgFrame1Copy, cv::Size(5, 5), 0);
        cv::GaussianBlur(imgFrame2Copy, imgFrame2Copy, cv::Size(5, 5), 0);

        // 对imgFrame1Copy和imgFrame2Copy进行差分，得到两帧图像的差异图像imgDifference。
        cv::absdiff(imgFrame1Copy, imgFrame2Copy, imgDifference);

        // 对imgDifference进行阈(二)值化处理，将大于30的像素设为255，小于等于30的像素设为0，得到前景物体的二值化图像imgThresh。
        cv::threshold(imgDifference, imgThresh, 30, 255.0, CV_THRESH_BINARY);

        // 最后，使用cv::imshow()函数将二值化图像imgThresh显示在名为"imgThresh"的窗口中，以便检查和调试。
        cv::imshow("imgThresh", imgThresh);

        // for循环之前这段代码是在进行形态学变换和轮廓提取。
        // 创建了一个3x3的矩形结构元素。
        cv::Mat structuringElement3x3 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
        // 创建了一个5x5的矩形结构元素。
        cv::Mat structuringElement5x5 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
        // 创建了一个7x7的矩形结构元素。
        cv::Mat structuringElement7x7 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7));
        // 创建了一个15x15的矩形结构元素。
        cv::Mat structuringElement15x15 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15));

        // 对imgThresh进行形态学闭运算，以填充前景物体中的空洞。
        // 通过for循环对imgThresh进行膨胀和腐蚀操作，可以使目标区域更加平滑，去除一些不必要的细节。
        for (unsigned int i = 0; i < 2; i++) {

            cv::dilate(imgThresh, imgThresh, structuringElement5x5);
            cv::dilate(imgThresh, imgThresh, structuringElement5x5);
            cv::erode(imgThresh, imgThresh, structuringElement5x5);
        }

        // 复制imgThresh到imgThreshCopy，以便于在之后的处理中进行比较。
        cv::Mat imgThreshCopy = imgThresh.clone();

        // 定义一个vector容器contours，用于存储轮廓。
        std::vector<std::vector<cv::Point> > contours;

        // 从imgThreshCopy中查找轮廓，将轮廓存储到contours中。cv::RETR_EXTERNAL表示只查找最外层轮廓，cv::CHAIN_APPROX_SIMPLE表示使用简单的逼近方法来压缩轮廓，从而减少轮廓点数。
        cv::findContours(imgThreshCopy, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        // 调用自定义函数drawAndShowContours()，该函数用于在图像上绘制轮廓，并显示图像窗口。
        drawAndShowContours(imgThresh.size(), contours, "imgContours");

        // 定义一个vector容器convexHulls，用于存储凸包。
        std::vector<std::vector<cv::Point> > convexHulls(contours.size());

        /*
        这段代码是在对凸包进行筛选，将符合条件的凸包转化为可能的运动物体Blob，并存储到currentFrameBlobs中。
        其中，凸包的筛选条件包括面积、宽高比、对角线长度和轮廓面积比等，可以根据具体情况进行调整。
        具体流程如下：
            遍历所有的轮廓，对每个轮廓进行凸包操作，并将凸包存储在convexHulls中。
            将所有凸包绘制出来，并显示在名为"imgConvexHulls"的窗口中。
            遍历所有的凸包，将符合条件的凸包转化为可能的运动物体Blob，并存储到currentFrameBlobs中。
            将所有可能的运动物体Blob绘制出来，并显示在名为"imgCurrentFrameBlobs"的窗口中。
        */
        for (unsigned int i = 0; i < contours.size(); i++) { // 对于图像中的每个轮廓
            cv::convexHull(contours[i], convexHulls[i]); // 执行凸包算法，得到凸包（convex hull）
        }

        drawAndShowContours(imgThresh.size(), convexHulls, "imgConvexHulls"); // 将所有凸包绘制出来，并显示在名为"imgConvexHulls"的窗口中。

        for (auto& convexHull : convexHulls) { // 对于图像中的每个凸包
            Blob possibleBlob(convexHull); // 将凸包转化为可能的运动物体Blob类实例

            if (possibleBlob.currentBoundingRect.area() > 400 &&
                possibleBlob.dblCurrentAspectRatio > 0.2 &&
                possibleBlob.dblCurrentAspectRatio < 4.0 &&
                possibleBlob.currentBoundingRect.width > 30 &&
                possibleBlob.currentBoundingRect.height > 30 &&
                possibleBlob.dblCurrentDiagonalSize > 60.0 &&
                (cv::contourArea(possibleBlob.currentContour) / (double)possibleBlob.currentBoundingRect.area()) > 0.50) {
                currentFrameBlobs.push_back(possibleBlob); // 将符合条件的凸包转化为可能的运动物体Blob，并存储到currentFrameBlobs中
            }
        }

        drawAndShowContours(imgThresh.size(), currentFrameBlobs, "imgCurrentFrameBlobs"); // 将所有可能的运动物体Blob绘制出来，并显示在名为"imgCurrentFrameBlobs"的窗口中。


        /*
            这段代码是跟踪汽车运动的核心部分，其中的if条件判断是用来判断当前是否为视频流的第一帧。
            如果是，则将currentFrameBlobs中的所有Blob对象全部添加到blobs中，
            如果不是，则将currentFrameBlobs中的Blob对象与blobs中已有的Blob对象进行匹配。
                接下来的一系列操作包括绘制轮廓、绘制Blob信息、检查Blob是否穿过了一个水平的线、在图像上绘制车的数量以及显示输出图像。
                具体来说，drawAndShowContours()函数用于在图像上绘制轮廓，它接受图像大小、轮廓和显示的窗口名称作为参数。
                drawBlobInfoOnImage()函数用于在图像上绘制Blob的信息，例如ID、中心位置、外接矩形等。
                checkIfBlobsCrossedTheLine()函数用于检查Blob是否穿过了预设的水平线，它接受包含所有Blob的vector、水平线位置和当前检测到的车辆数量作为参数。
                // drawCarCountOnImage()函数用于在图像上绘制当前检测到的车辆数量。
                最后，imshow()函数用于显示输出图像。
                下面是每行代码具体注释。
        */
        // 这段代码是在对当前帧中的Blob进行匹配，将匹配成功的Blob存储到blobs中。
        if (blnFirstFrame == true) { // 如果是第一帧，则将当前帧中的Blob存储到blobs中。
            for (auto& currentFrameBlob : currentFrameBlobs) { // 遍历当前帧中的所有 Blob
                // 将当前帧中的Blob存储到blobs中。
                blobs.push_back(currentFrameBlob);
            }
        }
        else {
            // 如果不是第一帧，则调用matchCurrentFrameBlobsToExistingBlobs()函数，将当前帧的 Blob 与之前的 Blob 进行匹配
            matchCurrentFrameBlobsToExistingBlobs(blobs, currentFrameBlobs);
        }

        // 调用自定义函数drawAndShowContours()，该函数用于在图像上绘制Blob，并显示所有 Blob 的轮廓在图像窗口上。
        drawAndShowContours(imgThresh.size(), blobs, "imgBlobs");

        // 获取 frame2 的另一份副本，因为上面的处理已经改变了前一个第2帧的副本
        imgFrame2Copy = imgFrame2.clone();

        // 在图像上绘制 Blob 的信息
        drawBlobInfoOnImage(blobs, imgFrame2Copy);

        // 检查是否有 Blob 跨越了预设的水平线
        bool blnAtLeastOneBlobCrossedTheLine = checkIfBlobsCrossedTheLine(blobs, intHorizontalLinePosition, carCount);

        if (blnAtLeastOneBlobCrossedTheLine == true) { // 如果至少有一个 Blob 跨越了线
            cv::line(imgFrame2Copy, crossingLine[0], crossingLine[1], SCALAR_GREEN, 2); // 在图像上绘制绿色的跨线，这里速度很快绿色删一下就过了。
        }
        else {
            cv::line(imgFrame2Copy, crossingLine[0], crossingLine[1], SCALAR_RED, 2); // 在图像上绘制红色的跨线，所以一直没有Blob跨越默认就为红色的跨线。
        }

        // 在图像上绘制汽车计数
        drawCarCountOnImage(carCount, imgFrame2Copy);

        // 输出显示图像副本
        cv::imshow("imgFrame2Copy", imgFrame2Copy);

        //cv::waitKey(0);                 // 取消注释此行以进行逐帧调试

        // 现在我们为下一次迭代做准备

        currentFrameBlobs.clear(); // 清空当前帧中的 Blob(容器)因为我们已经处理了它们了

        imgFrame1 = imgFrame2.clone();           // 将帧 1 移动到帧 2 的位置

        if ((capVideo.get(CV_CAP_PROP_POS_FRAMES) + 1) < capVideo.get(CV_CAP_PROP_FRAME_COUNT)) { // 如果视频中还有帧,也就是如果当前帧不是最后一帧
            capVideo.read(imgFrame2); // 读取下一帧
        }
        else {
            std::cout << "end of video\n"; // 如果当前帧是最后一帧，视频结束，输出“end of video”，
            break; // 跳出循环
        }

        blnFirstFrame = false; // 现在我们已经处理了第一帧，所以我们可以设置 blnFirstFrame = false
        frameCount++; // 帧计数器自增
        chCheckForEscKey = cv::waitKey(1); // 等待 1 毫秒，检查是否有 ESC 键按下
    }

    if (chCheckForEscKey != 27) {               // 如果用户没有按下 ESC 键（即我们到达了视频的结尾）
        cv::waitKey(0);                         // 保持窗口打开，以便“end of video”消息能够显示
    }
    // 请注意，如果用户按下 ESC 键，我们不需要保持窗口打开，我们可以让程序结束，从而关闭窗口


    return(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/*
这个函数的作用是将当前帧的Blob与已存在的Blob进行匹配，更新已存在的Blob的信息，如果当前帧中的Blob没有匹配到已存在的Blob，则将其视为新的Blob并添加到已存在的Blob列表中。具体的实现方式如下：

对于每一个已存在的Blob，首先将其标记为没有匹配到或者是新的Blob，然后根据历史轨迹预测其下一帧出现的位置。

对于当前帧中的每一个Blob，遍历已存在的Blob列表，找到与之距离最近的已存在的Blob，若其距离小于当前Blob的对角线长度的一半，则将当前Blob与已存在的Blob进行匹配；否则，将当前Blob视为新的Blob。

遍历已存在的Blob列表，将没有匹配到的Blob的连续未匹配帧数加1，如果已经连续未匹配了5帧，则将其从已存在的Blob列表中移除。

函数中用到的一些变量：

existingBlobs：已存在的Blob列表；
currentFrameBlobs：当前帧中的Blob列表；
intIndexOfLeastDistance：距离当前Blob最近的已存在的Blob的索引；
dblLeastDistance：距离当前Blob最近的已存在的Blob的距离；
distanceBetweenPoints：计算两点间距离的函数。

std::vector<Blob>& existingBlobs：已经被跟踪的 Blob 的集合
std::vector<Blob>& currentFrameBlobs：当前帧中的 Blob 的集合


*/
void matchCurrentFrameBlobsToExistingBlobs(std::vector<Blob>& existingBlobs, std::vector<Blob>& currentFrameBlobs) {

    // 对于每一个已经被跟踪的 Blob
    for (auto& existingBlob : existingBlobs) {

        // 设置当前没有找到匹配的标志为 false
        existingBlob.blnCurrentMatchFoundOrNewBlob = false;

        // 预测下一个位置
        existingBlob.predictNextPosition();
    }

    // 对于每一个当前帧中的 Blob
    for (auto& currentFrameBlob : currentFrameBlobs) {

        // 找到距离当前 Blob 最近的已经被跟踪的 Blob 的索引
        int intIndexOfLeastDistance = 0;
        double dblLeastDistance = 100000.0;

        for (unsigned int i = 0; i < existingBlobs.size(); i++) {

            // 如果 Blob 还在跟踪中
            if (existingBlobs[i].blnStillBeingTracked == true) {

                // 计算当前 Blob 和已经被跟踪的 Blob 之间的距离
                double dblDistance = distanceBetweenPoints(currentFrameBlob.centerPositions.back(), existingBlobs[i].predictedNextPosition);

                // 如果距离比之前计算的距离还要小，则更新
                if (dblDistance < dblLeastDistance) {
                    dblLeastDistance = dblDistance;
                    intIndexOfLeastDistance = i;
                }
            }
        }

        // 如果最小距离小于当前 Blob 的对角线的 50%，则将其添加到已经存在的 Blob 中
        if (dblLeastDistance < currentFrameBlob.dblCurrentDiagonalSize * 0.5) {
            addBlobToExistingBlobs(currentFrameBlob, existingBlobs, intIndexOfLeastDistance);
        }
        // 如果最小距离大于当前 Blob 的对角线的 50%，则将其添加为一个新的 Blob
        else {
            addNewBlob(currentFrameBlob, existingBlobs);
        }

    }

    // 对于每一个已经被跟踪的 Blob
    for (auto& existingBlob : existingBlobs) {

        // 如果当前没有找到匹配的 Blob，则增加未匹配帧的计数器
        if (existingBlob.blnCurrentMatchFoundOrNewBlob == false) {
            existingBlob.intNumOfConsecutiveFramesWithoutAMatch++;
        }

        // 如果未匹配的帧的数量超过了 5 帧，则停止跟踪该 Blob
        if (existingBlob.intNumOfConsecutiveFramesWithoutAMatch >= 5) {
            existingBlob.blnStillBeingTracked = false;
        }

    }

}


///////////////////////////////////////////////////////////////////////////////////////////////////
/*
Blob& currentFrameBlob：传入当前帧检测到的Blob对象。
std::vector<Blob>& existingBlobs：传入存储原有Blob对象的vector容器。
int& intIndex：传入指向原有Blob的索引值。

该函数的作用是将当前帧检测到的Blob对象的信息添加到原有的Blob对象中，从而更新Blob对象。
其中，原有的Blob对象是通过前一帧得到的，而当前帧检测到的Blob对象可能与前一帧的某个Blob对象匹配，
也可能是新出现的。通过intIndex索引值的判断，可以将当前帧的Blob对象与原有的Blob对象匹配起来，更新原有的Blob对象的相关信息。

这个函数的作用是将当前帧中检测到的blob与已存在的blob进行匹配，
并更新已存在的blob的相关属性。函数的输入参数包括当前帧的blob、
已存在的blob向量和一个int类型的变量intIndex，
该变量记录了当前blob在已存在的blob向量中的索引。

具体来说，函数将当前帧的blob的轮廓和外接矩形赋值给已存在的blob向量中相应的blob，
同时将当前blob的中心位置、对角线长度、长宽比等属性更新到已存在的blob中。
最后，函数将该blob的blnStillBeingTracked属性和blnCurrentMatchFoundOrNewBlob属性都设置为true，表示该blob在当前帧中被成功匹配。




*/

void addBlobToExistingBlobs(Blob& currentFrameBlob, std::vector<Blob>& existingBlobs, int& intIndex) {

    // 将当前帧的轮廓和轮廓外接矩形赋值给原有的Blob
    existingBlobs[intIndex].currentContour = currentFrameBlob.currentContour;
    existingBlobs[intIndex].currentBoundingRect = currentFrameBlob.currentBoundingRect;

    // 将当前帧的中心点坐标加入原有的Blob中心点坐标列表
    existingBlobs[intIndex].centerPositions.push_back(currentFrameBlob.centerPositions.back());

    // 将当前帧的对角线长度和长宽比赋值给原有的Blob
    existingBlobs[intIndex].dblCurrentDiagonalSize = currentFrameBlob.dblCurrentDiagonalSize;
    existingBlobs[intIndex].dblCurrentAspectRatio = currentFrameBlob.dblCurrentAspectRatio;

    // 将原有的Blob的跟踪状态和匹配状态设置为true
    existingBlobs[intIndex].blnStillBeingTracked = true;
    existingBlobs[intIndex].blnCurrentMatchFoundOrNewBlob = true;
}






///////////////////////////////////////////////////////////////////////////////////////////////////
/*
这个函数的作用是将一个新的 Blob 对象添加到已经存在的 Blob 向量中。
该函数接受两个参数：当前帧中的 Blob 对象和已经存在的 Blob 向量。
在函数内部，将标记当前 Blob 已经匹配，并将其添加到现有 Blob 向量中。
*/
void addNewBlob(Blob& currentFrameBlob, std::vector<Blob>& existingBlobs) {
    // 将当前帧的blob标记为匹配到的
    currentFrameBlob.blnCurrentMatchFoundOrNewBlob = true;
    // 将当前帧的blob添加到已存在的blob列表中
    existingBlobs.push_back(currentFrameBlob);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/*
这个函数的作用是计算两个二维坐标点之间的欧几里得距离，返回距离值。
在这个函数中，通过计算点1和点2在水平和垂直方向上的距离差，然后使用勾股定理计算距离。
其中，abs()函数用于获取绝对值，pow()函数用于计算指定数字的幂，
sqrt()函数用于获取平方根。
*/
double distanceBetweenPoints(cv::Point point1, cv::Point point2) {

    int intX = abs(point1.x - point2.x);  // 计算x坐标之差的绝对值
    int intY = abs(point1.y - point2.y);  // 计算y坐标之差的绝对值

    return(sqrt(pow(intX, 2) + pow(intY, 2)));  // 计算两点之间的距离并返回

}

///////////////////////////////////////////////////////////////////////////////////////////////////
/*
这个函数的作用是在图像上绘制并展示轮廓。它接收一个cv::Size类型的参数imageSize，表示图像的大小，
一个std::vector<std::vectorcv::Point >类型的参数contours，表示所有要绘制的轮廓，
以及一个std::string类型的参数strImageName，表示展示窗口的名称。

该函数首先根据图像大小创建一个黑色的图像。
然后，使用cv::drawContours函数在黑色的图像上绘制所有传入的轮廓。
最后，使用cv::imshow函数展示绘制好轮廓的图像。

drawAndShowContours：绘制轮廓并展示在窗口中
imageSize：窗口大小
contours：轮廓
strImageName：窗口名称
cv::Mat image(imageSize, CV_8UC3, SCALAR_BLACK);：创建一个大小为 imageSize 的黑色图像，其中 CV_8UC3 表示像素类型为 8 位无符号整数，通道数为 3
cv::drawContours(image, contours, -1, SCALAR_WHITE, -1);：在 image 中绘制 contours 中的轮廓，使用白色线条，线条宽度为 -1，表示绘制轮廓内部区域
cv::imshow(strImageName, image);：在窗口中展示图像，窗口名称为 strImageName
*/
void drawAndShowContours(cv::Size imageSize, std::vector<std::vector<cv::Point> > contours, std::string strImageName) {
    cv::Mat image(imageSize, CV_8UC3, SCALAR_BLACK);  // 创建一个黑色图像

    cv::drawContours(image, contours, -1, SCALAR_WHITE, -1);  // 绘制轮廓

    cv::imshow(strImageName, image);  // 在窗口中展示图像
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/*
这个函数的作用是在图像上绘制轮廓并显示结果。它接受三个参数：图像大小、
一个Blob对象的向量和窗口名称。函数首先创建一个黑色的图像。然后，函数遍历Blob对象向量
，并将包含仍在被跟踪的Blob对象的轮廓添加到轮廓向量中。
接下来，cv::drawContours()函数将轮廓向量绘制在黑色图像上，并用白色填充。
最后，函数将显示带有指定名称的图像窗口
*/
void drawAndShowContours(cv::Size imageSize, std::vector<Blob> blobs, std::string strImageName) {

    // 创建指定大小的黑色图像
    cv::Mat image(imageSize, CV_8UC3, SCALAR_BLACK);

    std::vector<std::vector<cv::Point> > contours;  // 存储轮廓的容器

    // 遍历所有的blob，如果该blob仍然被跟踪，则将其轮廓添加到contours中
    for (auto& blob : blobs) {
        if (blob.blnStillBeingTracked == true) {
            contours.push_back(blob.currentContour);
        }
    }

    // 在图像上绘制所有的轮廓
    cv::drawContours(image, contours, -1, SCALAR_WHITE, -1);

    // 在屏幕上显示图像
    cv::imshow(strImageName, image);

}

///////////////////////////////////////////////////////////////////////////////////////////////////
/*
这个函数的作用是检查运动的物体是否跨过了给定的水平线，如果有跨过则增加车辆计数器，
函数返回是否至少有一个物体跨过了水平线。

函数输入参数包括一个Blob对象的向量，表示检测到的所有物体，一个整数表示水平线的位置，
以及一个整数表示车辆计数器。在函数内部，对于向量中的每个Blob对象，检查是否仍在跟踪
该物体，如果是并且该物体至少在两个帧中被检测到，那么检查该物体的中心位置是否跨越了
水平线。如果是，则将车辆计数器增加1，函数返回值设置为true，表示至少有一个物体跨越了水平线。函数返回false表示没有物体跨过水平线。
*/
bool checkIfBlobsCrossedTheLine(std::vector<Blob>& blobs, int& intHorizontalLinePosition, int& carCount) {
    bool blnAtLeastOneBlobCrossedTheLine = false; // 标记是否有blob穿过线

    for (auto blob : blobs) {  // 遍历所有blob

        if (blob.blnStillBeingTracked == true && blob.centerPositions.size() >= 2) {  // 如果blob还在被追踪并且至少有两个位置信息

            int prevFrameIndex = (int)blob.centerPositions.size() - 2;  // 前一个位置的索引
            int currFrameIndex = (int)blob.centerPositions.size() - 1;  // 当前位置的索引

            if (blob.centerPositions[prevFrameIndex].y > intHorizontalLinePosition && blob.centerPositions[currFrameIndex].y <= intHorizontalLinePosition) {  // 如果blob上一帧在下方，当前帧在上方，说明blob穿过线
                carCount++;  // 车辆计数加一
                blnAtLeastOneBlobCrossedTheLine = true;  // 标记有blob穿过线
            }
        }

    }

    return blnAtLeastOneBlobCrossedTheLine;  // 返回是否有blob穿过线

}

///////////////////////////////////////////////////////////////////////////////////////////////////
/*
这个函数是用来在图像上绘制每个 Blob 的信息的。它接受两个参数：一个包含所有 Blob 对象的向量，以及需要绘制信息的图像。

在函数中，它通过循环迭代每个 Blob，检查该 Blob 是否仍在跟踪中。如果是，则在图像上绘制矩形，该矩形由 Blob 的 currentBoundingRect 变量定义，用红色绘制，边框宽度为 2。

然后，函数使用当前 Blob 对象的 centerPositions 向量中的最后一个点来定位该 Blob 对象在图像上的位置，该点是它在当前帧中的中心。它使用这个点来将 Blob 的 ID 绘制在图像上，
使用 cv::putText 函数，文字大小和颜色可以根据 Blob 的大小进行缩放。
*/
void drawBlobInfoOnImage(std::vector<Blob>& blobs, cv::Mat& imgFrame2Copy) {

    for (unsigned int i = 0; i < blobs.size(); i++) {
        // 如果该blob仍在被跟踪，则在图像中绘制该blob的当前边界矩形
        if (blobs[i].blnStillBeingTracked == true) {
            cv::rectangle(imgFrame2Copy, blobs[i].currentBoundingRect, SCALAR_RED, 2);

            // 设置字体、大小、厚度
            int intFontFace = CV_FONT_HERSHEY_SIMPLEX;
            double dblFontScale = blobs[i].dblCurrentDiagonalSize / 60.0;
            int intFontThickness = (int)std::round(dblFontScale * 1.0);

            // 在blob的最后一个位置上绘制blob的编号
            cv::putText(imgFrame2Copy, std::to_string(i), blobs[i].centerPositions.back(), intFontFace, dblFontScale, SCALAR_GREEN, intFontThickness);
        }
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
//这个函数用于在图像上绘制车辆数量的文本
/*
这个函数的作用是在给定的图像上绘制车辆计数。它接受一个整数类型的车辆计数和一个OpenCV Mat类型的图像，
并在图像的右上角绘制文本，显示车辆计数。函数使用CV_FONT_HERSHEY_SIMPLEX字体类型，计算适当的字体大小和线宽，然后在给定的位置绘制文本。
*/
void drawCarCountOnImage(int& carCount, cv::Mat& imgFrame2Copy) {

    int intFontFace = CV_FONT_HERSHEY_SIMPLEX;  // 字体类型
    double dblFontScale = (imgFrame2Copy.rows * imgFrame2Copy.cols) / 300000.0;  // 字体大小
    int intFontThickness = (int)std::round(dblFontScale * 1.5);  // 字体线宽

    cv::Size textSize = cv::getTextSize(std::to_string(carCount), intFontFace, dblFontScale, intFontThickness, 0);  // 获取文本大小

    cv::Point ptTextBottomLeftPosition;  // 文本左下角位置

    ptTextBottomLeftPosition.x = imgFrame2Copy.cols - 1 - (int)((double)textSize.width * 1.25);  // 计算文本左下角x坐标
    ptTextBottomLeftPosition.y = (int)((double)textSize.height * 1.25);  // 计算文本左下角y坐标

    cv::putText(imgFrame2Copy, std::to_string(carCount), ptTextBottomLeftPosition, intFontFace, dblFontScale, SCALAR_GREEN, intFontThickness);  // 绘制文本

}
