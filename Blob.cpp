// Blob.cpp

#include "Blob.h"


Blob::Blob(std::vector<cv::Point> _contour) {  // ���캯������������˶�������������vector����

    currentContour = _contour;  // ���洫���������

    currentBoundingRect = cv::boundingRect(currentContour);  // �������㴴���߿���Σ�����

    cv::Point currentCenter;  // ����һ�����浱ǰ���ĵ������Point����

    currentCenter.x = (currentBoundingRect.x + currentBoundingRect.x + currentBoundingRect.width) / 2;  // ���㵱ǰ���ĵ�x����
    currentCenter.y = (currentBoundingRect.y + currentBoundingRect.y + currentBoundingRect.height) / 2;  // ���㵱ǰ���ĵ�y����

    centerPositions.push_back(currentCenter);  // ����ǰ���ĵ㱣�浽vector������

    dblCurrentDiagonalSize = sqrt(pow(currentBoundingRect.width, 2) + pow(currentBoundingRect.height, 2));  // ���㵱ǰ�߿���εĶԽ��ߴ�С

    dblCurrentAspectRatio = (float)currentBoundingRect.width / (float)currentBoundingRect.height;  // ���㵱ǰ�߿���εĿ�߱�

    blnStillBeingTracked = true;  // ���õ�ǰBlob�����ڱ�׷��

    blnCurrentMatchFoundOrNewBlob = true;  // ���õ�ǰBlob�������µĻ����Ѿ�ƥ�䵽

    intNumOfConsecutiveFramesWithoutAMatch = 0;  // ��ʼ����ǰBlob����δƥ�䵽��֡��
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Blob::predictNextPosition(void) {

    int numPositions = (int)centerPositions.size();
    // ����λ�������ĳ���

    if (numPositions == 1) {
        // �������λ�������ĳ���Ϊ1

        predictedNextPosition.x = centerPositions.back().x;
        predictedNextPosition.y = centerPositions.back().y;
        // Ԥ�����һ��λ�õ��ڵ�ǰ��λ��

    }
    else if (numPositions == 2) {
        // �������λ�������ĳ���Ϊ2

        int deltaX = centerPositions[1].x - centerPositions[0].x;
        int deltaY = centerPositions[1].y - centerPositions[0].y;
        // ����x��y�����ϵı仯��

        predictedNextPosition.x = centerPositions.back().x + deltaX;
        predictedNextPosition.y = centerPositions.back().y + deltaY;
        // Ԥ����һ��λ�õ��ڵ�ǰλ�ü��ϱ仯��

    }
    else if (numPositions == 3) {
        // �������λ�������ĳ���Ϊ3

        int sumOfXChanges = ((centerPositions[2].x - centerPositions[1].x) * 2) +
            ((centerPositions[1].x - centerPositions[0].x) * 1);
        // ����x�����ϵı仯��֮��

        int deltaX = (int)std::round((float)sumOfXChanges / 3.0);
        // ����x�����ϵ�ƽ���仯��

        int sumOfYChanges = ((centerPositions[2].y - centerPositions[1].y) * 2) +
            ((centerPositions[1].y - centerPositions[0].y) * 1);
        // ����y�����ϵı仯��֮��

        int deltaY = (int)std::round((float)sumOfYChanges / 3.0);
        // ����y�����ϵ�ƽ���仯��

        predictedNextPosition.x = centerPositions.back().x + deltaX;
        predictedNextPosition.y = centerPositions.back().y + deltaY;
        // Ԥ����һ��λ�õ��ڵ�ǰλ�ü���ƽ���仯��
    }
    // ���Ŀ����������ĵ�������ʷ��¼����Ϊ4
    else if (numPositions == 4) {
        // ����Ŀ��������ˮƽ�����ϵı仯����Ȩ�طֱ�Ϊ3��2��1
        int sumOfXChanges = ((centerPositions[3].x - centerPositions[2].x) * 3) +
            ((centerPositions[2].x - centerPositions[1].x) * 2) +
            ((centerPositions[1].x - centerPositions[0].x) * 1);

        // ����ˮƽ�����ϵı仯��ƽ��ֵ
        int deltaX = (int)std::round((float)sumOfXChanges / 6.0);

        // ����Ŀ����������ֱ�����ϵı仯����Ȩ�طֱ�Ϊ3��2��1
        int sumOfYChanges = ((centerPositions[3].y - centerPositions[2].y) * 3) +
            ((centerPositions[2].y - centerPositions[1].y) * 2) +
            ((centerPositions[1].y - centerPositions[0].y) * 1);

        // ������ֱ�����ϵı仯��ƽ��ֵ
        int deltaY = (int)std::round((float)sumOfYChanges / 6.0);

        // Ԥ��Ŀ��������һ�ε����ĵ�����
        predictedNextPosition.x = centerPositions.back().x + deltaX;
        predictedNextPosition.y = centerPositions.back().y + deltaY;


    }
    else if (numPositions >= 5) {

        // ����Ŀ��������ˮƽ�����ϵı仯����Ȩ�طֱ�Ϊ3��2��1
        int sumOfXChanges = ((centerPositions[3].x - centerPositions[2].x) * 3) +
            ((centerPositions[2].x - centerPositions[1].x) * 2) +
            ((centerPositions[1].x - centerPositions[0].x) * 1);

        // ����ˮƽ�����ϵı仯��ƽ��ֵ
        int deltaX = (int)std::round((float)sumOfXChanges / 6.0);

        // ����Ŀ����������ֱ�����ϵı仯����Ȩ�طֱ�Ϊ3��2��1
        int sumOfYChanges = ((centerPositions[3].y - centerPositions[2].y) * 3) +
            ((centerPositions[2].y - centerPositions[1].y) * 2) +
            ((centerPositions[1].y - centerPositions[0].y) * 1);

        // ������ֱ�����ϵı仯��ƽ��ֵ
        int deltaY = (int)std::round((float)sumOfYChanges / 6.0);

        // Ԥ��Ŀ��������һ�ε����ĵ�����
        predictedNextPosition.x = centerPositions.back().x + deltaX;
        predictedNextPosition.y = centerPositions.back().y + deltaY;

    }
    else {
        // should never get here
    }

}

/*
    # ���� X ��λ�ñ仯��֮�ͣ��������һ��λ�ñ仯������3�������ڶ���λ�ñ仯������2������������λ�ñ仯������1
    int sumOfXChanges = ((centerPositions[3].x - centerPositions[2].x) * 3) +
        ((centerPositions[2].x - centerPositions[1].x) * 2) +
        ((centerPositions[1].x - centerPositions[0].x) * 1);

    # ���� X ��仯����ƽ��ֵ���������������������
    int deltaX = (int)std::round((float)sumOfXChanges / 6.0);

    # ���� Y ��λ�ñ仯��֮�ͣ��������һ��λ�ñ仯������3�������ڶ���λ�ñ仯������2������������λ�ñ仯������1
    int sumOfYChanges = ((centerPositions[3].y - centerPositions[2].y) * 3) +
        ((centerPositions[2].y - centerPositions[1].y) * 2) +
        ((centerPositions[1].y - centerPositions[0].y) * 1);

    # ���� Y ��仯����ƽ��ֵ���������������������
    int deltaY = (int)std::round((float)sumOfYChanges / 6.0);

    # Ԥ�������һ��λ�õ� X��Y ����ֵ
    predictedNextPosition.x = centerPositions.back().x + deltaX;
    predictedNextPosition.y = centerPositions.back().y + deltaY;

}
# �����⵽������λ���������ڵ��� 5
else if (numPositions >= 5) {

    # ���� X ��λ�ñ仯��֮�ͣ��������һ��λ�ñ仯������4�������ڶ���λ�ñ仯������3������������λ�ñ仯������2���������ĸ�λ�ñ仯������1
    int sumOfXChanges = ((centerPositions[numPositions - 1].x - centerPositions[numPositions - 2].x) * 4) +
        ((centerPositions[numPositions - 2].x - centerPositions[numPositions - 3].x) * 3) +
        ((centerPositions[numPositions - 3].x - centerPositions[numPositions - 4].x) * 2) +
        ((centerPositions[numPositions - 4].x - centerPositions[numPositions - 5].x) * 1);

    # ���� X ��仯����ƽ��ֵ���������������������
    int deltaX = (int)std::round((float)sumOfXChanges / 10.0);

    # ���� Y ��λ�ñ仯��֮�ͣ��������һ��λ�ñ仯������4�������ڶ���λ�ñ仯������3������������λ��
*/