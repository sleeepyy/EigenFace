#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>

// Bad Implementation Because of my Laziness.
// Plz Don't Use Namespace in *.h as me.
using namespace std;
using namespace cv;

const int WIDTH = 73;
const int HEIGHT = 89;


class FaceEntry {
public:
	Mat origin_pic;
	Mat gray_pic;
	Mat transformed_pic;
	int x1, y1, x2, y2;
	Mat trans_mat;
	Mat_<double> equalized_mat;
	Mat_<double> vect;
	void load(string& path, string ext) {
		load_eye_pos(path);
		origin_pic = imread(path + ext);
		gray_pic = imread(path + ext, IMREAD_GRAYSCALE);
		transform();
	}
	void load(string& path) {
		load_eye_pos(path.substr(0, path.length()-4));
		origin_pic = imread(path);
		gray_pic = imread(path, IMREAD_GRAYSCALE);
		transform();
	}

	void load_eye_pos(string& path) {
		ifstream file(path + ".txt", ifstream::in);
		file >> x1 >> y1 >> x2 >> y2;
	}

	void transform() {
		Point center((x1 + x2) / 2, (y1 + y2) / 2);
		double angle = atan((double)(y2 - y1) / (double)(x2 - x1)) * 180.0 / CV_PI;
		trans_mat = getRotationMatrix2D(center, angle, 1.0);
		trans_mat.at<double>(0, 2) += 37 - center.x;
		trans_mat.at<double>(1, 2) += 30 - center.y;
		warpAffine(gray_pic, transformed_pic, trans_mat, gray_pic.size()*4/5);
		equalizeHist(transformed_pic, transformed_pic);
		transformed_pic.copyTo(equalized_mat);
		vect = equalized_mat.reshape(1, 1).t();
	}

};

class FaceLib {
public:
	int num_of_faces = 200;
	int num_of_persons = 40;
	int faces_per_person = 5;
	vector<FaceEntry*> faces;
	vector<Mat_<double>> _samples;
	Mat_<double> samples;

	void load(string& path) {
		for (int i = 1; i <= num_of_persons; i++)
		{
			for (int j = 1; j <= faces_per_person; ++j) {
				string entry_path = path + "/s" + to_string(i) + "/" + to_string(j);
				FaceEntry* face = new FaceEntry();
				face->load(entry_path, ".pgm");
				faces.push_back(face);
				_samples.push_back(face->vect);
			}
		}
		hconcat(_samples, samples);
	}
};