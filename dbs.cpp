#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
using namespace std;

struct Point {
    double x, y;
    int core=-1; // 1: core point, 0: no core point, -1: no clasificado
};

struct Cluster {
    int id; // 0: ruido, >0: cluster ID
    vector<Point> points;
};
//A= (2,10); B=(2, 5); C=(8,4); D=(5,8); E=(7,5); F=(6,4); G=(1,2); H=(4,9)
vector<Point> puntos = {
    {2,10}, // A
    {2,5},  // B
    {8,4},  // C
    {5,8},  // D
    {7,5},  // E
    {6,4},  // F
    {1,2},  // G
    {4,9}   // H
};

double distancia(const Point& a, const Point& b) {
    return sqrt((a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y));
}


void core(vector<Point>& pts, double eps, int minPts) {
    for (int i = 0; i < pts.size(); ++i) {
        int vecinos = 0;
        for (int j = 0; j < pts.size(); ++j) {
            if (distancia(pts[i], pts[j]) <= eps)
                vecinos++;
        }
        if (vecinos >= minPts)
            pts[i].core = 1;
        else
            pts[i].core = 0;
    }
}

// Expande recursivamente los core points conectados a idx y los agrega al cluster
void expandirCore(vector<Point>& pts, int idx, Cluster& cluster, double eps) {
    if (idx < 0 || idx >= pts.size() || pts[idx].core != 1)
        return;
    Point p = pts[idx];
    cluster.points.push_back(p);
    pts.erase(pts.begin() + idx);

    // Buscar vecinos core
    for (int j = 0; j < pts.size(); ) {
        if (pts[j].core == 1 && distancia(p, pts[j]) <= eps) {
            expandirCore(pts, j, cluster, eps);
            // No incrementar j porque el vector se reduce
        } else {
            ++j;
        }
    }
}

void asignarNoCore(vector<Point>& pts, int idx, vector<Cluster>& clusters, double eps) {
    Point& punto = pts[idx];
    for (auto& cluster : clusters) {
        for (auto& corep : cluster.points) {
            if (corep.core == 1 && distancia(punto, corep) <= eps) {
                cluster.points.push_back(punto);
                pts.erase(pts.begin() + idx);
                return; // ya no seguir buscando en otros clusters para este punto
            }
        }
    }
}

vector<Cluster> dbscan(double eps, int minPts, vector<Point>& pts) {
    core(pts, eps, minPts);

    vector<Cluster> clusters;
    int cluster_id = 1;
    vector<Point> no_core;

    // 1. Asignar core points a clusters y expandir a vecinos core recursivamente
    while (true) {
        int idx = -1;
        for (int i = 0; i < pts.size(); ++i) {
            if (pts[i].core == 1) {
                idx = i;
                break;
            }
        }
        if (idx == -1) break;

        Cluster cluster;
        cluster.id = cluster_id;
        expandirCore(pts, idx, cluster, eps);
        clusters.push_back(cluster);
        cluster_id++;
    }


    // 2. Asignar no-core a clusters si están cerca de algún core del cluster
    for (int i = 0; i < pts.size(); ) {
        int prev_size = pts.size();
        asignarNoCore(pts, i, clusters, eps);
        if (pts.size() < prev_size) {
            // Se eliminó el punto, no incrementar i
        } else {
            ++i;
        }
    }

    // 3. Los puntos restantes en pts son ruido
    if (!pts.empty()) {
        Cluster ruido;
        ruido.id = 0;
        for (auto& p : pts)
            ruido.points.push_back(p);
        clusters.insert(clusters.begin(), ruido);
        pts.clear();
    }

    return clusters;
}

void imprimirClusters(vector<Cluster> Clusters) {
    for (const auto& cluster : Clusters) {
        if (cluster.id == 0)
            cout << "Ruido: ";
        else
            cout << "Cluster " << cluster.id << ": ";
        for (const auto& p : cluster.points) {
            cout << "(" << p.x << "," << p.y << ") ";
        }
        cout << endl;
    }
}

void imprimirGrilla(vector<Cluster> Clusters) {
    // Grilla de 10x10, inicializar con '.'
    char grilla[10][10];
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
            grilla[i][j] = '.';

    for (const auto& cluster : Clusters) {
        char simbolo;
        if (cluster.id == 0)
            simbolo = 'R'; // Ruido
        else if (cluster.id < 10)
            simbolo = '0' + cluster.id; // '1', '2', ...
        else
            simbolo = '*'; // Para ids mayores a 9
        for (const auto& p : cluster.points) {
            int x = static_cast<int>(p.x);
            int y = static_cast<int>(p.y);
            // Invertir y para que (0,0) esté abajo a la izquierda
            if (x >= 0 && x <= 10 && y >= 0 && y <= 10)
                grilla[10-y][x-1] = simbolo;
        }
    }

    // Imprimir la grilla
    cout << "Grilla 10x10:\n";
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            cout << grilla[i][j] << " ";
        }
        cout << endl;
    }
}

int main() {
    // Caso 1: eps = 2, minPts = 2
    vector<Point> pts1 = puntos;
    vector<Cluster> clusters=dbscan(2.0, 2, pts1);
    cout << "DBSCAN con eps=2, minPts=2\n";
    imprimirClusters(clusters);
    imprimirGrilla(clusters);

    // Caso 2: eps = sqrt(10), minPts = 2
    vector<Point> pts2 = puntos;
    clusters=dbscan(sqrt(10.0), 2, pts2);
    cout << "\nDBSCAN con eps=sqrt(10), minPts=2\n";
    imprimirClusters(clusters);
    imprimirGrilla(clusters);

    return 0;
}