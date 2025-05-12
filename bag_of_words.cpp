#include <omp.h>
#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <chrono>
#include <map>

using namespace std;

//-------------------------------------------------------------
// 1. Crear diccionario desde vocabulario
map<string,int> crear_dicc(string vocab_filename){
    ifstream in(vocab_filename);
    long long int word_count = 0;
    string word;
    map<string,int> word_dict;

    while (getline(in, word)) {
        if (!word.empty()) {
            word_dict[word] = word_count;
            word_count++;
        }
    }

    return word_dict;
}

//-------------------------------------------------------------
// 2. Contar palabras y llenar matriz bolsa[doc][word]
void count_words(const string& file_name, const map<string,int>& word_dict, int** bolsa, int doc_number) {
    ifstream in(file_name);
    string word;

    while (getline(in, word, ',')) {
        auto it = word_dict.find(word);
        if (it != word_dict.end()) {
            int word_pos = it->second;
            bolsa[doc_number][word_pos]++;
        }
        // else {
        //     cout << "Palabra no encontrada en vocabulario: " << word << "\n";
        // }
    }
}
//-------------------------------------------------------------
// 3. Guardar la matriz en CSV
void save_to_CSV(const string& file_name, int** bolsa, int vocab_size, int num_docs, const map<string,int>& word_dict) {
    fstream fout(file_name, ios::out);
    string* vocab_ordenado = new string[vocab_size];

    // reconstruir vocabulario en orden
    for (const auto& it : word_dict) {
        vocab_ordenado[it.second] = it.first;
    }

    // Escribir encabezado
    for (int i = 0; i < vocab_size; i++) {
        fout << vocab_ordenado[i];
        if (i != vocab_size - 1) fout << ",";
    }
    fout << "\n";

    // Escribir bolsa de palabras
    for (int doc = 0; doc < num_docs; doc++) {
        for (int word = 0; word < vocab_size; word++) {
            fout << bolsa[doc][word];
            if (word != vocab_size - 1) fout << ",";
        }
        fout << "\n";
    }

    delete[] vocab_ordenado;
}

//-------------------------------------------------------------
// 4. Main
int main(int argc, char** argv) {
    auto start = chrono::high_resolution_clock::now();

    int num_docs = atoi(argv[1]);
    string archivos[num_docs];
    for(int i = 0; i < num_docs; i++){
        archivos[i] = argv[2 + i];
    }

    string vocab_file = argv[num_docs + 2];
    int vocab_size = atoi(argv[num_docs + 3]);

    // 1. Crear vocabulario
    map<string,int> word_dict = crear_dicc(vocab_file);

    // 2. Crear matriz bolsa de palabras e inicializar en cero
    int** bolsa = new int*[num_docs];
    for(int i = 0; i < num_docs; i++){
        bolsa[i] = new int[vocab_size];
        for (int j = 0; j < vocab_size; j++) {
            bolsa[i][j] = 0;
        }
    }

    // 3. Versión serial
    for(int i = 0; i < num_docs; i++){
        count_words(archivos[i], word_dict, bolsa, i);
    }

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "tiempo: " << (float)duration.count() / 1e6 << "\n";

    // 4. Guardar CSV
    save_to_CSV("bolsa_serial.csv", bolsa, vocab_size, num_docs, word_dict);

    // 5. Liberar memoria
    for(int i = 0; i < num_docs; i++){
        delete[] bolsa[i];
    }
    delete[] bolsa;


    return 0;
}
