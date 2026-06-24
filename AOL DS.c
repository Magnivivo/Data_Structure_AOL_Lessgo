#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 1. Struktur Data Menu Node

//Data isi Node menu -
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

// 2. Fungsi Insert BST - 
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

// 5. Fitur Tampilan Buku Menu Kafe Lengkap
void viewKategoriSpesifik(struct MenuNode* root, const char* targetKategori) {
    if (root == NULL) return;
    
    viewKategoriSpesifik(root->left, targetKategori);
    
    if (strcmp(root->kategori, targetKategori) == 0) {
        printf(">> %-35s Rp %d\n", root->nama, root->harga);
        printf("   %s (Bahan: %s)\n", root->deskripsi, root->bahan);
        printf("   *Pilihan: %s\n\n", root->variasi);
    }
    
    viewKategoriSpesifik(root->right, targetKategori);
}

void viewKategoriLainnya(struct MenuNode* root) {
    if (root == NULL) return;
    
    viewKategoriLainnya(root->left);
    
    if (strcmp(root->kategori, "Kopi") != 0 && strcmp(root->kategori, "Non-Kopi") != 0 && 
        strcmp(root->kategori, "Makanan") != 0 && strcmp(root->kategori, "Snack") != 0) {
        
        printf(">> %-35s Rp %d [Kategori: %s]\n", root->nama, root->harga, root->kategori);
        printf("   %s (Bahan: %s)\n", root->deskripsi, root->bahan);
        printf("   *Pilihan: %s\n\n", root->variasi);
    }
    
    viewKategoriLainnya(root->right);
}

void viewSemuaRapih(struct MenuNode* root) {
    if (root == NULL) {
        printf("Sistem: Data menu masih kosong.\n");
        return;
    }
    
    printf("\n====================================================\n");
    printf("                  BUKU MENU KAFE                    \n");
    printf("====================================================\n\n");
    
    printf("[ KOPI ]\n----------------------------------------------------\n");
    viewKategoriSpesifik(root, "Kopi");
    printf("[ NON-KOPI ]\n----------------------------------------------------\n");
    viewKategoriSpesifik(root, "Non-Kopi");
    printf("[ MAKANAN ]\n----------------------------------------------------\n");
    viewKategoriSpesifik(root, "Makanan");
    printf("[ SNACK ]\n----------------------------------------------------\n");
    viewKategoriSpesifik(root, "Snack");
    printf("[ LAINNYA ]\n----------------------------------------------------\n");
    viewKategoriLainnya(root);
    
    printf("====================================================\n");
}

// 6. Fitur Cari Detail Menu
void searchMenu(struct MenuNode* root, const char* targetNama) {
    if (root == NULL) {
        printf("Menu '%s' tidak ditemukan di sistem kafe.\n", targetNama);
        return;
    }
    int cmp = strcmp(targetNama, root->nama);
    if (cmp == 0) {
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
    } else if (cmp < 0) {
        searchMenu(root->left, targetNama);
    } else {
        searchMenu(root->right, targetNama);
    }
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
        // Copy seluruh 7 data dari successor
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
    FILE *file = fopen("admin.txt", "a"); 
    if (file == NULL) return;

    printf("\n--- REGISTER ADMIN BARU ---\n");
    printf("Masukkan Username: ");
    scanf(" %[^\n]", username);
    printf("Masukkan Password: ");
    scanf(" %[^\n]", password);

    fprintf(file, "%s,%s\n", username, password);
    fclose(file);
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
        if (isAdmin) printf("5. Logout Admin\n");
        else printf("5. Login / Register Admin\n");
        printf("6. Keluar Aplikasi\n");
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
                printf("Masukkan ID        : "); scanf(" %[^\n]", inputID);
                printf("Masukkan Nama Menu : "); scanf(" %[^\n]", inputNama);
                printf("Masukkan Kategori  : "); scanf(" %[^\n]", inputKategori);
                printf("Masukkan Harga     : Rp "); scanf("%d", &inputHarga);
                printf("Masukkan Variasi   : "); scanf(" %[^\n]", inputVariasi);
                printf("Masukkan Bahan     : "); scanf(" %[^\n]", inputBahan);
                printf("Masukkan Deskripsi : "); scanf(" %[^\n]", inputDeskripsi);
                
                root = insertMenu(root, inputID, inputNama, inputKategori, inputHarga, inputDeskripsi, inputBahan, inputVariasi);
                printf("\nSistem: Menu baru berhasil didaftarkan!\n");
                break;
                
            case 2:
                printf("\n--- CARI DETAIL MENU ---\n");
                if (root == NULL) printf("Sistem: Data menu masih kosong.\n");
                else {
                    printf("Masukkan Nama Menu yang dicari: ");
                    scanf(" %[^\n]", inputNama);
                    searchMenu(root, inputNama);
                }
                break;
                
            case 3:
                viewSemuaRapih(root); 
                break;
                
            case 4:
                if (!isAdmin) {
                    printf("\nAKSES DITOLAK! Anda harus Login Admin.\n");
                    break;
                }
                printf("\n--- HAPUS MENU ---\n");
                if (root == NULL) printf("Sistem: Data menu masih kosong.\n");
                else {
                    printf("Masukkan Nama Menu yang akan dihapus: ");
                    scanf(" %[^\n]", inputNama);
                    root = deleteMenu(root, inputNama);
                }
                break;
                
            case 5:
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

            case 6:
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
