#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 1. Struktur Data Menu Node
struct MenuNode {
    char id[10];
    char nama[100];
    char kategori[50];
    int harga;
    struct MenuNode* left;
    struct MenuNode* right;
};

// Fungsi membuat node menu baru
struct MenuNode* createMenuNode(const char* id, const char* nama, const char* kategori, int harga) {
    struct MenuNode* newNode = (struct MenuNode*)malloc(sizeof(struct MenuNode));
    strcpy(newNode->id, id);
    strcpy(newNode->nama, nama);
    strcpy(newNode->kategori, kategori);
    newNode->harga = harga;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

// 2. Fungsi Insert BST (Berdasarkan Nama Menu -> Abjad A-Z)
struct MenuNode* insertMenu(struct MenuNode* root, const char* id, const char* nama, const char* kategori, int harga) {
    if (root == NULL) {
        return createMenuNode(id, nama, kategori, harga);
    }

    // Menggunakan strcmp untuk membandingkan string (huruf)
    int cmp = strcmp(nama, root->nama);
    
    if (cmp < 0) {
        // Jika nama menu lebih awal secara abjad, masuk ke cabang kiri
        root->left = insertMenu(root->left, id, nama, kategori, harga);
    } else if (cmp > 0) {
        // Jika nama menu lebih akhir secara abjad, masuk ke cabang kanan
        root->right = insertMenu(root->right, id, nama, kategori, harga);
    } else {
        printf("Sistem: Menu '%s' sudah terdaftar!\n", nama);
    }

    return root;
}

// 3. Fungsi File Processing (Membaca file CSV)
struct MenuNode* loadDataFromFile(struct MenuNode* root, const char* filename) {
    FILE* file = fopen(filename, "r");
    
    if (file == NULL) {
        printf("Peringatan: File '%s' tidak ditemukan. Memulai dengan data kosong.\n", filename);
        return root;
    }

    char line[256];
    fgets(line, sizeof(line), file); // Melewati baris pertama (Header CSV)

    int count = 0;
    while (fgets(line, sizeof(line), file)) {
        // Memecah teks berdasarkan koma (,) dan karakter newline (\r\n)
        char* idStr = strtok(line, ",\r\n");
        char* namaStr = strtok(NULL, ",\r\n");
        char* kategoriStr = strtok(NULL, ",\r\n");
        char* hargaStr = strtok(NULL, ",\r\n");

        if (idStr != NULL && namaStr != NULL && kategoriStr != NULL && hargaStr != NULL) {
            int hargaMenu = atoi(hargaStr); 
            root = insertMenu(root, idStr, namaStr, kategoriStr, hargaMenu);
            count++;
        }
    }

    fclose(file);
    printf("Sistem: Berhasil memuat %d data menu dari '%s'!\n", count, filename);
    return root;
}

// 4. [Menu 4] Fungsi In-Order Traversal (Menampilkan Menu Terurut Abjad)
void viewAllMenu(struct MenuNode* root) {
    if (root == NULL) return;
    
    viewAllMenu(root->left); // Telusuri yang lebih kecil secara abjad
    
    printf("| %-5s | %-25s | %-15s | Rp %-7d |\n", root->id, root->nama, root->kategori, root->harga);
    
    viewAllMenu(root->right); // Telusuri yang lebih besar secara abjad
}

// 5. [Menu 2 & 3] Fungsi Mencari Menu Spesifik
void searchMenu(struct MenuNode* root, const char* targetNama) {
    if (root == NULL) {
        printf("Menu '%s' tidak ditemukan di sistem kafe.\n", targetNama);
        return;
    }

    int cmp = strcmp(targetNama, root->nama);
    
    if (cmp == 0) {
        printf("\n--- Detail Menu Ditemukan ---\n");
        printf("ID       : %s\n", root->id);
        printf("Nama     : %s\n", root->nama);
        printf("Kategori : %s\n", root->kategori);
        printf("Harga    : Rp %d\n", root->harga);
    } else if (cmp < 0) {
        searchMenu(root->left, targetNama);
    } else {
        searchMenu(root->right, targetNama);
    }
}

// Fungsi Helper untuk Delete: Mencari node terkecil di cabang kanan
struct MenuNode* findMin(struct MenuNode* root) {
    while (root->left != NULL) root = root->left;
    return root;
}

// 6. [Menu 5] Fungsi Menghapus Menu dari BST (Bonus Poin)
struct MenuNode* deleteMenu(struct MenuNode* root, const char* targetNama) {
    if (root == NULL) {
        printf("Menu '%s' tidak ditemukan untuk dihapus.\n", targetNama);
        return root;
    }

    int cmp = strcmp(targetNama, root->nama);
    
    if (cmp < 0) {
        root->left = deleteMenu(root->left, targetNama);
    } else if (cmp > 0) {
        root->right = deleteMenu(root->right, targetNama);
    } else {
        // Menu ditemukan!
        
        // Kasus 1 & 2: Tidak ada anak atau 1 anak
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
        
        // Kasus 3: Punya 2 anak. Cari in-order successor.
        struct MenuNode* temp = findMin(root->right);
        
        // Salin data dari node temp ke node root yang ingin dihapus
        strcpy(root->id, temp->id);
        strcpy(root->nama, temp->nama);
        strcpy(root->kategori, temp->kategori);
        root->harga = temp->harga;
        
        // Hapus duplikat data di cabang kanan
        root->right = deleteMenu(root->right, temp->nama);
    }
    return root;
}

// Fungsi Bantuan: Menelusuri BST dan menulis setiap node ke dalam file
void saveToFileHelper(struct MenuNode* root, FILE* file) {
    if (root == NULL) return;
    
    saveToFileHelper(root->left, file); // Ke kiri
    
    // Tulis data node dengan format CSV (dipisahkan koma)
    fprintf(file, "%s,%s,%s,%d\n", root->id, root->nama, root->kategori, root->harga);
    
    saveToFileHelper(root->right, file); // Ke kanan
}

// Fungsi Utama Simpan File (Auto-Save)
void syncDataToFile(struct MenuNode* root, const char* filename) {
    // Menggunakan mode "w" (write) untuk menimpa file lama dengan data terbaru
    FILE* file = fopen(filename, "w"); 
    
    if (file == NULL) {
        printf("Error: Gagal menyimpan data ke file '%s'!\n", filename);
        return;
    }
    
    // 1. Tulis ulang header CSV di baris pertama
    fprintf(file, "ID_Menu,Nama_Menu,Kategori,Harga\n");
    
    // 2. Tulis seluruh isi BST ke dalam file
    saveToFileHelper(root, file);
    
    fclose(file);
    printf("Sistem: Seluruh perubahan data berhasil disimpan ke '%s' secara permanen.\n", filename);
}

// Fungsi Pembantu: Hanya mencetak menu dengan kategori tertentu
void viewKategoriSpesifik(struct MenuNode* root, const char* targetKategori) {
    if (root == NULL) return;
    
    // Telusuri cabang kiri (abjad awal)
    viewKategoriSpesifik(root->left, targetKategori);
    
    // Jika kategori node saat ini cocok dengan target, cetak datanya!
    if (strcmp(root->kategori, targetKategori) == 0) {
        printf("| %-5s | %-25s | Rp %-7d |\n", root->id, root->nama, root->harga);
    }
    
    // Telusuri cabang kanan (abjad akhir)
    viewKategoriSpesifik(root->right, targetKategori);
}

// Fungsi Baru untuk Menu 3 (Tampilkan Semua Menu Dikelompokkan)
void viewSemuaRapih(struct MenuNode* root) {
    if (root == NULL) {
        printf("Sistem: Data menu masih kosong.\n");
        return;
    }

    printf("\n======================================================\n");
    printf("                  DAFTAR MENU KAFE                    \n");
    printf("======================================================\n");

    printf("\n[ KATEGORI: Kopi ]\n");
    printf("------------------------------------------------------\n");
    viewKategoriSpesifik(root, "Kopi");

    printf("\n[ KATEGORI: Non-Kopi]\n");
    printf("------------------------------------------------------\n");
    viewKategoriSpesifik(root, "Non-Kopi");

    printf("\n[ KATEGORI: Makanan ]\n");
    printf("------------------------------------------------------\n");
    viewKategoriSpesifik(root, "Makanan");

    printf("\n[ KATEGORI: Snack ]\n");
    printf("------------------------------------------------------\n");
    viewKategoriSpesifik(root, "Snack");
    
    printf("------------------------------------------------------\n");
}

// Fungsi untuk mendaftarkan Admin baru
void registerAdmin() {
    char username[50], password[50];
    FILE *file = fopen("admin.txt", "a"); // Mode 'a' untuk menambahkan data di baris bawah

    if (file == NULL) {
        printf("Error: Tidak bisa membuka file database admin.\n");
        return;
    }

    printf("\n--- REGISTER ADMIN BARU ---\n");
    printf("Masukkan Username baru: ");
    scanf(" %[^\n]", username);
    printf("Masukkan Password baru: ");
    scanf(" %[^\n]", password);

    // Menyimpan dengan format username,password
    fprintf(file, "%s,%s\n", username, password);
    fclose(file);

    printf("Sistem: Akun Admin '%s' berhasil didaftarkan!\n", username);
}

// Fungsi untuk mengecek kredensial Login Admin
int loginAdmin() {
    char inputUser[50], inputPass[50];
    char fileUser[50], filePass[50];
    FILE *file = fopen("admin.txt", "r"); // Mode 'r' untuk membaca

    if (file == NULL) {
        printf("Sistem: Belum ada admin yang terdaftar. Silakan Register terlebih dahulu.\n");
        return 0; // Gagal login
    }

    printf("\n--- LOGIN ADMIN ---\n");
    printf("Username: ");
    scanf(" %[^\n]", inputUser);
    printf("Password: ");
    scanf(" %[^\n]", inputPass);

    char line[150];
    // Membaca baris demi baris dari admin.txt
    while (fgets(line, sizeof(line), file)) {
        // Menghilangkan newline dari file
        line[strcspn(line, "\r\n")] = 0; 
        
        // Memecah teks berdasarkan koma
        char* tokenUser = strtok(line, ",");
        char* tokenPass = strtok(NULL, ",");

        if (tokenUser != NULL && tokenPass != NULL) {
            // Jika username dan password cocok
            if (strcmp(inputUser, tokenUser) == 0 && strcmp(inputPass, tokenPass) == 0) {
                fclose(file);
                printf("Sistem: Login Berhasil! Selamat datang, %s.\n", inputUser);
                return 1; // 1 berarti True / Berhasil
            }
        }
    }

    fclose(file);
    printf("Sistem: Login Gagal! Username atau Password salah.\n");
    return 0; // 0 berarti False / Gagal
}

// === FUNGSI UTAMA PROGRAM ===
int main() {
    struct MenuNode* root = NULL;
    int pilihan;
    
    char inputID[10], inputNama[100], inputKategori[50];
    int inputHarga;

    printf("=========================================\n");
    printf("   MEMULAI SISTEM MANAJEMEN MENU KAFE    \n");
    printf("=========================================\n");
    // Otomatis memuat data dari file saat program berjalan
    root = loadDataFromFile(root, "menu_kafe.csv");
    printf("Tekan ENTER untuk masuk ke menu utama...");
    getchar();

    do {
        printf("\n=========================================\n");
        printf("        SISTEM KASIR KAFE (BST)          \n");
        printf("=========================================\n");
        printf("1. Tambah Menu Baru\n");
        printf("2. Cari Detail Menu (Search)\n");
        printf("3. Tampilkan Semua Menu (A-Z)\n");
        printf("4. Hapus Menu\n");
        printf("5. Keluar\n");
        printf("=========================================\n");
        printf("Pilih aksi [1-5]: ");
        
        if (scanf("%d", &pilihan) != 1) {
            while (getchar() != '\n');
            pilihan = 0;
        }

        switch (pilihan) {
            case 1:
                printf("\n--- TAMBAH MENU BARU ---\n");
                printf("Masukkan ID       : ");
                scanf(" %[^\n]", inputID);
                printf("Masukkan Nama Menu: ");
                scanf(" %[^\n]", inputNama);
                printf("Masukkan Kategori : ");
                scanf(" %[^\n]", inputKategori);
                printf("Masukkan Harga    : Rp ");
                scanf("%d", &inputHarga);
                
                root = insertMenu(root, inputID, inputNama, inputKategori, inputHarga);
                printf("\nSistem: Menu baru berhasil didaftarkan!\n");
                break;
                
            case 2:
                printf("\n--- CARI MENU ---\n");
                if (root == NULL) {
                    printf("Sistem: Data menu masih kosong.\n");
                } else {
                    printf("Masukkan Nama Menu yang dicari: ");
                    scanf(" %[^\n]", inputNama);
                    searchMenu(root, inputNama);
                }
                break;
                
            case 3:
                // Menggunakan tampilan yang sudah dikelompokkan per kategori
                viewSemuaRapih(root);
                break;
                
            case 4:
                printf("\n--- HAPUS MENU ---\n");
                if (root == NULL) {
                    printf("Sistem: Data menu masih kosong.\n");
                } else {
                    printf("Masukkan Nama Menu yang akan dihapus: ");
                    scanf(" %[^\n]", inputNama);
                    root = deleteMenu(root, inputNama);
                }
                break;
                
            case 5:
                printf("\n--- MENYIMPAN DATA ---\n");
                // Sinkronisasi data dari memori (BST) ke file CSV sebelum keluar
                syncDataToFile(root, "menu_kafe.csv");
                printf("Terima kasih. Sistem dimatikan.\n");
                break;
                
            default:
                if (pilihan != 0) printf("Pilihan tidak valid!\n");
        }
        
        if (pilihan != 5 && pilihan != 0) {
            printf("\nTekan ENTER untuk melanjutkan...");
            while (getchar() != '\n'); // Membersihkan buffer
            getchar(); // Menunggu enter
        }
        
    } while (pilihan != 5);

    return 0;
}
