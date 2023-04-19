// main.cpp
// ����OpenCV���еĺ��ġ�ͼ���û������ͼ����ģ���ͷ�ļ�
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

// ��������������������ı�׼��
#include<iostream>
#include<conio.h>           // �������ʹ��Windowsϵͳ��������Ҫ���Ļ�ɾ������



#include "Blob.h"

// ������������Ƿ���ʾ�м䴦������
#define SHOW_STEPS            // ȡ����ע�͵���������ʾ�����ز���

// ȫ�ֱ��� ///////////////////////////////////////////////////////////////////////////////
// ����Scalar���͵ĳ��������ڱ�ʾOpenCV�е���ɫ
const cv::Scalar SCALAR_BLACK = cv::Scalar(0.0, 0.0, 0.0);
const cv::Scalar SCALAR_WHITE = cv::Scalar(255.0, 255.0, 255.0);
const cv::Scalar SCALAR_YELLOW = cv::Scalar(0.0, 255.0, 255.0);
const cv::Scalar SCALAR_GREEN = cv::Scalar(0.0, 200.0, 0.0);
const cv::Scalar SCALAR_RED = cv::Scalar(0.0, 0.0, 255.0);

// ��������Ĺ����ǵ���OpenCV��ͱ�׼�������ͷ�ļ��ͺ꣬�Լ����峣���ͱ��������г����Ǳ�ʾ��ɫ�ĳ���������������Blob����ȫ�ֱ�����

// ����ԭ�� ////////////////////////////////////////////////////////////////////////////
// ����ǰ֡��Blob�����е�Blob����ƥ��
void matchCurrentFrameBlobsToExistingBlobs(std::vector<Blob>& existingBlob 你  s, std::vector<Blob>& currentFrameBlobs);
// ����ǰ֡��Blob�������е�Blob��
void addBlobToExistingBlobs(Blob& currentFrameBlob, std::vector<Blob>& existingBlobs, int& intIndex);
// ���µ�Blob�������е�Blob��
void addNewBlob(Blob& currentFrameBlob, std::vector<Blob>& existingBlobs);
// ����������֮��ľ���
double distanceBetweenPoints(cv::Point point1, cv::Point point2);
// ���Ʋ���ʾ����
void drawAndShowContours(cv::Size imageSize, std::vector<std::vector<cv::Point> > contours, std::string strImageName);
// ���Ʋ���ʾBlob
void drawAndShowContours(cv::Size imageSize, std::vector<Blob> blobs, std::string strImageName);
// ���Blob�Ƿ񴩹��������ߣ������ش����ߵ�Blob����
bool checkIfBlobsCrossedTheLine(std::vector<Blob>& blobs, int& intHorizontalLinePosition, int& carCount);
// ��ͼ���ϻ���Blob��Ϣ
void drawBlobInfoOnImage(std::vector<Blob>& blobs, cv::Mat& imgFrame2Copy);
// ��ͼ���ϻ�����������
void drawCarCountOnImage(int& carCount, cv::Mat& imgFrame2Copy);

// ��������Ĺ�������������ԭ�ͣ���������Blobƥ�䡢Blob���ƺ����������ߵļ��ȡ�

///////////////////////////////////////////////////////////////////////////////////////////////////
int main(void) {

    // ������Ƶ�ļ���������Ƿ�ɹ��򿪡�����޷�����Ƶ�ļ�������ʾ������Ϣ���˳�����
    // ������һ����ΪcapVideo��cv::VideoCapture�������ڴ���Ƶ�ļ��ж�ȡ֡��
    cv::VideoCapture capVideo;

    // ������һ����ΪimgFrame1��cv::Mat�������ڴ洢��Ƶ�ĵ�һ֡ͼ��
    cv::Mat imgFrame1;
    // ������һ����ΪimgFrame2��cv::Mat�������ڴ洢��Ƶ�ĵڶ�֡ͼ��
    cv::Mat imgFrame2;

    // ������һ����Ϊblobs��std::vector�������ڴ洢��⵽�ĳ�����
    std::vector<Blob> blobs;

    // ������һ����ΪcrossingLine��cv::Point���飬���ڴ洢��Խ���߶ε������յ����ꡣ
    cv::Point crossingLine[2];

    // ������һ����ΪcarCount���������������ڴ洢��⵽�ĳ����������������ʼ��Ϊ0��
    int carCount = 0;

    // ��Ϊ��Ƶ�ļ��������Ǿ���·��������Ŀ¼�ļ����ڵ���Ƶ�ļ���·��һ�¾��У������丳ֵ��capVideo����
    capVideo.open("CarsDrivingUnderBridge.mp4");

    // ���capVideo�����Ƿ�ɹ�����Ƶ�ļ��������ʧ�ܣ�����ʾ������Ϣ���ȴ��û������κ��ַ����˳�����
    if (!capVideo.isOpened()) {                                                 // �������Ƶ�ļ�ʧ��
        std::cout << "error reading video file" << std::endl << std::endl;      // ��ʾ������Ϣ
        _getch();                   // �������Windowsƽ̨���룬������Ҫ���Ļ�ɾ������
        return(0);                                                              // Ȼ���˳�����
    }

    if (capVideo.get(CV_CAP_PROP_FRAME_COUNT) < 2) {
        std::cout << "error: video file must have at least two frames";
        _getch();                   // �������Windowsƽ̨���룬������Ҫ���Ļ�ɾ������
        return(0);                  // Ȼ���˳�����
    }

    // ����Ƶ�ļ��ж�ȡһ֡ͼ��imgFrame1��
    capVideo.read(imgFrame1);
    // ����Ƶ�ļ��ж�ȡһ֡ͼ��imgFrame2��
    capVideo.read(imgFrame2);

    // �����Խ�ߵ�λ��// ����ͨ��ͼ���м��ˮƽ�ߵ�y����
    int intHorizontalLinePosition = (int)std::round((double)imgFrame1.rows * 0.35);

    // ����ˮƽ�ߵ������x����Ϊ0
    crossingLine[0].x = 0;
    // ����ˮƽ�ߵ������y����Ϊ����õ���y����
    crossingLine[0].y = intHorizontalLinePosition;

    // ����ˮƽ�ߵ��Ҳ���x����Ϊͼ����ȼ�1
    crossingLine[1].x = imgFrame1.cols - 1;
    // ����ˮƽ�ߵ��Ҳ���y����Ϊ����õ���y����
    crossingLine[1].y = intHorizontalLinePosition;

    // ����һ����ΪchCheckForEscKey���ַ����������ڴ洢�û�������ֵ��// ��ʼ�����Esc���ı���Ϊ0
    char chCheckForEscKey = 0;

    // ��ʼ����һ֡ͼ��ı���Ϊtrue
    bool blnFirstFrame = true;

    // ��ʼ��֡������Ϊ2����Ϊ�Ѿ���������֡ͼ��
    int frameCount = 2;

    // ����Ƶ�ļ�����û�а���Esc��ʱ��ִ��ѭ��
    while (capVideo.isOpened() && chCheckForEscKey != 27) {

        // ����һ�����������ڴ洢��ǰ֡�м�⵽���˶�����
        std::vector<Blob> currentFrameBlobs;

        // ����Ĵ����Ƕ���֡ͼ����д������õ�ǰ������Ķ�ֵ��ͼ��
        // ���ȶ���һ��currentFrameBlobs���������ڴ洢��ǰ֡�м�⵽������Blob��һ������������ɵĶ��󣬱�ʾͼ���е�һ�����壩��
        // Ȼ��imgFrame1��imgFrame2�ֱ��Ƶ�imgFrame1Copy��imgFrame2Copy�У��Ա�����ǽ��д���������Ӱ��ԭʼͼ��
        cv::Mat imgFrame1Copy = imgFrame1.clone();
        cv::Mat imgFrame2Copy = imgFrame2.clone();

        // ����һ����ΪimgDifference��cv::Mat�������ڴ洢��֡ͼ��Ĳ�ֵ��
        cv::Mat imgDifference;
        // ����һ����ΪimgThresh��cv::Mat�������ڴ洢��ֵ��ͼ��
        cv::Mat imgThresh;

        // ��imgFrame1Copy��imgFrame2Copyת��Ϊ�Ҷ�ͼ��
        cv::cvtColor(imgFrame1Copy, imgFrame1Copy, CV_BGR2GRAY);
        cv::cvtColor(imgFrame2Copy, imgFrame2Copy, CV_BGR2GRAY);

        // ʹ�ø�˹ģ����imgFrame1Copy��imgFrame2Copy����ƽ����������ȥ��������ϸ�ڡ�
        cv::GaussianBlur(imgFrame1Copy, imgFrame1Copy, cv::Size(5, 5), 0);
        cv::GaussianBlur(imgFrame2Copy, imgFrame2Copy, cv::Size(5, 5), 0);

        // ��imgFrame1Copy��imgFrame2Copy���в�֣��õ���֡ͼ��Ĳ���ͼ��imgDifference��
        cv::absdiff(imgFrame1Copy, imgFrame2Copy, imgDifference);

        // ��imgDifference������(��)ֵ��������������30��������Ϊ255��С�ڵ���30��������Ϊ0���õ�ǰ������Ķ�ֵ��ͼ��imgThresh��
        cv::threshold(imgDifference, imgThresh, 30, 255.0, CV_THRESH_BINARY);

        // ���ʹ��cv::imshow()��������ֵ��ͼ��imgThresh��ʾ����Ϊ"imgThresh"�Ĵ����У��Ա���͵��ԡ�
        cv::imshow("imgThresh", imgThresh);

        // forѭ��֮ǰ��δ������ڽ�����̬ѧ�任��������ȡ��
        // ������һ��3x3�ľ��νṹԪ�ء�
        cv::Mat structuringElement3x3 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
        // ������һ��5x5�ľ��νṹԪ�ء�
        cv::Mat structuringElement5x5 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
        // ������һ��7x7�ľ��νṹԪ�ء�
        cv::Mat structuringElement7x7 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7));
        // ������һ��15x15�ľ��νṹԪ�ء�
        cv::Mat structuringElement15x15 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15));

        // ��imgThresh������̬ѧ�����㣬�����ǰ�������еĿն���
        // ͨ��forѭ����imgThresh�������ͺ͸�ʴ����������ʹĿ���������ƽ����ȥ��һЩ����Ҫ��ϸ�ڡ�
        for (unsigned int i = 0; i < 2; i++) {

            cv::dilate(imgThresh, imgThresh, structuringElement5x5);
            cv::dilate(imgThresh, imgThresh, structuringElement5x5);
            cv::erode(imgThresh, imgThresh, structuringElement5x5);
        }

        // ����imgThresh��imgThreshCopy���Ա�����֮��Ĵ����н��бȽϡ�
        cv::Mat imgThreshCopy = imgThresh.clone();

        // ����һ��vector����contours�����ڴ洢������
        std::vector<std::vector<cv::Point> > contours;

        // ��imgThreshCopy�в����������������洢��contours�С�cv::RETR_EXTERNAL��ʾֻ���������������cv::CHAIN_APPROX_SIMPLE��ʾʹ�ü򵥵ıƽ�������ѹ���������Ӷ���������������
        cv::findContours(imgThreshCopy, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        // �����Զ��庯��drawAndShowContours()���ú���������ͼ���ϻ�������������ʾͼ�񴰿ڡ�
        drawAndShowContours(imgThresh.size(), contours, "imgContours");

        // ����һ��vector����convexHulls�����ڴ洢͹����
        std::vector<std::vector<cv::Point> > convexHulls(contours.size());

        /*
        ��δ������ڶ�͹������ɸѡ��������������͹��ת��Ϊ���ܵ��˶�����Blob�����洢��currentFrameBlobs�С�
        ���У�͹����ɸѡ����������������߱ȡ��Խ��߳��Ⱥ���������ȵȣ����Ը��ݾ���������е�����
        �����������£�
            �������е���������ÿ����������͹������������͹���洢��convexHulls�С�
            ������͹�����Ƴ���������ʾ����Ϊ"imgConvexHulls"�Ĵ����С�
            �������е�͹����������������͹��ת��Ϊ���ܵ��˶�����Blob�����洢��currentFrameBlobs�С�
            �����п��ܵ��˶�����Blob���Ƴ���������ʾ����Ϊ"imgCurrentFrameBlobs"�Ĵ����С�
        */
        for (unsigned int i = 0; i < contours.size(); i++) { // ����ͼ���е�ÿ������
            cv::convexHull(contours[i], convexHulls[i]); // ִ��͹���㷨���õ�͹����convex hull��
        }

        drawAndShowContours(imgThresh.size(), convexHulls, "imgConvexHulls"); // ������͹�����Ƴ���������ʾ����Ϊ"imgConvexHulls"�Ĵ����С�

        for (auto& convexHull : convexHulls) { // ����ͼ���е�ÿ��͹��
            Blob possibleBlob(convexHull); // ��͹��ת��Ϊ���ܵ��˶�����Blob��ʵ��

            if (possibleBlob.currentBoundingRect.area() > 400 &&
                possibleBlob.dblCurrentAspectRatio > 0.2 &&
                possibleBlob.dblCurrentAspectRatio < 4.0 &&
                possibleBlob.currentBoundingRect.width > 30 &&
                possibleBlob.currentBoundingRect.height > 30 &&
                possibleBlob.dblCurrentDiagonalSize > 60.0 &&
                (cv::contourArea(possibleBlob.currentContour) / (double)possibleBlob.currentBoundingRect.area()) > 0.50) {
                currentFrameBlobs.push_back(possibleBlob); // ������������͹��ת��Ϊ���ܵ��˶�����Blob�����洢��currentFrameBlobs��
            }
        }

        drawAndShowContours(imgThresh.size(), currentFrameBlobs, "imgCurrentFrameBlobs"); // �����п��ܵ��˶�����Blob���Ƴ���������ʾ����Ϊ"imgCurrentFrameBlobs"�Ĵ����С�


        /*
            ��δ����Ǹ��������˶��ĺ��Ĳ��֣����е�if�����ж��������жϵ�ǰ�Ƿ�Ϊ��Ƶ���ĵ�һ֡��
            ����ǣ���currentFrameBlobs�е�����Blob����ȫ�����ӵ�blobs�У�
            ������ǣ���currentFrameBlobs�е�Blob������blobs�����е�Blob�������ƥ�䡣
                ��������һϵ�в���������������������Blob��Ϣ�����Blob�Ƿ񴩹���һ��ˮƽ���ߡ���ͼ���ϻ��Ƴ��������Լ���ʾ���ͼ��
                ������˵��drawAndShowContours()����������ͼ���ϻ���������������ͼ���С����������ʾ�Ĵ���������Ϊ������
                drawBlobInfoOnImage()����������ͼ���ϻ���Blob����Ϣ������ID������λ�á���Ӿ��εȡ�
                checkIfBlobsCrossedTheLine()�������ڼ��Blob�Ƿ񴩹���Ԥ���ˮƽ�ߣ������ܰ�������Blob��vector��ˮƽ��λ�ú͵�ǰ��⵽�ĳ���������Ϊ������
                // drawCarCountOnImage()����������ͼ���ϻ��Ƶ�ǰ��⵽�ĳ���������
                ���imshow()����������ʾ���ͼ��
                ������ÿ�д������ע�͡�
        */
        // ��δ������ڶԵ�ǰ֡�е�Blob����ƥ�䣬��ƥ��ɹ���Blob�洢��blobs�С�
        if (blnFirstFrame == true) { // ����ǵ�һ֡���򽫵�ǰ֡�е�Blob�洢��blobs�С�
            for (auto& currentFrameBlob : currentFrameBlobs) { // ������ǰ֡�е����� Blob
                // ����ǰ֡�е�Blob�洢��blobs�С�
                blobs.push_back(currentFrameBlob);
            }
        }
        else {
            // ������ǵ�һ֡�������matchCurrentFrameBlobsToExistingBlobs()����������ǰ֡�� Blob ��֮ǰ�� Blob ����ƥ��
            matchCurrentFrameBlobsToExistingBlobs(blobs, currentFrameBlobs);
        }

        // �����Զ��庯��drawAndShowContours()���ú���������ͼ���ϻ���Blob������ʾ���� Blob ��������ͼ�񴰿��ϡ�
        drawAndShowContours(imgThresh.size(), blobs, "imgBlobs");

        // ��ȡ frame2 ����һ�ݸ�������Ϊ����Ĵ����Ѿ��ı���ǰһ����2֡�ĸ���
        imgFrame2Copy = imgFrame2.clone();

        // ��ͼ���ϻ��� Blob ����Ϣ
        drawBlobInfoOnImage(blobs, imgFrame2Copy);

        // ����Ƿ��� Blob ��Խ��Ԥ���ˮƽ��
        bool blnAtLeastOneBlobCrossedTheLine = checkIfBlobsCrossedTheLine(blobs, intHorizontalLinePosition, carCount);

        if (blnAtLeastOneBlobCrossedTheLine == true) { // ���������һ�� Blob ��Խ����
            cv::line(imgFrame2Copy, crossingLine[0], crossingLine[1], SCALAR_GREEN, 2); // ��ͼ���ϻ�����ɫ�Ŀ��ߣ������ٶȺܿ���ɫɾһ�¾͹��ˡ�
        }
        else {
            cv::line(imgFrame2Copy, crossingLine[0], crossingLine[1], SCALAR_RED, 2); // ��ͼ���ϻ��ƺ�ɫ�Ŀ��ߣ�����һֱû��Blob��ԽĬ�Ͼ�Ϊ��ɫ�Ŀ��ߡ�
        }

        // ��ͼ���ϻ�����������
        drawCarCountOnImage(carCount, imgFrame2Copy);

        // �����ʾͼ�񸱱�
        cv::imshow("imgFrame2Copy", imgFrame2Copy);

        //cv::waitKey(0);                 // ȡ��ע�ʹ����Խ�����֡����

        // ��������Ϊ��һ�ε�����׼��

        currentFrameBlobs.clear(); // ��յ�ǰ֡�е� Blob(����)��Ϊ�����Ѿ�������������

        imgFrame1 = imgFrame2.clone();           // ��֡ 1 �ƶ���֡ 2 ��λ��

        if ((capVideo.get(CV_CAP_PROP_POS_FRAMES) + 1) < capVideo.get(CV_CAP_PROP_FRAME_COUNT)) { // �����Ƶ�л���֡,Ҳ���������ǰ֡�������һ֡
            capVideo.read(imgFrame2); // ��ȡ��һ֡
        }
        else {
            std::cout << "end of video\n"; // �����ǰ֡�����һ֡����Ƶ�����������end of video����
            break; // ����ѭ��
        }

        blnFirstFrame = false; // ���������Ѿ������˵�һ֡���������ǿ������� blnFirstFrame = false
        frameCount++; // ֡����������
        chCheckForEscKey = cv::waitKey(1); // �ȴ� 1 ���룬����Ƿ��� ESC ������
    }

    if (chCheckForEscKey != 27) {               // ����û�û�а��� ESC ���������ǵ�������Ƶ�Ľ�β��
        cv::waitKey(0);                         // ���ִ��ڴ򿪣��Ա㡰end of video����Ϣ�ܹ���ʾ
    }
    // ��ע�⣬����û����� ESC �������ǲ���Ҫ���ִ��ڴ򿪣����ǿ����ó���������Ӷ��رմ���


    return(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/*
��������������ǽ���ǰ֡��Blob���Ѵ��ڵ�Blob����ƥ�䣬�����Ѵ��ڵ�Blob����Ϣ�������ǰ֡�е�Blobû��ƥ�䵽�Ѵ��ڵ�Blob��������Ϊ�µ�Blob�����ӵ��Ѵ��ڵ�Blob�б��С������ʵ�ַ�ʽ���£�

����ÿһ���Ѵ��ڵ�Blob�����Ƚ�����Ϊû��ƥ�䵽�������µ�Blob��Ȼ�������ʷ�켣Ԥ������һ֡���ֵ�λ�á�

���ڵ�ǰ֡�е�ÿһ��Blob�������Ѵ��ڵ�Blob�б����ҵ���֮����������Ѵ��ڵ�Blob���������С�ڵ�ǰBlob�ĶԽ��߳��ȵ�һ�룬�򽫵�ǰBlob���Ѵ��ڵ�Blob����ƥ�䣻���򣬽���ǰBlob��Ϊ�µ�Blob��

�����Ѵ��ڵ�Blob�б�����û��ƥ�䵽��Blob������δƥ��֡����1������Ѿ�����δƥ����5֡��������Ѵ��ڵ�Blob�б����Ƴ���

�������õ���һЩ������

existingBlobs���Ѵ��ڵ�Blob�б���
currentFrameBlobs����ǰ֡�е�Blob�б���
intIndexOfLeastDistance�����뵱ǰBlob������Ѵ��ڵ�Blob��������
dblLeastDistance�����뵱ǰBlob������Ѵ��ڵ�Blob�ľ��룻
distanceBetweenPoints��������������ĺ�����

std::vector<Blob>& existingBlobs���Ѿ������ٵ� Blob �ļ���
std::vector<Blob>& currentFrameBlobs����ǰ֡�е� Blob �ļ���


*/
void matchCurrentFrameBlobsToExistingBlobs(std::vector<Blob>& existingBlobs, std::vector<Blob>& currentFrameBlobs) {

    // ����ÿһ���Ѿ������ٵ� Blob
    for (auto& existingBlob : existingBlobs) {

        // ���õ�ǰû���ҵ�ƥ��ı�־Ϊ false
        existingBlob.blnCurrentMatchFoundOrNewBlob = false;

        // Ԥ����һ��λ��
        existingBlob.predictNextPosition();
    }

    // ����ÿһ����ǰ֡�е� Blob
    for (auto& currentFrameBlob : currentFrameBlobs) {

        // �ҵ����뵱ǰ Blob ������Ѿ������ٵ� Blob ������
        int intIndexOfLeastDistance = 0;
        double dblLeastDistance = 100000.0;

        for (unsigned int i = 0; i < existingBlobs.size(); i++) {

            // ��� Blob ���ڸ�����
            if (existingBlobs[i].blnStillBeingTracked == true) {

                // ���㵱ǰ Blob ���Ѿ������ٵ� Blob ֮��ľ���
                double dblDistance = distanceBetweenPoints(currentFrameBlob.centerPositions.back(), existingBlobs[i].predictedNextPosition);

                // ��������֮ǰ����ľ��뻹ҪС�������
                if (dblDistance < dblLeastDistance) {
                    dblLeastDistance = dblDistance;
                    intIndexOfLeastDistance = i;
                }
            }
        }

        // �����С����С�ڵ�ǰ Blob �ĶԽ��ߵ� 50%���������ӵ��Ѿ����ڵ� Blob ��
        if (dblLeastDistance < currentFrameBlob.dblCurrentDiagonalSize * 0.5) {
            addBlobToExistingBlobs(currentFrameBlob, existingBlobs, intIndexOfLeastDistance);
        }
        // �����С������ڵ�ǰ Blob �ĶԽ��ߵ� 50%����������Ϊһ���µ� Blob
        else {
            addNewBlob(currentFrameBlob, existingBlobs);
        }

    }

    // ����ÿһ���Ѿ������ٵ� Blob
    for (auto& existingBlob : existingBlobs) {

        // �����ǰû���ҵ�ƥ��� Blob��������δƥ��֡�ļ�����
        if (existingBlob.blnCurrentMatchFoundOrNewBlob == false) {
            existingBlob.intNumOfConsecutiveFramesWithoutAMatch++;
        }

        // ���δƥ���֡������������ 5 ֡����ֹͣ���ٸ� Blob
        if (existingBlob.intNumOfConsecutiveFramesWithoutAMatch >= 5) {
            existingBlob.blnStillBeingTracked = false;
        }

    }

}


///////////////////////////////////////////////////////////////////////////////////////////////////
/*
Blob& currentFrameBlob�����뵱ǰ֡��⵽��Blob����
std::vector<Blob>& existingBlobs������洢ԭ��Blob�����vector������
int& intIndex������ָ��ԭ��Blob������ֵ��

�ú����������ǽ���ǰ֡��⵽��Blob�������Ϣ���ӵ�ԭ�е�Blob�����У��Ӷ�����Blob����
���У�ԭ�е�Blob������ͨ��ǰһ֡�õ��ģ�����ǰ֡��⵽��Blob���������ǰһ֡��ĳ��Blob����ƥ�䣬
Ҳ�������³��ֵġ�ͨ��intIndex����ֵ���жϣ����Խ���ǰ֡��Blob������ԭ�е�Blob����ƥ������������ԭ�е�Blob����������Ϣ��

��������������ǽ���ǰ֡�м�⵽��blob���Ѵ��ڵ�blob����ƥ�䣬
�������Ѵ��ڵ�blob��������ԡ��������������������ǰ֡��blob��
�Ѵ��ڵ�blob������һ��int���͵ı���intIndex��
�ñ�����¼�˵�ǰblob���Ѵ��ڵ�blob�����е�������

������˵����������ǰ֡��blob����������Ӿ��θ�ֵ���Ѵ��ڵ�blob��������Ӧ��blob��
ͬʱ����ǰblob������λ�á��Խ��߳��ȡ������ȵ����Ը��µ��Ѵ��ڵ�blob�С�
��󣬺�������blob��blnStillBeingTracked���Ժ�blnCurrentMatchFoundOrNewBlob���Զ�����Ϊtrue����ʾ��blob�ڵ�ǰ֡�б��ɹ�ƥ�䡣




*/

void addBlobToExistingBlobs(Blob& currentFrameBlob, std::vector<Blob>& existingBlobs, int& intIndex) {

    // ����ǰ֡��������������Ӿ��θ�ֵ��ԭ�е�Blob
    existingBlobs[intIndex].currentContour = currentFrameBlob.currentContour;
    existingBlobs[intIndex].currentBoundingRect = currentFrameBlob.currentBoundingRect;

    // ����ǰ֡�����ĵ��������ԭ�е�Blob���ĵ������б�
    existingBlobs[intIndex].centerPositions.push_back(currentFrameBlob.centerPositions.back());

    // ����ǰ֡�ĶԽ��߳��Ⱥͳ����ȸ�ֵ��ԭ�е�Blob
    existingBlobs[intIndex].dblCurrentDiagonalSize = currentFrameBlob.dblCurrentDiagonalSize;
    existingBlobs[intIndex].dblCurrentAspectRatio = currentFrameBlob.dblCurrentAspectRatio;

    // ��ԭ�е�Blob�ĸ���״̬��ƥ��״̬����Ϊtrue
    existingBlobs[intIndex].blnStillBeingTracked = true;
    existingBlobs[intIndex].blnCurrentMatchFoundOrNewBlob = true;
}






///////////////////////////////////////////////////////////////////////////////////////////////////
/*
��������������ǽ�һ���µ� Blob �������ӵ��Ѿ����ڵ� Blob �����С�
�ú�������������������ǰ֡�е� Blob ������Ѿ����ڵ� Blob ������
�ں����ڲ�������ǵ�ǰ Blob �Ѿ�ƥ�䣬���������ӵ����� Blob �����С�
*/
void addNewBlob(Blob& currentFrameBlob, std::vector<Blob>& existingBlobs) {
    // ����ǰ֡��blob���Ϊƥ�䵽��
    currentFrameBlob.blnCurrentMatchFoundOrNewBlob = true;
    // ����ǰ֡��blob���ӵ��Ѵ��ڵ�blob�б���
    existingBlobs.push_back(currentFrameBlob);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/*
��������������Ǽ���������ά�����֮���ŷ����þ��룬���ؾ���ֵ��
����������У�ͨ�������1�͵�2��ˮƽ�ʹ�ֱ�����ϵľ���Ȼ��ʹ�ù��ɶ���������롣
���У�abs()�������ڻ�ȡ����ֵ��pow()�������ڼ���ָ�����ֵ��ݣ�
sqrt()�������ڻ�ȡƽ������
*/
double distanceBetweenPoints(cv::Point point1, cv::Point point2) {

    int intX = abs(point1.x - point2.x);  // ����x����֮��ľ���ֵ
    int intY = abs(point1.y - point2.y);  // ����y����֮��ľ���ֵ

    return(sqrt(pow(intX, 2) + pow(intY, 2)));  // ��������֮��ľ��벢����

}

///////////////////////////////////////////////////////////////////////////////////////////////////
/*
�����������������ͼ���ϻ��Ʋ�չʾ������������һ��cv::Size���͵Ĳ���imageSize����ʾͼ��Ĵ�С��
һ��std::vector<std::vectorcv::Point >���͵Ĳ���contours����ʾ����Ҫ���Ƶ�������
�Լ�һ��std::string���͵Ĳ���strImageName����ʾչʾ���ڵ����ơ�

�ú������ȸ���ͼ���С����һ����ɫ��ͼ��
Ȼ��ʹ��cv::drawContours�����ں�ɫ��ͼ���ϻ������д����������
���ʹ��cv::imshow����չʾ���ƺ�������ͼ��

drawAndShowContours������������չʾ�ڴ�����
imageSize�����ڴ�С
contours������
strImageName����������
cv::Mat image(imageSize, CV_8UC3, SCALAR_BLACK);������һ����СΪ imageSize �ĺ�ɫͼ������ CV_8UC3 ��ʾ��������Ϊ 8 λ�޷���������ͨ����Ϊ 3
cv::drawContours(image, contours, -1, SCALAR_WHITE, -1);���� image �л��� contours �е�������ʹ�ð�ɫ��������������Ϊ -1����ʾ���������ڲ�����
cv::imshow(strImageName, image);���ڴ�����չʾͼ�񣬴�������Ϊ strImageName
*/
void drawAndShowContours(cv::Size imageSize, std::vector<std::vector<cv::Point> > contours, std::string strImageName) {
    cv::Mat image(imageSize, CV_8UC3, SCALAR_BLACK);  // ����һ����ɫͼ��

    cv::drawContours(image, contours, -1, SCALAR_WHITE, -1);  // ��������

    cv::imshow(strImageName, image);  // �ڴ�����չʾͼ��
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/*
�����������������ͼ���ϻ�����������ʾ���������������������ͼ���С��
һ��Blob����������ʹ������ơ��������ȴ���һ����ɫ��ͼ��Ȼ�󣬺�������Blob��������
�������������ڱ����ٵ�Blob������������ӵ����������С�
��������cv::drawContours()�������������������ں�ɫͼ���ϣ����ð�ɫ��䡣
��󣬺�������ʾ����ָ�����Ƶ�ͼ�񴰿�
*/
void drawAndShowContours(cv::Size imageSize, std::vector<Blob> blobs, std::string strImageName) {

    // ����ָ����С�ĺ�ɫͼ��
    cv::Mat image(imageSize, CV_8UC3, SCALAR_BLACK);

    std::vector<std::vector<cv::Point> > contours;  // �洢����������

    // �������е�blob�������blob��Ȼ�����٣������������ӵ�contours��
    for (auto& blob : blobs) {
        if (blob.blnStillBeingTracked == true) {
            contours.push_back(blob.currentContour);
        }
    }

    // ��ͼ���ϻ������е�����
    cv::drawContours(image, contours, -1, SCALAR_WHITE, -1);

    // ����Ļ����ʾͼ��
    cv::imshow(strImageName, image);

}

///////////////////////////////////////////////////////////////////////////////////////////////////
/*
��������������Ǽ���˶��������Ƿ����˸�����ˮƽ�ߣ�����п�������ӳ�����������
���������Ƿ�������һ����������ˮƽ�ߡ�

���������������һ��Blob�������������ʾ��⵽���������壬һ��������ʾˮƽ�ߵ�λ�ã�
�Լ�һ��������ʾ�������������ں����ڲ������������е�ÿ��Blob���󣬼���Ƿ����ڸ���
�����壬����ǲ��Ҹ���������������֡�б���⵽����ô�������������λ���Ƿ��Խ��
ˮƽ�ߡ�����ǣ��򽫳�������������1����������ֵ����Ϊtrue����ʾ������һ�������Խ��ˮƽ�ߡ���������false��ʾû��������ˮƽ�ߡ�
*/
bool checkIfBlobsCrossedTheLine(std::vector<Blob>& blobs, int& intHorizontalLinePosition, int& carCount) {
    bool blnAtLeastOneBlobCrossedTheLine = false; // ����Ƿ���blob������

    for (auto blob : blobs) {  // ��������blob

        if (blob.blnStillBeingTracked == true && blob.centerPositions.size() >= 2) {  // ���blob���ڱ�׷�ٲ�������������λ����Ϣ

            int prevFrameIndex = (int)blob.centerPositions.size() - 2;  // ǰһ��λ�õ�����
            int currFrameIndex = (int)blob.centerPositions.size() - 1;  // ��ǰλ�õ�����

            if (blob.centerPositions[prevFrameIndex].y > intHorizontalLinePosition && blob.centerPositions[currFrameIndex].y <= intHorizontalLinePosition) {  // ���blob��һ֡���·�����ǰ֡���Ϸ���˵��blob������
                carCount++;  // ����������һ
                blnAtLeastOneBlobCrossedTheLine = true;  // �����blob������
            }
        }

    }

    return blnAtLeastOneBlobCrossedTheLine;  // �����Ƿ���blob������

}

///////////////////////////////////////////////////////////////////////////////////////////////////
/*
���������������ͼ���ϻ���ÿ�� Blob ����Ϣ�ġ�����������������һ���������� Blob ������������Լ���Ҫ������Ϣ��ͼ��

�ں����У���ͨ��ѭ������ÿ�� Blob������ Blob �Ƿ����ڸ����С�����ǣ�����ͼ���ϻ��ƾ��Σ��þ����� Blob �� currentBoundingRect �������壬�ú�ɫ���ƣ��߿����Ϊ 2��

Ȼ�󣬺���ʹ�õ�ǰ Blob ����� centerPositions �����е����һ��������λ�� Blob ������ͼ���ϵ�λ�ã��õ������ڵ�ǰ֡�е����ġ���ʹ����������� Blob �� ID ������ͼ���ϣ�
ʹ�� cv::putText ���������ִ�С����ɫ���Ը��� Blob �Ĵ�С�������š�
*/
void drawBlobInfoOnImage(std::vector<Blob>& blobs, cv::Mat& imgFrame2Copy) {

    for (unsigned int i = 0; i < blobs.size(); i++) {
        // �����blob���ڱ����٣�����ͼ���л��Ƹ�blob�ĵ�ǰ�߽����
        if (blobs[i].blnStillBeingTracked == true) {
            cv::rectangle(imgFrame2Copy, blobs[i].currentBoundingRect, SCALAR_RED, 2);

            // �������塢��С�����
            int intFontFace = CV_FONT_HERSHEY_SIMPLEX;
            double dblFontScale = blobs[i].dblCurrentDiagonalSize / 60.0;
            int intFontThickness = (int)std::round(dblFontScale * 1.0);

            // ��blob�����һ��λ���ϻ���blob�ı��
            cv::putText(imgFrame2Copy, std::to_string(i), blobs[i].centerPositions.back(), intFontFace, dblFontScale, SCALAR_GREEN, intFontThickness);
        }
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
//�������������ͼ���ϻ��Ƴ����������ı�
/*
����������������ڸ�����ͼ���ϻ��Ƴ���������������һ���������͵ĳ���������һ��OpenCV Mat���͵�ͼ��
����ͼ������Ͻǻ����ı�����ʾ��������������ʹ��CV_FONT_HERSHEY_SIMPLEX�������ͣ������ʵ��������С���߿���Ȼ���ڸ�����λ�û����ı���
*/
void drawCarCountOnImage(int& carCount, cv::Mat& imgFrame2Copy) {

    int intFontFace = CV_FONT_HERSHEY_SIMPLEX;  // ��������
    double dblFontScale = (imgFrame2Copy.rows * imgFrame2Copy.cols) / 300000.0;  // �����С
    int intFontThickness = (int)std::round(dblFontScale * 1.5);  // �����߿�

    cv::Size textSize = cv::getTextSize(std::to_string(carCount), intFontFace, dblFontScale, intFontThickness, 0);  // ��ȡ�ı���С

    cv::Point ptTextBottomLeftPosition;  // �ı����½�λ��

    ptTextBottomLeftPosition.x = imgFrame2Copy.cols - 1 - (int)((double)textSize.width * 1.25);  // �����ı����½�x����
    ptTextBottomLeftPosition.y = (int)((double)textSize.height * 1.25);  // �����ı����½�y����

    cv::putText(imgFrame2Copy, std::to_string(carCount), ptTextBottomLeftPosition, intFontFace, dblFontScale, SCALAR_GREEN, intFontThickness);  // �����ı�

}
