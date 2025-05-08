# Bolsa de palabras con MPI

## Proyecto de Cómputo Paralelo y en la Nube. Primavera 2025. ITAM

###### Pablo Alazraki [@Palazrak](https://github.com/Palazrak) y Frida Márquez [@fridamarquezg](https://github.com/fridamarquezg)

### Introducción

El presente trabajo muestra la implementación de un algoritmo de conteo de palabras utilizando MPI. El proyecto consiste en la lectura de 6 archivos `.txt`, cuyas palabras estan separadas con comas y no contienen espacios, para posteriormente crear una matriz que muestre cuántas veces aparece cada palabra en cada uno de los libros proporcionados. Utilizamos un vocabulario fijo `vocab.txt` generado en el notebook `computo_paralelo_bolsa_de_palabras.ipynb`.

En este repositorio se encuentran los archivos que contienen las versiones serial y paralela del proyecto, el notebook con el que se generó el vocabulario, un experimento que ejecuta 30 veces cada version y un csv con los resultados de dichas ejecuciones.

### Descripción del equipo

- **Máquina**: MSI Thin GF63 12VF
- **Procesador**: Intel® Core™ i7-12650H de 12ª generación (2.30 GHz)
- **Núcleos**: 16 núcleos virtuales
- **Memoria**: 32 GB de RAM
- **Tarjeta gráfica**: NVIDIA GeForce RTX 4060 Laptop GPU (16 GB)
- **Sistema operativo**: Subsistema de Linux (WSL2) con Ubuntu 22.04.5 LTS
- **Entorno de desarrollo**: Visual Studio Code, utilizando la extensión oficial de C/C++ de Microsoft

### Version serial

La version serial crea una bolsa de palabras (internamente es un diccionario) para cada uno de los libros analizados de forma secuencial, y posteriormente guarda dichas bolsas en un `.csv`.

### Estrategia de paralelización

Se va a dedicar un proceso por cada libro, y al terminar de ejecutar, el master ejecuta un "gather" para juntar la informacion en un mismo `.csv`.
