#include "facelib.h"

Mat toImg(Mat vect, int w, int h) {
	assert(vect.type() == 6);
	assert(vect.cols == w*h);
	Mat result(Size(w, h), CV_64FC1);
	for (int i = 0; i < h; ++i) {
		vect.colRange(i*w, (i + 1)*w).convertTo(result.row(i), CV_64FC1);
	}
	normalize(result, result, 1.0, 0.0, NORM_MINMAX);
	return result;

}

int _main(int argc, char** argv) {
	char* model_name = "eigen.model";
	double energy = 0.95;
	if (argc >= 3) {
			model_name = argv[2];
			energy = atof(argv[1]);
	}
	FaceLib facelib;
	facelib.load(string("att_faces"));
	Mat samples, cov_mat, mean_mat;
	facelib.samples.copyTo(samples);
	cout << "Calculating Covariance Mat..." << endl;
	calcCovarMatrix(samples, cov_mat, mean_mat, CV_COVAR_ROWS | CV_COVAR_NORMAL);
	cout << mean_mat.size() << endl;
	cov_mat = cov_mat / (samples.rows - 1);
	Mat e_vector_mat, e_value_mat;
	cout << "Calculating Eigen Vector..." << endl;
	eigen(cov_mat, e_value_mat, e_vector_mat);
	cout << "eigen size " << e_value_mat.size() << endl;
	cout << e_value_mat << endl;
	for (int i = 0; i < samples.rows; ++i) {
		samples.row(i) -= mean_mat;
	}
	
	double value_sum = sum(e_value_mat)[0];
	cout << e_vector_mat.size() << endl;
	double energy_level = value_sum * energy;
	double energy_sum = 0;
	int k = 0;
	for (k = 0; k < e_value_mat.rows; k++)
	{
		energy_sum += e_value_mat.at<double>(k, 0);
		if (energy_sum >= energy_level) break;
	}
	cout << k << endl;
	e_vector_mat = (samples * e_vector_mat.t()).t();
	e_vector_mat = e_vector_mat.rowRange(0, k);
	e_value_mat = e_value_mat.rowRange(0, k);

	FileStorage model(model_name, FileStorage::WRITE);
	model << "e_vector_mat" << e_vector_mat;
	model << "e_value_mat" << e_value_mat;
	model.release();

	vector<Mat> Top10EigenFace;
	for (int i = 0; i < 10; ++i) {
		Top10EigenFace.push_back(toImg(e_vector_mat.row(i), WIDTH, HEIGHT));
	}
	Mat result;
	hconcat(Top10EigenFace, result);

	result.convertTo(result, CV_8U, 255);

	imshow("Top10EigenFace", result);
	imwrite("Top10EigenFace.png", result);


	waitKey(0);
	destroyAllWindows();
	return 0;
}