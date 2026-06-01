#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <limits>
#include <algorithm>

using namespace std;

// Struktur data tambahan untuk mempermudah operasi CRUD file
struct DataBarang {
    int id;
    string nama;
    int jumlah;
};

// Kelas utama sesuai ketentuan OOP
class TokoElektronik {
private:
    // 1. Atribut Private: Array berkapasitas 3 elemen (Enkapsulasi)
    array<string, 3> etalase;
    const string namaFileGudang = "gudang.txt";

    // Helper method untuk membaca semua baris dari file txt ke dalam vector (Memudahkan Update & Delete)
    vector<DataBarang> bacaSemuaData() {
        vector<DataBarang> listBarang;
        ifstream file(namaFileGudang);
        string line;
        
        while (getline(file, line)) {
            if (line.empty()) continue;
            // Format file: ID|Nama Barang|Jumlah
            size_t pos1 = line.find('|');
            size_t pos2 = line.find('|', pos1 + 1);
            
            if (pos1 != string::npos && pos2 != string::npos) {
                DataBarang b;
                b.id = stoi(line.substr(0, pos1));
                b.nama = line.substr(pos1 + 1, pos2 - pos1 - 1);
                b.jumlah = stoi(line.substr(pos2 + 1));
                listBarang.push_back(b);
            }
        }
        file.close();
        return listBarang;
    }

    // Helper method untuk menimpa ulang file dengan data vector terbaru
    void tulisSemuaData(const vector<DataBarang>& listBarang) {
        ofstream file(namaFileGudang, ios::trunc);
        for (const auto& b : listBarang) {
            file << b.id << "|" << b.nama << "|" << b.jumlah << "\n";
        }
        file.close();
    }

public:
    // 1. Constructor: Mengisi data awal etalase secara otomatis
    TokoElektronik() {
        etalase[0] = "TV LED Samsung 43 Inch";
        etalase[1] = "Kulkas Polytron 2 Pintu";
        etalase[2] = "Mesin Cuci LG 8 Kg";
    }

    // 1 & 2. Method Public dan Mekanisme Exception Handling
    string ambilProduk(size_t nomorRak) {
        try {
            // Wajib menggunakan .at()
            return etalase.at(nomorRak); 
        } 
        catch (const out_of_range& e) {
            // Tangkap error out_of_range bawaan, lalu lempar ulang pesan kustom
            string pesanError = "Gagal Mengambil Barang : Rak nomor " + to_string(nomorRak) + " kosong atau tidak tersedia!";
            throw out_of_range(pesanError);
        }
    }


    // READ: Membaca dan menampilkan daftar barang dari dalam file
    void readBarang() {
        vector<DataBarang> listBarang = bacaSemuaData();
        cout << "\n=========== DAFTAR BARANG GUDANG ===========" << endl;
        if (listBarang.empty()) {
            cout << "Data gudang masih kosong." << endl;
        } else {
            cout << "ID\t| Jumlah\t| Nama Barang" << endl;
            cout << "--------------------------------------------" << endl;
            for (const auto& b : listBarang) {
                cout << b.id << "\t| " << b.jumlah << "\t\t| " << b.nama << endl;
            }
        }
        cout << "============================================" << endl;
    }

    // CREATE: Menambahkan data barang baru
    void createBarang() {
        vector<DataBarang> listBarang = bacaSemuaData();
        
        // Auto-increment ID
        int newId = 1;
        if (!listBarang.empty()) {
            newId = listBarang.back().id + 1;
        }

        DataBarang barangBaru;
        barangBaru.id = newId;

        cout << "\n--- TAMBAH BARANG BARU ---" << endl;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Bersihkan buffer
        cout << "Masukkan Nama Barang: ";
        getline(cin, barangBaru.nama);
        cout << "Masukkan Jumlah Stok: ";
        cin >> barangBaru.jumlah;

        // Buka file mode append untuk menambahkan baris baru di akhir
        ofstream file(namaFileGudang, ios::app);
        if (file.is_open()) {
            file << barangBaru.id << "|" << barangBaru.nama << "|" << barangBaru.jumlah << "\n";
            file.close();
            cout << ">> SUKSES: Barang '" << barangBaru.nama << "' berhasil ditambahkan!" << endl;
        } else {
            cout << ">> ERROR: Gagal membuka file gudang.txt!" << endl;
        }
    }

    // UPDATE: Memperbarui data barang berdasarkan ID
    void updateBarang() {
        vector<DataBarang> listBarang = bacaSemuaData();
        if (listBarang.empty()) {
            cout << "\n>> ERROR: Tidak ada data untuk diubah!" << endl;
            return;
        }

        readBarang();
        int targetId;
        cout << "Masukkan ID Barang yang ingin diubah: ";
        cin >> targetId;

        bool ditemukan = false;
        for (auto& b : listBarang) {
            if (b.id == targetId) {
                ditemukan = true;
                cout << "Data ditemukan: " << b.nama << " (Stok: " << b.jumlah << ")" << endl;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Masukkan Nama Barang Baru: ";
                getline(cin, b.nama);
                cout << "Masukkan Stok Barang Baru: ";
                cin >> b.jumlah;
                break;
            }
        }

        if (ditemukan) {
            tulisSemuaData(listBarang);
            cout << ">> SUKSES: Data barang berhasil diperbarui!" << endl;
        } else {
            cout << ">> ERROR: ID barang tidak ditemukan!" << endl;
        }
    }

    // DELETE: Menghapus data barang berdasarkan ID
    void deleteBarang() {
        vector<DataBarang> listBarang = bacaSemuaData();
        if (listBarang.empty()) {
            cout << "\n>> ERROR: Tidak ada data untuk dihapus!" << endl;
            return;
        }

        readBarang();
        int targetId;
        cout << "Masukkan ID Barang yang ingin dihapus: ";
        cin >> targetId;

        auto it = remove_if(listBarang.begin(), listBarang.end(), 
                            [targetId](const DataBarang& b) { return b.id == targetId; });

        if (it != listBarang.end()) {
            listBarang.erase(it, listBarang.end());
            tulisSemuaData(listBarang);
            cout << ">> SUKSES: Data barang berhasil dihapus!" << endl;
        } else {
            cout << ">> ERROR: ID barang tidak ditemukan!" << endl;
        }
    }
};

// 4. Pengujian pada Fungsi main()
int main() {
    TokoElektronik tokoGibran;
    int pilihan;

    // Sistem Menu Utama berjalan dengan do-while
    do {
        cout << "\n============================================" << endl;
        cout << "     SISTEM TOKO ELEKTRONIK GIBRAN JAYA     " << endl;
        cout << "============================================" << endl;
        // Read otomatis dipanggil saat menu diakses sebagai preview awal
        tokoGibran.readBarang(); 
        
        cout << "Menu Utama:" << endl;
        cout << "1. Tambah Data Barang Gudang (Create)" << endl;
        cout << "2. Ubah Data Barang Gudang (Update)" << endl;
        cout << "3. Hapus Data Barang Gudang (Delete)" << endl;
        cout << "4. Jalankan Simulasi Etalase (Test Error Handling)" << endl;
        cout << "0. Keluar Program" << endl;
        cout << "Pilih menu (0-4): ";
        
        // Cek validitas input menu
        if (!(cin >> pilihan)) {
            cout << "Input tidak valid! Harap masukkan angka." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (pilihan) {
            case 1:
                tokoGibran.createBarang();
                break;
            case 2:
                tokoGibran.updateBarang();
                break;
            case 3:
                tokoGibran.deleteBarang();
                break;
            case 4: {
                // Pengujian 4: Skenario Simulasi Etalase
                cout << "\n=== SIMULASI PENGAMBILAN ETALASE ===" << endl;
                
                // Skenario 1: Sukses
                cout << "\n[Skenario 1] Mengambil barang di rak indeks ke-1:" << endl;
                try {
                    string produk1 = tokoGibran.ambilProduk(1);
                    cout << ">> HASIL: Sukses! Anda mengambil " << produk1 << endl;
                } catch (const exception& e) {
                    cout << ">> HASIL TERTANGKAP ERROR: " << e.what() << endl;
                }

                // Skenario 2: Gagal (Melewati batas Array 3 elemen)
                cout << "\n[Skenario 2] Mengambil barang di rak indeks ke-5:" << endl;
                try {
                    string produk2 = tokoGibran.ambilProduk(5);
                    cout << ">> HASIL: Sukses! Anda mengambil " << produk2 << endl;
                } catch (const exception& e) {
                    // Di sini pesan error kustom (throw out_of_range) akan dicetak
                    cout << ">> HASIL TERTANGKAP ERROR: " << e.what() << endl;
                }
                break;
            }
            case 0:
                cout << "\nTerima kasih telah menggunakan sistem Toko Gibran Jaya. Sampai jumpa!" << endl;
                break;
            default:
                cout << "\nPilihan tidak tersedia! Silakan pilih menu 0-4." << endl;
        }
    } while (pilihan != 0);

    return 0;
}