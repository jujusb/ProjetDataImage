#include <vector>
#include <iostream>
#include <opencv2/core/types.hpp>
#include "shape.h"

using namespace std;
using namespace cv;


/*============================================
FONCTIONS RELATIVES A LA CLASSE CARRE 
==============================================*/
Carre::Carre(const std::vector<cv::Point>& p) {
	int xMin = p.at(0).x;
	int xMax = p.at(0).x;
	int yMin = p.at(0).y;
	int yMax = p.at(0).y;

	for (int i = 0; i < p.size(); i++)
	{
		if (xMin > p.at(i).x)
		{
			xMin = p.at(i).x;
		}
		if (yMin > p.at(i).y) {
			yMin = p.at(i).y;
		}
		if (xMax < p.at(i).x)
		{
			xMax = p.at(i).x;
		}
		if (yMax < p.at(i).y) {
			yMax = p.at(i).y;
		}
	}
	psg = cv::Point(xMin, yMin);
	psd = cv::Point(xMax, yMin);
	pig = cv::Point(xMin, yMax);
	pid = cv::Point(xMax, yMax);
}

Carre::Carre(int xMin, int yMin, int xMax, int yMax) {
	psg = cv::Point(xMin, yMin);
	psd = cv::Point(xMax, yMin);
	pig = cv::Point(xMin, yMax);
	pid = cv::Point(xMax, yMax);
}

bool Carre::isSquare() {
	return abs((psd.x - psg.x) - (pig.y - psg.y)) < 40;
}

void Carre::display(cv::Mat& im, int b, int g, int r) {
	line(im, psg, psd, Scalar(b, g, r), 2, LINE_8);
	line(im, psd, pid, Scalar(b, g, r), 2, LINE_8);
	line(im, pid, pig, Scalar(b, g, r), 2, LINE_8);
	line(im, pig, psg, Scalar(b, g, r), 2, LINE_8);
}

int Carre::taille() {
	return pig.y - psg.y;
}

//fonction interne pour le tri par ligne
bool _compCarreLigne(const Carre& a, const Carre& b) {
	return a.psg.y < b.psg.y;
}

//fonction interne pour le classement au sein d'une ligne
bool _compCarreColonne(const Carre& a, const Carre& b) {
	return a.psg.x < b.psg.x;
}


/*============================================
FONCTIONS RELATIVES AU TRAITEMENT DE L'IMAGE
==============================================*/
static double _angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1 * dy2) / sqrt((dx1*dx1 + dy1 * dy1)*(dx2*dx2 + dy2 * dy2) + 1e-10);
}

void detectionCarre(std::vector<std::vector<Carre>>& listeCarre, vector<vector<Point>> contours) {
	Carre carre;
	std::vector<cv::Point> approx;
	std::vector<Carre> listeCarreTmp; //Tout les carr� en vrac

	for (int i = 0; i < contours.size(); i++)
	{
		cv::approxPolyDP(Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);
		//Ne tiens pas compte des petits objets 
		if (std::fabs(cv::contourArea(contours[i])) < 100 || !cv::isContourConvex(approx)) {
			continue;
		}

		if (approx.size() == 4)
		{
			size_t vtc = approx.size();
			vector<double> cos;
			for (int j = 2; j < vtc + 1; j++)
				cos.push_back(_angle(approx[j%vtc], approx[j - 2], approx[j - 1]));

			//Important pour l'identification des carr�s / rectangles (angles droits)
			std::sort(cos.begin(), cos.end());
			double mincos = cos.front();
			double maxcos = cos.back();


			if (vtc == 4 && mincos >= -0.1 && maxcos <= 0.3) {
				carre = Carre(approx);
				if (find_if(listeCarreTmp.begin(), listeCarreTmp.end(), [&carre](const Carre& a) {
					return abs(a.pid.x - carre.pid.x) < 20 && abs(a.pid.y - carre.pid.y) < 20 ||
						abs(a.pig.x - carre.pig.x) < 20 && abs(a.pig.y - carre.pig.y) < 20 ||
						abs(a.psd.x - carre.psd.x) < 20 && abs(a.psd.y - carre.psd.y) < 20 ||
						abs(a.psg.x - carre.psg.x) < 20 && abs(a.psg.y - carre.psg.y) < 20;
				}) == listeCarreTmp.end() && carre.isSquare()) {
					listeCarreTmp.push_back(carre);
				}

			}
		}
	}

	//Tri les carr�s dans l'ordre (en partant d'en haut a gauche)
	sort(listeCarreTmp.begin(), listeCarreTmp.end(), _compCarreLigne);
	int index = 99999999;
	int j = -1;
	for (int i = 0; i < listeCarreTmp.size(); i++) {
		if (abs(index - listeCarreTmp.at(i).psg.y) > 50) {
			j++;
			index = listeCarreTmp.at(i).psg.y;
			listeCarre.push_back(std::vector<Carre>());
		}
		listeCarre.at(j).push_back(listeCarreTmp.at(i));
	}

	for (int i = 0; i < listeCarre.size(); i++) {
		sort(listeCarre.at(i).begin(), listeCarre.at(i).end(), _compCarreColonne);
	}

}


void detectionCarreRef(std::vector<std::vector<Carre>>& listeCarre, std::vector<Carre>& listeRef) {
	int xMin;
	int yMin;
	int taille = listeCarre.at(0).at(0).taille();
	cout << "taille d'un carre : " << taille << endl;
	for (int i = 0; i < listeCarre.size(); i++) {
		xMin = 99999;
		yMin = 0;
		for (int j = 0; j < listeCarre.at(i).size(); j++) {
			if (xMin > listeCarre.at(i).at(j).psg.x) {
				xMin = listeCarre.at(i).at(j).psg.x;
				yMin = listeCarre.at(i).at(j).psg.y;
			}
		}
		//Peupler la liste des references
		Carre carreRef = Carre(
			xMin - 1.5 * taille,
			yMin,
			xMin - 0.5 * taille,
			yMin + taille
		);
		listeRef.push_back(carreRef);
	}
}


void detectionNumPage(Mat& img, Mat& binary) {
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    Mat contour_output;
    //finding all contours in the image
    Canny(binary, contour_output, 200, 255, 3);
    Mat dilated_output;
    cv::dilate(contour_output, dilated_output, Mat(),Point(-1, -1), 3, 1, 1);
    Mat closed_output;
    cv::erode(dilated_output, closed_output, Mat(),Point(-1, -1), 3, 1, 1);
    //cv::imshow("fermeture",closed_output);
    findContours(closed_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
    //cv::imshow("contour",contour_output);
    Mat dst = contour_output.clone();
    Carre carre;
    std::vector<cv::Point> approx;
    std::vector<Carre> listeCarre;
    for (int i = 0; i < contours.size(); i++)
    {
        cv::approxPolyDP(Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);
        //Ne tiens pas compte des petits objets
        if (std::fabs(cv::contourArea(contours[i])) < 100 || !cv::isContourConvex(approx)) {
            continue;
        }

        if (approx.size() == 4) {
            size_t vtc = approx.size();
            vector<double> cos;
            for (int j = 2; j < vtc + 1; j++)
                cos.push_back(_angle(approx[j % vtc], approx[j - 2], approx[j - 1]));

            //Important pour l'identification des carr�s / rectangles (angles droits)
            std::sort(cos.begin(), cos.end());
            double mincos = cos.front();
            double maxcos = cos.back();

            //TODO : taille du rectangle en hauteur < 70 pour ne détecter que ces carrés
            //TODO : classe rectangle presque identique à carré + fonction taille à faire pour la hauteur
            if (vtc == 4 && mincos >= -0.1 && maxcos <= 0.3 && carre.taille()<50) {
                carre = Carre(approx);
                if (find_if(listeCarre.begin(), listeCarre.end(), [&carre](const Carre &a) {
                    return abs(a.pid.x - carre.pid.x) < 20 && abs(a.pid.y - carre.pid.y) < 20 ||
                           abs(a.pig.x - carre.pig.x) < 20 && abs(a.pig.y - carre.pig.y) < 20 ||
                           abs(a.psd.x - carre.psd.x) < 20 && abs(a.psd.y - carre.psd.y) < 20 ||
                           abs(a.psg.x - carre.psg.x) < 20 && abs(a.psg.y - carre.psg.y) < 20;
                }) == listeCarre.end()) {
                    listeCarre.push_back(carre);
                    carre.display(img,0,255,255);
                }
            }
        }
    }
    cv::imshow("numberImgDetected", img);
}

/*============================================
FONCTIONS TEMPORAIRES (affichage console)
==============================================*/
void displayCarre(Carre& c) {
	cout << c.psg << " psg // "
		<< c.psd << " psd // "
		<< c.pig << " pig // "
		<< c.pid << " pid // " << endl;
}
