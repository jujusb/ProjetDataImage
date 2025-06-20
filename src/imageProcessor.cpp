#include "imageProcessor.h"

string vig[] = { "accident",
                     "bomb",
                     "car",
                     "casualty",
                     "electricity",
                     "fire",
                     "fire_brigade",
                     "flood",
                     "gas",
                     "injury",
                     "paramedics",
                     "person",
                     "police",
                     "road_block"};
vector<string> vignettes(vig, vig + sizeof(vig)/ sizeof(vig[0]));
string taillevig[] = {"small", "large", "medium"};
vector<string> tailleVignettes(taillevig, taillevig+ sizeof(taillevig)/ sizeof(taillevig[0]));

int traitementDuneImage(string numPageIm)
{
    Mat img = imread("../"+numPageIm+".png");
    Mat gray; // image en niveau de gris
    Mat binary; // image en noir et blanc
    Mat display; // image reduite (coucou le zoom / elevator qui marche pas)

    std::vector<vector<Carre>> listeCarre; //Liste des 7 lignes avec les 5 images / lignes
    std::vector<Carre> listeRef; // Liste des 7 images de reference sur la gauche

    RNG rng(12345); //Init de la seed aleatoire

    namedWindow("ImageTest", WINDOW_NORMAL);

    //Nuances de gris
    cvtColor(img, gray, COLOR_BGR2GRAY);
    //Binarisation
    threshold(gray, binary, 200, 255, THRESH_BINARY);
    //Flou
    blur(binary, binary, Size(3, 3));

    /*DETECTION DES CONTOURS*/
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    Mat dst; //Image en negatif avec les contours

    //finding all contours in the image (a remettre dans les fonctions)
    Canny(binary, dst, 200, 255, 3);
    findContours(dst, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

    //Peuplement de la liste des carres (carre trie)
    detectionCarre(listeCarre, contours);
    //cout << listeCarre.at(0).at(0).pid << endl;

    for (int i = 0; i < listeCarre.size(); i++) {
        for(int j = 0 ; j < 5; j++) {
            listeCarre.at(i).at(j).display(img, 255, 255, 0);
        }
    }
    //Pleuplement de listeRef
    detectionCarreRef(listeCarre, listeRef);
    resize(img, display, Size(), 0.25, 0.25);
    cv::imshow("image", display);
    cv::waitKey(0);

    //pour toute les colonnes
    for (int i = 0; i < listeRef.size(); i++) {
        Carre refi = listeRef.at(i);
        Mat mat = img(Range(refi.pig.y, refi.psd.y), Range(refi.psd.x, refi.pig.x));
        string refvignette;
        string size;

        //retrouver la vignette reconnaissant la ligne
        for (String s : vignettes) {
            string chemin ;
            chemin = "../vignettes/"+s+".png";
            int val = trouverFormeDansCarre(chemin ,mat);
            if(!(val == -1)) {
                refvignette = s;
                break;
            }
        }

        //retrouver la taille du dessin reconnaissant la ligne
        for (String s : tailleVignettes) {
            string chemin ;
            chemin = "../Tailles_vignettes/"+s+".png";
            int taille = trouverFormeDansCarre(chemin,mat);
            if(!(taille==-1)) { //TODO que représente le retour de la fonction trouverFormeDansCarre ?
                size = s;
                break;
            }
        }
        //à partir du nom de l'image pour le moment
        int num = atoi(numPageIm.c_str());
        double div = num/100.0;
        //numéro écrivain
        int scripter = floor(div);
        //numéro page
        int nbPage = num % 100;
        for(int j= 0; j<5 ; j++) {
            Carre ij = listeCarre.at(i).at(j) ;
            Mat imgij = img(Range(ij.pig.y, ij.psd.y), Range(ij.psd.x, ij.pig.x));
            saveFile(imgij, "../rendu/" , refvignette, scripter, nbPage, i, j, size);
        }
    }
    return 0;
}
