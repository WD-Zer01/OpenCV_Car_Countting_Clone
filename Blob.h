// Blob.h

#ifndef MY_BLOB // ��� MY_BLOB û�б�����������������
#define MY_BLOB

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
class Blob {
public:
    // member variables ///////////////////////////////////////////////////////////////////////////
    std::vector<cv::Point> currentContour;

    cv::Rect currentBoundingRect;   // һ�� Blob ����Ӿ���

    std::vector<cv::Point> centerPositions;   // һ�� Blob ���ĵ��λ������

    double dblCurrentDiagonalSize;   // ��ǰ Blob �ĶԽ��߳���

    double dblCurrentAspectRatio;   // ��ǰ Blob �ĳ����

    bool blnCurrentMatchFoundOrNewBlob;   // ��ʾ��ǰ Blob �Ƿ���֮ǰ�� Blob ƥ��

    bool blnStillBeingTracked;   // ��ʾ��ǰ Blob �Ƿ���Ȼ������

    int intNumOfConsecutiveFramesWithoutAMatch;   // һϵ������֡�У�δƥ��� Blob ������

    cv::Point predictedNextPosition;   // Ԥ�⵱ǰ Blob ����һ��λ��

    // function prototypes ////////////////////////////////////////////////////////////////////////
    Blob(std::vector<cv::Point> _contour);   // Blob ��Ĺ��캯��

    void predictNextPosition(void);   // Ԥ�⵱ǰ Blob ����һ��λ�ã�������洢�� predictedNextPosition ��
};

#endif // MY_BLOB

#pragma once // ָʾ������ֻ��Ҫ��һ���ļ��а������ͷ�ļ�
