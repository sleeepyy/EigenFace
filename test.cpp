#include "facelib.h"

int main(int argc, char** argv) {
	FaceLib facelib;
	facelib.load(string("att_faces"));

	char* model_name = "eigen.model";
	char* file_name = "att_faces/s23/8.png";
	if (argc >= 3) {
		model_name = argv[2];
		file_name = argv[1];
	}
	FileStorage model(model_name, FileStorage::READ);
	Mat e_vector_mat, e_value_mat;
	model["e_vector_mat"] >> e_vector_mat;
	model["e_value_mat"] >> e_value_mat;
	Mat distance;
	Mat samples;
	FaceEntry face;
	facelib.samples.copyTo(samples);
	distance = e_vector_mat * samples;
	for (int _i = 1; _i <= 41; ++_i) {
		for (int _j = 1; _j <= 10; _j++)
		{
			face.load(string("att_faces/s") + to_string(_i) + "/" + to_string(_j), ".pgm");
			Mat face_vect = e_vector_mat * face.vect;
			double min_d = norm(face_vect, distance.col(0), NORM_L2);
			double temp_d = 0;
			int min_i = 0;

			for (int i = 1; i < distance.cols; ++i) {
				temp_d = norm(face_vect, distance.col(i), NORM_L2);
				if (temp_d <= min_d) {
					min_d = temp_d;
					min_i = i;
				}
			}
			cout << (min_i/5)+1 << "/" << (min_i % 5)+1 << " ";
		}
		cout << endl;
	}

	face.load(string(file_name));
	
	Mat face_vect = e_vector_mat * face.vect;
	double min_d = norm(face_vect, distance.col(0), NORM_L2);
	double temp_d = 0;
	int min_i = 0;

	for (int i = 1; i < distance.cols; ++i) {
		temp_d = norm(face_vect, distance.col(i), NORM_L2);
		if (temp_d <= min_d) {
			min_d = temp_d;
			min_i = i;
		}
	}
	cout << (min_i / 5) + 1 << "/" << (min_i % 5) + 1 << " " << endl;
	Mat origin_mat = face.origin_pic;
	Mat similar_mat = facelib.faces.at(min_i)->origin_pic;
	string text = "s" + to_string(min_i / 5 + 1) + " No." + to_string(min_i % 5 + 1);
	cout << text << endl;
	putText(origin_mat, text, Point(10, 20), FONT_HERSHEY_COMPLEX, 0.5, (0, 255, 255), 2, 8);
	imshow("FaceResult", origin_mat);
	imshow("Similar Pic", similar_mat);
	waitKey(0);
	destroyAllWindows();
	return 0;



}