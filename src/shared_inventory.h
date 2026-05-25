// =============================================================================
//  TokoNet v2 - shared_inventory.h
//  Berisi: OOP (4 Pilar), Manual Linked List, Linear Search, Quick Sort, JSON
// =============================================================================

#ifndef SHARED_INVENTORY_H
#define SHARED_INVENTORY_H

#include <iostream>
#include <string>
#include <sstream>

// =============================================================================
//  BAGIAN 1: JSON UTILITY (Sederhana - hanya fungsi yang benar-benar dipakai)
// =============================================================================
namespace Json {

    // Ambil nilai string dari JSON: {"key":"nilai"} -> "nilai"
    std::string getString(const std::string& json, const std::string& key) {
        std::string cari = "\"" + key + "\"";
        size_t pos = json.find(cari);
        if (pos == std::string::npos) return "";
        size_t titikDua = json.find(':', pos);
        size_t awal = json.find('"', titikDua) + 1;
        size_t akhir = json.find('"', awal);
        return json.substr(awal, akhir - awal);
    }

    // Ambil nilai angka dari JSON: {"key":123} -> "123"
    std::string getAngka(const std::string& json, const std::string& key) {
        std::string cari = "\"" + key + "\"";
        size_t pos = json.find(cari);
        if (pos == std::string::npos) return "0";
        size_t titikDua = json.find(':', pos) + 1;
        while (titikDua < json.size() && json[titikDua] == ' ') titikDua++;
        size_t akhir = titikDua;
        while (akhir < json.size() && (isdigit(json[akhir]) || json[akhir] == '.'))
            akhir++;
        return json.substr(titikDua, akhir - titikDua);
    }

    // Buat response sukses transaksi
    std::string sukses(int sisaStok, double totalHarga, double diskon) {
        std::ostringstream oss;
        oss << "{\"status\":\"sukses\","
            << "\"sisaStok\":" << sisaStok << ","
            << "\"totalHarga\":" << (int)totalHarga << ","
            << "\"diskon\":" << (int)diskon << "}";
        return oss.str();
    }

    // Buat response gagal
    std::string gagal(const std::string& pesan) {
        return "{\"status\":\"gagal\",\"pesan\":\"" + pesan + "\"}";
    }

    // Buat JSON satu item
    std::string item(const std::string& nama, const std::string& tipe,
                     double harga, int stok, double diskon) {
        std::ostringstream oss;
        oss << "{\"nama\":\"" << nama << "\","
            << "\"tipe\":\"" << tipe << "\","
            << "\"harga\":" << (int)harga << ","
            << "\"stok\":" << stok << ","
            << "\"diskon\":" << (int)diskon << "}";
        return oss.str();
    }

    // Bungkus array jadi response daftar
    std::string daftar(const std::string& arrayIsi) {
        return "{\"status\":\"sukses\",\"daftar\":[" + arrayIsi + "]}";
    }

} // namespace Json


// =============================================================================
//  BAGIAN 2: HIERARKI KELAS OOP
//
//  [ABSTRAKSI]   : Item = abstract class, punya pure virtual method
//  [ENKAPSULASI] : semua atribut private, akses lewat getter/setter
//  [PEWARISAN]   : Makanan dan Elektronik mewarisi Item
//  [POLIMORFISME]: setiap subclass override tampilInfo() dan hitungDiskon()
// =============================================================================

// -----------------------------------------------------------------------------
//  Abstract Class: Item
// -----------------------------------------------------------------------------
class Item {
private:
    std::string nama;
    double      harga;
    int         stok;

public:
    Item(const std::string& nama, double harga, int stok)
        : nama(nama), harga(harga), stok(stok) {}

    virtual ~Item() {} // virtual destructor: wajib ada di abstract class

    // Getter
    std::string getNama()  const { return nama;  }
    double      getHarga() const { return harga; }
    int         getStok()  const { return stok;  }

    // Setter
    void setStok(int s) { stok = s; }

    // Pure virtual: WAJIB diisi subclass [ABSTRAKSI]
    virtual std::string getTipe()      const = 0;
    virtual void        tampilInfo()         = 0;
    virtual double      hitungDiskon()       = 0;

    // Konversi ke JSON string
    std::string toJson() {
        return Json::item(nama, getTipe(), harga, stok, hitungDiskon());
    }
};

// -----------------------------------------------------------------------------
//  Subclass: Makanan  [PEWARISAN]
// -----------------------------------------------------------------------------
class Makanan : public Item {
private:
    std::string kadaluarsa; // atribut tambahan khusus Makanan

public:
    Makanan(const std::string& nama, double harga, int stok,
            const std::string& kadaluarsa)
        : Item(nama, harga, stok), kadaluarsa(kadaluarsa) {}

    std::string getTipe() const override { return "Makanan"; }

    void tampilInfo() override { // [POLIMORFISME]
        std::cout << "[MAKANAN]    " << getNama()
                  << " | Rp" << getHarga()
                  << " | Stok: " << getStok()
                  << " | Kadaluarsa: " << kadaluarsa
                  << " | Diskon: " << hitungDiskon() << "%\n";
    }

    double hitungDiskon() override { return 5.0; } // [POLIMORFISME]
};

// -----------------------------------------------------------------------------
//  Subclass: Elektronik  [PEWARISAN]
// -----------------------------------------------------------------------------
class Elektronik : public Item {
private:
    int garansiBulan; // atribut tambahan khusus Elektronik

public:
    Elektronik(const std::string& nama, double harga, int stok, int garansiBulan)
        : Item(nama, harga, stok), garansiBulan(garansiBulan) {}

    std::string getTipe() const override { return "Elektronik"; }

    void tampilInfo() override { // [POLIMORFISME]
        std::cout << "[ELEKTRONIK] " << getNama()
                  << " | Rp" << getHarga()
                  << " | Stok: " << getStok()
                  << " | Garansi: " << garansiBulan << " bulan"
                  << " | Diskon: " << hitungDiskon() << "%\n";
    }

    double hitungDiskon() override { return 10.0; } // [POLIMORFISME]
};

// -----------------------------------------------------------------------------
//  Subclass: Minuman  [PEWARISAN]
// -----------------------------------------------------------------------------
class Minuman : public Item {
private:
    bool dingin; // atribut tambahan khusus Minuman

public:
    Minuman(const std::string& nama, double harga, int stok, bool dingin)
        : Item(nama, harga, stok), dingin(dingin) {}

    std::string getTipe() const override { return "Minuman"; }

    void tampilInfo() override { // [POLIMORFISME]
        std::cout << "[MINUMAN]    " << getNama()
                  << " | Rp" << getHarga()
                  << " | Stok: " << getStok()
                  << " | Dingin: " << (dingin ? "Ya" : "Tidak")
                  << " | Diskon: " << hitungDiskon() << "%\n";
    }

    double hitungDiskon() override { return 3.0; } // [POLIMORFISME]
};


// =============================================================================
//  BAGIAN 3: LINKED LIST MANUAL (BONUS POIN)
//  Tidak menggunakan std::list, std::vector, atau pustaka data bawaan.
// =============================================================================

struct Node {
    Item* data; // pointer ke Item — mendukung polimorfisme
    Node* next;
    Node(Item* d) : data(d), next(nullptr) {}
};

class LinkedList {
private:
    Node* head;
    int   ukuran;

    // --- Helper Quick Sort ---
    Node* nodeTerakhir(Node* n) {
        while (n && n->next) n = n->next;
        return n;
    }

    // Partisi: tukar DATA antar node (bukan tukar node)
    Node* partisi(Node* awal, Node* akhir) {
        double pivot = akhir->data->getHarga();
        Node* batas  = awal;
        for (Node* cur = awal; cur != akhir; cur = cur->next) {
            if (cur->data->getHarga() <= pivot) {
                Item* tmp   = batas->data;
                batas->data = cur->data;
                cur->data   = tmp;
                batas = batas->next;
            }
        }
        Item* tmp    = batas->data;
        batas->data  = akhir->data;
        akhir->data  = tmp;
        return batas;
    }

    void quickSort(Node* awal, Node* akhir) {
        if (!awal || awal == akhir) return;
        Node* p = partisi(awal, akhir);
        // rekursi sub-list sebelum pivot
        if (p != awal) {
            Node* prev = awal;
            while (prev->next != p) prev = prev->next;
            quickSort(awal, prev);
        }
        // rekursi sub-list setelah pivot
        if (p->next) quickSort(p->next, nodeTerakhir(p->next));
    }

public:
    LinkedList() : head(nullptr), ukuran(0) {}

    // Destructor: hapus semua node dan item dari memori
    ~LinkedList() {
        Node* cur = head;
        while (cur) {
            Node* next = cur->next;
            delete cur->data;
            delete cur;
            cur = next;
        }
    }

    int getUkuran() const { return ukuran; }

    // Tambah item baru ke akhir list
    void tambah(Item* item) {
        Node* baru = new Node(item);
        if (!head) { head = baru; }
        else {
            Node* cur = head;
            while (cur->next) cur = cur->next;
            cur->next = baru;
        }
        ukuran++;
    }

    // -------------------------------------------------------------------------
    //  LINEAR SEARCH
    //  Best Case  : O(1)  — item ada di node pertama
    //  Avg Case   : O(n)  — item ada di tengah list
    //  Worst Case : O(n)  — item tidak ada / ada di node terakhir
    //  Space      : O(1)  — tidak butuh memori tambahan
    //
    //  Alasan pilih Linear Search (bukan Binary Search):
    //  Linked List tidak mendukung random access (tidak bisa langsung ke
    //  indeks tengah seperti arr[n/2]), sehingga Binary Search tidak bisa
    //  diterapkan secara langsung dan efisien pada struktur ini.
    // -------------------------------------------------------------------------
    Item* cari(const std::string& nama) {
        Node* cur = head;
        while (cur) {
            if (cur->data->getNama() == nama)
                return cur->data;
            cur = cur->next;
        }
        return nullptr;
    }

    // -------------------------------------------------------------------------
    //  QUICK SORT (urutkan berdasarkan harga, ascending)
    //  Best Case  : O(n log n) — pivot selalu membagi list secara seimbang
    //  Avg Case   : O(n log n) — secara statistik partisi cukup seimbang
    //  Worst Case : O(n²)      — data sudah terurut, pivot selalu terkecil
    //  Space      : O(log n)   — stack rekursi
    //
    //  Alasan pilih Quick Sort (bukan Bubble Sort):
    //  Quick Sort O(n log n) rata-rata vs Bubble Sort yang selalu O(n²).
    //  Untuk 100 item: Quick Sort ~664 operasi, Bubble Sort 10.000 operasi.
    // -------------------------------------------------------------------------
    void urutkanHarga() {
        if (!head || !head->next) return;
        quickSort(head, nodeTerakhir(head));
    }

    // Tampilkan semua item (polimorfisme: tampilInfo() tiap subclass berbeda)
    void tampilSemua() {
        if (!head) { std::cout << "  [Inventory kosong]\n"; return; }
        Node* cur = head;
        int no = 1;
        while (cur) {
            std::cout << "  " << no++ << ". ";
            cur->data->tampilInfo(); // [POLIMORFISME]
            cur = cur->next;
        }
    }

    // Konversi seluruh list ke JSON array string
    std::string toJsonArray() {
        std::string result;
        Node* cur = head;
        while (cur) {
            if (!result.empty()) result += ",";
            result += cur->data->toJson();
            cur = cur->next;
        }
        return result;
    }
};

#endif // SHARED_INVENTORY_H
