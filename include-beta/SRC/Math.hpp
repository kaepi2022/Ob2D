#ifndef MATH_HPP
#define MATH_HPP

// C++ 標準ライブラリ
#include <iostream>
#include <cmath>
#include <random>
#include <ctime>

// C標準ライブラリ
#include <cstdio>
#include <cstdlib>
#include <ctime>

/**
 * @file math.hpp
 * @brief suugaku数学ライブラリ（ob2d向け統合版）
 * @author MineKura
 * @github https://github.com/kaepi2022/suugaku-c-library/tree/main
 */

namespace ob2d {
namespace suugaku {

    /// @brief 40桁以上の円周率（定数）
    constexpr double PI_SUUGAKU = 3.1415926535897932384626433832795028841971;

    /// @brief 角度（度）→ ラジアンに変換
    inline double to_radian(float deg) {
        return deg * PI_SUUGAKU / 180.0;
    }

    /// @brief π（円周率）を返す
    inline double pi() {
        return PI_SUUGAKU;
    }

    // ---------- ▼ 三角形系 ▼ ----------

    namespace triangle {

        namespace pythagoras {
            /// @brief 直角三角形の a を求める（c^2 - b^2 のルート）
            inline double strtri_a(float b, float c) {
                return std::sqrt(-std::pow(b, 2) + std::pow(c, 2));
            }

            /// @brief 直角三角形の b を求める（c^2 - a^2 のルート）
            inline double strtri_b(float a, float c) {
                return std::sqrt(-std::pow(a, 2) + std::pow(c, 2));
            }

            /// @brief 直角三角形の斜辺 c を求める（a^2 + b^2 のルート）
            inline double strtri_c(float a, float b) {
                return std::sqrt(std::pow(a, 2) + std::pow(b, 2));
            }
        }

        /// @brief 余弦定理を使って辺の長さを求める
        inline double coslaw(float a, float b, float angle_C_deg) {
            return std::sqrt(std::pow(a, 2) + std::pow(b, 2) - 2 * a * b * std::cos(to_radian(angle_C_deg)));
        }

        /// @brief sin(角度[°]) の値を返す
        inline double sin_deg(float deg) {
            return std::sin(to_radian(deg));
        }

        /// @brief cos(角度[°]) の値を返す
        inline double cos_deg(float deg) {
            return std::cos(to_radian(deg));
        }

        /// @brief tan(角度[°]) の値を返す
        inline double tan_deg(float deg) {
            return std::tan(to_radian(deg));
        }

        /// @brief asin(a/c) → 角度[°]
        inline double asin_ratio(float a, float c) {
            return std::asin(a / c) * 180 / PI_SUUGAKU;
        }

        /// @brief acos(b/c) → 角度[°]
        inline double acos_ratio(float b, float c) {
            return std::acos(b / c) * 180 / PI_SUUGAKU;
        }

        /// @brief atan(a/b) → 角度[°]
        inline double atan_ratio(float a, float b) {
            return std::atan(a / b) * 180 / PI_SUUGAKU;
        }
    }

    // ---------- ▼ 自然数関連 ▼ ----------

    namespace nature {

        /// @brief 偶数判定
        inline bool is_even(int n) {
            return n % 2 == 0;
        }

        /// @brief 奇数判定
        inline bool is_odd(int n) {
            return n % 2 != 0;
        }

        /// @brief 素数判定（n < 2 は false）
        inline bool is_prime(int n) {
            if (n < 2) return false;
            for (int i = 2; i < n; ++i) {
                if (n % i == 0) return false;
            }
            return true;
        }

        /// @brief 平方数判定
        inline bool is_square(int n) {
            int root = std::sqrt(n);
            return std::pow(root, 2) == n;
        }

        /// @brief 立方数判定
        inline bool is_cube(int n) {
            int root = std::cbrt(n);
            return std::pow(root, 3) == n;
        }

        /// @brief フィボナッチ数判定（5n²±4が平方数ならtrue）
        inline bool is_fibo(int n) {
            int sqtest1 = 5 * n * n + 4;
            int sqtest2 = 5 * n * n - 4;
            auto is_square = [](int x) {
                int r = std::sqrt(x);
                return r * r == x;
            };
            return is_square(sqtest1) || is_square(sqtest2);
        }
    }

    // ---------- ▼ 乱数関連 ▼ ----------

    namespace random {

        /// @brief 指定範囲 [min, max] の乱数を返す（rand使用）
        inline int rand_int(int min, int max) {
            static bool initialized = false;
            if (!initialized) {
                std::srand(static_cast<unsigned int>(std::time(nullptr)));
                initialized = true;
            }
            return min + (std::rand() % (max - min + 1));
        }

        /// @brief Mersenne Twister による乱数（[min, max]）
        inline int rand_mt(int min, int max) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<int> dist(min, max);
            return dist(gen);
        }
    }

    // ---------- ▼ その他数学系 ▼ ----------

    /// @brief 階乗 n! を求める
    inline long long fact(int n) {
        long long result = 1;
        for (int i = 1; i <= n; ++i) result *= i;
        return result;
    }

    /// @brief 素数階乗 n#（2,3,5,...<n の積）
    inline long long prime_fact(int n) {
        long long result = 1;
        for (int i = 2; i < n; ++i) {
            if (nature::is_prime(i)) result *= i;
        }
        return result;
    }

} // namespace suugaku
} // namespace ob2d

#endif