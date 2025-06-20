#include<opencv2/opencv.hpp>
#include<iostream>
#include"shape.h"
#include "LSWMS.h"

using namespace std;
using namespace cv;


static double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1 * dy2) / sqrt((dx1*dx1 + dy1 * dy1)*(dx2*dx2 + dy2 * dy2) + 1e-10);
}

//Fonction temporaire
void displayCarre(Carre& c) {
    cout << c.psg << " "
         << c.psd << " "
         << c.pig << " "
         << c.pid << " " << endl;
}

int drawLine();

int main()
{
    Mat img = imread("../01106.png");
    Mat gray;
    Mat binary;
    Mat display;


    std::vector<cv::Point> approx;
    std::vector<Carre> listeCarre;

    RNG rng(12345);

    namedWindow("ImageTest", WINDOW_NORMAL);

    //Nuances de gris
    cvtColor(img, gray, COLOR_BGR2GRAY);
    //Binarisation
    threshold(gray, binary, 200, 255, THRESH_BINARY);
    //Flou
    blur(binary, binary, Size(3, 3));

    //DETECTION DES CONTOURS
     Mat contour_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    //finding all contours in the image
    Canny(binary, contour_output, 200, 255, 3);
    findContours(contour_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
    cv::imshow("contour",contour_output);
    Mat dst = contour_output.clone();
    Carre carre;



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
                cos.push_back(angle(approx[j%vtc], approx[j - 2], approx[j - 1]));

            //Important pour l'identification des carr�s / rectangles (angles droits)
            std::sort(cos.begin(), cos.end());
            double mincos = cos.front();
            double maxcos = cos.back();


            if (vtc == 4 && mincos >= -0.1 && maxcos <= 0.3) {
                carre = Carre(approx);
                if (find_if(listeCarre.begin(), listeCarre.end(), [&carre](const Carre& a) {
                    return abs(a.pid.x - carre.pid.x)<20 && abs(a.pid.y - carre.pid.y)<20 ||
                           abs(a.pig.x - carre.pig.x)<20 && abs(a.pig.y - carre.pig.y)<20 ||
                           abs(a.psd.x - carre.psd.x)<20 && abs(a.psd.y - carre.psd.y)<20 ||
                           abs(a.psg.x - carre.psg.x)<20 && abs(a.psg.y - carre.psg.y)<20;
                }) == listeCarre.end() && carre.isSquare()) {
                    listeCarre.push_back(carre);
                    carre.display(img);
                }

            }
        }
    }

    cout << listeCarre.size() << " < population carre" << endl;
    listeCarre.at(0).isSquare();
    displayCarre(listeCarre.at(1));
    displayCarre(listeCarre.at(2));
    displayCarre(listeCarre.at(3));
    displayCarre(listeCarre.at(4));

    resize(img, display, Size(), 0.25, 0.25);
    cv::imshow("image", display);
    imageNumberDetected(img,binary);
    cv::waitKey(0);
    return 0;
}

void imageNumberDetected(Mat img, Mat binary) {
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    Mat contour_output;
    //finding all contours in the image
    Canny(binary, contour_output, 200, 255, 3);
    Mat dilated_output;
    cv::dilate(contour_output, dilated_output, Mat(),Point(-1, -1), 3, 1, 1);
    Mat closed_output;
    cv::erode(dilated_output, closed_output, Mat(),Point(-1, -1), 3, 1, 1);
    cv::imshow("fermeture",closed_output);
    findContours(closed_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
    cv::imshow("contour",contour_output);
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
                cos.push_back(angle(approx[j % vtc], approx[j - 2], approx[j - 1]));

            //Important pour l'identification des carr�s / rectangles (angles droits)
            std::sort(cos.begin(), cos.end());
            double mincos = cos.front();
            double maxcos = cos.back();

            //TODO : taille du rectangle en hauteur < 70 pour ne détecter que ces carrés
            //TODO : classe rectangle presque identique à carré + fonction taille à faire pour la hauteur
            if (vtc == 4 && mincos >= -0.1 && maxcos <= 0.3 && carre.taille()) {
                carre = Carre(approx);
                if (find_if(listeCarre.begin(), listeCarre.end(), [&carre](const Carre &a) {
                        return abs(a.pid.x - carre.pid.x) < 20 && abs(a.pid.y - carre.pid.y) < 20 ||
                               abs(a.pig.x - carre.pig.x) < 20 && abs(a.pig.y - carre.pig.y) < 20 ||
                               abs(a.psd.x - carre.psd.x) < 20 && abs(a.psd.y - carre.psd.y) < 20 ||
                               abs(a.psg.x - carre.psg.x) < 20 && abs(a.psg.y - carre.psg.y) < 20;
                    }) == listeCarre.end() && carre.isSquare()) {
                        listeCarre.push_back(carre);
                        carre.display(img);
                    }
                }
            //}
        }
    }
    cv::imshow("numberImgDetected", img);
}