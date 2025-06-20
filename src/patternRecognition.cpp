#include"patternRecognition.h"


// transformer un carre en image : Mat mat = img(Range(carre.pig.y,carre.psd.y),Range(carre.psd.x,carre.pig.x));
double trouverFormeDansCarre(char* cheminFormeAReconnaitre, Mat carreOuChercher){
    Mat pat = imread(cheminFormeAReconnaitre,IMREAD_COLOR);

    // image qui sera affichée à la fin de la fonction
    Mat img_display;
    carreOuChercher.copyTo( img_display );

    // 263 : taille des carrés dans lesquels avaient été trouvés les motifs (qui ont une largeur/hauteur de 115)
    // Le calcul suivant donne donc une bonne taille de motif par rapport au carré qui est donné

    double multiplicateur =  ((double)(carreOuChercher.cols) / 263.0);
    int x = (int)((double)(pat.cols)*multiplicateur);
    int y = (int)((double)(pat.cols)*multiplicateur);
    Size tailleReduitePat(x, y);
    Mat patreduite = Mat(tailleReduitePat,CV_8UC3); //cree une image � 3 canaux de profondeur 8 bits chacuns
    resize(pat,patreduite,tailleReduitePat);


    // matrice resultat
    Mat result;
    int result_cols =  carreOuChercher.cols ;
    int result_rows = carreOuChercher.rows ;
    result.create( result_rows, result_cols, CV_32FC1 );

    // match
    matchTemplate( carreOuChercher, patreduite, result, TM_SQDIFF);


    // recherche de la zone qui a matché
    double minVal; double maxVal; Point minLoc; Point maxLoc;
    Point matchLoc;
    minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc );

    matchLoc = minLoc;

    // encadrement de cette zone
    rectangle( img_display, matchLoc, Point( matchLoc.x + patreduite.cols , matchLoc.y + patreduite.rows ), Scalar::all(0), 2, 8, 0 );
    rectangle( result, matchLoc, Point( matchLoc.x + patreduite.cols , matchLoc.y + patreduite.rows ), Scalar::all(0), 2, 8, 0 );

    // affichage du résultat
    //Size tailleReduite(100,100);
    //img_display = Mat(tailleReduite,CV_8UC3); //cree une image � 3 canaux de profondeur 8 bits chacuns

    imshow("motif",patreduite);
    imshow("display",img_display);
    //imshow("result",result);

    return maxVal;
}
