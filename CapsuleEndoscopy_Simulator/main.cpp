#include <direct.h>
#include <iostream>
#include "opencv_set_3.2.0.h"

using namespace std;
using namespace cv;

const double pi = 3.141592;
const char* window01 = "Capsule01";
const char* pathWindow = "Path";

string numbering(int n, int digit);

class capsule
{
public:
	const double height = 1.0;    // �� �۰�?
	const double FOV_angle = pi / 2 / 90 * 55;     // 55��
	const double FOV_length = height * 0.5;

	Point2d pCur;    // Current position
	Point2d pPre;    // Previous position
	Point2d vDirec;    // Direction vector

	int t, s, a;
	//Mat pathImg;
	String path;    // Image file path
	Mat curImg;    // ���� ĸ���� ���� �ִ� �̹���: Ȧ ã�� �˰��� ����, ���� �� ���� �� ������Ʈ

	//const Size pathImgSize = Size(40, 40);

	// ����
	double step;
	// ����
	const double theta = pi / 2 / 45;    // 2��
	const double cosTheta = cos(theta);
	const double sinTheta = sin(theta);

public:
	capsule(const string path) {
		this->pCur = Point2d(5.0, 1.0);
		this->pPre = Point2d(5.0, 0.0);
		this->vDirec = Point2d(pCur - pPre);
		this->t = 0, this->s = 0, this->a = 0;
		//this->pathImg = Mat(pathImgSize * scale, CV_8UC3, Scalar(255, 255, 255));
		this->path = path;
		this->curImg = imread(path + "\\c_t" + numbering(t, 2) + "_s" + to_string(s) + "_a" + numbering(a, 2) + ".0.jpg");
		// TODO: ���÷��̸� ���� ���� ����
		putText(curImg, "#c_t" + numbering(t, 2) + "_s" + to_string(s) + "_a" + numbering(a, 2) + ".0", Point(20, 40), CV_FONT_HERSHEY_SIMPLEX, 1, Scalar::all(255));

		this->step = height * 0.25;
	}

	void rotate(int dgree) {
		if (dgree == 2) {    // CCW
			vDirec.x = (vDirec.x * cosTheta) + (vDirec.y * -sinTheta);
			vDirec.y = (vDirec.x * sinTheta) + (vDirec.y * cosTheta);
			pCur = pPre + vDirec;
		}
		else if (dgree == -2) {    // CW
			vDirec.x = (vDirec.x * cosTheta) + (vDirec.y * sinTheta);
			vDirec.y = (vDirec.x * -sinTheta) + (vDirec.y * cosTheta);
			pCur = pPre + vDirec;
		}
	}

	void move(int step) {
		double vDirecL2N = norm(vDirec);
		vDirec /= vDirecL2N;

		pPre.x += vDirec.x * step;
		pPre.y += vDirec.y * step;
		pCur.x += vDirec.x * step;
		pCur.y += vDirec.y * step;
		vDirec = pCur - pPre;
	}



	void updateImg(const int direcChange, const char* direction, const int* distance) {
		// a�� s�� ����
		if (s == 1) {    // �߾��� �������� ĸ���� CW(-)�� Ʋ�����ִ� ���� (Ȧ�� �������� ġ������ ����)
			a = -a;
		}

		// a���� s�� �и�
		if (a >= 0) {
			s = 0;
		}
		else {
			s = 1;
		}
		a = abs(a);

		// a�� 30�� �Ѿ�� �ִ� ������ 30���� ����
		if (a > 30) { a = 30; }

		// t�� 0~27 �ȿ��� �ݺ��ǰ� ��.
		t++;
		if (t == 28) { t = 0; }

		// ������Ʈ
		updatedImg = imread(path + "\\c_t" + numbering(t, 2) + "_s" + to_string(s) + "_a" + numbering(a, 2) + ".0.jpg");
		// TODO: ���÷��̸� ���� ���� ����
		putText(updatedImg, "#c_t" + numbering(t, 2) + "_s" + to_string(s) + "_a" + numbering(a, 2) + ".0", Point(20, 40), CV_FONT_HERSHEY_SIMPLEX, 1, Scalar::all(255));

		// Reset
		a = 0;
	}

	/*
	void drawPath(int pathLen) {
		circle(pathImg, pPath[0] * scale, 0, Scalar(255, 0, 0), 2);
		for (int i = 1; i < pathLen; i++) {
			line(pathImg, pPath[i - 1] * scale, pPath[i] * scale, Scalar(0, 255, 0));
			circle(pathImg, pPath[i] * scale, 0, Scalar(255, 0, 0), 2);
		}
	}
	*/

	/*
	void drawCapsule(Mat displayImg) {
		displayImg = pathImg.clone();
		line(displayImg, pPre * scale, pCur * scale, Scalar(0, 0, 0));
		circle(displayImg, pCur * scale, 0, Scalar(0, 0, 255), 3);
		line(displayImg, pPath[idx] * scale, pPath[idx + 1] * scale, Scalar(255, 0, 0));        // Draw vRef
		flip(displayImg, displayImg, 0);
		imshow(pathWindow, displayImg);
		moveWindow(pathWindow, 50, 50);
	}
	*/

	~capsule() {

	}
};

// x = 0
Point2d func01(double x, double y, double xStart = 0.0, double xEnd = 0.0, double yStart = 0.0, double yEnd = 10.0);
// pow((x - 10), 2) + pow((y - 10), 2) = pow(10, 2)
Point2d func02(double x, double y, double xStart = 0.0, double xEnd = 10.0, double yStart = 10.0, double yEnd = 20.0);
// y = 20
Point2d func03(double x, double y, double xStart = 10.0, double xEnd = 20.0, double yStart = 20.0, double yEnd = 20.0);
void generatePath(double xStart, double xEnd, double yStart, double yEnd, double deltaX, double deltaY = 0.0);
void simulator(capsule* capsule, const int direcChange, const char* direction, const int* distance);
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

	// 3) Generate path
	generatePath(-25.0, 25.0, -25.0, 25.0, 0.25);
	
	// 4) Run simulator
	//simulator(&capsule01, direcChange, direction, distance);
	
	return 0;
}


// x = 0
Point2d func01(double x, double y, double xStart, double xEnd, double yStart, double yEnd) {
	// ���ǿ� �Ǵ� ġ���� �ϳ��� ������ ��� ���
	if (x < xStart || x > xEnd || y < yStart || y > yEnd) {
		return Point(999, 999);
	}

	// x-axis �Ǵ� y-axis�� ���� ������ ������ ���
	if (xStart == xEnd) {
		x = xStart;
	}
	else if (yStart == yEnd) {
		y = yStart;
	}

	return Point2d(x, y);
}

// pow((x - 10), 2) + pow((y - 10), 2) = 10
Point2d func02(double x, double y, double xStart, double xEnd, double yStart, double yEnd) {
	// ���ǿ� �Ǵ� ġ���� �ϳ��� ������ ��� ���
	if (x < xStart || x > xEnd || y < yStart || y > yEnd) {
		return Point(999, 999);
	}

	// x-axis �Ǵ� y-axis�� ���� ������ ������ ���
	if (xStart == xEnd) {
		x = xStart;
	}
	else if (yStart == yEnd) {
		y = yStart;
	}
	
	y = sqrt(100 - pow((x - 10), 2)) + 10;
	return Point2d(x, y);
}

// y = 20
Point2d func03(double x, double y, double xStart, double xEnd, double yStart, double yEnd) {
	// ���ǿ� �Ǵ� ġ���� �ϳ��� ������ ��� ���
	if (x < xStart || x > xEnd || y < yStart || y > yEnd) {
		return Point(999, 999);
	}

	// x-axis �Ǵ� y-axis�� ���� ������ ������ ���
	if (xStart == xEnd) {
		x = xStart;
	}
	else if (yStart == yEnd) {
		y = yStart;
	}

	return Point2d(x, y);
}

void generatePath(double xStart, double xEnd, double yStart, double yEnd, double deltaX, double deltaY) {
	if (deltaY == 0) {
		deltaY = deltaX;
	}

	const double scale = 5.0;    // pathImg Ȯ�븦 ���� scale factor
	Point2d pDraw(0.0, 0.0);
	Point2d pOffset(-xStart, -yStart);    // (0, 0)�� pathImg�� ����� �����ֱ� ���� offset

	Size2d pathImgSize(xEnd - xStart + 1, yEnd - yStart + 1);    // 51 x 51
	Mat pathImg = Mat(pathImgSize * scale, CV_64FC3, Scalar(255, 255, 255));

	for (double y = yStart; y <= yEnd; y += deltaY) {
		cout << "row (y): " << y << endl;
		//Vec3d* p = pathImg.ptr<Vec3d>(y);
		for (double x = xStart; x <= xEnd; x += deltaX) {
			//cout << x << ", " << y << endl;
			pDraw = func01(x, y);
			if (pDraw != Point2d(999, 999)) {
				circle(pathImg, (pDraw + pOffset) * scale, 1, Scalar(0, 255, 0), -1);
			}
			pDraw = func02(x, y);
			if (pDraw != Point2d(999, 999)) {
				circle(pathImg, (pDraw + pOffset) * scale, 1, Scalar(0, 255, 0), -1);
			}
			pDraw = func03(x, y);
			if (pDraw != Point2d(999, 999)) {
				circle(pathImg, (pDraw + pOffset) * scale, 1, Scalar(0, 255, 0), -1);
			}
		}
	}
	//line(pathImg, Point(0, pOffset.y) * scale, Point(pathImg.cols, pOffset.y) * scale, Scalar(0, 0, 0));    // x-axis
	//line(pathImg, Point(pOffset.x, 0) * scale, Point(pOffset.x, pathImg.rows) * scale, Scalar(0, 0, 0));    // y-axis
	flip(pathImg, pathImg, 0);
	imshow(pathWindow, pathImg);
	waitKey(0);
}

void simulator(capsule* capsule, const int direcChange, const char* direction, const int* distance) {
	Mat displayImg;
	int key;

	Point2d endCheck;

	// Show a correct path and a capsule endoscopy
	//capsule->drawPath(pathLen);
	//capsule->drawCapsule(displayImg);
	
	// ���� �Ÿ��� ȸ����
	int step = 1;
	int rotation = 0;    // Radian?

	while (true) {
		// ��ǥ���� ��ó�� �����ϸ� ����
		endCheck = capsule->pCur - pPath[pathLen - 1];
		if (norm(endCheck) < 1) { break; }

		// ������ ������Ʈ�� ȭ���� ���� ȭ������ ������.
		capsule->curImg = capsule->updatedImg.clone();
		imshow(window01, capsule->curImg);
		moveWindow(window01, 1050, 50);
		capsule->drawCapsule(displayImg);

		// TODO: Hole ã�� �˰��� ���� step�� rotation�� ������Ʈ �ǵ��� ����
		/// [Debugging] Key �Է����� ĸ�� ����
		while (true) {
			int key = waitKey(0);
			if (key == 'a') {    /// 'a' key: CCW
				rotation++;
				capsule->rotate(2);
				capsule->drawCapsule(displayImg);
			}
			else if (key == 'd') {    /// 'd' key: CW
				rotation--;
				capsule->rotate(-2);
				capsule->drawCapsule(displayImg);
			}
			else if (key == 32) {    /// 'space bar' key: ����
				capsule->move(step);
				capsule->drawCapsule(displayImg);
				break;
			}
		}

		// ���� ����� path point�� idx�� reference vector�� ���
		capsule->findMinDistance();

		// ������ �Ÿ��� ���� ���
		capsule->calculateDiff();

		// ĸ�� ȭ�� ������Ʈ
		capsule->updateImg(direcChange, direction, distance);
	}
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