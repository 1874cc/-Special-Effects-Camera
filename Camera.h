#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Camera.h"
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>
#include<iostream>
#include <opencv2\objdetect.hpp>
using namespace std;
class Camera : public QMainWindow
{
    Q_OBJECT

public:
    Camera(QWidget *parent = Q_NULLPTR);
    bool openCamera(int device);//������ͷ
    double getFrameRate();//��ȡ��Ƶ��֡���ʣ�
    void stopIt();//�����Ա����������ֹ��������Լ�������ֹ״̬��//��������
    bool isStopped();//��������Ƿ��Ѿ���ֹ��
    QString getNowTime();//��ȡ����ʱ����Ϊ�����ͼ���ļ���
    bool readNextFrame(cv::Mat& frame);//�����Ա������ȡ��һ֡//������ʱms,0��ʾÿһ֡���ȴ���������ʾ����ʱ
    void setDelay(int d);//cout << "��ǰ����֡�ʣ�" << 1000.0/d << endl;
    //ͼƬ������
    void initPhoto();//��ʼ��ͼƬ����
    cv::Mat glassShow(cv::Mat& image);//ë����
    cv::Mat oldShow(cv::Mat& image);//���ŷ�
    cv::Mat sobelShow(cv::Mat& image);//����Ч��
    cv::Mat gaussianBlurShow(cv::Mat& image);//ĥƤ����˹ģ��
    cv::Mat hsvShow(cv::Mat& image);//hsv�ռ�滭Ч��
    cv::Mat sharpenShow(cv::Mat& image);//��Ч��
    cv::Mat rainShow(cv::Mat& image);//����Ч��
    cv::Mat faceRecognitionShow(cv::Mat& imgge);//����ʶ��
    cv::Mat sketchShow(cv::Mat& image);//����
    cv::Mat contrastPlusShow(cv::Mat& image);//�η�
public slots:
    void saveImg();//���հ�ť�ĵ���¼�������ͼƬ������
    void run();//���Ĵ�������ץȡ��������Ƶ�е�ͼ��
    //��⸴ѡ��״̬
    void isGlassCheck();
    void isOldCheck(); 
    void isSobelCheck();
    void isGaussianBlurCheck();
    void isHsvCheck();
    void isSharpenCheck();
    void isRainCheck();
    void isFaceRecognitionCheck();
    void isSketchCheck();
    void isContrastPlusCheck();
private:
    Ui::CameraClass ui;
    QImage qImg;//���ͼ��������ʾ�뱣�浽����
    cv::Mat outputImg;//�����е�ͼ�񣬻�ȡ����ͷͼ�񣬲����д���
    cv::VideoCapture capture; //opencv��Ƶ���������!!!
    cv::CascadeClassifier face_cascade;//�����������ļ���������
    long f_number;//�Ѿ������֡��
    double delay;//֮֡�����ʱ
    QTimer *qTime;//֮֡�����ʱ
    bool stop;//���������ֱ
    QTimer* time;
    bool TAKE_PHOTO;//�����б���
    bool GLASS_SHOW;//Ч���б���
    bool OLD_SHOW;
    bool SOBEL_SHOW;
    bool GAUSSIANBLUR_SHOW;
    bool HSV_SHOW;
    bool SHARPEN_SHOW;
    bool RAIN_SHOW;
    bool FACERECOGNITION_SHOW;
    bool SKETCH_SHOW;
    bool CONTRASTPLUS_SHOW;
};
