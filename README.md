# TokoNet — Sistem Manajemen Toko Berbasis Client-Server

**Proyek Akhir Mata Kuliah Algoritma dan Pemrograman**  
Teknik Komputer — Universitas Indonesia — 2026

| Anggota | NPM |
|---|---|
| Altafico Fattan Wildana | 2506591186 |
| Fahrul Gustian Rezky | 2506585510 |
| Josias Shihkai Nazaro Sitanggang | 2506609435 |

---

## Deskripsi Proyek

TokoNet adalah sistem manajemen toko berbasis arsitektur **Client-Server** menggunakan **TCP Socket**. Server menyimpan data inventori dan memproses seluruh logika bisnis, sementara client menyediakan antarmuka kasir berbasis terminal yang terhubung ke server melalui jaringan.

### Fitur Utama
- Lihat seluruh daftar barang inventori
- Lihat daftar barang diurutkan berdasarkan harga (Quick Sort)
- Transaksi penjualan dengan pengurangan stok otomatis dan perhitungan diskon
- Server mendukung banyak client secara bersamaan (multithreading)

---

## Teknologi yang Digunakan

| Komponen | Detail |
|---|---|
| Bahasa | C++11 |
| Networking | POSIX Socket API (TCP) |
| Threading | std::thread + std::mutex |
| Data Format | JSON (manual, tanpa library eksternal) |
| Platform | Linux / WSL (Windows Subsystem for Linux) |

---

## Struktur Proyek

```
TokoNet/
├── README.md                  # Dokumentasi dan instruksi kompilasi
├── src/
│   ├── shared_inventory.h     # OOP (4 Pilar), Linked List, Linear Search, Quick Sort, JSON
│   ├── server.cpp             # TCP Server + Multithreading + Mutex
│   └── client.cpp             # TCP Client + Menu Interaktif
└── docs/
    └── Laporan_TokoNet.pdf    # Laporan teknis lengkap
```

### Penjelasan File

**`shared_inventory.h`**  
Header utama yang di-include oleh server dan client. Berisi:
- Namespace `Json` — utilitas JSON manual (parser + formatter)
- Abstract class `Item` — abstract class dengan 4 pilar OOP
- Subclass `Makanan`, `Elektronik`, `Minuman` — implementasi pewarisan dan polimorfisme
- `struct Node` dan `class LinkedList` — Linked List manual tanpa std::list/std::vector
- Algoritma Linear Search dan Quick Sort beserta analisis Big O

**`server.cpp`**  
Program server yang berjalan di background. Berisi:
- Inisialisasi 6 item data inventori awal
- Setup TCP Socket (socket → bind → listen → accept)
- Multithreading: setiap client dilayani thread terpisah
- Mutex untuk sinkronisasi akses data inventori

**`client.cpp`**  
Program antarmuka kasir berbasis terminal. Berisi:
- Koneksi TCP ke server
- Menu interaktif: lihat barang, lihat urut harga, transaksi penjualan
- Parsing dan tampilan JSON response dalam format tabel

---

## Persyaratan Sistem

- Sistem operasi **Linux** atau **Windows dengan WSL** (Ubuntu)
- Compiler **g++** dengan dukungan C++11
- Tidak memerlukan library eksternal apapun

### Instalasi g++ (jika belum ada)

```bash
sudo apt update && sudo apt install g++ -y
```

---

## Instruksi Kompilasi

### Clone Repository

```bash
git clone https://github.com/[username]/TokoNet.git
cd TokoNet/src
```

### Kompilasi Server

```bash
g++ -std=c++11 -pthread -o server server.cpp
```

Flag `-pthread` diperlukan karena server menggunakan `std::thread` untuk multithreading.

### Kompilasi Client

```bash
g++ -std=c++11 -o client client.cpp
```

---

## Cara Menjalankan

### Langkah 1 — Jalankan Server

Buka **terminal pertama**, masuk ke folder `src`, lalu jalankan:

```bash
./server
```

Output yang muncul saat server berhasil berjalan:

```
==============================================
     TokoNet v2 Server — Port 8080
==============================================
[SERVER] Inventory siap: 6 item
[SERVER] Mendengarkan koneksi...
[SERVER] Tekan Ctrl+C untuk berhenti
----------------------------------------------
```

### Langkah 2 — Jalankan Client

Buka **terminal kedua** (server harus tetap berjalan), masuk ke folder `src`, lalu jalankan:

```bash
./client
```

Output yang muncul saat client berhasil terhubung:

```
============================================
     TokoNet v2 — Sistem Kasir Toko
============================================
  Server: 127.0.0.1:8080
  Koneksi ke server OK!
============================================

  [1] Lihat Semua Barang
  [2] Lihat Barang (Urut Harga)
  [3] Transaksi Penjualan
  [4] Keluar
  Pilih:
```

### Menjalankan di PC Berbeda (Jaringan Lokal)

Jika server dan client berada di PC yang berbeda dalam satu jaringan:

**1. Cari IP server:**
```bash
ip addr show | grep inet
```

**2. Edit `SERVER_IP` di `client.cpp`:**
```cpp
const char* SERVER_IP = "192.168.x.x";  // ganti dengan IP server
```

**3. Kompilasi ulang client:**
```bash
g++ -std=c++11 -o client client.cpp
```

---

## Contoh Penggunaan

### Melihat Daftar Barang
Pilih menu `[1]`:

```
No  Nama                  Tipe        Harga     Stok   Diskon
---------------------------------------------------------------
1   Indomie Goreng        Makanan     Rp3500    50     5%
2   Biskuit Roma          Makanan     Rp8000    30     5%
3   Aqua 600ml            Minuman     Rp4000    100    3%
4   Teh Botol Sosro       Minuman     Rp5000    80     3%
5   Baterai ABC AA        Elektronik  Rp5000    60     10%
6   Lampu LED 10W         Elektronik  Rp18000   40     10%
---------------------------------------------------------------
```

### Melihat Daftar Urut Harga
Pilih menu `[2]` — diurutkan menggunakan **Quick Sort**:

```
No  Nama                  Tipe        Harga     Stok   Diskon
---------------------------------------------------------------
1   Indomie Goreng        Makanan     Rp3500    50     5%
2   Aqua 600ml            Minuman     Rp4000    100    3%
3   Teh Botol Sosro       Minuman     Rp5000    80     3%
4   Baterai ABC AA        Elektronik  Rp5000    60     10%
5   Biskuit Roma          Makanan     Rp8000    30     5%
6   Lampu LED 10W         Elektronik  Rp18000   40     10%
---------------------------------------------------------------
```

### Transaksi Penjualan
Pilih menu `[3]`:

```
  Nama barang (ketik TEPAT): Aqua 600ml
  Jumlah beli: 3

  ----------------------------------------
  TRANSAKSI BERHASIL
  ----------------------------------------
  Barang    : Aqua 600ml
  Jumlah    : 3 pcs
  Diskon    : 3%
  Total     : Rp 11640
  Sisa Stok : 97 pcs
  ----------------------------------------
```

---

## Implementasi Teknis

### OOP — 4 Pilar

| Pilar | Implementasi |
|---|---|
| Abstraksi | `Item` adalah abstract class dengan pure virtual method `tampilInfo()` dan `hitungDiskon()` |
| Enkapsulasi | Atribut `nama`, `harga`, `stok` bersifat `private`, diakses melalui getter/setter |
| Pewarisan | `Makanan`, `Elektronik`, `Minuman` mewarisi `Item` dengan atribut tambahan masing-masing |
| Polimorfisme | `hitungDiskon()` menghasilkan nilai berbeda: Makanan 5%, Minuman 3%, Elektronik 10% |

### Algoritma dan Kompleksitas

| Algoritma | Fungsi | Best | Average | Worst |
|---|---|---|---|---|
| Linear Search | `LinkedList::cari()` | O(1) | O(n) | O(n) |
| Quick Sort | `LinkedList::urutkanHarga()` | O(n log n) | O(n log n) | O(n²) |

### Format JSON

**Request dari client:**
```json
{"aksi":"jual","namaBarang":"Aqua 600ml","jumlah":3}
```

**Response sukses:**
```json
{"status":"sukses","sisaStok":97,"totalHarga":11640,"diskon":3}
```

**Response gagal:**
```json
{"status":"gagal","pesan":"Stok tidak cukup. Tersedia: 2"}
```

---

## Bonus yang Diimplementasikan

| Kriteria Bonus | Status | Detail |
|---|---|---|
| Linked List Manual | ✅ | Tidak menggunakan `std::list`, `std::vector`, atau pustaka bawaan |
| Analisis Big O | ✅ | Tercantum sebagai komentar di `shared_inventory.h` pada fungsi `cari()` dan `urutkanHarga()` |
| Multithreading | ✅ | `std::thread` + `std::mutex` — setiap client dilayani thread terpisah |

---

## Troubleshooting

**`bind gagal: Address already in use`**
```bash
pkill server && ./server
```

**`Tidak bisa connect ke server`**  
Pastikan server sudah berjalan di terminal lain sebelum menjalankan client.

**`socket gagal` atau `bind gagal`**  
Pastikan menggunakan flag kompilasi yang benar (server wajib pakai `-pthread`).

---

## Lisensi

Proyek ini dibuat untuk keperluan akademik mata kuliah Algoritma dan Pemrograman,  
Teknik Komputer, Universitas Indonesia.
