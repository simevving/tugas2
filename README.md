# tugas2
Tugas Modul 2 Lutfi Programming
https://youtu.be/CF4SMzUnKF8

## Cara Kerja Program

Secara umum, program memiliki dua proses utama:

1. Menghitung koordinat titik-titik pembentuk roda gigi.
2. Menggerakkan kura-kura menuju titik-titik tersebut menggunakan umpan balik posisi.

Kedua proses tersebut diatur oleh class `GearTurtle`.

### 1. Menghitung Koordinat Lingkaran

Program menggunakan koordinat polar untuk menentukan titik-titik pada roda gigi.

Koordinat polar menggunakan dua nilai:

- `r`: jarak titik dari pusat.
- `sudut`: sudut titik terhadap sumbu horizontal.

Turtlesim menggunakan koordinat Kartesius, sehingga koordinat polar perlu diubah menjadi koordinat `x` dan `y`.

Rumusnya:

```text
x = pusat_x + r * cos(sudut)
y = pusat_y + r * sin(sudut)
```

Perhitungan ini diimplementasikan melalui fungsi `polar()`:

```cpp
Titik polar(double r, double sudut) const
{
  return {
    pusat_x_ + r * std::cos(sudut),
    pusat_y_ + r * std::sin(sudut)
  };
}
```

Fungsi `std::cos()` dan `std::sin()` pada C++ menggunakan satuan radian, bukan derajat.

Hubungan antara derajat dan radian:

```text
360° = 2π radian
180° = π radian
```

Karena itu, perhitungan sudut dalam program menggunakan konstanta `M_PI`.

### 2. Menghitung Jatah Sudut Setiap Gigi

Program menggunakan 8 gigi.

Satu putaran penuh dibagi rata menjadi delapan bagian:

```text
langkah = 2π / jumlah_gigi
        = 2π / 8
        = π / 4
        = 45°
```

Variabel `langkah` menyimpan jatah sudut untuk setiap gigi.

Pusat sudut gigi ke-i dihitung menggunakan:

```cpp
const double pusat = i * langkah;
```

Dengan demikian, posisi sudut pusat setiap gigi adalah:

```text
Gigi 1:   0°
Gigi 2:  45°
Gigi 3:  90°
Gigi 4: 135°
Gigi 5: 180°
Gigi 6: 225°
Gigi 7: 270°
Gigi 8: 315°
```

Pembagian sudut yang merata menghasilkan pola gigi yang terdistribusi di sepanjang lingkaran.

### 3. Membentuk Gigi Trapesium

Program menggunakan dua jari-jari untuk membentuk roda gigi.

| Parameter | Nilai | Fungsi |
|---|---:|---|
| `r_dalam` | 2.2 | Jari-jari lingkaran dasar |
| `r_luar` | 3.0 | Jari-jari ujung gigi |

Kedua parameter tersebut diberikan saat objek dibuat:

```cpp
auto gear = std::make_shared<GearTurtle>(8, 2.2, 3.0);
```

Tinggi gigi diperoleh dari selisih jari-jari luar dan dalam:

```text
tinggi_gigi = r_luar - r_dalam
            = 3.0 - 2.2
            = 0.8
```

Untuk setiap gigi, program menghitung empat titik utama:

```text
Dasar kiri   : (r_dalam, pusat - setengah_dasar)
Puncak kiri  : (r_luar,  pusat - setengah_puncak)
Puncak kanan : (r_luar,  pusat + setengah_puncak)
Dasar kanan  : (r_dalam, pusat + setengah_dasar)
```

Lebar sudut dasar dan puncak ditentukan oleh:

```cpp
const double setengah_dasar = 0.25 * langkah;
const double setengah_puncak = 0.15 * langkah;
```

Dengan jatah sudut 45 derajat:

```text
Lebar dasar = 2 * 0.25 * 45°
            = 22.5°

Lebar puncak = 2 * 0.15 * 45°
             = 13.5°
```

Lebar puncak yang lebih kecil daripada lebar dasar menghasilkan bentuk menyerupai trapesium.

Perhitungan titik tersebut dilakukan berulang kali untuk seluruh gigi di dalam fungsi `hitungTitik()`.

### 4. Membentuk Celah Melengkung

Celah di antara gigi berada pada jari-jari `r_dalam`.

Program membagi setiap celah menjadi enam ruas menggunakan variabel:

```cpp
const int bagian_celah = 6;
```

Sudut satu celah adalah:

```text
sudut_celah = 45° - 22.5°
            = 22.5°
```

Sudut setiap ruas:

```text
sudut_segmen = 22.5° / 6
             = 3.75°
```

Program menambahkan titik-titik di antara kedua ujung celah:

```cpp
for (int k = 1; k < bagian_celah; k++) {
  hasil.push_back(
    polar(
      r_dalam_,
      awal + (akhir - awal) * k / bagian_celah
    )
  );
}
```

Perulangan tersebut menambahkan lima titik pada setiap celah jika kedua ujungnya dihitung bersama titik gigi yang sudah ada.

Titik-titik tersebut berada pada lingkaran dasar. Ketika dihubungkan oleh kura-kura, garis lurus pendek antartitik membuat celah tampak melengkung.

Semakin banyak pembagian celah, semakin halus pendekatan garis terhadap lengkungan ideal. Namun, jumlah titik yang harus dikunjungi juga bertambah.

### 5. Menghitung Jumlah Titik

Untuk setiap gigi, program menghasilkan:

- 4 titik utama pembentuk gigi.
- 5 titik untuk bagian celah, termasuk titik dasar awal celah.

Jumlah titik per gigi:

```text
4 + 5 = 9 titik
```

Dengan 8 gigi:

```text
8 * 9 = 72 titik
```

Titik-titik tersebut disimpan dalam `std::vector<Titik>`.

Setelah seluruh titik dikunjungi, program memanggil:

```cpp
jalanKe(titik.front());
```

Pemanggilan tersebut mengarahkan kura-kura kembali menuju titik pertama untuk membantu menutup gambar.

### 6. Menentukan Pusat Roda Gigi

Titik awal gambar disesuaikan dengan posisi kura-kura saat program dijalankan.

Tujuannya adalah agar kura-kura memulai gambar dari titik pertama roda gigi tanpa perlu bergerak terlebih dahulu dari posisi awal menuju titik tersebut.

Pertama, program menghitung jatah sudut setiap gigi dan sudut awal:

```cpp
const double langkah = 2.0 * M_PI / jumlah_gigi_;
const double sudut_awal = -0.25 * langkah;
```

Untuk delapan gigi:

```text
langkah = 45°
sudut_awal = -11.25°
```

Pusat roda gigi kemudian dihitung:

```cpp
pusat_x_ = pose_.x - r_dalam_ * std::cos(sudut_awal);
pusat_y_ = pose_.y - r_dalam_ * std::sin(sudut_awal);
```

Dengan asumsi posisi awal kura-kura adalah sekitar `(5.544, 5.544)`, hasilnya kurang lebih:

```text
pusat_x = 5.544 - 2.2 * cos(-11.25°)
        ≈ 3.39

pusat_y = 5.544 - 2.2 * sin(-11.25°)
        ≈ 5.97
```

Jendela Turtlesim biasanya menggunakan koordinat sekitar 0 sampai 11 pada setiap sumbu.

Dengan jari-jari luar 3.0, perkiraan rentang gambar adalah:

```text
x: 3.39 ± 3.0 → 0.39 sampai 6.39
y: 5.97 ± 3.0 → 2.97 sampai 8.97
```

Rentang tersebut masih berada di dalam jendela Turtlesim.

Nilai ini merupakan perkiraan berdasarkan posisi awal yang diasumsikan. Pusat sebenarnya dihitung dari pose yang diterima program saat dijalankan.

### 7. Menggerakkan Kura-Kura Menggunakan Umpan Balik

Program menggunakan umpan balik posisi agar kura-kura dapat bergerak menuju setiap titik secara bertahap.

Publisher mengirimkan perintah gerak melalui:

```text
/turtle1/cmd_vel
```

Subscriber membaca posisi dan orientasi aktual melalui:

```text
/turtle1/pose
```

Data pose berisi:

- `x`: posisi horizontal.
- `y`: posisi vertikal.
- `theta`: orientasi kura-kura.

Proses pengendalian dilakukan berulang kali di dalam fungsi `jalanKe()`.

#### a. Mengambil pose terbaru

```cpp
rclcpp::spin_some(shared_from_this());
```

Perintah tersebut memproses callback yang menunggu agar pose terbaru dapat diterima dan disimpan.

#### b. Menghitung jarak menuju tujuan

```cpp
const double dx = tujuan.x - pose_.x;
const double dy = tujuan.y - pose_.y;
const double jarak = std::hypot(dx, dy);
```

Jarak dihitung menggunakan teorema Pythagoras:

```text
jarak = sqrt(dx² + dy²)
```

Nilai `jarak` menunjukkan seberapa jauh kura-kura dari titik tujuan.

#### c. Memeriksa apakah titik sudah tercapai

```cpp
if (jarak < toleransi_) {
  break;
}
```

Program menggunakan toleransi:

```cpp
const double toleransi_ = 0.03;
```

Jika jarak ke titik tujuan kurang dari 0.03, program menganggap titik tersebut sudah tercapai dan melanjutkan ke titik berikutnya.

#### d. Menghitung arah tujuan

```cpp
const double error =
  normalisasi(std::atan2(dy, dx) - pose_.theta);
```

Fungsi `std::atan2(dy, dx)` menghitung arah dari posisi kura-kura menuju titik tujuan.

Selanjutnya, arah tersebut dibandingkan dengan orientasi kura-kura saat ini.

```text
error = arah_tujuan - theta
```

Nilai `error` menunjukkan perbedaan arah hadap dengan arah tujuan.

#### e. Menormalisasi sudut

Fungsi `normalisasi()` memastikan nilai sudut berada di antara `-π` dan `+π`.

```cpp
static double normalisasi(double a)
{
  while (a > M_PI) {
    a -= 2.0 * M_PI;
  }

  while (a < -M_PI) {
    a += 2.0 * M_PI;
  }

  return a;
}
```

Normalisasi diperlukan karena dua sudut yang berdekatan dapat menghasilkan selisih numerik yang besar jika dihitung secara langsung melewati batas ±π.

Dengan normalisasi, pengendali dapat memilih arah putaran yang lebih pendek.

#### f. Menentukan kecepatan gerak

Program menggunakan aturan kontrol proporsional sederhana.

Jika kesalahan sudut lebih besar dari 0.35 radian:

```cpp
if (std::fabs(error) > 0.35) {
  cmd.angular.z = std::clamp(4.0 * error, -3.0, 3.0);
}
```

Kura-kura berputar di tempat untuk memperbaiki arah hadapnya.

Kecepatan sudut dibatasi antara -3 dan +3 radian per detik.

Jika kesalahan sudut tidak lebih besar dari 0.35 radian:

```cpp
else {
  cmd.linear.x = std::clamp(3.0 * jarak, 0.8, 2.0);
  cmd.angular.z = 6.0 * error;
}
```

Kura-kura bergerak maju sambil terus mengoreksi orientasinya.

Kecepatan maju dibatasi antara 0.8 dan 2.0 satuan per detik. Karena terdapat batas minimum 0.8, kura-kura tidak selalu semakin pelan saat mendekati titik tujuan.

#### g. Mengirimkan perintah gerak

Perintah gerak dikirim menggunakan:

```cpp
pub_->publish(cmd);
```

Perulangan pengendali menggunakan:

```cpp
rclcpp::Rate rate(100);
```

Nilai tersebut mengatur laju perulangan nominal menjadi 100 Hz, atau 100 kali per detik.

Setiap iterasi membaca pose terbaru, menghitung kesalahan, menentukan kecepatan, dan mengirimkan perintah gerak.

Dengan demikian, kura-kura bergerak menuju titik-titik roda gigi menggunakan pembaruan posisi secara berulang, bukan teleportasi.

## Penjelasan Struktur Kode

Seluruh implementasi utama terdapat di `src/gear.cpp`.

### 1. `struct Titik`

```cpp
struct Titik
{
  double x;
  double y;
};
```

`Titik` merupakan struktur sederhana yang menyimpan koordinat horizontal dan vertikal.

Struktur ini memudahkan program menyimpan dan mengirimkan pasangan koordinat sebagai satu objek.

### 2. `class GearTurtle`

```cpp
class GearTurtle : public rclcpp::Node
```

Class `GearTurtle` mewarisi `rclcpp::Node`, sehingga objeknya dapat berfungsi sebagai node ROS 2.

Class ini menggabungkan perhitungan geometri, penerimaan data pose, dan pengendalian gerak kura-kura dalam satu objek.

### 3. Konstruktor `GearTurtle`

Konstruktor menerima tiga parameter:

```cpp
GearTurtle(int jumlah_gigi, double r_dalam, double r_luar)
```

Parameter tersebut menentukan jumlah gigi, jari-jari dasar, dan jari-jari luar roda gigi.

Konstruktor juga membuat publisher:

```cpp
pub_ = create_publisher<geometry_msgs::msg::Twist>(
  "/turtle1/cmd_vel", 10);
```

Publisher mengirimkan perintah kecepatan linear dan sudut.

Subscriber dibuat untuk menerima pose:

```cpp
sub_ = create_subscription<turtlesim::msg::Pose>(
  "/turtle1/pose", 10,
  [this](turtlesim::msg::Pose::SharedPtr msg) {
    pose_ = *msg;
    ada_pose_ = true;
  });
```

Setiap kali pose baru diterima, data disimpan di `pose_` dan penanda `ada_pose_` diubah menjadi `true`.

### 4. `gambarGerigi()`

Method ini merupakan pintu masuk utama untuk proses menggambar.

Urutan kerjanya:

1. Menunggu pose pertama diterima dan publisher memiliki subscriber.
2. Menghitung pusat roda gigi berdasarkan posisi awal kura-kura.
3. Memanggil `hitungTitik()` untuk menghasilkan koordinat roda gigi.
4. Memanggil `jalanKe()` untuk mengunjungi setiap titik.
5. Kembali menuju titik pertama.
6. Mengirimkan perintah kecepatan nol untuk menghentikan gerakan.

### 5. `hitungTitik()`

Method ini menghitung seluruh titik pembentuk roda gigi.

Perhitungan dilakukan dengan perulangan berdasarkan jumlah gigi. Setiap iterasi menghasilkan empat titik utama gigi dan titik-titik tambahan pada celah.

Hasil perhitungan disimpan dalam `std::vector<Titik>`.

Method ini bersifat `const` karena tidak mengubah keadaan objek ketika melakukan perhitungan.

### 6. `polar()`

Method ini mengubah koordinat polar menjadi koordinat Kartesius menggunakan fungsi sinus dan kosinus.

Hasilnya digunakan untuk menentukan posisi setiap titik pada roda gigi.

### 7. `normalisasi()`

Method ini menormalkan selisih sudut ke rentang `-π` sampai `+π`.

Fungsinya membantu pengendali menentukan arah putaran berdasarkan selisih orientasi yang lebih pendek.

### 8. `jalanKe()`

Method ini bertanggung jawab atas gerakan kura-kura.

Prosesnya meliputi:

1. Membaca pose terbaru.
2. Menghitung jarak ke titik tujuan.
3. Memeriksa toleransi pencapaian titik.
4. Menghitung kesalahan orientasi.
5. Menentukan kecepatan linear dan sudut.
6. Mengirimkan perintah melalui publisher.

Proses tersebut diulang sampai titik tujuan dianggap tercapai atau ROS 2 berhenti berjalan.

### 9. `main()`

Fungsi `main()` menginisialisasi ROS 2, membuat objek `GearTurtle`, menjalankan proses menggambar, lalu mengakhiri ROS 2.

```cpp
int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  auto gear = std::make_shared<GearTurtle>(8, 2.2, 3.0);

  gear->gambarGerigi();

  rclcpp::shutdown();
  return 0;
}
```

Parameter `8`, `2.2`, dan `3.0` masing-masing menentukan jumlah gigi, jari-jari dasar, dan jari-jari luar.

## Mengubah Bentuk Roda Gigi

Bentuk roda gigi dapat diubah melalui parameter saat objek dibuat:

```cpp
auto gear = std::make_shared<GearTurtle>(8, 2.2, 3.0);
```

| Parameter | Fungsi |
|---|---|
| `8` | Jumlah gigi |
| `2.2` | Jari-jari lingkaran dasar |
| `3.0` | Jari-jari ujung gigi |

Mengubah jumlah gigi akan mengubah pembagian sudut dan jumlah titik yang dihasilkan.

Mengubah jari-jari dasar dan jari-jari luar akan memengaruhi ukuran serta tinggi gigi.

Pastikan ukuran roda gigi tetap berada dalam batas jendela Turtlesim. Pusat roda gigi juga dihitung berdasarkan pose awal, sehingga perubahan parameter dapat memengaruhi posisi dan ukuran gambar relatif terhadap jendela.

### Mengubah Kehalusan Celah

Di dalam `hitungTitik()`, terdapat parameter:

```cpp
const int bagian_celah = 6;
```

Nilai tersebut menentukan jumlah pembagian sudut celah.

Jika jumlah pembagian ditingkatkan, lengkungan akan didekati menggunakan lebih banyak segmen. Namun, jumlah titik yang dikunjungi juga meningkat sehingga waktu menggambar dapat bertambah.

### Mengubah Kecepatan Gerak

Kecepatan diatur dalam method `jalanKe()`.

Bagian berikut menentukan batas kecepatan maju:

```cpp
cmd.linear.x = std::clamp(3.0 * jarak, 0.8, 2.0);
```

Sementara itu, kecepatan putar saat memperbaiki arah diatur oleh:

```cpp
cmd.angular.z = std::clamp(4.0 * error, -3.0, 3.0);
```

Nilai pengendali dapat disesuaikan, tetapi perubahan perlu diuji untuk memastikan gerakan tetap stabil dan gambar tidak menyimpang terlalu jauh.

### Mengubah Toleransi Pencapaian Titik

Nilai toleransi berada di bagian private class:

```cpp
const double toleransi_ = 0.03;
```

Nilai yang lebih kecil menuntut kura-kura mendekati titik tujuan dengan lebih presisi. Namun, waktu eksekusi dapat meningkat.

Nilai yang lebih besar dapat mempercepat perpindahan antartitik, tetapi berpotensi menghasilkan penyimpangan yang lebih besar pada bentuk gambar.

## Membangun Ulang Setelah Perubahan

Setelah mengubah kode, jalankan:

```bash
cd ~/ros2_ws
colcon build --packages-select gear_turtle
source install/setup.bash
```

Jalankan kembali Turtlesim dan program setelah proses build selesai.



Tambahkan informasi lisensi jika proyek ini akan didistribusikan secara publik. Pastikan lisensi yang dipilih sesuai dengan kebutuhan dan ketentuan dependensi yang digunakan.
