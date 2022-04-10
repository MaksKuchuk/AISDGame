#include <iostream>
#include <vector>
using namespace std;

class Labyrinth{
public:
    Labyrinth(){}
    static vector < vector <int> > create_labyrinth(int w, int h);
    static vector<int> read_labyrinth(vector < vector <int> > a_double);
};

vector < vector <int> > Labyrinth::create_labyrinth(int w, int h){
    vector < vector <int> > array(h , vector <int> (w) );
    for (int i = 0; i < h; ++i){
        for (int j = 0; j < w; ++j){
            if (i == 0 or i == array.size()-1 or j == 0 or j == array[0].size()-1){
                array[i][j] = 1;
            }
            else array[i][j] = 0;
        }
    }
    return array;
}

vector<int> Labyrinth::read_labyrinth(vector < vector <int> > a_double){
    vector<int> result;
    result.push_back(a_double[0].size());
    result.push_back(a_double.size());
    for (int i = 0; i < a_double.size(); i++){
        for (int j = 0; j < a_double[0].size() ; j++)
            result.push_back(a_double[i][j]);
    }
    return result;
}


int main()
{
    //------------------Проверка------------------
    Labyrinth lab;
    int w, h;
    cin >> w >> h;
    vector<int> a;
    a = lab.read_labyrinth(lab.create_labyrinth(w,h));
    for (int i = 0; i < a.size(); i++)
        cout << a[i];
}