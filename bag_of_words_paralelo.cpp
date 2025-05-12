#include <mpi.h>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>

using namespace std;

//-------------------------------------------------------------
// 1. Crear diccionario desde vocabulario
map<string, int> crear_diccionario(const string& vocab_file) {
    ifstream in(vocab_file);
    map<string, int> dict;
    string palabra;
    int index = 0;

    while (getline(in, palabra)) {
        if (!palabra.empty()) {
            dict[palabra] = index++;
        }
    }
    return dict;
}

//-------------------------------------------------------------
// 2. Contar palabras y llenar matriz bolsa[doc][word]
void contar_palabras(const string& archivo, const map<string, int>& dict, int* bolsa, int vocab_size) {
    ifstream in(archivo);
    string palabra;

    while (getline(in, palabra, ',')) {
        auto it = dict.find(palabra);
        if (it != dict.end()) {
            int pos = it->second;
            bolsa[pos]++;
        }
    }
}

//-------------------------------------------------------------
// 3. Guardar la matriz en CSV
void guardar_csv(const string& nombre_archivo, int* bolsa_total, int num_docs, int vocab_size, const map<string, int>& dict) {
    ofstream out(nombre_archivo);
    vector<string> vocab_ordenado(vocab_size);

    // reconstruir vocabulario en orden
    for (const auto& par : dict) {
        vocab_ordenado[par.second] = par.first;
    }

    // Escribir encabezado
    for (int i = 0; i < vocab_size; i++) {
        out << vocab_ordenado[i];
        if (i != vocab_size - 1) out << ",";
    }
    out << "\n";

    // Escribir filas
    for (int doc = 0; doc < num_docs; doc++) {
        for (int word = 0; word < vocab_size; word++) {
            out << bolsa_total[doc * vocab_size + word];
            if (word != vocab_size - 1) out << ",";
        }
        out << "\n";
    }

    out.close();
}

//-------------------------------------------------------------
// 4. Main
int main(int argc, char** argv) {

    // Inicializamos el entorno de MPI
    const int MASTER = 0;
    int num_processes, process_id, name_length;
    char host_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
    MPI_Get_processor_name(host_name, &name_length);

    double t_start = MPI_Wtime(); // Inicia contador

    string vocab_file = argv[argc - 2];
    int vocab_size = atoi(argv[argc - 1]);

    // 1. Crear vocabulario
    map<string, int> dict = crear_diccionario(vocab_file);

    string archivo_actual = argv[1 + process_id];

    // 2. Crear matriz bolsa de palabras locales e inicializar en cero
    int* bolsa_local = new int[vocab_size];
    for (int i = 0; i < vocab_size; i++) bolsa_local[i] = 0;

    MPI_Barrier(MPI_COMM_WORLD); // sincronización antes del conteo

    contar_palabras(archivo_actual, dict, bolsa_local, vocab_size);

    // El maestro reúne todas las bolsas
    int* bolsa_total = nullptr;
    if (process_id == MASTER) {
        bolsa_total = new int[vocab_size * num_processes];
    }

    MPI_Gather(bolsa_local, vocab_size, MPI_INT,
               bolsa_total, vocab_size, MPI_INT,
               MASTER, MPI_COMM_WORLD);

    // El maestro guarda el CSV
    if (process_id == MASTER) {
        guardar_csv("bolsa_paralelo.csv", bolsa_total, num_processes, vocab_size, dict);
        double t_end = MPI_Wtime();
        double t_total = t_end - t_start;
        cout << "Tiempo total (MPI_Wtime): " << t_total << " segundos\n";
    }

    delete[] bolsa_local;
    if (process_id == MASTER) {
        delete[] bolsa_total;
    }

    MPI_Finalize();
    return 0;
}
