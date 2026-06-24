#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Fungsi Case-Insensitive
int isMatchIgnoreCase(const char *str1, const char *str2) {
  while (*str1 && *str2) {
    if (tolower((unsigned char)*str1) != tolower((unsigned char)*str2)) {
      return 0;
    }
    str1++;
    str2++;
  }
  return (*str1 == *str2);
}

// Data isi Node menu -
struct MenuNode {
  char id[10];
  char nama[100];
  char kategori[50];
  int harga;
  char deskripsi[200];
  char bahan[200];
  char variasi[50];
  struct MenuNode *left;
  struct MenuNode *right;
};

// Fungsi membuat node menu baru
struct MenuNode *createMenuNode(const char *id, const char *nama,
                                const char *kategori, int harga,
                                const char *deskripsi, const char *bahan,
                                const char *variasi) {
  struct MenuNode *newNode = (struct MenuNode *)malloc(sizeof(struct MenuNode));
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

// Fungsi Cek Duplikasi
int isDuplicateName(struct MenuNode *root, const char *nama) {
  if (root == NULL)
    return 0;
  int cmp = strcmp(nama, root->nama);
  if (cmp == 0)
    return 1;
  if (cmp < 0)
    return isDuplicateName(root->left, nama);
  return isDuplicateName(root->right, nama);
}

int isDuplicateID(struct MenuNode *root, const char *id) {
  if (root == NULL)
    return 0;
  if (strcmp(root->id, id) == 0)
    return 1;
  return isDuplicateID(root->left, id) || isDuplicateID(root->right, id);
}

// 2. Fungsi Insert BST -
struct MenuNode *insertMenu(struct MenuNode *root, const char *id,
                            const char *nama, const char *kategori, int harga,
                            const char *deskripsi, const char *bahan,
                            const char *variasi) {
  if (root == NULL) {
    return createMenuNode(id, nama, kategori, harga, deskripsi, bahan, variasi);
  }

  int cmp = strcmp(nama, root->nama);
  if (cmp < 0) {
    root->left = insertMenu(root->left, id, nama, kategori, harga, deskripsi,
                            bahan, variasi);
  } else if (cmp > 0) {
    root->right = insertMenu(root->right, id, nama, kategori, harga, deskripsi,
                             bahan, variasi);
  } else {
    printf("Sistem: Menu '%s' sudah ditambahkan!\n", nama);
  }
  return root;
}

struct MenuNode *loadDataFromFile(struct MenuNode *root, const char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    printf(
        "Peringatan: File '%s' tidak ditemukan. Memulai dengan data kosong.\n",
        filename);
    return root;
  }

  char line[500];
  fgets(line, sizeof(line), file); // Skip header CSV

  int count = 0;
  while (fgets(line, sizeof(line), file)) {
    char *idStr = strtok(line, ",\r\n");
    char *namaStr = strtok(NULL, ",\r\n");
    char *kategoriStr = strtok(NULL, ",\r\n");
    char *hargaStr = strtok(NULL, ",\r\n");
    char *descStr = strtok(NULL, ",\r\n");
    char *bahanStr = strtok(NULL, ",\r\n");
    char *varStr = strtok(NULL, ",\r\n");

    if (idStr && namaStr && kategoriStr && hargaStr && descStr && bahanStr &&
        varStr) {
      int hargaMenu = atoi(hargaStr);
      root = insertMenu(root, idStr, namaStr, kategoriStr, hargaMenu, descStr,
                        bahanStr, varStr);
      count++;
    }
  }
  fclose(file);
  printf("Sistem: Berhasil memuat %d data menu dari '%s'!\n", count, filename);
  return root;
}

// 4. Fungsi File Processing (Auto-Save)
void saveToFileHelper(struct MenuNode *root, FILE *file) {
  if (root == NULL)
    return;
  saveToFileHelper(root->left, file);
  fprintf(file, "%s,%s,%s,%d,%s,%s,%s\n", root->id, root->nama, root->kategori,
          root->harga, root->deskripsi, root->bahan, root->variasi);
  saveToFileHelper(root->right, file);
}

void syncDataToFile(struct MenuNode *root, const char *filename) {
  FILE *file = fopen(filename, "w");
  if (file == NULL) {
    printf("Error: Gagal menyimpan data ke file '%s'!\n", filename);
    return;
  }
  fprintf(file,
          "ID_Menu,Nama_Menu,Kategori,Harga,Deskripsi,Bahan_Dasar,Variasi\n");
  saveToFileHelper(root, file);
  fclose(file);
}

void viewKategoriSpesifik(struct MenuNode *root, const char *targetKategori) {
  if (root == NULL)
    return;

  viewKategoriSpesifik(root->left, targetKategori);

  if (strcmp(root->kategori, targetKategori) == 0) {
    printf(">> %-35s Rp %d\n", root->nama, root->harga);
    printf("   %s (Bahan: %s)\n", root->deskripsi, root->bahan);
    printf("   *Pilihan: %s\n\n", root->variasi);
  }

  viewKategoriSpesifik(root->right, targetKategori);
}

void viewKategoriLainnya(struct MenuNode *root) {
  if (root == NULL)
    return;

  viewKategoriLainnya(root->left);

  if (strcmp(root->kategori, "Kopi") != 0 &&
      strcmp(root->kategori, "Non-Kopi") != 0 &&
      strcmp(root->kategori, "Makanan") != 0 &&
      strcmp(root->kategori, "Snack") != 0) {

    printf(">> %-35s Rp %d [Kategori: %s]\n", root->nama, root->harga,
           root->kategori);
    printf("   %s (Bahan: %s)\n", root->deskripsi, root->bahan);
    printf("   *Pilihan: %s\n\n", root->variasi);
  }

  viewKategoriLainnya(root->right);
}

void viewSemuaRapih(struct MenuNode *root) {
  if (root == NULL) {
    printf("Sistem: Data menu masih kosong.\n");
    return;
  }

  printf("\n====================================================\n");
  printf("                  BUKU MENU KAFE                    \n");
  printf("====================================================\n\n");

  printf("[ KOPI ]\n----------------------------------------------------\n");
  viewKategoriSpesifik(root, "Kopi");
  printf(
      "[ NON-KOPI ]\n----------------------------------------------------\n");
  viewKategoriSpesifik(root, "Non-Kopi");
  printf("[ MAKANAN ]\n----------------------------------------------------\n");
  viewKategoriSpesifik(root, "Makanan");
  printf("[ SNACK ]\n----------------------------------------------------\n");
  viewKategoriSpesifik(root, "Snack");
  printf("[ LAINNYA ]\n----------------------------------------------------\n");
  viewKategoriLainnya(root);

  printf("====================================================\n");
}

void searchMenu(struct MenuNode *root, const char *target, int *found) {
  if (root == NULL)
    return;

  if (isMatchIgnoreCase(target, root->nama) ||
      isMatchIgnoreCase(target, root->id)) {
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
    *found = 1;
  }

  // Lanjutkan penelusuran ke seluruh node karena ID/Nama mungkin di mana saja
  searchMenu(root->left, target, found);
  searchMenu(root->right, target, found);
}

struct MenuNode *findMin(struct MenuNode *root) {
  while (root->left != NULL)
    root = root->left;
  return root;
}

struct MenuNode *deleteMenu(struct MenuNode *root, const char *targetNama) {
  if (root == NULL) {
    printf("Menu '%s' tidak ditemukan untuk dihapus.\n", targetNama);
    return root;
  }
  int cmp = strcmp(targetNama, root->nama);
  if (cmp < 0)
    root->left = deleteMenu(root->left, targetNama);
  else if (cmp > 0)
    root->right = deleteMenu(root->right, targetNama);
  else {
    if (root->left == NULL) {
      struct MenuNode *temp = root->right;
      free(root);
      printf("Sistem: Menu berhasil dihapus.\n");
      return temp;
    } else if (root->right == NULL) {
      struct MenuNode *temp = root->left;
      free(root);
      printf("Sistem: Menu berhasil dihapus.\n");
      return temp;
    }
    struct MenuNode *temp = findMin(root->right);
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

void registerAdmin() {
  char username[50], password[50];
  FILE *file = fopen("admin.txt", "a");
  if (file == NULL) {
    printf("Sistem: Gagal membuat file admin.txt!\n");
    return;
  }

  printf("\n--- REGISTER ADMIN BARU ---\n");
  printf("Masukkan Username: ");
  scanf(" %49[^\n]", username);
  printf("Masukkan Password: ");
  scanf(" %49[^\n]", password);

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
  scanf(" %49[^\n]", inputUser);
  printf("Password: ");
  scanf(" %49[^\n]", inputPass);

  char line[150];
  while (fgets(line, sizeof(line), file)) {
    line[strcspn(line, "\r\n")] = 0;
    char *tokenUser = strtok(line, ",");
    char *tokenPass = strtok(NULL, ",");

    if (tokenUser && tokenPass && strcmp(inputUser, tokenUser) == 0 &&
        strcmp(inputPass, tokenPass) == 0) {
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
  struct MenuNode *root = NULL;
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
    if (isAdmin)
      printf("  [ STATUS: ADMIN AKTIF ]\n");
    else
      printf("  [ STATUS: KASIR/TAMU ]\n");
    printf("-----------------------------------------\n");
    printf("1. Tambah Menu Baru  [Butuh Akses Admin]\n");
    printf("2. Cari Detail Menu\n");
    printf("3. Tampilkan Semua Menu (Buku Menu)\n");
    printf("4. Hapus Menu        [Butuh Akses Admin]\n");
    if (isAdmin)
      printf("5. Logout Admin\n");
    else
      printf("5. Login / Register Admin\n");
    printf("6. Keluar Aplikasi\n");
    printf("=========================================\n");
    printf("Pilih aksi: ");

    if (scanf("%d", &pilihan) != 1) {
      while (getchar() != '\n')
        ;
      pilihan = 0;
    }

    switch (pilihan) {
    case 1:
      if (!isAdmin) {
        printf("\nAKSES DITOLAK! Anda harus Login Admin.\n");
        break;
      }
      printf("\n--- TAMBAH MENU BARU ---\n");
      printf("Masukkan ID        : ");
      scanf(" %9[^\n]", inputID);

      if (isDuplicateID(root, inputID)) {
        printf("Sistem: Gagal! ID '%s' sudah terdaftar dalam sistem.\n",
               inputID);
        break;
      }

      printf("Masukkan Nama Menu : ");
      scanf(" %99[^\n]", inputNama);

      if (isDuplicateName(root, inputNama)) {
        printf("Sistem: Gagal! Nama Menu '%s' sudah terdaftar dalam sistem.\n",
               inputNama);
        break;
      }
      printf("Masukkan Kategori  : ");
      scanf(" %49[^\n]", inputKategori);
      printf("Masukkan Harga     : Rp ");
      while (scanf("%d", &inputHarga) != 1) {
        printf("Sistem: Harga harus berupa angka! Silakan coba lagi.\n");
        while (getchar() != '\n')
          ;
        printf("Masukkan Harga     : Rp ");
      }
      printf("Masukkan Variasi   : ");
      scanf(" %49[^\n]", inputVariasi);
      printf("Masukkan Bahan     : ");
      scanf(" %199[^\n]", inputBahan);
      printf("Masukkan Deskripsi : ");
      scanf(" %199[^\n]", inputDeskripsi);

      root = insertMenu(root, inputID, inputNama, inputKategori, inputHarga,
                        inputDeskripsi, inputBahan, inputVariasi);
      printf("\nSistem: Menu baru berhasil didaftarkan!\n");
      break;

    case 2:
      printf("\n--- CARI DETAIL MENU ---\n");
      if (root == NULL)
        printf("Sistem: Data menu masih kosong.\n");
      else {
        printf("Masukkan ID atau Nama Menu yang dicari: ");
        scanf(" %99[^\n]", inputNama);
        int found = 0;
        searchMenu(root, inputNama, &found);
        if (!found) {
          printf("Sistem: Menu dengan keyword '%s' tidak ditemukan di sistem "
                 "kafe.\n",
                 inputNama);
        }
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
      if (root == NULL)
        printf("Sistem: Data menu masih kosong.\n");
      else {
        printf("Masukkan Nama Menu yang akan dihapus: ");
        scanf(" %99[^\n]", inputNama);

        char konfirmasi;
        printf("Apakah Anda yakin ingin menghapus menu '%s'? (Y/N): ",
               inputNama);
        scanf(" %c", &konfirmasi);

        if (konfirmasi == 'Y' || konfirmasi == 'y') {
          root = deleteMenu(root, inputNama);
        } else {
          printf("Sistem: Penghapusan dibatalkan.\n");
        }
      }
      break;

    case 5:
      if (isAdmin) {
        isAdmin = 0;
        printf("\nSistem: Berhasil Logout.\n");
      } else {
        int authPilihan;
        printf("\n--- PORTAL ADMIN ---\n1. Login\n2. Register Admin "
               "Baru\nPilih: ");
        scanf("%d", &authPilihan);
        if (authPilihan == 1)
          isAdmin = loginAdmin();
        else if (authPilihan == 2)
          registerAdmin();
      }
      break;

    case 6:
      printf("\n--- MENYIMPAN DATA ---\n");
      syncDataToFile(root, "menu_kafe.csv");
      printf("Terima kasih. Sistem dimatikan.\n");
      break;

    default:
      if (pilihan != 0)
        printf("Pilihan tidak valid!\n");
    }
    if (pilihan != 6 && pilihan != 0) {
      printf("\nTekan ENTER untuk melanjutkan...");
      while (getchar() != '\n')
        ;
      getchar();
    }
  } while (pilihan != 6);

  return 0;
}
