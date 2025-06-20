#ifndef SQUARE_H
#define	SQUARE_H

#include<opencv2/opencv.hpp>
#include <vector>


/*============================================
FONCTIONS RELATIVES A LA CLASSE CARRE
==============================================*/

class Carre {
public:
	cv::Point psg; // point sup�rieur gauche
	cv::Point psd; // point sup�rieur droit
	cv::Point pig; // point inf�rieur gauche
	cv::Point pid; // point inf�rieur droit

	Carre() = default;
	Carre(int xMin, int yMin, int xMax, int yMax);
	Carre(const std::vector<cv::Point>& p);
	void display(cv::Mat& im, int b, int g, int r);
	bool isSquare();
	int taille();
};



/*============================================
FONCTIONS RELATIVES AU TRAITEMENT DE L'IMAGE
==============================================*/

//Detection des 35 carres a partir des points dans contours et peuplement de la liste des carres
void detectionCarre(std::vector<std::vector<Carre>>& listeCarre, std::vector<std::vector<cv::Point>> contours);

//Detection des 7 carres de reference a partir des 35 carres
void detectionCarreRef(std::vector<std::vector<Carre>>& listeCarre, std::vector<Carre>& listeRef);

//Detection du numéro de la page
void detectionNumPage(cv::Mat& img,cv::Mat& binary);

/*============================================
FONCTIONS TEMPORAIRES (affichage console)
==============================================*/
void displayCarre(Carre& c);

#endif SQUARE_H