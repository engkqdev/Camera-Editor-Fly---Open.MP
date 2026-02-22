# ## Camera-Editor-Fly --- Open.MP

Sistem editor kamera berbasis navigasi *free-roam* (fly mode) yang dirancang khusus untuk lingkungan **open.mp** dan **SA-MP**. Alat ini mempermudah pembuatan transisi sinematik dengan akurasi tinggi pada posisi koordinat dan arah pandang (*look-at*).

---

## 🛠 Informasi Penting
**Copyright (c) 2026 Engkq.**

Script ini sepenuhnya hasil riset dan pengembangan saya sendiri **Engkq**. Saya sangat terbuka bagi siapa pun yang ingin menggunakan atau memodifikasi script ini untuk kebutuhan server. Namun, saya minta satu hal: **Hargai penulis asli.** Jangan menghapus kredit hanya untuk pengakuan sepihak. Jika kamu melakukan perubahan, cukup tambahkan namamu sebagai kontributor di bawah nama pengembang asli.

---

## ✨ Fitur Utama
* **Seamless Noclip:** Pergerakan kamera tanpa hambatan menggunakan kalkulasi vektor posisi.
* **Dual-Platform Support:** Optimal untuk pengguna PC (Key Input) dan Android (Command Input).
* **Front Vector Calculation:** Menghitung arah pandang kamera secara matematis sehingga hasil interpolasi selalu presisi.
* **Automated Export:** Data yang dihasilkan langsung berupa baris kode `InterpolateCameraPos` dan `InterpolateCameraLookAt` yang siap pakai.
* **Integrated Preview:** Fitur uji coba gerak sebelum melakukan proses penyimpanan (*saving*).

---

## 🎮 Kontrol & Navigasi

| Fungsi | PC (Keyboard) | Android (Command) |
| :--- | :--- | :--- |
| **Gerakan** | `W, A, S, D` | Analog Joystick |
| **Akselerasi** | `Space / Shift` | - |
| **Set Point** | `Mouse Kiri (LKM)` | `/setpoint` |
| **Menu Utama** | Otomatis (UI Dialog) | Otomatis (UI Dialog) |

---

## 📦 Instalasi & Penggunaan
1. Pastikan *dependency hook* (seperti `y_hooks`) sudah tersedia digamemode kamu.
2. Masukkan file ini kedalam pawno/qawno > include lalu tempel.
3. Jangan lupa masukan text `#include <ecameditor.inc>` diatas .pwn utama
4. Gunakan `/cameditor` untuk masuk ke mode edit.
5. Tentukan titik **Start** dan **End**.
6. Atur durasi pergerakan (dalam milidetik).
7. Simpan, dan ambil hasilnya di folder `scriptfiles`.

---

## 📝 Catatan Teknis
Script ini menggunakan `AttachCameraToPlayerObject` dengan objek transparan (**ID 19300**) sebagai *anchor*. Pastikan gamemode/server kamu tidak membatasi limit objek pemain agar proses rendering tetap stabil saat editing berlangsung.

---

<div align="center">
  <p><b>Developed by engkqdev</b></p>
  <img src="https://img.shields.io/badge/Status-Stable-green?style=flat-square" />
  <img src="https://img.shields.io/badge/Language-PAWN-orange?style=flat-square" />
</div>
