// =============================================================================
//  TokoNet v2 - server.cpp
//  TCP Server dengan Multithreading + Mutex (BONUS POIN)
//
//  KOMPILASI : g++ -std=c++11 -pthread -o server server.cpp
//  JALANKAN  : ./server
// =============================================================================

#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <mutex>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "shared_inventory.h"

// --- Konfigurasi ---
const int PORT   = 8080;
const int BUFFER = 4096;

// --- Data global yang dibagi semua thread ---
LinkedList inventory;
std::mutex mtx; // mutex untuk thread-safety

// =============================================================================
//  Seed data awal
// =============================================================================
void isiInventory() {
    inventory.tambah(new Makanan   ("Indomie Goreng",  3500,  50, "2026-12-31"));
    inventory.tambah(new Makanan   ("Biskuit Roma",    8000,  30, "2026-10-01"));
    inventory.tambah(new Minuman   ("Aqua 600ml",      4000, 100, false));
    inventory.tambah(new Minuman   ("Teh Botol Sosro", 5000,  80, false));
    inventory.tambah(new Elektronik("Baterai ABC AA",  5000,  60, 0));
    inventory.tambah(new Elektronik("Lampu LED 10W",  18000,  40, 12));
    std::cout << "[SERVER] Inventory siap: " << inventory.getUkuran() << " item\n";
}

// =============================================================================
//  Proses request "lihat" — kirim daftar semua barang
// =============================================================================
std::string prosesLihat() {
    std::lock_guard<std::mutex> lock(mtx);
    return Json::daftar(inventory.toJsonArray());
}

// =============================================================================
//  Proses request "lihat_urut" — urutkan dulu (Quick Sort), lalu kirim
// =============================================================================
std::string prosesLihatUrut() {
    std::lock_guard<std::mutex> lock(mtx);
    inventory.urutkanHarga();
    return Json::daftar(inventory.toJsonArray());
}

// =============================================================================
//  Proses request "jual" — cari (Linear Search), cek stok, kurangi, hitung harga
// =============================================================================
std::string prosesJual(const std::string& nama, int jumlah) {
    if (jumlah <= 0)
        return Json::gagal("Jumlah harus lebih dari 0");

    std::lock_guard<std::mutex> lock(mtx); // kunci mutex agar thread-safe

    Item* item = inventory.cari(nama); // Linear Search
    if (!item)
        return Json::gagal("Barang '" + nama + "' tidak ditemukan");
    if (item->getStok() < jumlah) {
        std::ostringstream oss;
        oss << "Stok tidak cukup. Tersedia: " << item->getStok();
        return Json::gagal(oss.str());
    }

    item->setStok(item->getStok() - jumlah);

    // Hitung harga pakai polimorfisme: hitungDiskon() berbeda tiap subclass
    double diskon  = item->hitungDiskon();
    double total   = item->getHarga() * jumlah * (1.0 - diskon / 100.0);

    return Json::sukses(item->getStok(), total, diskon);
}

// =============================================================================
//  handleClient — dijalankan di thread terpisah untuk setiap client
//  [BONUS MULTITHREADING]
// =============================================================================
void handleClient(int sock, sockaddr_in addr) {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, ip, INET_ADDRSTRLEN);
    std::cout << "[THREAD] Client masuk: " << ip << "\n";

    char buf[BUFFER];
    while (true) {
        memset(buf, 0, BUFFER);
        int n = recv(sock, buf, BUFFER - 1, 0);
        if (n <= 0) break; // client disconnect

        std::string req(buf, n);
        std::string aksi = Json::getString(req, "aksi");
        std::string resp;

        if (aksi == "lihat") {
            resp = prosesLihat();
        } else if (aksi == "lihat_urut") {
            resp = prosesLihatUrut();
        } else if (aksi == "jual") {
            std::string nama = Json::getString(req, "namaBarang");
            int jumlah = 0;
            try { jumlah = std::stoi(Json::getAngka(req, "jumlah")); }
            catch (...) { jumlah = 0; }
            resp = prosesJual(nama, jumlah);
        } else {
            resp = Json::gagal("Aksi tidak dikenal: " + aksi);
        }

        std::cout << "[THREAD][" << ip << "] " << aksi << "\n";
        send(sock, resp.c_str(), resp.size(), 0);
    }

    close(sock);
    std::cout << "[THREAD] Client " << ip << " disconnect\n";
}

// =============================================================================
//  main() — setup socket, loop accept + spawn thread
// =============================================================================
int main() {
    std::cout << "==============================================\n";
    std::cout << "     TokoNet v2 Server — Port " << PORT << "\n";
    std::cout << "==============================================\n";

    isiInventory();

    // 1. Buat socket TCP
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock < 0) { perror("socket gagal"); return 1; }

    // 2. SO_REUSEADDR: agar port bisa langsung dipakai ulang setelah restart
    int opt = 1;
    setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 3. Bind ke port 8080
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(PORT);
    if (bind(serverSock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind gagal"); return 1;
    }

    // 4. Listen
    listen(serverSock, 5);
    std::cout << "[SERVER] Mendengarkan koneksi...\n";
    std::cout << "[SERVER] Tekan Ctrl+C untuk berhenti\n";
    std::cout << "----------------------------------------------\n";

    // 5. Loop: terima client baru, spawn thread baru tiap client
    while (true) {
        sockaddr_in clientAddr;
        socklen_t   clientLen = sizeof(clientAddr);
        int clientSock = accept(serverSock, (sockaddr*)&clientAddr, &clientLen);
        if (clientSock < 0) continue;

        // Spawn thread baru, detach agar main() tidak perlu tunggu
        std::thread t(handleClient, clientSock, clientAddr);
        t.detach();
    }

    close(serverSock);
    return 0;
}
