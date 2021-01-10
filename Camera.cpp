#include "Camera.h"
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>
#include<iostream>
#include<windows.h>
#include<qtimer.h>
#include<qtimezone.h>
#include <opencv2\objdetect.hpp>
using namespace std;
using namespace cv;
Camera::Camera(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	initPhoto();
	time = new QTimer(this);
	openCamera(0);//������ͷ
	setDelay(1000 / getFrameRate() * 1.0);
	connect(time, &QTimer::timeout, this, &Camera::run);
}

bool Camera::openCamera(int device)
{
	f_number = 0;
	//�ͷ���Դ����ֹ������Դ��VideoCaptureʵ������
	capture.release();
	TAKE_PHOTO = false;
	//����Ƶ�ļ�
	
	if (capture.open(device)) {
		ui.label->setText(QString::fromLocal8Bit("������ͷ�ɹ�"));
		return true;
	}
	else {
		ui.viewLabel->setText(QString::fromLocal8Bit("δ�ҵ�����ͷ"));
		return false;
	}

	//return capture.open(device);
}

double Camera::getFrameRate()
{
	double rate = capture.get(CV_CAP_PROP_FPS);
    return rate;
}

void Camera::stopIt()
{
	stop = true;
}

bool Camera::isStopped()
{
	return stop;
}

QString Camera::getNowTime()
{
	QTime qtime = QTime::currentTime();
	QDate qyear = QDate::currentDate();//��ȡ����ʱ����Ϊ��Ƭ�ļ�������֤��ƬΨһ��
	QString text = qyear.toString("yyyyMMdd-")+qtime.toString("HHmmss");
	return text;
}
bool Camera::readNextFrame(cv::Mat& frame)
{
	return capture.read(frame);
}
void Camera::setDelay(int d)
{
	delay = d;
	time->start(delay);
}
void Camera::initPhoto()//��ʼ��������key
{
	TAKE_PHOTO = false;
	GLASS_SHOW = false;
	OLD_SHOW = false;
	SOBEL_SHOW = false;
	GAUSSIANBLUR_SHOW = false;
	HSV_SHOW = false;
	SHARPEN_SHOW = false;
	RAIN_SHOW = false;
	FACERECOGNITION_SHOW = false;
	SKETCH_SHOW = false;
	CONTRASTPLUS_SHOW= false;
}

void Camera::run() {
	cv::Mat frame;//�洢��ǰ֡
	cv::Mat output;//�洢���֡
	stop = false;
	while (!isStopped()) {
		if (!readNextFrame(frame)) {
			break;
		}

		output = frame;//���֡��������֡
		f_number++;
		////�ڴ˶�ͼ����д������������������������������==================����������������������������
		if (GLASS_SHOW)//���ë����ѡ��	
			output = glassShow(output);
		if (OLD_SHOW)//���ŷ�
			output = oldShow(output);
		if (SOBEL_SHOW)//����
			output = sobelShow(output);
		if (GAUSSIANBLUR_SHOW)//ĥƤ����˹ģ��
			output = gaussianBlurShow(output);
		if (HSV_SHOW)//�滭Ч��
			output=hsvShow(output);
		if (SHARPEN_SHOW)//��
			output = sharpenShow(output);
		if (RAIN_SHOW)//����
			output = rainShow(output);
		if (FACERECOGNITION_SHOW)//�������
			output = faceRecognitionShow(output);
		if (SKETCH_SHOW)//��������
			output = sketchShow(output);
		if (CONTRASTPLUS_SHOW)//�Աȶ�+
			output = contrastPlusShow(output);
		////�ڴ˶�ͼ����д������������������������������==================����������������������������	
		
		//��qt��������ʾ����������ͼ��
		cv::cvtColor(output, output, CV_BGR2RGB);
		qImg = QImage((const unsigned char*)(output.data), output.cols, output.rows, output.step, QImage::Format_RGB888);
		ui.viewLabel->setPixmap(QPixmap::fromImage(qImg));
		ui.viewLabel->show();
		ui.label->setText(QString::fromLocal8Bit("��������ͷͼ��ɹ�"));

		//������������հ�ť���򱣴���һ֡ͼƬ
		if (TAKE_PHOTO == true) {
			ui.label->setText(QString::fromLocal8Bit("��������"));
			QString savePath = getNowTime();
			qImg.save(savePath + ".jpg", "JPG", 100);
			TAKE_PHOTO = false;
		}

		return;
	}
}
//---------------------------0ë����------------------------------------------------------//
cv::Mat Camera::glassShow(cv::Mat& image)
{
	int width = image.cols;
	int heigh = image.rows;
	RNG rng;
	Mat img1(image.size(), CV_8UC3);
	for (int y = 1; y < heigh - 1; y++)
	{
		uchar* P0 = image.ptr<uchar>(y);
		uchar* P1 = img1.ptr<uchar>(y);
		for (int x = 1; x < width - 1; x++)
		{
			int tmp = rng.uniform(0, 9);
			P1[3 * x] = image.at<uchar>(y - 1 + tmp / 3, 3 * (x - 1 + tmp % 3));
			P1[3 * x + 1] = image.at<uchar>(y - 1 + tmp / 3, 3 * (x - 1 + tmp % 3) + 1);
			P1[3 * x + 2] = image.at<uchar>(y - 1 + tmp / 3, 3 * (x - 1 + tmp % 3) + 2);
		}
	}
	return img1;
}

//---------------------------1���ŷ�------------------------------------------------------//
cv::Mat Camera::oldShow(cv::Mat& image)//���ŷ�ͼ��Ч��style
{
	int width = image.cols;
	int heigh = image.rows;
	RNG rng;
	Mat img1(image.size(), CV_8UC3);
	for (int y = 0; y < heigh; y++)
	{
		uchar* P0 = image.ptr<uchar>(y);
		uchar* P1 = img1.ptr<uchar>(y);
		for (int x = 0; x < width; x++)
		{
			float B = P0[3 * x];
			float G = P0[3 * x + 1];
			float R = P0[3 * x + 2];
			float newB = 0.272 * R + 0.534 * G + 0.131 * B;
			float newG = 0.349 * R + 0.686 * G + 0.168 * B;
			float newR = 0.393 * R + 0.769 * G + 0.189 * B;
			if (newB < 0)newB = 0;
			if (newB > 255)newB = 255;
			if (newG < 0)newG = 0;
			if (newG > 255)newG = 255;
			if (newR < 0)newR = 0;
			if (newR > 255)newR = 255;
			P1[3 * x] = (uchar)newB;
			P1[3 * x + 1] = (uchar)newG;
			P1[3 * x + 2] = (uchar)newR;
		}
	}
	return img1;
}
//---------------------------2����------------------------------------------------------//
cv::Mat Camera::sobelShow(cv::Mat& image)//����ͼ��Ч��
{
	Mat img0(image.size(), CV_8UC3);
	Mat img1(image.size(), CV_8UC3);
	for (int y = 1; y < image.rows - 1; y++)
	{
		uchar* p0 = image.ptr<uchar>(y);
		uchar* p1 = image.ptr<uchar>(y + 1);

		uchar* q0 = img0.ptr<uchar>(y);
		uchar* q1 = img1.ptr<uchar>(y);

		for (int x = 1; x < image.cols - 1; x++)
		{
			for (int i = 0; i < 3; i++)
			{
				int tmp0 = p1[3 * (x + 1) + i] - p0[3 * (x - 1) + i] + 128;//����
				if (tmp0 < 0)
					q0[3 * x + i] = 0;
				else if (tmp0 > 255)
					q0[3 * x + i] = 255;
				else
					q0[3 * x + i] = tmp0;

				int tmp1 = p0[3 * (x - 1) + i] - p1[3 * (x + 1) + i] + 128;//���
				if (tmp1 < 0)
					q1[3 * x + i] = 0;
				else if (tmp1 > 255)
					q1[3 * x + i] = 255;
				else
					q1[3 * x + i] = tmp1;
			}
		}
	}
	return img0;
}
//---------------------------3ĥƤ����˹ģ����------------------------------------------------------//
cv::Mat Camera::gaussianBlurShow(cv::Mat& image)
{
	cv::Mat gaussianImage = image.clone();
	Mat dst_step1;//�����պ��ͼ
	Mat dst;//��˹ģ��ͼ
	int value1 = 3, value2 = 1; //ĥƤ�̶���ϸ�ڳ̶ȵ�ȷ��
	int dx = value1 * 5;    //˫���˲�����֮һ  
	double fc = value1 * 12.5; //˫���˲�����֮һ  
	int p = 50; //͸����  
	Mat temp1, temp2, temp3, temp4;

	//˫���˲�  
	bilateralFilter(gaussianImage, temp1, dx, fc, fc);
	temp2 = (temp1 - gaussianImage + 128);

	//��˹ģ��  
	GaussianBlur(temp2, temp3, Size(2 * value2 - 1, 2 * value2 - 1), 0, 0);
	temp4 = image + 2 * temp3 - 255;
	dst = (image * (100 - p) + temp4 * p) / 100;
	dst.copyTo(dst_step1);
	return dst;
}
//---------------------------4 hsvˮ�ʻ滭------------------------------------------------------//
cv::Mat Camera::hsvShow(cv::Mat& image) {
	//ת����HSVɫ�ʿռ�
	cv::Mat hsv;
	cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);
	//������ͨ���ָ�������ͼ����
	vector<cv::Mat> channels;
	cv::split(hsv, channels);
	////channels[0]��ɫ����h������
	////channels[1]�Ǳ��Ͷȣ�S������
	////channels[2]�����ȣ�V������

	//��ɫ�������Ͷ������ȱ�ʾ��ɫ---���ı�����Ϊ�̶�ֵ
	channels[2] = 255;
	//���ºϲ�ͨ��
	cv::merge(channels, hsv);
	//ת����RGB
	cv::Mat RGB_image;
	cv::cvtColor(hsv, RGB_image, cv::COLOR_HSV2BGR);
	return RGB_image;
}
//---------------------------5��------------------------------------------------------//
cv::Mat Camera::sharpenShow(cv::Mat& image) {
	//�ж��Ƿ���Ҫ����ͼ�����ݡ������Ҫ���ͷ���
	//cv::Mat result;
	//result.create(image.size(), image.type());
	//int nchannels = image.channels();//���ͨ����
	////���������У����˵�һ�к����һ�У�
	//for (int j = 1; j < image.rows - 1; j++) {
	//	const uchar* previous = image.ptr<const uchar>(j - 1);//����ָ��
	//	const uchar* current = image.ptr<const uchar>(j);
	//	const uchar* next = image.ptr<const uchar>(j + 1);

	//	uchar* output = result.ptr<uchar>(j);
	//	for (int i = nchannels; i < (image.cols - 1) * nchannels; i++) {
	//		//Ӧ��������
	//		*output++ = cv::saturate_cast<uchar>(
	//			5 * current[i] - current[i - nchannels] -
	//			current[i + nchannels] - previous[i] - next[i]
	//			);
	//	}
	//}
	//result.row(0).setTo(cv::Scalar(0));
	//result.row(result.rows - 1).setTo(cv::Scalar(0));
	//result.col(0).setTo(cv::Scalar(0));
	//result.col(result.cols - 1).setTo(cv::Scalar(0));
	//return result;

	//�·���
	//�����ںˣ�������ڶ���ʼ��Ϊ0
	cv::Mat kernel(3, 3, CV_32F, cv::Scalar(0));
	//���ں˸�ֵ
	kernel.at<float>(1, 1) = 5.0;
	kernel.at<float>(0, 1) = -1.0;
	kernel.at<float>(2, 1) = -1.0;
	kernel.at<float>(1, 0) = -1.0;
	kernel.at<float>(1, 2) = -1.0;
	//��ͼ���˲�
	cv::Mat result;
	cv::filter2D(image, result, image.depth(), kernel);
	return result;
}
//---------------------------6����------------------------------------------------------//
cv::Mat Camera::rainShow(cv::Mat& image) {
	cv::Mat rain = cv::imread("rain.png");
	cv::Mat result;
	cv::Mat src = image.clone();
	cv::resize(rain, rain, cv::Size(src.cols, src.rows), CV_INTER_LINEAR);
	cv::add(src * 0.9, rain * 0.4, result);
	return result;
}
//---------------------------7�������------------------------------------------------------//
cv::Mat Camera::faceRecognitionShow(cv::Mat& image) {
	if (!face_cascade.load("haarcascade_frontalface_alt2.xml")) {//���������������
		ui.label_tip->setText(QString::fromLocal8Bit("�������ѵ������δ�ҵ���"));
		return image;
	}
	cv::Mat output_faces = image;
	int nSize = 30;//��������С�ߴ�
	vector<cv::Rect> faces;
	//ִ�ж�߶��������
	face_cascade.detectMultiScale(
		image,//����ͼ
		faces,//���ڱ����������ε�vector
		1.1,//��߶ȼ�������õĵ�������ϵ��
		3,//���ڿ������Ĳ���������3���ص�������Ϊ����ȷʵ����
		cv::CASCADE_DO_CANNY_PRUNING,//����canny�����ڼ�����ų���Ե�ٵ�����
		cv::Size(nSize, nSize)//��С�����ߴ�
	);
	if (faces.size() > 0) {
		//�����⵽����
		//��output��ʾ�����е���������
		for (int i = 0; i < faces.size(); i++) {
			cv::rectangle(output_faces, faces[i], cv::Scalar(0, 0, 255));
		}
		faces.clear();//��ձ����������ε�vector
	}
	return output_faces;
}
//---------------------------8����------------------------------------------------------//
cv::Mat Camera::sketchShow(cv::Mat& image)
{
	//ת�Ҷ�ͼ(һ����ԻҶ�ͼ����Sobel�˲�)
	cv::Mat gray;
	cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
	//ˮƽ����Sobel�˲�
	cv::Mat sobelX;
	cv::Sobel(gray,	// ����ͼ��
		sobelX,	// ���ͼ��
		CV_8U,	// ���ͼ���������
		1, 0,		// x��y����������
		3,		// �������ں˳ߴ磬����Ϊ����
		0.4, 128);	// ���ű�����ƫ����(Ĭ��ֵΪ1��0)

	//��ֱ�����˲�
	cv::Mat sobelY;
	cv::Sobel(gray,	// ����ͼ��
		sobelY,	// ���ͼ��
		CV_8U,	// ���ͼ���������
		0, 1,		// x��y����������
		3,		// �������ں˳ߴ磬����Ϊ����
		0.4, 128);	// ���ű�����ƫ����(Ĭ��ֵΪ1��0)
	// ����Sobel�˲�����ģ
	cv::Sobel(gray, sobelX, CV_16S, 1, 0);
	cv::Sobel(gray, sobelY, CV_16S, 0, 1);
	cv::Mat sobel;
	// ����L1ģ
	sobel = abs(sobelX) + abs(sobelY);
	// ����Sobel���ӣ������ø���������
	cv::Sobel(gray, sobelX, CV_32F, 1, 0);
	cv::Sobel(gray, sobelY, CV_32F, 0, 1);
	// �����ݶȵ�L2ģ�ͷ���
	cv::Mat norm, dir;
	cv::cartToPolar(sobelX, sobelY, norm, dir); 	// �ѿ�������ת������

	//Ϊ�˸���������ͼ����ʽ��ʾSobel�˲�����ģ������תΪ�Ҷ�ͼ���������ģֵ��ӦΪ��ɫ��
		// �ҵ�Sobel���������ֵ
	double sobmin, sobmax;
	cv::minMaxLoc(sobel, &sobmin, &sobmax);
	// ת����8λͼ��
	// sobelImage = -255. / sobmax * sobel + 255
	cv::Mat sobelImage;
	sobel.convertTo(sobelImage, CV_8U,
		-255. / sobmax, 255);
	cv::cvtColor(sobelImage, sobelImage, CV_GRAY2BGR);		
	return sobelImage;
}
//---------------------------9�Աȶ�+------------------------------------------------------//
cv::Mat Camera::contrastPlusShow(cv::Mat& image)
{
	cv::Mat result;
	cv::Mat hsv;
	cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);//��rgbתΪhsv�ռ�
	vector<cv::Mat>channels3;
	cv::split(hsv, channels3);//���Ϊ����ͨ����channels3[2]Ϊ����ͨ��
	cv::equalizeHist(channels3[2], channels3[2]);//����opencv�Դ���ֱ��ͼ���⻯����
	cv::merge(channels3, hsv);//�ϲ�ͨ��
	cv::cvtColor(hsv, result, cv::COLOR_HSV2BGR);//HSV�ռ�ת����BGR
	return result;
}

//=============��ѡ��ѡЧ��======================
void Camera::isGlassCheck()//��ë����ѡ����
{
	if(ui.checkBox_galssshow->isChecked()) {
		GLASS_SHOW = true;
	}
	else {
		GLASS_SHOW = false;
	}
}
void Camera::isOldCheck()//���ŷ�
{
	if (ui.checkBox_oldstyle->isChecked())
		OLD_SHOW = true;
	else
		OLD_SHOW = false;
}
void Camera::isSobelCheck()//����
{
	if (ui.checkBox_sobel->isChecked())
		SOBEL_SHOW = true;
	else
		SOBEL_SHOW = false;
}
void Camera::isGaussianBlurCheck()//ĥƤ
{
	if (ui.checkBox_gaussianBlur->isChecked())
		GAUSSIANBLUR_SHOW = true;
	else
		GAUSSIANBLUR_SHOW = false;
}
void Camera::isHsvCheck() {//�滭
	if (ui.checkBox_hsv->isChecked())
		HSV_SHOW = true;
	else
		HSV_SHOW = false;
}
void Camera::isSharpenCheck() {//��
	if (ui.checkBox_sharpen->isChecked())
		SHARPEN_SHOW = true;
	else {
		SHARPEN_SHOW = false;
	}
}
void Camera::isRainCheck() {//����
	if (ui.checkBox_rain->isChecked())
		RAIN_SHOW = true;
	else {
		RAIN_SHOW = false;
	}
}
void Camera::isFaceRecognitionCheck() {//�������
	if (ui.checkBox_faceRecognition->isChecked()) {
		FACERECOGNITION_SHOW = true;
	}
	else
		FACERECOGNITION_SHOW = false;
}
void Camera::isSketchCheck()//����
{
	if (ui.checkBox_sketch->isChecked())
		SKETCH_SHOW = true;
	else
		SKETCH_SHOW = false;
}
void Camera::isContrastPlusCheck()//�Աȶ���ǿ
{
	if (ui.checkBox_contrastPlus->isChecked())
		CONTRASTPLUS_SHOW = true;
	else
		CONTRASTPLUS_SHOW = false;
}
//=============��ѡ��ѡЧ��======================
//=============���հ�ť======================
void Camera::saveImg() {
	TAKE_PHOTO = true;
}
