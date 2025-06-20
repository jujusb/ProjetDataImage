#include <fstream>
#include"explorer.h"


void saveFile(Mat im, string destDirectory,string label, int scripter, int page, int row, int column, string size){

    // path :
    // image/txt file: iconeID_scripterNumber_pageNumber_row_column.png/.txt

    // image :
    string path = destDirectory+"/"+label+"_"+to_string(scripter)+"_"+to_string(page)+"_"+to_string(row)+"_"+to_string(column);
    imwrite( path+".png", im);




    // txt :
    /*
    # free comment (group name, year...) (other comment lines allowed)
    label <labelName>
    form <formNumber=scripterNumberpageNumber>
    scripter <scripterNumber>
    page <pageNumber>
    row <rowNumber
    column <columnNumber>
    size <small/medium/large (or nothing if size not extracted)>*/

    ofstream file;
    file.open (path+".txt");
    
    file << "# copyright Antoine Cellier Pierre-Yves Le Rolland Raumer Julio Santilario Berthiller Guillaume Sonnet\n";
    file << "label "+label+"\n";
    file << "form "+to_string(scripter)+to_string(page)+"\n";
    file << "scripter "+to_string(scripter)+"\n";
    file << "page "+to_string(page)+"\n";
    file << "row "+to_string(row)+"\n";
    file << "column "+to_string(column)+"\n";
    file << "size "+size+"\n";

    file.close();
}
