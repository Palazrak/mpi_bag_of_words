#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sstream>

using namespace std;

string ejecutar_serial() {
    string comando = "./bag_of_words 6 dickens_a_christmas_carol.txt dickens_a_tale_of_two_cities.txt dickens_oliver_twist.txt shakespeare_hamlet.txt shakespeare_romeo_juliet.txt shakespeare_the_merchant_of_venice.txt vocab.txt 15164 > temp_serial.txt";
    system(comando.c_str());

    ifstream in("temp_serial.txt");
    string linea, tiempo;
    while (getline(in, linea)) {
        if (linea.find("tiempo:") != string::npos) {
            stringstream ss(linea);
            string desc;
            ss >> desc >> tiempo;
            break;
        }
    }
    return tiempo;
}

string ejecutar_paralelo() {
    string comando = "mpirun -np 6 ./bag_of_words_paralelo dickens_a_christmas_carol.txt dickens_a_tale_of_two_cities.txt dickens_oliver_twist.txt shakespeare_hamlet.txt shakespeare_romeo_juliet.txt shakespeare_the_merchant_of_venice.txt vocab.txt 15164 > temp_paralelo.txt";
    system(comando.c_str());

    ifstream in("temp_paralelo.txt");
    string linea, tiempo;
    while (getline(in, linea)) {
        if (linea.find("Tiempo total") != string::npos) {
            stringstream ss(linea);
            string temp;
            while (ss >> temp) {
                if (isdigit(temp[0])) {
                    tiempo = temp;
                    break;
                }
            }
        }
    }
    return tiempo;
}

int main() {
    ofstream out("resultados.csv");
    out << "tiempo_serial,tiempo_paralelo\n";

    for (int i = 0; i < 30; i++) {
        cout << "Ejecución " << (i+1) << "/30\n";

        string tiempo_serial = ejecutar_serial();
        string tiempo_paralelo = ejecutar_paralelo();

        out << tiempo_serial << "," << tiempo_paralelo << "\n";
    }

    out.close();
    cout << "✅ Experimentos finalizados. Resultados guardados en resultados.csv\n";
    return 0;
}
