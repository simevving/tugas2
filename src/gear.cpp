#include <algorithm>
#include <chrono>
#include <cmath>
#include <memory>
#include <thread>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "turtlesim/msg/pose.hpp"

using namespace std::chrono_literals;

// Pembungkus satu titik (x, y)
struct Titik
{
  double x;
  double y;
};

class GearTurtle : public rclcpp::Node
{
public:
  GearTurtle(int jumlah_gigi, double r_dalam, double r_luar)
  : Node("gear_turtle"),
    jumlah_gigi_(jumlah_gigi),
    r_dalam_(r_dalam),
    r_luar_(r_luar)
  {
    pub_ = create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
    sub_ = create_subscription<turtlesim::msg::Pose>(
      "/turtle1/pose", 10,
      [this](turtlesim::msg::Pose::SharedPtr msg) {
        pose_ = *msg;
        ada_pose_ = true;
      });
  }

  // Satu-satunya method public: gambar seluruh gerigi sambil berjalan
  void gambarGerigi()
  {
    // Tunggu turtlesim siap: posisi sudah diterima dan cmd_vel sudah ada yang mendengar
    while (rclcpp::ok() && (!ada_pose_ || pub_->get_subscription_count() == 0)) {
      rclcpp::spin_some(shared_from_this());
      std::this_thread::sleep_for(10ms);
    }

    // Geser pusat gerigi supaya titik pertamanya persis di posisi kura-kura sekarang
    const double langkah = 2.0 * M_PI / jumlah_gigi_;
    const double sudut_awal = -0.25 * langkah;
    pusat_x_ = pose_.x - r_dalam_ * std::cos(sudut_awal);
    pusat_y_ = pose_.y - r_dalam_ * std::sin(sudut_awal);

    std::vector<Titik> titik = hitungTitik();

    for (size_t i = 0; i < titik.size(); i++) {
      jalanKe(titik[i]);
    }
    jalanKe(titik.front());   // tutup kembali ke titik awal

    pub_->publish(geometry_msgs::msg::Twist());   // berhenti
  }

private:
  // Koordinat polar (r, sudut) menjadi koordinat x, y
  Titik polar(double r, double sudut) const
  {
    return {pusat_x_ + r * std::cos(sudut), pusat_y_ + r * std::sin(sudut)};
  }

  // Hitung semua titik gerigi: 4 titik gigi + 5 titik celah per gigi
  std::vector<Titik> hitungTitik() const
  {
    std::vector<Titik> hasil;
    const double langkah = 2.0 * M_PI / jumlah_gigi_;   // jatah sudut tiap gigi
    const double setengah_dasar = 0.25 * langkah;       // setengah lebar gigi di dasar
    const double setengah_puncak = 0.15 * langkah;      // setengah lebar gigi di puncak
    const int bagian_celah = 6;                         // celah dipotong 6 ruas

    for (int i = 0; i < jumlah_gigi_; i++) {
      const double pusat = i * langkah;                 // sudut tengah gigi ke-i

      hasil.push_back(polar(r_dalam_, pusat - setengah_dasar));
      hasil.push_back(polar(r_luar_, pusat - setengah_puncak));
      hasil.push_back(polar(r_luar_, pusat + setengah_puncak));
      hasil.push_back(polar(r_dalam_, pusat + setengah_dasar));

      const double awal = pusat + setengah_dasar;
      const double akhir = pusat + langkah - setengah_dasar;
      for (int k = 1; k < bagian_celah; k++) {
        hasil.push_back(polar(r_dalam_, awal + (akhir - awal) * k / bagian_celah));
      }
    }
    return hasil;
  }

  // Jaga selisih sudut tetap di antara -pi dan +pi
  static double normalisasi(double a)
  {
    while (a > M_PI) {a -= 2.0 * M_PI;}
    while (a < -M_PI) {a += 2.0 * M_PI;}
    return a;
  }

  // Jalankan kura-kura ke titik tujuan dengan umpan balik posisi (pose)
  void jalanKe(Titik tujuan)
  {
    rclcpp::Rate rate(100);   // ulangi 100 kali per detik

    while (rclcpp::ok()) {
      rclcpp::spin_some(shared_from_this());   // ambil pose terbaru

      const double dx = tujuan.x - pose_.x;
      const double dy = tujuan.y - pose_.y;
      const double jarak = std::hypot(dx, dy);
      if (jarak < toleransi_) {
        break;   // sudah sampai
      }

      const double error = normalisasi(std::atan2(dy, dx) - pose_.theta);

      geometry_msgs::msg::Twist cmd;
      if (std::fabs(error) > 0.35) {
        // arah masih jauh meleset: putar di tempat dulu
        cmd.angular.z = std::clamp(4.0 * error, -3.0, 3.0);
      } else {
        // arah sudah hampir benar: maju sambil terus meluruskan arah
        cmd.linear.x = std::clamp(3.0 * jarak, 0.8, 2.0);
        cmd.angular.z = 6.0 * error;
      }
      pub_->publish(cmd);
      rate.sleep();
    }
  }

  int jumlah_gigi_;
  double r_dalam_;
  double r_luar_;
  double pusat_x_ = 0.0;
  double pusat_y_ = 0.0;
  const double toleransi_ = 0.03;   // dianggap sampai kalau jaraknya kurang dari ini

  turtlesim::msg::Pose pose_;
  bool ada_pose_ = false;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_;
  rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr sub_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto gear = std::make_shared<GearTurtle>(8, 2.2, 3.0);   // 8 gigi, r dalam 2.2, r luar 3.0
  gear->gambarGerigi();
  rclcpp::shutdown();
  return 0;
}
