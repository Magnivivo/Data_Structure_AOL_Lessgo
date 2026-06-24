#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// === DEFINISI KONSTANTA ===
#define MAX_MENU_ITEMS 300

// 1. Struktur Data Menu Node
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

// === FORWARD DECLARATIONS ===
void syncDataToFile(struct MenuNode *root, const char *filename);

// === FUNGSI UTILITAS ===

// Cek apakah string kosong atau hanya berisi spasi
int isEmptyOrWhitespace(const char *str) {
  if (str == NULL || str[0] == '\0')
    return 1;
  int i;
  for (i = 0; str[i] != '\0'; i++) {
    if (str[i] != ' ' && str[i] != '\t')
      return 0;
  }
  return 1;
}

// Cek apakah string mengandung tanda koma
int containsComma(const char *str) {
  if (str == NULL)
    return 0;
  return strchr(str, ',') != NULL;
}

// Membersihkan sisa karakter di input buffer (stdin)
void clearInputBuffer() {
  int c;
  while ((c = getchar()) != '\n' && c != EOF)
    ;
}

// Validasi format ID: prefix (K/NK/M/S) + minimal 1 digit angka
// Mengembalikan 1 jika valid, 0 jika tidak. Mengisi outKategori jika valid.
int isValidIDFormat(const char *id, char *outKategori) {
  int prefixLen = 0;

  if (strncmp(id, "NK", 2) == 0) {
    prefixLen = 2;
    strcpy(outKategori, "Non-Kopi");
  } else if (id[0] == 'K') {
    prefixLen = 1;
    strcpy(outKategori, "Kopi");
  } else if (id[0] == 'M') {
    prefixLen = 1;
    strcpy(outKategori, "Makanan");
  } else if (id[0] == 'S') {
    prefixLen = 1;
    strcpy(outKategori, "Snack");
  } else {
    return 0;
  }

  // Harus ada minimal 1 karakter setelah prefix
  if (id[prefixLen] == '\0')
    return 0;

  // Semua karakter setelah prefix harus digit
  int i;
  for (i = prefixLen; id[i] != '\0'; i++) {
    if (!isdigit((unsigned char)id[i]))
      return 0;
  }

  return 1;
}

// Fungsi membuat node menu baru (dengan malloc NULL check & strncpy)
struct MenuNode *createMenuNode(const char *id, const char *nama,
                                const char *kategori, int harga,
                                const char *deskripsi, const char *bahan,
                                const char *variasi) {
  struct MenuNode *newNode = (struct MenuNode *)malloc(sizeof(struct MenuNode));
  if (newNode == NULL) {
    printf("Error: Gagal mengalokasikan memori untuk menu baru!\n");
    return NULL;
  }
  strncpy(newNode->id, id, sizeof(newNode->id) - 1);
  newNode->id[sizeof(newNode->id) - 1] = '\0';
  strncpy(newNode->nama, nama, sizeof(newNode->nama) - 1);
  newNode->nama[sizeof(newNode->nama) - 1] = '\0';
  strncpy(newNode->kategori, kategori, sizeof(newNode->kategori) - 1);
  newNode->kategori[sizeof(newNode->kategori) - 1] = '\0';
  newNode->harga = harga;
  strncpy(newNode->deskripsi, deskripsi, sizeof(newNode->deskripsi) - 1);
  newNode->deskripsi[sizeof(newNode->deskripsi) - 1] = '\0';
  strncpy(newNode->bahan, bahan, sizeof(newNode->bahan) - 1);
  newNode->bahan[sizeof(newNode->bahan) - 1] = '\0';
  strncpy(newNode->variasi, variasi, sizeof(newNode->variasi) - 1);
  newNode->variasi[sizeof(newNode->variasi) - 1] = '\0';
  newNode->left = NULL;
  newNode->right = NULL;
  return newNode;
}

// 2. Fungsi Insert BST (sorted by nama)
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

// 3. Fungsi File Processing (Load)
struct MenuNode *loadDataFromFile(struct MenuNode *root, const char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    printf(
        "Peringatan: File '%s' tidak ditemukan. Memulai dengan data kosong.\n",
        filename);
    return root;
  }

  char line[1024];
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

// 5. Fitur Tampilan Buku Menu Kafe — Optimized single traversal O(n)
//    (Sebelumnya: 5x full traversal = O(5n). Sekarang: 1x collect + iterasi
//    array)
// Menghitung jumlah node dalam tree
int countNodes(struct MenuNode *root) {
  if (root == NULL)
    return 0;
  return 1 + countNodes(root->left) + countNodes(root->right);
}

void collectNodes(struct MenuNode *root, struct MenuNode **arr, int *count) {
  if (root == NULL)
    return;
  collectNodes(root->left, arr, count);
  arr[(*count)++] = root;
  collectNodes(root->right, arr, count);
}

void printMenuItem(struct MenuNode *node, int isAdmin) {
  if (isAdmin) {
    printf(">> [%-4s] %-30s Rp %d\n", node->id, node->nama, node->harga);
  } else {
    printf(">> %-35s Rp %d\n", node->nama, node->harga);
  }
  printf("   %s (Bahan: %s)\n", node->deskripsi, node->bahan);
  printf("   *Pilihan: %s\n\n", node->variasi);
}

void viewSemuaRapih(struct MenuNode *root, int isAdmin) {
  if (root == NULL) {
    printf("Sistem: Data menu masih kosong.\n");
    return;
  }

  // Collect semua node secara dinamis dalam 1x traversal
  int totalItems = countNodes(root);
  struct MenuNode **allItems =
      (struct MenuNode **)malloc(sizeof(struct MenuNode *) * totalItems);
  if (allItems == NULL) {
    printf("Error: Gagal mengalokasikan memori untuk menampilkan menu!\n");
    return;
  }
  int totalItemsCollected = 0;
  collectNodes(root, allItems, &totalItemsCollected);

  printf("\n====================================================\n");
  printf("                  BUKU MENU KAFE                    \n");
  printf("====================================================\n\n");

  const char *categories[] = {"Kopi", "Non-Kopi", "Makanan", "Snack"};
  const char *headers[] = {"[ KOPI ]", "[ NON-KOPI ]", "[ MAKANAN ]",
                           "[ SNACK ]"};
  int i, c;

  // Cetak 4 kategori utama
  for (c = 0; c < 4; c++) {
    printf("%s\n----------------------------------------------------\n",
           headers[c]);
    for (i = 0; i < totalItems; i++) {
      if (strcmp(allItems[i]->kategori, categories[c]) == 0) {
        printMenuItem(allItems[i], isAdmin);
      }
    }
  }

  // Cetak kategori lainnya (jika ada)
  printf("[ LAINNYA ]\n----------------------------------------------------\n");
  for (i = 0; i < totalItems; i++) {
    int isKnownCategory = 0;
    for (c = 0; c < 4; c++) {
      if (strcmp(allItems[i]->kategori, categories[c]) == 0) {
        isKnownCategory = 1;
        break;
      }
    }
    if (!isKnownCategory) {
      if (isAdmin) {
        printf(">> [%-4s] %-30s Rp %d [Kategori: %s]\n", allItems[i]->id,
               allItems[i]->nama, allItems[i]->harga, allItems[i]->kategori);
      } else {
        printf(">> %-35s Rp %d [Kategori: %s]\n", allItems[i]->nama,
               allItems[i]->harga, allItems[i]->kategori);
      }
      printf("   %s (Bahan: %s)\n", allItems[i]->deskripsi, allItems[i]->bahan);
      printf("   *Pilihan: %s\n\n", allItems[i]->variasi);
    }
  }

  free(allItems);
  printf("====================================================\n");
}

// 6. Fitur Cari Detail Menu — BST-efficient search by nama O(log n)
//    + fallback search by ID O(n)
struct MenuNode *searchByNama(struct MenuNode *root, const char *nama) {
  if (root == NULL)
    return NULL;
  int cmp = strcmp(nama, root->nama);
  if (cmp == 0)
    return root;
  else if (cmp < 0)
    return searchByNama(root->left, nama);
  else
    return searchByNama(root->right, nama);
}

struct MenuNode *searchByID(struct MenuNode *root, const char *id) {
  if (root == NULL)
    return NULL;
  if (strcmp(root->id, id) == 0)
    return root;
  struct MenuNode *found = searchByID(root->left, id);
  if (found)
    return found;
  return searchByID(root->right, id);
}

void displayMenuDetail(struct MenuNode *node) {
  printf("\n======= DETAIL MENU DITEMUKAN =======\n");
  printf("ID Menu     : %s\n", node->id);
  printf("Nama Menu   : %s\n", node->nama);
  printf("Kategori    : %s\n", node->kategori);
  printf("Variasi     : %s\n", node->variasi);
  printf("Harga       : Rp %d\n", node->harga);
  printf("-------------------------------------\n");
  printf("Bahan Dasar : %s\n", node->bahan);
  printf("Deskripsi   : %s\n", node->deskripsi);
  printf("=====================================\n");
}

// 7. Fitur Hapus Menu — Fixed BST delete with dangling pointer prevention
struct MenuNode *findMin(struct MenuNode *root) {
  while (root && root->left)
    root = root->left;
  return root;
}

struct MenuNode *deleteMenu(struct MenuNode *root, const char *targetNama,
                            int *deleted) {
  if (root == NULL)
    return NULL;

  int cmp = strcmp(targetNama, root->nama);
  if (cmp < 0) {
    root->left = deleteMenu(root->left, targetNama, deleted);
  } else if (cmp > 0) {
    root->right = deleteMenu(root->right, targetNama, deleted);
  } else {
    // Node ditemukan
    *deleted = 1;

    // Kasus 1: Tidak punya anak, atau punya 1 anak
    if (root->left == NULL) {
      struct MenuNode *temp = root->right;
      free(root);
      return temp;
    } else if (root->right == NULL) {
      struct MenuNode *temp = root->left;
      free(root);
      return temp;
    }

    // Kasus 2: Punya 2 anak — gunakan in-order successor
    struct MenuNode *successor = findMin(root->right);

    // Simpan nama successor ke buffer lokal SEBELUM copy (fix dangling pointer)
    char namaSuccessor[100];
    strncpy(namaSuccessor, successor->nama, sizeof(namaSuccessor) - 1);
    namaSuccessor[sizeof(namaSuccessor) - 1] = '\0';

    // Copy semua field dari successor ke node saat ini (pakai strncpy)
    strncpy(root->id, successor->id, sizeof(root->id) - 1);
    root->id[sizeof(root->id) - 1] = '\0';
    strncpy(root->nama, successor->nama, sizeof(root->nama) - 1);
    root->nama[sizeof(root->nama) - 1] = '\0';
    strncpy(root->kategori, successor->kategori, sizeof(root->kategori) - 1);
    root->kategori[sizeof(root->kategori) - 1] = '\0';
    root->harga = successor->harga;
    strncpy(root->deskripsi, successor->deskripsi, sizeof(root->deskripsi) - 1);
    root->deskripsi[sizeof(root->deskripsi) - 1] = '\0';
    strncpy(root->bahan, successor->bahan, sizeof(root->bahan) - 1);
    root->bahan[sizeof(root->bahan) - 1] = '\0';
    strncpy(root->variasi, successor->variasi, sizeof(root->variasi) - 1);
    root->variasi[sizeof(root->variasi) - 1] = '\0';

    // Hapus successor menggunakan buffer lokal (bukan pointer ke node yang
    // sudah di-copy)
    root->right = deleteMenu(root->right, namaSuccessor, deleted);
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
    scanf(" %49[^\n]", username);

    if (isEmptyOrWhitespace(username)) {
      printf(" -> Error: Username tidak boleh kosong!\n");
      isDuplicate = 1;
      continue;
    }

    if (containsComma(username)) {
      printf(" -> Error: Username tidak boleh mengandung tanda koma (',')!\n");
      isDuplicate = 1;
      continue;
    }

    FILE *fileCek = fopen("admin.txt", "r");
    if (fileCek != NULL) {
      char line[150];
      while (fgets(line, sizeof(line), fileCek)) {
        line[strcspn(line, "\r\n")] = 0;
        char *tokenUser = strtok(line, ",");

        if (tokenUser != NULL && strcmp(username, tokenUser) == 0) {
          isDuplicate = 1;
          printf(" -> Error: Username '%s' sudah terdaftar! Silakan gunakan "
                 "username lain.\n",
                 username);
          break;
        }
      }
      fclose(fileCek);
    }
  } while (isDuplicate);

  do {
    printf("Masukkan Password baru: ");
    scanf(" %49[^\n]", password);
    if (isEmptyOrWhitespace(password)) {
      printf(" -> Error: Password tidak boleh kosong!\n");
    } else if (containsComma(password)) {
      printf(" -> Error: Password tidak boleh mengandung tanda koma (',')!\n");
    }
  } while (isEmptyOrWhitespace(password) || containsComma(password));

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

// Fungsi pembantu: Mengecek apakah ID kembar (O(n) — expected, ID bukan BST
// key)
int isIDExist(struct MenuNode *root, const char *targetID) {
  if (root == NULL)
    return 0;
  if (strcmp(root->id, targetID) == 0)
    return 1;

  int ketemuDiKiri = isIDExist(root->left, targetID);
  if (ketemuDiKiri)
    return 1;

  return isIDExist(root->right, targetID);
}

// Fungsi pembantu: Mengecek apakah Nama Menu kembar (O(log n) — memanfaatkan
// BST ordering)
int isNamaExist(struct MenuNode *root, const char *targetNama) {
  if (root == NULL)
    return 0;

  int cmp = strcmp(targetNama, root->nama);
  if (cmp == 0)
    return 1;
  else if (cmp < 0)
    return isNamaExist(root->left, targetNama);
  else
    return isNamaExist(root->right, targetNama);
}

// Fungsi pembantu: Mencari Nama Menu berdasarkan ID (O(n) — expected)
const char *findNameByID(struct MenuNode *root, const char *targetID) {
  if (root == NULL)
    return NULL;

  if (strcmp(root->id, targetID) == 0)
    return root->nama;

  const char *leftSearch = findNameByID(root->left, targetID);
  if (leftSearch != NULL)
    return leftSearch;

  return findNameByID(root->right, targetID);
}

// Fungsi membebaskan seluruh tree dari memori
void freeTree(struct MenuNode *root) {
  if (root == NULL)
    return;
  freeTree(root->left);
  freeTree(root->right);
  free(root);
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
      printf("8. Logout Admin\n");
    else
      printf("8. Login / Register Admin\n");
    printf("9. Keluar Aplikasi\n");
    printf("=========================================\n");
    printf("Pilih aksi: ");

    if (scanf("%d", &pilihan) != 1) {
      while (getchar() != '\n')
        ;
      pilihan = 0;
    }

    switch (pilihan) {
    case 1: {
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

      // Validasi ID: format prefix+angka, tidak duplikat
      int validID = 0;
      do {
        printf("Masukkan ID        : ");
        scanf(" %9[^\n]", inputID);

        if (!isValidIDFormat(inputID, inputKategori)) {
          printf(" -> Error: Format ID salah! Gunakan awalan (K/NK/M/S) "
                 "diikuti angka. Contoh: K11, NK03\n");
          continue;
        }

        if (isIDExist(root, inputID)) {
          printf(" -> Error: ID '%s' sudah dipakai oleh menu lain! Silakan "
                 "gunakan ID berbeda.\n",
                 inputID);
        } else {
          validID = 1;
        }
      } while (!validID);

      // Validasi Nama: tidak kosong, tidak duplikat, tidak ada koma
      int validNama = 0;
      do {
        printf("Masukkan Nama Menu : ");
        scanf(" %99[^\n]", inputNama);

        if (isEmptyOrWhitespace(inputNama)) {
          printf(" -> Error: Nama menu tidak boleh kosong!\n");
          continue;
        }

        if (containsComma(inputNama)) {
          printf(" -> Error: Nama menu tidak boleh mengandung tanda koma "
                 "(',')!\n");
          continue;
        }

        if (isNamaExist(root, inputNama)) {
          printf(" -> Error: Menu bernama '%s' sudah ada di sistem! Gunakan "
                 "nama yang sedikit berbeda (misal: %s Ice).\n",
                 inputNama, inputNama);
        } else {
          validNama = 1;
        }
      } while (!validNama);

      // Validasi Harga: harus bilangan positif
      int validHarga = 0;
      do {
        printf("Masukkan Harga     : Rp ");
        if (scanf("%d", &inputHarga) != 1) {
          printf(" -> Error: Harga harus berupa angka!\n");
          clearInputBuffer();
          continue;
        }
        clearInputBuffer();
        if (inputHarga <= 0) {
          printf(" -> Error: Harga harus lebih dari 0!\n");
          continue;
        }
        validHarga = 1;
      } while (!validHarga);

      // Validasi Deskripsi: tidak boleh kosong, tidak ada koma
      do {
        printf("Masukkan Deskripsi produk   : ");
        scanf(" %199[^\n]", inputDeskripsi);
        if (isEmptyOrWhitespace(inputDeskripsi)) {
          printf(" -> Error: Deskripsi tidak boleh kosong!\n");
        } else if (containsComma(inputDeskripsi)) {
          printf(" -> Error: Deskripsi tidak boleh mengandung tanda koma "
                 "(',')!\n");
        }
      } while (isEmptyOrWhitespace(inputDeskripsi) ||
               containsComma(inputDeskripsi));

      // Validasi Bahan: tidak boleh kosong, tidak ada koma
      do {
        printf("Masukkan Bahan utama produk : ");
        scanf(" %199[^\n]", inputBahan);
        if (isEmptyOrWhitespace(inputBahan)) {
          printf(" -> Error: Bahan tidak boleh kosong!\n");
        } else if (containsComma(inputBahan)) {
          printf(" -> Error: Bahan tidak boleh mengandung tanda koma (',')!\n");
        }
      } while (isEmptyOrWhitespace(inputBahan) || containsComma(inputBahan));

      // Validasi Variasi: tidak boleh kosong, tidak ada koma
      do {
        printf("Masukkan Variasi [JANGAN pakai koma](Contoh: (Hot & Ice)/(Hot "
               "Only)/(Ice/Hot)/Level 1-n)  : ");
        scanf(" %49[^\n]", inputVariasi);
        if (isEmptyOrWhitespace(inputVariasi)) {
          printf(" -> Error: Variasi tidak boleh kosong!\n");
        } else if (containsComma(inputVariasi)) {
          printf(
              " -> Error: Variasi tidak boleh mengandung tanda koma (',')!\n");
        }
      } while (isEmptyOrWhitespace(inputVariasi) ||
               containsComma(inputVariasi));

      root = insertMenu(root, inputID, inputNama, inputKategori, inputHarga,
                        inputDeskripsi, inputBahan, inputVariasi);
      syncDataToFile(root, "menu_kafe.csv"); // Auto-save setelah insert

      printf("\nSistem: Menu '%s' berhasil didaftarkan ke kategori '%s'!\n",
             inputNama, inputKategori);
      break;
    }

    case 2: {
      printf("\n--- CARI DETAIL MENU ---\n");
      if (root == NULL) {
        printf("Sistem: Data menu masih kosong.\n");
      } else {
        char inputCari[100];
        printf("Masukkan ID atau Nama Menu yang dicari: ");
        scanf(" %99[^\n]", inputCari);

        // Coba BST search by nama dulu — O(log n)
        struct MenuNode *found = searchByNama(root, inputCari);

        // Fallback ke search by ID — O(n)
        if (found == NULL) {
          found = searchByID(root, inputCari);
        }

        if (found != NULL) {
          displayMenuDetail(found);
        } else {
          printf(
              "Sistem: Menu atau ID '%s' tidak ditemukan di seluruh katalog.\n",
              inputCari);
        }
      }
      break;
    }

    case 3:
      viewSemuaRapih(root, isAdmin);
      break;

    case 4: {
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
        scanf(" %99[^\n]", inputHapus);

        // Resolve target nama — simpan ke buffer lokal (hindari dangling
        // pointer)
        char targetNamaBuf[100];
        int targetFound = 0;

        const char *namaTarget = findNameByID(root, inputHapus);
        if (namaTarget != NULL) {
          printf("Sistem: Mendeteksi ID '%s' (Menu: %s)\n", inputHapus,
                 namaTarget);
          strncpy(targetNamaBuf, namaTarget, sizeof(targetNamaBuf) - 1);
          targetNamaBuf[sizeof(targetNamaBuf) - 1] = '\0';
          targetFound = 1;
        } else if (isNamaExist(root, inputHapus)) {
          strncpy(targetNamaBuf, inputHapus, sizeof(targetNamaBuf) - 1);
          targetNamaBuf[sizeof(targetNamaBuf) - 1] = '\0';
          targetFound = 1;
        } else {
          printf("Sistem: Menu atau ID '%s' tidak ditemukan untuk dihapus.\n",
                 inputHapus);
        }

        if (targetFound) {
          char konfirmasi;
          printf("\n[PERINGATAN] Apakah Anda yakin ingin menghapus '%s' secara "
                 "permanen? (Y/N): ",
                 targetNamaBuf);
          scanf(" %c", &konfirmasi);

          if (konfirmasi == 'Y' || konfirmasi == 'y') {
            int deleted = 0;
            root = deleteMenu(root, targetNamaBuf, &deleted);
            if (deleted) {
              printf("Sistem: Menu '%s' berhasil dihapus.\n", targetNamaBuf);
              syncDataToFile(root, "menu_kafe.csv"); // Auto-save setelah delete
            } else {
              printf("Sistem: Gagal menghapus menu '%s'.\n", targetNamaBuf);
            }
          } else {
            printf("Sistem: Penghapusan dibatalkan. Data tetap aman.\n");
          }
        }
      }
      break;
    }

    case 8: {
      if (isAdmin) {
        isAdmin = 0;
        printf("\nSistem: Berhasil Logout.\n");
      } else {
        int authPilihan;
        printf("\n--- PORTAL ADMIN ---\n1. Login\n2. Register Admin "
               "Baru\nPilih: ");
        if (scanf("%d", &authPilihan) != 1) {
          printf("Input tidak valid!\n");
          clearInputBuffer();
          break;
        }
        clearInputBuffer();
        if (authPilihan == 1)
          isAdmin = loginAdmin();
        else if (authPilihan == 2)
          registerAdmin();
        else
          printf("Pilihan tidak valid!\n");
      }
      break;
    }

    case 9:
      printf("\n--- MENYIMPAN DATA ---\n");
      syncDataToFile(root, "menu_kafe.csv");
      printf("Terima kasih. Sistem dimatikan.\n");
      break;

    default:
      if (pilihan != 0)
        printf("Pilihan tidak valid!\n");
    }
    if (pilihan != 9 && pilihan != 0) {
      printf("\nTekan ENTER untuk melanjutkan...");
      while (getchar() != '\n')
        ;
      getchar();
    }
  } while (pilihan != 9);

  freeTree(root);
  return 0;
}
