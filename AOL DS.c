#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 1. Struktur Data Menu Node
struct MenuNode {
    char id[10];
    char nama[100];
    char kategori[50];
    int harga;
    char deskripsi[200];
    char bahan[200];
    char variasi[50];
    struct MenuNode* left;
    struct MenuNode* right;
};

// Fungsi membuat node menu baru
struct MenuNode* createMenuNode(const char* id, const char* nama, const char* kategori, int harga, const char* deskripsi, const char* bahan, const char* variasi) {
    struct MenuNode* newNode = (struct MenuNode*)malloc(sizeof(struct MenuNode));
    strcpy(newNode->id, id);
    strcpy(newNode->nama, nama);
    strcpy(newNode->kategori, kategori);
    newNode->harga = harga;
    strcpy(newNode->deskripsi, deskripsi);
    strcpy(newNode->bahan, bahan);
    strcpy(newNode->variasi, variasi);
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

// 2. Fungsi Insert BST
struct MenuNode* insertMenu(struct MenuNode* root, const char* id, const char* nama, const char* kategori, int harga, const char* deskripsi, const char* bahan, const char* variasi) {
    if (root == NULL) {
        return createMenuNode(id, nama, kategori, harga, deskripsi, bahan, variasi);
    }

    int cmp = strcmp(nama, root->nama);
    if (cmp < 0) {
        root->left = insertMenu(root->left, id, nama, kategori, harga, deskripsi, bahan, variasi);
    } else if (cmp > 0) {
        root->right = insertMenu(root->right, id, nama, kategori, harga, deskripsi, bahan, variasi);
    } else {
        printf("Sistem: Menu '%s' sudah ditambahkan!\n", nama);
    }
    return root;
}

// 3. Fungsi File Processing (Load)
struct MenuNode* loadDataFromFile(struct MenuNode* root, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Peringatan: File '%s' tidak ditemukan. Memulai dengan data kosong.\n", filename);
        return root;
    }

    char line[500]; 
    fgets(line, sizeof(line), file); // Skip header CSV

    int count = 0;
    while (fgets(line, sizeof(line), file)) {
        char* idStr = strtok(line, ",\r\n");
        char* namaStr = strtok(NULL, ",\r\n");
        char* kategoriStr = strtok(NULL, ",\r\n");
        char* hargaStr = strtok(NULL, ",\r\n");
        char* descStr = strtok(NULL, ",\r\n");
        char* bahanStr = strtok(NULL, ",\r\n");
        char* varStr = strtok(NULL, ",\r\n");

        if (idStr && namaStr && kategoriStr && hargaStr && descStr && bahanStr && varStr) {
            int hargaMenu = atoi(hargaStr); 
            root = insertMenu(root, idStr, namaStr, kategoriStr, hargaMenu, descStr, bahanStr, varStr);
            count++;
        }
    }
    fclose(file);
    printf("Sistem: Berhasil memuat %d data menu dari '%s'!\n", count, filename);
    return root;
}

// 4. Fungsi File Processing (Auto-Save)
void saveToFileHelper(struct MenuNode* root, FILE* file) {
    if (root == NULL) return;
    saveToFileHelper(root->left, file); 
    fprintf(file, "%s,%s,%s,%d,%s,%s,%s\n", root->id, root->nama, root->kategori, root->harga, root->deskripsi, root->bahan, root->variasi);
    saveToFileHelper(root->right, file); 
}

void syncDataToFile(struct MenuNode* root, const char* filename) {
    FILE* file = fopen(filename, "w"); 
    if (file == NULL) {
        printf("Error: Gagal menyimpan data ke file '%s'!\n", filename);
        return;
    }
    fprintf(file, "ID_Menu,Nama_Menu,Kategori,Harga,Deskripsi,Bahan_Dasar,Variasi\n");
    saveToFileHelper(root, file);
    fclose(file);
}

// 5. Fitur Tampilan Buku Menu Kafe (Dinamis berdasarkan Login)
void viewKategoriSpesifik(struct MenuNode* root, const char* targetKategori, int isAdmin) {
    if (root == NULL) return;
    
    viewKategoriSpesifik(root->left, targetKategori, isAdmin);
    
    if (strcmp(root->kategori, targetKategori) == 0) {
        if (isAdmin) {
            printf(">> [%-4s] %-30s Rp %d\n", root->id, root->nama, root->harga);
        } else {
            printf(">> %-35s Rp %d\n", root->nama, root->harga);
        }
        printf("   %s (Bahan: %s)\n", root->deskripsi, root->bahan);
        printf("   *Pilihan: %s\n\n", root->variasi);
    }
    
    viewKategoriSpesifik(root->right, targetKategori, isAdmin);
}

void viewKategoriLainnya(struct MenuNode* root, int isAdmin) {
    if (root == NULL) return;
    
    viewKategoriLainnya(root->left, isAdmin);
    
    if (strcmp(root->kategori, "Kopi") != 0 && strcmp(root->kategori, "Non-Kopi") != 0 && 
        strcmp(root->kategori, "Makanan") != 0 && strcmp(root->kategori, "Snack") != 0) {
        
        if (isAdmin) {
            printf(">> [%-4s] %-30s Rp %d [Kategori: %s]\n", root->id, root->nama, root->harga, root->kategori);
        } else {
            printf(">> %-35s Rp %d [Kategori: %s]\n", root->nama, root->harga, root->kategori);
        }
        printf("   %s (Bahan: %s)\n", root->deskripsi, root->bahan);
        printf("   *Pilihan: %s\n\n", root->variasi);
    }
    
    viewKategoriLainnya(root->right, isAdmin);
}

void viewSemuaRapih(struct MenuNode* root, int isAdmin) {
    if (root == NULL) {
        printf("Sistem: Data menu masih kosong.\n");
        return;
    }
    
    printf("\n====================================================\n");
    printf("                  BUKU MENU KAFE                    \n");
    printf("====================================================\n\n");
    
    printf("[ KOPI ]\n----------------------------------------------------\n");
    viewKategoriSpesifik(root, "Kopi", isAdmin);
    printf("[ NON-KOPI ]\n----------------------------------------------------\n");
    viewKategoriSpesifik(root, "Non-Kopi", isAdmin);
    printf("[ MAKANAN ]\n----------------------------------------------------\n");
    viewKategoriSpesifik(root, "Makanan", isAdmin);
    printf("[ SNACK ]\n----------------------------------------------------\n");
    viewKategoriSpesifik(root, "Snack", isAdmin);
    printf("[ LAINNYA ]\n----------------------------------------------------\n");
    viewKategoriLainnya(root, isAdmin);
    
    printf("====================================================\n");
}

// 6. Fitur Cari Detail Menu (Mendukung Pencarian berdasarkan Nama ATAU ID)
int searchMenu(struct MenuNode* root, const char* target) {
    if (root == NULL) return 0; 

    if (strcmp(target, root->nama) == 0 || strcmp(target, root->id) == 0) {
        printf("\n======= DETAIL MENU DITEMUKAN =======\n");
        printf("ID Menu     : %s\n", root->id);
        printf("Nama Menu   : %s\n", root->nama);
        printf("Kategori    : %s\n", root->kategori);
        printf("Variasi     : %s\n", root->variasi);
        printf("Harga       : Rp %d\n", root->harga);
        printf("-------------------------------------\n");
        printf("Bahan Dasar : %s\n", root->bahan);
        printf("Deskripsi   : %s\n", root->deskripsi);
        printf("=====================================\n");
        return 1; 
    }

    int ketemuDiKiri = searchMenu(root->left, target);
    if (ketemuDiKiri) return 1; 

    return searchMenu(root->right, target); 
}

// 7. Fitur Hapus Menu
struct MenuNode* findMin(struct MenuNode* root) {
    while (root->left != NULL) root = root->left;
    return root;
}

struct MenuNode* deleteMenu(struct MenuNode* root, const char* targetNama) {
    if (root == NULL) {
        printf("Menu '%s' tidak ditemukan untuk dihapus.\n", targetNama);
        return root;
    }
    int cmp = strcmp(targetNama, root->nama);
    if (cmp < 0) root->left = deleteMenu(root->left, targetNama);
    else if (cmp > 0) root->right = deleteMenu(root->right, targetNama);
    else {
        if (root->left == NULL) {
            struct MenuNode* temp = root->right;
            free(root);
            printf("Sistem: Menu berhasil dihapus.\n");
            return temp;
        } else if (root->right == NULL) {
            struct MenuNode* temp = root->left;
            free(root);
            printf("Sistem: Menu berhasil dihapus.\n");
            return temp;
        }
        struct MenuNode* temp = findMin(root->right);
        strcpy(root->id, temp->id);
        strcpy(root->nama, temp->nama);
        strcpy(root->kategori, temp->kategori);
        root->harga = temp->harga;
        strcpy(root->deskripsi, temp->deskripsi);
        strcpy(root->bahan, temp->bahan);
        strcpy(root->variasi, temp->variasi);
        
        root->right = deleteMenu(root->right, temp->nama);
    }
    return root;
}

// 8. Sistem Autentikasi Admin
void registerAdmin() {
    char username[50], password[50];
    int isDuplicate;

    printf("\n--- REGISTER ADMIN BARU ---\n");

    do {
        isDuplicate = 0; 
        printf("Masukkan Username baru: ");
        scanf(" %[^\n]", username);

        FILE *fileCek = fopen("admin.txt", "r");
        if (fileCek != NULL) {
            char line[150];
            while (fgets(line, sizeof(line), fileCek)) {
                line[strcspn(line, "\r\n")] = 0; 
                char* tokenUser = strtok(line, ","); 

                if (tokenUser != NULL && strcmp(username, tokenUser) == 0) {
                    isDuplicate = 1;
                    printf(" -> Error: Username '%s' sudah terdaftar! Silakan gunakan username lain.\n", username);
                    break; 
                }
            }
            fclose(fileCek); 
        }
    } while (isDuplicate); 

    printf("Masukkan Password baru: ");
    scanf(" %[^\n]", password);

    FILE *fileSimpan = fopen("admin.txt", "a"); 
    if (fileSimpan == NULL) {
        printf("Error: Tidak bisa membuka file database admin.\n");
        return;
    }

    fprintf(fileSimpan, "%s,%s\n", username, password);
    fclose(fileSimpan);
    
    printf("Sistem: Akun Admin '%s' berhasil didaftarkan!\n", username);
}

int loginAdmin() {
    char inputUser[50], inputPass[50];
    FILE *file = fopen("admin.txt", "r"); 
    if (file == NULL) {
        printf("Sistem: Belum ada admin terdaftar. Silakan Register dulu.\n");
        return 0; 
    }

    printf("\n--- LOGIN ADMIN ---\n");
    printf("Username: ");
    scanf(" %[^\n]", inputUser);
    printf("Password: ");
    scanf(" %[^\n]", inputPass);

    char line[150];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = 0; 
        char* tokenUser = strtok(line, ",");
        char* tokenPass = strtok(NULL, ",");

        if (tokenUser && tokenPass && strcmp(inputUser, tokenUser) == 0 && strcmp(inputPass, tokenPass) == 0) {
            fclose(file);
            printf("Sistem: Login Berhasil! Selamat datang, %s.\n", inputUser);
            return 1; 
        }
    }
    fclose(file);
    printf("Sistem: Login Gagal! Username atau Password salah.\n");
    return 0; 
}

// Fungsi pembantu: Mengecek apakah ID kembar
int isIDExist(struct MenuNode* root, const char* targetID) {
    if (root == NULL) return 0; 
    if (strcmp(root->id, targetID) == 0) return 1; 
    
    int ketemuDiKiri = isIDExist(root->left, targetID);
    if (ketemuDiKiri) return 1;
    
    return isIDExist(root->right, targetID);
}

// Fungsi pembantu: Mengecek apakah Nama Menu kembar
int isNamaExist(struct MenuNode* root, const char* targetNama) {
    if (root == NULL) return 0; 
    
    int cmp = strcmp(targetNama, root->nama);
    if (cmp == 0) return 1; 
    else if (cmp < 0) return isNamaExist(root->left, targetNama); 
    else return isNamaExist(root->right, targetNama); 
}

// Fungsi pembantu: Mencari Nama Menu jika Admin mengetikkan ID
const char* findNameByID(struct MenuNode* root, const char* targetID) {
    if (root == NULL) return NULL; 

    if (strcmp(root->id, targetID) == 0) return root->nama;

    const char* leftSearch = findNameByID(root->left, targetID);
    if (leftSearch != NULL) return leftSearch;

    return findNameByID(root->right, targetID);
}

// === FUNGSI UTAMA PROGRAM ===
int main() {
    struct MenuNode* root = NULL;
    int pilihan;
    int isAdmin = 0; 
    
    char inputID[10], inputNama[100], inputKategori[50];
    int inputHarga;
    char inputDeskripsi[200], inputBahan[200], inputVariasi[50];

    printf("=========================================\n");
    printf("   MEMULAI SISTEM MANAJEMEN MENU KAFE    \n");
    printf("=========================================\n");
    root = loadDataFromFile(root, "menu_kafe.csv");
    printf("Tekan ENTER untuk masuk ke sistem...");
    getchar();

    do {
        printf("\n=========================================\n");
        printf("        SISTEM KASIR KAFE (BST)          \n");
        printf("=========================================\n");
        if (isAdmin) printf("  [ STATUS: ADMIN AKTIF ]\n");
        else printf("  [ STATUS: KASIR/TAMU ]\n");
        printf("-----------------------------------------\n");
        printf("1. Tambah Menu Baru  [Butuh Akses Admin]\n");
        printf("2. Cari Detail Menu\n");
        printf("3. Tampilkan Semua Menu (Buku Menu)\n");
        printf("4. Hapus Menu        [Butuh Akses Admin]\n");
        if (isAdmin) printf("8. Logout Admin\n");
        else printf("8. Login / Register Admin\n");
        printf("9. Keluar Aplikasi\n");
        printf("=========================================\n");
        printf("Pilih aksi: ");
        
        if (scanf("%d", &pilihan) != 1) {
            while (getchar() != '\n');
            pilihan = 0;
        }

        switch (pilihan) {
            case 1:
                if (!isAdmin) {
                    printf("\nAKSES DITOLAK! Anda harus Login Admin.\n");
                    break;
                }
                printf("\n--- TAMBAH MENU BARU ---\n");
                printf("[ Panduan Format ID Menu ]\n");
                printf("- Kopi     : K** (contoh: K11)\n");
                printf("- Non-Kopi : NK** (contoh: NK11)\n");
                printf("- Makanan  : M** (contoh: M11)\n");
                printf("- Snack    : S** (contoh: S11)\n");
                printf("-----------------------------------------\n");
                
                int validID = 0;
                do {
                    printf("Masukkan ID        : "); 
                    scanf(" %[^\n]", inputID); 
                    
                    if (strncmp(inputID, "NK", 2) == 0) {
                        strcpy(inputKategori, "Non-Kopi");
                        validID = 1; 
                    } else if (strncmp(inputID, "K", 1) == 0) {
                        strcpy(inputKategori, "Kopi");
                        validID = 1; 
                    } else if (strncmp(inputID, "M", 1) == 0) {
                        strcpy(inputKategori, "Makanan");
                        validID = 1; 
                    } else if (strncmp(inputID, "S", 1) == 0) {
                        strcpy(inputKategori, "Snack");
                        validID = 1; 
                    } else {
                        printf(" -> Error: Format ID salah! Gunakan awalan K, NK, M, atau S.\n");
                    }

                    if (validID == 1) {
                        if (isIDExist(root, inputID)) {
                            printf(" -> Error: ID '%s' sudah dipakai oleh menu lain! Silakan gunakan ID berbeda.\n", inputID);
                            validID = 0; 
                        }
                    }
                } while (!validID);

                int validNama = 0;
                do {
                    printf("Masukkan Nama Menu : "); 
                    scanf(" %[^\n]", inputNama);
                    
                    if (isNamaExist(root, inputNama)) {
                        printf(" -> Error: Menu bernama '%s' sudah ada di sistem! Gunakan nama yang sedikit berbeda (misal: %s Ice).\n", inputNama, inputNama);
                    } else {
                        validNama = 1; 
                    }
                } while (!validNama);

                printf("Masukkan Harga     : Rp "); scanf("%d", &inputHarga);
                printf("Masukkan Deskripsi produk   : "); scanf(" %[^\n]", inputDeskripsi);
                printf("Masukkan Bahan utama produk : "); scanf(" %[^\n]", inputBahan);
                printf("Masukkan Variasi [JANGAN pakai koma](Contoh: (Hot & Ice)/(Hot Only)/(Ice/Hot)/Level 1-n)  : "); scanf(" %[^\n]", inputVariasi);
                
                root = insertMenu(root, inputID, inputNama, inputKategori, inputHarga, inputDeskripsi, inputBahan, inputVariasi);
                
                printf("\nSistem: Menu '%s' berhasil didaftarkan ke kategori '%s'!\n", inputNama, inputKategori);
                break;
                
            case 2:
                printf("\n--- CARI DETAIL MENU ---\n");
                if (root == NULL) {
                    printf("Sistem: Data menu masih kosong.\n");
                } else {
                    char inputCari[100];
                    printf("Masukkan ID atau Nama Menu yang dicari: ");
                    scanf(" %[^\n]", inputCari);
                    
                    int statusKetemu = searchMenu(root, inputCari);
                    
                    if (!statusKetemu) {
                        printf("Sistem: Menu atau ID '%s' tidak ditemukan di seluruh katalog.\n", inputCari);
                    }
                }
                break;
                
            case 3:
                viewSemuaRapih(root, isAdmin); 
                break;
                
            case 4:
                if (!isAdmin) {
                    printf("\nAKSES DITOLAK! Anda harus Login Admin.\n");
                    break;
                }
                
                printf("\n--- HAPUS MENU ---\n");
                if (root == NULL) {
                    printf("Sistem: Data menu masih kosong.\n");
                } else {
                    char inputHapus[100];
                    printf("Masukkan ID atau Nama Menu yang akan dihapus: ");
                    scanf(" %[^\n]", inputHapus);

                    const char* targetHapus = NULL; 
                    const char* namaTarget = findNameByID(root, inputHapus);

                    if (namaTarget != NULL) {
                        printf("Sistem: Mendeteksi ID '%s' (Menu: %s)\n", inputHapus, namaTarget);
                        targetHapus = namaTarget; 
                    } 
                    else if (isNamaExist(root, inputHapus)) {
                        targetHapus = inputHapus; 
                    } 
                    else {
                        printf("Sistem: Menu atau ID '%s' tidak ditemukan untuk dihapus.\n", inputHapus);
                    }

                    if (targetHapus != NULL) {
                        char konfirmasi;
                        printf("\n[PERINGATAN] Apakah Anda yakin ingin menghapus '%s' secara permanen? (Y/N): ", targetHapus);
                        scanf(" %c", &konfirmasi); 

                        if (konfirmasi == 'Y' || konfirmasi == 'y') {
                            root = deleteMenu(root, targetHapus);
                        } else {
                            printf("Sistem: Penghapusan dibatalkan. Data tetap aman.\n");
                        }
                    }
                }
                break;
                
            case 8:
                if (isAdmin) {
                    isAdmin = 0; 
                    printf("\nSistem: Berhasil Logout.\n");
                } else {
                    int authPilihan;
                    printf("\n--- PORTAL ADMIN ---\n1. Login\n2. Register Admin Baru\nPilih: ");
                    scanf("%d", &authPilihan);
                    if (authPilihan == 1) isAdmin = loginAdmin(); 
                    else if (authPilihan == 2) registerAdmin();
                }
                break;

            case 9:
                printf("\n--- MENYIMPAN DATA ---\n");
                syncDataToFile(root, "menu_kafe.csv");
                printf("Terima kasih. Sistem dimatikan.\n");
                break;
                
            default:
                if (pilihan != 0) printf("Pilihan tidak valid!\n");
        }
        if (pilihan != 9 && pilihan != 0) {
            printf("\nTekan ENTER untuk melanjutkan...");
            while (getchar() != '\n'); 
            getchar(); 
        }
    } while (pilihan != 9);

    return 0;
}
