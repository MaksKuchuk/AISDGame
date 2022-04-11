#include <iostream>
#include <vector>
#include <ctime>
#include <time.h>
using namespace std;

class Labyrinth{
public:
    Labyrinth(){}
    static vector < vector <int> > create_labyrinth(int w, int h);
    static vector<int> read_labyrinth(vector < vector <int> > a_double);
};

vector < vector <int> > Labyrinth::create_labyrinth(int w, int h){
    vector < vector <int> > array(h , vector <int> (w) );
    vector< pair<int, int> > free_coordinates;
    for (int i = 0; i < h; ++i){
        for (int j = 0; j < w; ++j){
            if (i == 0 or i == array.size()-1 or j == 0 or j == array[0].size()-1)
                array[i][j] = 3;
            else {
                free_coordinates.emplace_back(i,j);
                array[i][j] = 0;
            }
        }
    }
    //---------------------Задаем стартовую позицую алгоритма-----------------------------------

    srand(time(0)); // устанавливаем значение системных часов в качестве стартового числа для srand()
    int r = 0 + rand() % (free_coordinates.size() - 0); // рандомное число
    int h1 = free_coordinates[r].first;
    int w1 = free_coordinates[r].second;
    array[h1][w1] = 5;

    vector< pair<int, int> > pm;

    if (array[h1 + 1][w1] == 0) {
        array[h1 + 1][w1] += 1;
        pm.emplace_back(h1 + 1, w1);
    }
    if (array[h1 - 1][w1] == 0) {
        array[h1 - 1][w1] += 1;
        pm.emplace_back(h1 - 1, w1);
    }
    if (array[h1][w1 + 1] == 0) {
        array[h1][w1 + 1] += 1;
        pm.emplace_back(h1, w1 + 1);
    }
    if (array[h1][w1 - 1] == 0) {
        array[h1][w1 - 1] += 1;
        pm.emplace_back(h1, w1 - 1);
    }
    //--------------------------------------------------------------------------------------
    //------------------------Основная часть алгоритма--------------------------------------
    while(!pm.empty())
    {
        srand(time(0));
        int rng = 0 + rand() % (pm.size());

        if (array[pm[rng].first][pm[rng].second] == 2){
            pm.erase(pm.begin() + rng);
            continue;
        }

        array[pm[rng].first][pm[rng].second] = 5;

        if (array[pm[rng].first + 1][pm[rng].second] != 3 and array[pm[rng].first + 1][pm[rng].second] != 5 and array[pm[rng].first + 1][pm[rng].second] != 2){
            array[pm[rng].first + 1][pm[rng].second] += 1;
            pm.emplace_back(pm[rng].first + 1, pm[rng].second);
        }

        if (array[pm[rng].first - 1][pm[rng].second] != 3 and array[pm[rng].first - 1][pm[rng].second] != 5 and array[pm[rng].first - 1][pm[rng].second] != 2){
            array[pm[rng].first - 1][pm[rng].second] += 1;
            pm.emplace_back(pm[rng].first - 1, pm[rng].second);
        }

        if (array[pm[rng].first][pm[rng].second + 1] != 3 and array[pm[rng].first][pm[rng].second + 1] != 5 and array[pm[rng].first][pm[rng].second + 1] != 2){
            array[pm[rng].first][pm[rng].second + 1] += 1;
            pm.emplace_back(pm[rng].first , pm[rng].second + 1);
        }

        if (array[pm[rng].first][pm[rng].second - 1] != 3 and array[pm[rng].first][pm[rng].second - 1] != 5 and array[pm[rng].first][pm[rng].second - 1] != 2){
            array[pm[rng].first][pm[rng].second - 1] += 1;
            pm.emplace_back(pm[rng].first , pm[rng].second - 1);
        }
        pm.erase(pm.begin() + rng);
    }
    for (int i = 0; i < array.size(); i++){
        for (int j = 0; j < array[0].size() ; j++){
            if (array[i][j] == 3 or array[i][j] == 2)
                array[i][j] = 1;
            else
                array[i][j] = 0;
        }
    }

    return array;
}

vector<int> Labyrinth::read_labyrinth(vector < vector <int> > a_double){
    vector<int> result;
    result.push_back(a_double[0].size());
    result.push_back(a_double.size());
    int x;
    for (int i = 0; i < a_double.size(); i++){
        for (int j = 0; j < a_double[0].size() ; j++){
            result.push_back(a_double[i][j]);
        }
    }
    return result;
}


int main()
{//------------------Результат------------------
    Labyrinth lab;
    int w, h;
    cin >> w >> h;
    vector<int> a;
    a = lab.read_labyrinth(lab.create_labyrinth(w,h));
    for (int i = 0; i < a.size(); ++i){
        cout << a[i];
    }
//------------ Вывод лабиринта для проверки ------------
//    vector < vector <int> > a = lab.create_labyrinth(w,h);
//    for (int i = 0; i < h; i++) // Цикл, который идёт по строкам
//    {
//        for (int j = 0; j < w ; j++) // Цикл, который идёт по элементам
//            cout << a[i][j] << ' '; // Вывод элементов i строки вектора
//        cout << endl;
//    }
//----------------------------------------------------
}