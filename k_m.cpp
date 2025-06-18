#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
using namespace std;

struct Punto {
    string nombre;
    double x, y;
    int cluster = -1;
};

double distancia(const Punto& a, const Punto& b) {
    return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
}

void dibujar_grilla(const vector<Punto>& puntos) {
    char grilla[10][10];
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
            grilla[i][j] = '.';
    char simbolos[] = {'*','@','o'};
    for (int i = 0; i < puntos.size(); ++i) {
        int x = (int)puntos[i].x;
        int y = (int)puntos[i].y;
        if (x >= 0 && x <= 10 && y >= 0 && y <= 10 && puntos[i].cluster >= 0)
            grilla[10-y][x-1] = simbolos[puntos[i].cluster];
    }
    cout << "Grilla 10x10 (#=Cluster1, @=Cluster2, o=Cluster3):\n";
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j)
            cout << grilla[i][j] << " ";
        cout << endl;
    }
}

int main() {
    //A= (2,10); B=(2, 5); C=(8,4); D=(5,8); E=(7,5); F=(6,4); G=(1,2); H=(4,9)
    vector<Punto> puntos = {
        {"A",2,10}, {"B",2,5}, {"C",8,4}, {"D",5,8},
        {"E",7,5}, {"F",6,4}, {"G",1,2}, {"H",4,9}
    };
    vector<Punto> centroides = {puntos[0], puntos[3], puntos[6]};

    for (int iter = 1; iter <= 3; ++iter) {
        for (int i = 0; i < puntos.size(); ++i) {
            double min_dist = 1e9;
            int cluster = -1;
            for (int c = 0; c < 3; ++c) {
                double d = distancia(puntos[i], centroides[c]);
                if (d < min_dist) {
                    min_dist = d;
                    cluster = c;
                }
            }
            puntos[i].cluster = cluster;
        }
        cout << "\nIteracion " << iter << ":\n";
        for (int c = 0; c < 3; ++c) {
            cout << "Cluster " << c+1 << ": ";
            for (int i = 0; i < puntos.size(); ++i)
                if (puntos[i].cluster == c)
                    cout << puntos[i].nombre << " ";
            cout << endl;
        }
        // Calcular nuevos centroides
        for (int c = 0; c < 3; ++c) {
            double sx = 0, sy = 0;//sumas
            int points_per_cluster = 0;
            for (int i = 0; i < puntos.size(); ++i) {
                if (puntos[i].cluster == c) {
                    sx += puntos[i].x;
                    sy += puntos[i].y;
                    points_per_cluster++;
                }
            }
            if (points_per_cluster > 0) {
                centroides[c].x = sx / points_per_cluster;
                centroides[c].y = sy / points_per_cluster;
            }
            else{
                cout << "Cluster " << c+1 << " no tiene puntos asignados.\n";
            }
        }
        cout << "Nuevos centroides:\n";
        for (int c = 0; c < 3; ++c)
            cout << "Cluster " << c+1 << ": (" << fixed << setprecision(2) << centroides[c].x << ", " << centroides[c].y << ")\n";
        // Dibujar grilla
        dibujar_grilla(puntos);
    }
    return 0;
}