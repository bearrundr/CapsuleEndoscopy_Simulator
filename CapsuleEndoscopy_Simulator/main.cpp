#define _USE_MATH_DEFINES
#include <cmath>  
#include <direct.h>
#include <iostream>
#include "opencv_set_3.2.0.h"

using namespace std;
using namespace cv;

const double scale = 20.0;    // pathImg Ȯ�븦 ���� scale factor
Point2d pOffset(0., 0.);
const char* window01 = "Capsule01";
const char* pathWindow = "Path";

string numbering(int n, int digit);
Point2d rotateVector(Point2d vec, double theta);

class capsule
{
public:
	Point2d pCur;    // Current position
	Point2d pPre;    // Previous position
	Point2d vDirec;    // Direction vector

	const double height = 1.0;    // TODO: �� �۰�?
	const double FOV_angle = M_PI / 180 * 55;     // 55��
	const double FOV_length = height * 0.5;
	Point2d pFOV_left;
	Point2d pFOV_right;
	Point2d pFOV_Contact;

	int t, s, a;
	String path;    // Image file path
	Mat curImg;    // ���� ĸ���� ���� �ִ� �̹���: holw ã�� �˰��� ����, ���� �� ���� �� ������Ʈ
	Mat blackImg;    // FOV���� holw�� ����� �� ������Ʈ�Ǵ� �̹���

	// ����
	double step;
	// ����
	const double theta = M_PI / 90;    // 2��
	const double cosTheta = cos(theta);
	const double sinTheta = sin(theta);

public:
	capsule(const string path) {
		this->pCur = Point2d(-20.0, -20.0);
		this->pPre = Point2d(-20.0, -20.0 - (height * 0.5));
		this->vDirec = Point2d(pCur - pPre) / norm(pCur - pPre);    // ���⺤���� ũ��� �׻� 1�� ����

		this->pFOV_left = Point2d(pCur + rotateVector(vDirec * FOV_length / cos(FOV_angle), FOV_angle));
		this->pFOV_right = Point2d(pCur + rotateVector(vDirec * FOV_length / cos(FOV_angle), -FOV_angle));
		this->pFOV_Contact = Point2d((pFOV_left + pFOV_right) / 2);

		this->t = 0, this->s = 0, this->a = 0;
		this->path = path;
		this->curImg = imread(path + "\\c_t" + numbering(t, 2) + "_s" + to_string(s) + "_a" + numbering(a, 2) + ".0.jpg");
		// TODO: ���÷��̸� ���� ���� ����
		putText(curImg, "#c_t" + numbering(t, 2) + "_s" + to_string(s) + "_a" + numbering(a, 2) + ".0", Point(20, 40), CV_FONT_HERSHEY_SIMPLEX, 1, Scalar::all(255));
		this->blackImg = Mat::zeros(curImg.size(), curImg.type());

		this->step = height * 0.25;
	}

	/** @brief FOV ������ �������� path�� ������ ��ǥ ���
	* ��, FOV�� ���� path�� ������ ���� FOV�� � path���� ������ �ϳ��� ���� ���(D < 0)�� ó���Ǿ� ���� ����. (Hole�� ������ �߸� ã�� ���)
	* @param inputNum �����Ŀ� ������ x �Ǵ� y
	* @param flag 0�̸� x��, 1�̸� y�� input�� ����. 2�̸� � path���� ������ ����.
	*/
	void calculateLineEq (double inputNum, int flag = 0) {
		if (flag == 0) {
			pFOV_Contact.x = inputNum;
			pFOV_Contact.y = (pFOV_right.y - pFOV_left.y) / (pFOV_right.x - pFOV_left.x) * (pFOV_Contact.x - pFOV_left.x) + pFOV_left.y;
		}
		else if (flag == 1) {
			pFOV_Contact.y = inputNum;
			pFOV_Contact.x = (pFOV_right.x - pFOV_left.x) / (pFOV_right.y - pFOV_left.y) * (pFOV_Contact.y - pFOV_left.y) + pFOV_left.x;
		}
		else {

		}
	}

	void rotate(int dgree) {
		double vDirecL2N = norm(vDirec);
		vDirec /= vDirecL2N;

		if (dgree == 2) {    // CCW
			vDirec.x = (vDirec.x * cosTheta) + (vDirec.y * -sinTheta);
			vDirec.y = (vDirec.x * sinTheta) + (vDirec.y * cosTheta);
			vDirec /= norm(vDirec);
			pCur = pPre + (vDirec * (height * 0.5));
		}
		else if (dgree == -2) {    // CW
			vDirec.x = (vDirec.x * cosTheta) + (vDirec.y * sinTheta);
			vDirec.y = (vDirec.x * -sinTheta) + (vDirec.y * cosTheta);
			vDirec /= norm(vDirec);
			pCur = pPre + (vDirec * (height * 0.5));
		}

		pFOV_left = pCur + rotateVector(vDirec * FOV_length / cos(FOV_angle), FOV_angle);
		pFOV_right = pCur + rotateVector(vDirec * FOV_length / cos(FOV_angle), -FOV_angle);
	}


	void move() {    // TODO: Step�� �������� �ʰ� �ϴ� parameter �߰�
		double vDirecL2N = norm(vDirec);
		vDirec /= vDirecL2N;

		pPre.x += vDirec.x * step;
		pPre.y += vDirec.y * step;
		pCur.x += vDirec.x * step;
		pCur.y += vDirec.y * step;
		vDirec = pCur - pPre;
		vDirec /= norm(vDirec);

		pFOV_left = pCur + rotateVector(vDirec * FOV_length / cos(FOV_angle), FOV_angle);
		pFOV_right = pCur + rotateVector(vDirec * FOV_length / cos(FOV_angle), -FOV_angle);
	}

	// TODO: ���� �Ѿ�� �� �ڷ� �̵� (���� ��)
	void move(int flag) {
		double vDirecL2N = norm(vDirec);
		vDirec /= vDirecL2N;

		pPre.x += vDirec.x * step;
		pPre.y += vDirec.y * step;
		pCur.x += vDirec.x * step;
		pCur.y += vDirec.y * step;
		vDirec = pCur - pPre;
		vDirec /= norm(vDirec);

		pFOV_left = pCur + rotateVector(vDirec * FOV_length / cos(FOV_angle), FOV_angle);
		pFOV_right = pCur + rotateVector(vDirec * FOV_length / cos(FOV_angle), -FOV_angle);
	}

	void updateA() {
		// TODO: Interpolation: round() ���� ����

		double ccw = norm(pFOV_Contact - pFOV_left);
		double cw = norm(pFOV_Contact - pFOV_right);
		double l = ccw + cw;
		ccw = 60 * ccw / l;
		cw = 60 * cw / l;
		if (round(ccw) == round(cw)) {
			s = 0;
			a = 0;
		}
		else if (ccw > cw) {
			s = 0;
			if ((round(ccw) - 30) > 29) {
				a = 30;
			}
			else {
				a = round(ccw) - 30;
			}
		}
		else {
			s = 1;
			if ((round(cw) - 30) > 29) {
				a = 30;
			}
			else {
				a = round(cw) - 30;
			}
		}
	}

	void updateImg() {
		// t�� 0~27 �ȿ��� �ݺ��ǰ� ��.
		t++;
		if (t == 28) {
			t = 0;
		}

		// ������Ʈ
		if (a == -1) {
			curImg = blackImg.clone();
		}
		else {
			curImg = imread(path + "\\c_t" + numbering(t, 2) + "_s" + to_string(s) + "_a" + numbering(a, 2) + ".0.jpg");
			// TODO: ���÷��̸� ���� ���� ����
			putText(curImg, "#c_t" + numbering(t, 2) + "_s" + to_string(s) + "_a" + numbering(a, 2) + ".0", Point(20, 40), CV_FONT_HERSHEY_SIMPLEX, 1, Scalar::all(255));
		}
	}

	void drawCapsule(Mat pathImg, Mat displayImg) {
		displayImg = pathImg.clone();
		line(displayImg, (pPre + pOffset) * scale, (pCur + pOffset) * scale, Scalar(0, 0, 0));    // ĸ�� ��ü
		circle(displayImg, (pCur + pOffset) * scale, 0, Scalar(0, 0, 255), 3);    // ĸ�� �Ӹ�
		line(displayImg, (pFOV_left + pOffset) * scale, (pFOV_right + pOffset) * scale, Scalar(255, 0, 0));    // FOV
		circle(displayImg, (pFOV_Contact + pOffset) * scale, 0, Scalar(0, 0, 255), 3);    // FOV�� path�� ������ ����
		flip(displayImg, displayImg, 0);
		imshow(pathWindow, displayImg);
		moveWindow(pathWindow, 50, 50);
	}
	
	~capsule() {

	}
};

void simulator(capsule* capsule, double xStart = -25., double xEnd = 25., double yStart = -25., double yEnd = 25., double deltaX = 0.25, double deltaY = 0.);
//void writeData();

// ���ð� ī�޶�� �Կ� �� �¿쿡 ����� ���� �κ�(�� 5px, �� 10px)�� ����
void resize();

int main(int argc, char* argv[]) {
	// 1) Set image path
	const int bufMax = 150;
	char cwd[bufMax] = "";
	getcwd(cwd, bufMax);
	const string path = string(cwd) + "\\Phantom";

	// 2) Initialize capsule endoscopy
	capsule capsule01 = capsule(path);

	// 3) Run simulator
	simulator(&capsule01);
	
	return 0;
}

/** @brief Generate a serial number with zeros and convert it to string
* From two digits to five digits: 00~99, 000~999, 0000 ~ 9999 or 00000 ~ 99999
* @author Taejin Kim
* @date 7 Apr 2017
* @param n An original number
* @param digit Digit of serial number to be returned
* @return A serial number with zeros
*/
string numbering(int n, int digit) {
	switch (digit) {
	case 2:
		if (n >= 0 && n < 10) {
			return "0" + to_string(n);
		}
		else if (n >= 10 && n < 100) {
			return to_string(n);
		}
		else {
			return "Error generating serial number";
		}

		break;
	case 3:
		if (n >= 0 && n < 10) {
			return "00" + to_string(n);
		}
		else if (n >= 10 && n < 100) {
			return "0" + to_string(n);
		}
		else if (n >= 100 && n < 1000) {
			return to_string(n);
		}
		else {
			return "Error generating serial number";
		}

		break;
	case 4:
		if (n >= 0 && n < 10) {
			return "000" + to_string(n);
		}
		else if (n >= 10 && n < 100) {
			return "00" + to_string(n);
		}
		else if (n >= 100 && n < 1000) {
			return "0" + to_string(n);
		}
		else if (n >= 1000 && n < 10000) {
			return to_string(n);
		}
		else {
			return "Error generating serial number";
		}

		break;
	case 5:
		if (n >= 0 && n < 10) {
			return "0000" + to_string(n);
		}
		else if (n >= 10 && n < 100) {
			return "000" + to_string(n);
		}
		else if (n >= 100 && n < 1000) {
			return "00" + to_string(n);
		}
		else if (n >= 1000 && n < 10000) {
			return "0" + to_string(n);
		}
		else if (n >= 10000 && n < 100000) {
			return to_string(n);
		}
		else {
			return "Error generating serial number";
		}

		break;
	default:
		break;
	}

	return "Error generating serial number";
}

Point2d rotateVector(Point2d vec, double theta) {
	Point2d vRotated(0.0, 0.0);
	const double cosTheta = cos(theta);
	const double sinTheta = sin(theta);

	vRotated.x = (vec.x * cosTheta) + (vec.y * -sinTheta);
	vRotated.y = (vec.x * sinTheta) + (vec.y * cosTheta);
	
	return vRotated;
}

void simulator(capsule* capsule, double xStart, double xEnd, double yStart, double yEnd, double deltaX, double deltaY) {
	if (deltaY == 0) {
		deltaY = deltaX;
	}

	double pathWidth_half = capsule->height; 
	Point2d pEnd(0., 0.);

	Mat displayImg;

	// Show a correct path and a capsule endoscopy
	Point2d pDraw(0.0, 0.0);
	pOffset = Point2d(-xStart, -yStart);    // (0, 0)�� pathImg�� ����� �����ֱ� ���� offset
	Size2d pathImgSize(xEnd - xStart + 1, yEnd - yStart + 1);    // 51 x 51
	Mat pathImg = Mat(pathImgSize * scale, CV_64FC3, Scalar(255, 255, 255));
	line(pathImg, Point2d(0, pOffset.y) * scale, Point2d(pathImg.cols, pOffset.y) * scale, Scalar(0, 0, 0));    // x-axis
	line(pathImg, Point2d(pOffset.x, 0) * scale, Point2d(pOffset.x, pathImg.rows) * scale, Scalar(0, 0, 0));    // y-axis
	// ���� 1
	line(pathImg, (Point2d(-20., -25.) + pOffset) * scale, (Point2d(-20., -10.) + pOffset) * scale, Scalar(0, 255, 0));
	line(pathImg, (Point2d(-20. - pathWidth_half, -25.) + pOffset) * scale, (Point2d(-20. - pathWidth_half, -10.) + pOffset) * scale, Scalar(51, 51, 0));
	line(pathImg, (Point2d(-20. + pathWidth_half, -25.) + pOffset) * scale, (Point2d(-20. + pathWidth_half, -10.) + pOffset) * scale, Scalar(51, 51, 0));
	// ���� 2
	circle(pathImg, (Point2d(-10, -10) + pOffset) * scale, 10 * scale, Scalar(0, 255, 0));
	circle(pathImg, (Point2d(-10, -10) + pOffset) * scale, (10 - pathWidth_half) * scale, Scalar(51, 51, 0));
	circle(pathImg, (Point2d(-10, -10) + pOffset) * scale, (10 + pathWidth_half) * scale, Scalar(51, 51, 0));
	// ���� 3
	line(pathImg, (Point2d(-10, 0) + pOffset) * scale, (Point2d(0, 0) + pOffset) * scale, Scalar(0, 255, 0));
	line(pathImg, (Point2d(-10, 0 - pathWidth_half) + pOffset) * scale, (Point2d(0, 0 - pathWidth_half) + pOffset) * scale, Scalar(51, 51, 0));
	line(pathImg, (Point2d(-10, 0 + pathWidth_half) + pOffset) * scale, (Point2d(0, 0 + pathWidth_half) + pOffset) * scale, Scalar(51, 51, 0));

	//capsule->drawCapsule(displayImg);

	// TODO: ���� �Ÿ��� ȸ����
	int step = 1;    // ĸ�� ���ο� �����س���. (���� �� ���� �ʿ� ���� ��)
	int rotation = 0;    // Radian���� ����?

	while (true) {
		// ��ǥ���� ��ó�� �����ϸ� ����
		if (norm(capsule->pCur - pEnd) < 1) {
			break;
		}

		// ���� ĸ���� ���� �ִ� ȭ���� �������� path ���� ĸ���� ��ġ�� FOV�� �׸�.
		imshow(window01, capsule->curImg);
		moveWindow(window01, 1050, 50);
		capsule->drawCapsule(pathImg, displayImg);

		/*
		for (double y = yStart; y <= yEnd; y += deltaY) {
			for (double x = xStart; x <= xEnd; x += deltaX) {
			}
		}
		*/

		// TODO: Hole ã�� �˰��� ���� step�� rotation�� ������Ʈ �ǰ� �� ���� ���� ���� �� ȸ��


		// �������� ĸ���� ��ġ�� ����, FOV �м�
		// ���� 1: x = -20
		if (capsule->pCur.x > (-20 - pathWidth_half) && capsule->pCur.x < (-20 + pathWidth_half)
			&& capsule->pCur.y > -25 && capsule->pCur.y < -10) {
			// [Debugging] Key �Է����� ĸ�� ����
			while (true) {
				int key = waitKey(0);
				if (key == 'a') {    /// 'a' key: CCW
					capsule->rotate(2);
					capsule->drawCapsule(pathImg, displayImg);
				}
				else if (key == 'd') {    /// 'd' key: CW
					capsule->rotate(-2);
					capsule->drawCapsule(pathImg, displayImg);
				}
				else if (key == 32) {    /// 'space bar' key: ����
					capsule->move();
					capsule->drawCapsule(pathImg, displayImg);
					break;
				}
			}

			// Hole�� FOV�� ����� ��(pFOV_Contact�� FOV�� ����� ������ path�� ������ ������ ���)���� Ȯ��
			capsule->calculateLineEq(-20., 0);
			if (capsule->pFOV_Contact.x > capsule->pFOV_left.x && capsule->pFOV_Contact.x < capsule->pFOV_right.x) {    // FOV�� hole�� �ִ� ���
				capsule->updateA();
			}
			else {    // FOV���� hole�� ��� ���
				capsule->a = -1;    // � ȭ��
			}

		}
		// ���� 2: (x + 10)^2 + (y + 10)^2 = 10^2
		else if (capsule->pCur.x <= -10 && capsule->pCur.y >= -10
			&& pow(capsule->pCur.x - (-10), 2) + pow(capsule->pCur.y - (-10), 2) > pow(10 - pathWidth_half, 2)
			&& pow(capsule->pCur.x - (-10), 2) + pow(capsule->pCur.y - (-10), 2) < pow(10 + pathWidth_half, 2)) {
			// [Debugging] Key �Է����� ĸ�� ����
			while (true) {
				int key = waitKey(0);
				if (key == 'a') {    /// 'a' key: CCW
					capsule->rotate(2);
					capsule->drawCapsule(pathImg, displayImg);
				}
				else if (key == 'd') {    /// 'd' key: CW
					capsule->rotate(-2);
					capsule->drawCapsule(pathImg, displayImg);
				}
				else if (key == 32) {    /// 'space bar' key: ����
					capsule->move();
					capsule->drawCapsule(pathImg, displayImg);
					break;
				}
			}
			
			// TODO: �(CW)�� FOV
			//capsule->calculateLineEq(, 2);
		}
		// ���� 3: y = 0
		else if (capsule->pCur.x > -10 && capsule->pCur.x < 5
			&& capsule->pCur.y >(0 - pathWidth_half) && capsule->pCur.y < (0 + pathWidth_half)) {
			// [Debugging] Key �Է����� ĸ�� ����
			while (true) {
				int key = waitKey(0);
				if (key == 'a') {    /// 'a' key: CCW
					capsule->rotate(2);
					capsule->drawCapsule(pathImg, displayImg);
				}
				else if (key == 'd') {    /// 'd' key: CW
					capsule->rotate(-2);
					capsule->drawCapsule(pathImg, displayImg);
				}
				else if (key == 32) {    /// 'space bar' key: ����
					capsule->move();
					capsule->drawCapsule(pathImg, displayImg);
					break;
				}
			}
				
			capsule->calculateLineEq(0., 1);
			if (capsule->pFOV_Contact.y > capsule->pFOV_right.y && capsule->pFOV_Contact.y < capsule->pFOV_left.y) {    // FOV�� hole�� �ִ� ���
				capsule->updateA();
			}
			else {    // FOV���� hole�� ��� ���
				capsule->a = -1;    // � ȭ��
			}
		}
		else {
			// TODO: ĸ���� ������ �Ѿ�� �� (���ʿ� �� �Ѿ��? ȸ�� �� �̵� �ϴ� �κп��� �˻��ؼ� ���� �Ѿ�� �ǵ�����)
			capsule->move(-1);    // �ٽ� ���� ������ ���� ������ �ڷ� �̵�
		}

		// ĸ�� ȭ�� ������Ʈ
		capsule->updateImg();
	}
}

void resize() {
	int t = 0;    // Time: 0~27 (iteration)
	int s = 0;    // Sign: 0 (counterclockwise) or 1 (clockwise)
	int a = 0;    // Angle: -30 (counterclockwise) ~ 0 (center) ~ 30 (clockwise)

	const int bufMax = 150;
	char cwd[bufMax] = "";
	getcwd(cwd, bufMax);
	string imgName;
	string imgPath;

	cout << "Start resize" << endl;
	for (t = 0; t < 28; t++) {
		cout << t << " / 28" << endl;
		for (s = 0; s < 2; s++) {
			for (a = 0; a < 31; a++) {
				if (s == 1 && a == 0) { continue; }

				imgName = "c_t" + numbering(t, 2) + "_s" + to_string(s) + "_a" + numbering(a, 2) + ".0.jpg";
				imgPath = string(cwd) + "\\Phantom" + "\\" + imgName;

				Mat img = imread(imgPath);
				//imshow("Debugging", img);
				//waitKey(0);
				Mat crop = img(Rect(5, 0, img.cols - 15, img.rows));    // Image size: 640 x 480 -> 625 x 480

				imgPath = string(cwd) + "\\Phantom" + "\\Resize" + "\\" + imgName;
				imwrite(imgPath, crop);
			}
		}
	}
}