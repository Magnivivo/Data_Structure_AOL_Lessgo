#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 1. Struktur Data Menu Node (7 Parameter)
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
        printf("Sistem: Menu '%s' sudah terdaftar!\n", nama);
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
    fgets(line, sizeof(line), file);

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
        // Jika Admin, tampilkan ID. Jika bukan, sembunyikan ID.
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
    if (root == NULL) {
        return 0; // Mengembalikan 0 jika mencapai ujung tree dan belum ketemu
    }

    // 1. Cek apakah input COCOK dengan Nama Menu ATAU ID Menu
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
        return 1; // Mengembalikan 1 karena data berhasil ditemukan
    }

    // 2. Jika belum ketemu, telusuri cabang kiri dan kanan secara rekursif
    // Karena target bisa berupa ID, kita harus menyisir kedua cabang jika tidak ketemu leksikografis nama
    int ketemuDiKiri = searchMenu(root->left, target);
    if (ketemuDiKiri) return 1; // Jika di kiri sudah ketemu, langsung stop dan return

    return searchMenu(root->right, target); // Jika tidak, cek cabang kanan
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

    // 1. Loop Validasi Username Anti-Duplikat
    do {
        isDuplicate = 0; // Reset status setiap kali mengulang input
        printf("Masukkan Username baru: ");
        scanf(" %[^\n]", username);

        // Buka file mode 'r' (read) untuk mengecek data yang sudah ada
        FILE *fileCek = fopen("admin.txt", "r");
        if (fileCek != NULL) {
            char line[150];
            while (fgets(line, sizeof(line), fileCek)) {
                line[strcspn(line, "\r\n")] = 0; // Hilangkan enter di akhir baris
                char* tokenUser = strtok(line, ","); // Ambil username-nya saja

                // Jika username yang diketik sama dengan yang ada di file
                if (tokenUser != NULL && strcmp(username, tokenUser) == 0) {
                    isDuplicate = 1;
                    printf(" -> Error: Username '%s' sudah terdaftar! Silakan gunakan username lain.\n", username);
                    break; // Berhenti mencari karena sudah pasti duplikat
                }
            }
            fclose(fileCek); // Tutup file setelah selesai mengecek
        }
    } while (isDuplicate); // Ulangi terus jika masih duplikat

    // 2. Lanjut input password jika username dipastikan unik
    printf("Masukkan Password baru: ");
    scanf(" %[^\n]", password);

    // 3. Simpan akun baru ke file mode 'a' (append / tambah di bawah)
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

// Fungsi pembantu untuk mengecek apakah ID kembar/sudah ada di Tree (Primary Key Validation)
int isIDExist(struct MenuNode* root, const char* targetID) {
    if (root == NULL) {
        return 0; // Ujung tree tercapai, ID aman (tidak duplikat)
    }
    
    // Jika ID ditemukan kembar di node saat ini
    if (strcmp(root->id, targetID) == 0) {
        return 1; // 1 berarti True (Duplikat ditemukan!)
    }
    
    // Lanjutkan pencarian ke cabang kiri dan kanan
    int ketemuDiKiri = isIDExist(root->left, targetID);
    if (ketemuDiKiri) return 1;
    
    return isIDExist(root->right, targetID);
}

// Fungsi pembantu untuk mengecek apakah Nama Menu sudah terdaftar (Cegah Duplikat)
int isNamaExist(struct MenuNode* root, const char* targetNama) {
    if (root == NULL) {
        return 0; // Ujung tree tercapai, Nama aman (belum terdaftar)
    }
    
    int cmp = strcmp(targetNama, root->nama);
    if (cmp == 0) {
        return 1; // 1 berarti True (Nama Duplikat ditemukan!)
    } else if (cmp < 0) {
        return isNamaExist(root->left, targetNama); // Cari ke kiri
    } else {
        return isNamaExist(root->right, targetNama); // Cari ke kanan
    }
}

// Fungsi pembantu: Mencari Nama Menu jika Admin mengetikkan ID
const char* findNameByID(struct MenuNode* root, const char* targetID) {
    if (root == NULL) return NULL; // Jika tidak ketemu

    // Jika ID cocok, kembalikan nama menunya
    if (strcmp(root->id, targetID) == 0) {
        return root->nama;
    }

    // Telusuri cabang kiri
    const char* leftSearch = findNameByID(root->left, targetID);
    if (leftSearch != NULL) return leftSearch;

    // Telusuri cabang kanan
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
                
                // 1. Loop Validasi ID (Format dan Anti-Duplikat)
                int validID = 0;
                do {
                    printf("Masukkan ID        : "); 
                    scanf(" %[^\n]", inputID); 
                    
                    // Cek format awalan dan Auto-Assign Kategori
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

                    // Jika formatnya sudah benar, pastikan ID tidak kembar di dalam Tree
                    if (validID == 1) {
                        if (isIDExist(root, inputID)) {
                            printf(" -> Error: ID '%s' sudah dipakai oleh menu lain! Silakan gunakan ID berbeda.\n", inputID);
                            validID = 0; // Gagalkan validasi agar sistem meminta input ulang
                        }
                    }
                } while (!validID);

                // 2. Loop Validasi Nama (Anti-Duplikat)
                int validNama = 0;
                do {
                    printf("Masukkan Nama Menu : "); 
                    scanf(" %[^\n]", inputNama);
                    
                    // Pastikan nama menu belum pernah didaftarkan
                    if (isNamaExist(root, inputNama)) {
                        printf(" -> Error: Menu bernama '%s' sudah ada di sistem! Gunakan nama yang sedikit berbeda (misal: %s Ice).\n", inputNama, inputNama);
                    } else {
                        validNama = 1; // Nama aman, keluar dari loop
                    }
                } while (!validNama);

                // 3. Input Data Sisanya
                printf("Masukkan Harga     : Rp "); scanf("%d", &inputHarga);
                printf("Masukkan Deskripsi produk   : "); scanf(" %[^\n]", inputDeskripsi);
                printf("Masukkan Bahan utama produk : "); scanf(" %[^\n]", inputBahan);
                printf("Masukkan Variasi [JANGAN pakai koma](Contoh: (Hot & Ice)/(Hot Only)/(Ice/Hot)/Level 1-n)  : "); scanf(" %[^\n]", inputVariasi);
                
                // 4. Masukkan ke dalam Tree
                root = insertMenu(root, inputID, inputNama, inputKategori, inputHarga, inputDeskripsi, inputBahan, inputVariasi);
                
                printf("\nSistem: Menu '%s' berhasil didaftarkan ke kategori '%s'!\n", inputNama, inputKategori);
                break;
                
            case 2:
                printf("\n--- CARI DETAIL MENU ---\n");
                if (root == NULL) {
                    printf("Sistem: Data menu masih kosong.\n");
                } else {
                    // Mengubah instruksi teks agar user tahu bisa menginput dua opsi
                    char inputCari[100];
                    printf("Masukkan ID atau Nama Menu yang dicari: ");
                    scanf(" %[^\n]", inputCari);
                    
                    // Memanggil fungsi pencarian baru
                    int statusKetemu = searchMenu(root, inputCari);
                    
                    // Jika sampai akhir tree status tetap 0, tampilkan pesan error
                    if (!statusKetemu) {
                        printf("Sistem: Menu atau ID '%s' tidak ditemukan di seluruh katalog.\n", inputCari);
                    }
                }
                break;
                
            case 3:
                // Melempar status login saat ini ke dalam fungsi
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

                    // Variabel untuk menyimpan nama menu yang fix akan dihapus
                    const char* targetHapus = NULL; 

                    // 1. Cek apakah input adalah ID
                    const char* namaTarget = findNameByID(root, inputHapus);

                    if (namaTarget != NULL) {
                        printf("Sistem: Mendeteksi ID '%s' (Menu: %s)\n", inputHapus, namaTarget);
                        targetHapus = namaTarget; // Set target dari ID
                    } 
                    // 2. Cek apakah input adalah Nama Menu
                    else if (isNamaExist(root, inputHapus)) {
                        targetHapus = inputHapus; // Set target dari Nama
                    } 
                    // 3. Jika tidak ketemu sama sekali
                    else {
                        printf("Sistem: Menu atau ID '%s' tidak ditemukan untuk dihapus.\n", inputHapus);
                    }

                    // Jika target menu berhasil ditemukan, minta konfirmasi!
                    if (targetHapus != NULL) {
                        char konfirmasi;
                        printf("\n[PERINGATAN] Apakah Anda yakin ingin menghapus '%s' secara permanen? (Y/N): ", targetHapus);
                        scanf(" %c", &konfirmasi); // Spasi sebelum %c untuk membersihkan sisa enter/newline

                        if (konfirmasi == 'Y' || konfirmasi == 'y') {
                            // Eksekusi penghapusan HANYA JIKA admin menekan Y
                            root = deleteMenu(root, targetHapus);
                        } else {
                            // Batalkan jika menekan N atau tombol lainnya
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
