# C POSIX Multi-threaded Web Server

Mini HTTP web server yang ditulis dalam bahasa C untuk lingkungan Linux. Server ini dirancang secara modular dan menggunakan arsitektur concurrency `pthread` (Multi-threading) untuk menangani beberapa request klien secara bersamaan.

## Fitur
* Menangani HTTP `GET` requests.
* Mengembalikan berkas statis (HTML, CSS, JS, Gambar).
* Multi-threaded architecture: tidak terblokir oleh klien yang lambat.
* Modular (pemisahan logika socket dan parsing protokol).

## Prasyarat
* Linux OS atau WSL.
* Kompilator GCC.
* GNU Make.

## Cara Mengompilasi
Jalankan perintah berikut di direktori utama proyek:
```bash
make

## Uji Konkurensi menggunakan Apache bench
    bash
    ab -n 10000 -c 100 http://localhost:8080/

