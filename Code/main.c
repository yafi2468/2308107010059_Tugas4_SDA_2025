#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include "sorting.h"

// Konfigurasi maksimum
#define PANJANG_MAKS_KATA 100
#define MAKS_DATA 2000000

// Variabel global
int *data_asli_int = NULL;
int *salinan_int = NULL;
char **data_asli_str = NULL;
char **salinan_str = NULL;
size_t ukuran_alokasi_sekarang = 0;

// Deklarasi fungsi
void muat_data_int(const char *namafile, int *array, int n);
void muat_data_str(const char *namafile, char **array, int n);
void jalankan_pengurutan_int(int n);
void jalankan_pengurutan_str(int n);
void bersihkan_memori();

// Fungsi untuk memuat data integer dari file
void muat_data_int(const char *namafile, int *array, int n) {
    FILE *file = fopen(namafile, "r");
    if (!file) {
        fprintf(stderr, "Error membuka file %s: %s\n", namafile, strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    for (int i = 0; i < n; i++) {
        if (fscanf(file, "%d", &array[i]) != 1) {
            fprintf(stderr, "Konten file %s tidak valid (harus %d angka). Hanya %d ditemukan.\n", namafile, n, i);
            fclose(file);
            exit(EXIT_FAILURE);
        }
    }
    fclose(file);
}

// Fungsi untuk memuat data string dari file
void muat_data_str(const char *namafile, char **array, int n) {
    FILE *file = fopen(namafile, "r");
    if (!file) {
        fprintf(stderr, "Error membuka file %s: %s\n", namafile, strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    char buffer[PANJANG_MAKS_KATA];
    for (int i = 0; i < n; i++) {
        if (fscanf(file, "%99s", buffer) != 1) {
            fprintf(stderr, "Konten file %s tidak valid (harus %d kata). Hanya %d ditemukan.\n", namafile, n, i);
            fclose(file);
            exit(EXIT_FAILURE);
        }
        
        array[i] = (char *)malloc(strlen(buffer) + 1);
        if (!array[i]) {
            fprintf(stderr, "Alokasi memori gagal untuk kata ke-%d\n", i);
            fclose(file);
            exit(EXIT_FAILURE);
        }
        strcpy(array[i], buffer);
    }
    fclose(file);
}

// Fungsi untuk menjalankan pengurutan data integer
void jalankan_pengurutan_int(int n) {
    data_asli_int = (int *)malloc(n * sizeof(int));
    salinan_int = (int *)malloc(n * sizeof(int));
    
    if (!data_asli_int || !salinan_int) {
        fprintf(stderr, "Alokasi memori gagal untuk %d data integer (%zu MB)\n",
                n, (2 * n * sizeof(int)) / (1024.0 * 1024.0));
        bersihkan_memori();
        exit(EXIT_FAILURE);
    }

    muat_data_int("../data/data_angka.txt", data_asli_int, n);

    // Tampilkan header tabel
    printf("\nMemulai pengurutan untuk %d data...\n\n");
    printf("+----------------------------+--------------+--------------+\n");
    printf("|         Algoritma          |  Waktu (s)   | Memori (MB)  |\n");
    printf("+----------------------------+--------------+--------------+\n");

    // Daftar algoritma pengurutan
    const char *label_algoritma[] = {
        "Bubble Sort (angka)", "Selection Sort (angka)", "Insertion Sort (angka)",
        "Merge Sort (angka)", "Quick Sort (angka)", "Shell Sort (angka)"
    };
    
    void (*fungsi_sorting[])(int *, int) = {
        bubble_sort_int, selection_sort_int, insertion_sort_int,
        merge_sort_int_wrapper, quick_sort_int_wrapper, shell_sort_int
    };

    // Jalankan benchmark untuk setiap algoritma
    for (int i = 0; i < 6; i++) {
        memcpy(salinan_int, data_asli_int, n * sizeof(int));
        benchmark_sort_int(label_algoritma[i], fungsi_sorting[i], salinan_int, n);
    }

    printf("+----------------------------+--------------+--------------+\n");
}

// Fungsi untuk membersihkan memori
void bersihkan_memori() {
    if (data_asli_int) free(data_asli_int);
    if (salinan_int) free(salinan_int);
    
    if (data_asli_str) {
        for (size_t i = 0; i < ukuran_alokasi_sekarang; i++) {
            if (data_asli_str[i]) free(data_asli_str[i]);
            if (salinan_str[i]) free(salinan_str[i]);
        }
        free(data_asli_str);
        free(salinan_str);
    }
}

// Fungsi utama
int main() {
    printf("=== PROGRAM BENCHMARK SORTING ===\n");
    
    // Pilih jenis data
    int jenis_data;
    printf("\nPilih jenis data:\n1. Angka\n2. Kata\nPilihan: ");
    scanf("%d", &jenis_data);

    // Pilih ukuran data
    int ukuran_data[] = {10000, 50000, 100000, 250000, 500000, 1000000, 1500000, 2000000};
    int pilihan;
    printf("\nPilih ukuran data:\n");
    for (int i = 0; i < 8; i++) {
        printf("%d. %d data\n", i+1, ukuran_data[i]);
    }
    printf("Pilihan: ");
    scanf("%d", &pilihan);
    
    int n = ukuran_data[pilihan-1];

    // Jalankan proses sesuai pilihan
    if (jenis_data == 1) {
        jalankan_pengurutan_int(n);
    } else {
        jalankan_pengurutan_str(n);
    }

    bersihkan_memori();
    return 0;
}