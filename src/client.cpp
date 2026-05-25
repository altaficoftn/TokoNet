// =============================================================================
//  TokoNet - client.cpp
//  TCP Client dengan menu terminal interaktif
//
//  KOMPILASI : g++ -std=c++11 -o client client.cpp
//  JALANKAN  : ./client
//              ./client 192.168.1.x   (kalau server di PC lain)
// =============================================================================

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <climits>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "shared_inventory.h"

const int   PORT       = 8080;
const int   BUFSIZE    = 32768;
const char* SERVER_IP  = "127.0.0.1";

// =============================================================================
//  Kirim JSON request ke server, terima JSON response
// =============================================================================
std::string kirimRequest(const std::string& jsonReq) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return Json::gagal("Gagal buat socket");

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &addr.sin_addr);

    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        close(sock);
        return Json::gagal("Tidak bisa connect ke server. Pastikan server sudah jalan.");
    }

    send(sock, jsonReq.c_str(), jsonReq.size(), 0);

    char buf[BUFSIZE];
    memset(buf, 0, BUFSIZE);
    int n = recv(sock, buf, BUFSIZE - 1, 0);
    close(sock);

    if (n <= 0) return Json::gagal("Tidak ada response dari server");
    return std::string(buf, n);
}

// =============================================================================
//  Parse array JSON [{...},{...}] — pisahkan jadi potongan per item
// =============================================================================
void tampilDaftar(const std::string& resp) {
    std::string status = Json::getString(resp, "status");
    if (status != "sukses") {
        std::cout << "  [GAGAL] " << Json::getString(resp, "pesan") << "\n";
        return;
    }

    // Temukan isi array [ ... ]
    size_t buka = resp.find('[');
    size_t tutup = resp.rfind(']');
    if (buka == std::string::npos) { std::cout << "  [Kosong]\n"; return; }
    std::string arr = resp.substr(buka + 1, tutup - buka - 1);

    // Pisahkan tiap object {..} berdasarkan kedalaman kurung kurawal
    std::cout << "\n";
    std::cout << std::left
              << std::setw(4)  << "No"
              << std::setw(22) << "Nama"
              << std::setw(12) << "Tipe"
              << std::setw(10) << "Harga"
              << std::setw(7)  << "Stok"
              << std::setw(8)  << "Diskon"
              << "\n";
    std::cout << std::string(63, '-') << "\n";

    int no = 1, depth = 0;
    size_t start = std::string::npos;
    for (size_t i = 0; i < arr.size(); i++) {
        if (arr[i] == '{') { if (!depth) start = i; depth++; }
        else if (arr[i] == '}') {
            depth--;
            if (!depth && start != std::string::npos) {
                std::string obj = arr.substr(start, i - start + 1);
                std::string nama   = Json::getString(obj, "nama");
                std::string tipe   = Json::getString(obj, "tipe");
                std::string hargaS = Json::getAngka(obj, "harga");
                std::string stokS  = Json::getAngka(obj, "stok");
                std::string disS   = Json::getAngka(obj, "diskon");

                std::cout << std::left
                          << std::setw(4)  << no++
                          << std::setw(22) << nama
                          << std::setw(12) << tipe
                          << std::setw(10) << ("Rp" + hargaS)
                          << std::setw(7)  << stokS
                          << std::setw(8)  << (disS + "%")
                          << "\n";
                start = std::string::npos;
            }
        }
    }
    std::cout << std::string(63, '-') << "\n";
}

// =============================================================================
//  Tampilkan hasil transaksi
// =============================================================================
void tampilHasil(const std::string& resp, const std::string& nama, int jml) {
    std::string status = Json::getString(resp, "status");
    std::cout << "\n" << std::string(40, '-') << "\n";
    if (status == "sukses") {
        std::cout << "  TRANSAKSI BERHASIL\n";
        std::cout << std::string(40, '-') << "\n";
        std::cout << "  Barang    : " << nama << "\n";
        std::cout << "  Jumlah    : " << jml << " pcs\n";
        std::cout << "  Diskon    : " << Json::getAngka(resp, "diskon") << "%\n";
        std::cout << "  Total     : Rp " << Json::getAngka(resp, "totalHarga") << "\n";
        std::cout << "  Sisa Stok : " << Json::getAngka(resp, "sisaStok") << " pcs\n";
    } else {
        std::cout << "  TRANSAKSI GAGAL\n";
        std::cout << std::string(40, '-') << "\n";
        std::cout << "  Alasan : " << Json::getString(resp, "pesan") << "\n";
    }
    std::cout << std::string(40, '-') << "\n";
}

// =============================================================================
//  Bersihkan buffer cin (penting setelah cin >> angka)
// =============================================================================
void bersihCin() {
    std::cin.clear();
    std::cin.ignore(INT_MAX, '\n');
}

// =============================================================================
//  main() — menu interaktif
// =============================================================================
int main() {
    std::cout << "============================================\n";
    std::cout << "     TokoNet — Sistem Kasir Toko\n";
    std::cout << "============================================\n";
    std::cout << "  Server: " << SERVER_IP << ":" << PORT << "\n";

    // Cek koneksi ke server
    std::string cek = kirimRequest("{\"aksi\":\"lihat\",\"namaBarang\":\"\",\"jumlah\":0}");
    if (Json::getString(cek, "status") != "sukses") {
        std::cout << "\n  [ERROR] " << Json::getString(cek, "pesan") << "\n";
        return 1;
    }
    std::cout << "  Koneksi ke server OK!\n";
    std::cout << "============================================\n";

    bool jalan = true;
    while (jalan) {
        std::cout << "\n  [1] Lihat Semua Barang\n";
        std::cout << "  [2] Lihat Barang (Urut Harga)\n";
        std::cout << "  [3] Transaksi Penjualan\n";
        std::cout << "  [4] Keluar\n";
        std::cout << "  Pilih: ";

        int pilih = 0;
        if (!(std::cin >> pilih)) { bersihCin(); continue; }
        bersihCin();

        if (pilih == 1) {
            std::string resp = kirimRequest(
                "{\"aksi\":\"lihat\",\"namaBarang\":\"\",\"jumlah\":0}");
            tampilDaftar(resp);

        } else if (pilih == 2) {
            std::string resp = kirimRequest(
                "{\"aksi\":\"lihat_urut\",\"namaBarang\":\"\",\"jumlah\":0}");
            tampilDaftar(resp);

        } else if (pilih == 3) {
            // Tampilkan daftar dulu biar kasir tahu nama barangnya
            std::string daftarResp = kirimRequest(
                "{\"aksi\":\"lihat\",\"namaBarang\":\"\",\"jumlah\":0}");
            tampilDaftar(daftarResp);

            std::cout << "\n  Nama barang (ketik TEPAT): ";
            std::string nama;
            std::getline(std::cin, nama);
            if (nama.empty()) { std::cout << "  Nama tidak boleh kosong.\n"; continue; }

            std::cout << "  Jumlah beli: ";
            int jml = 0;
            if (!(std::cin >> jml) || jml <= 0) {
                std::cout << "  Jumlah tidak valid.\n";
                bersihCin(); continue;
            }
            bersihCin();

            // Susun JSON request
            std::ostringstream oss;
            oss << "{\"aksi\":\"jual\","
                << "\"namaBarang\":\"" << nama << "\","
                << "\"jumlah\":" << jml << "}";

            std::string resp = kirimRequest(oss.str());
            tampilHasil(resp, nama, jml);

        } else if (pilih == 4) {
            jalan = false;
            std::cout << "\n  Terima kasih! Program selesai.\n";
        } else {
            std::cout << "  Pilihan tidak valid.\n";
        }
    }
    return 0;
}
