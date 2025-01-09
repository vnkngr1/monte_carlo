#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>

// Константы для параметров моделирования
const int DAYS = 100;              // Количество дней моделирования
const int ITERATIONS = 2000;       // Количество итераций Монте-Карло
const double POPULATION = 47500000; // Население Испании
const int INITIAL_INFECTED = 100;  // Начальное количество инфицированных людей
const double INFECTION_DURATION = 7.0; // Средняя продолжительность инфекции в днях
const double R0_MEAN = 2.5;        // Средний базовый коэффициент репродукции (R₀)
const double R0_STD = 0.2;         // Стандартное отклонение для R₀
const double CONTROL_EFFECTIVENESS = 0.3; // Эффективность мер контроля (уменьшение R₀)

// Функция для генерации случайного числа с нормальным распределением
double random_normal(double mean, double stddev) {
    static std::random_device rd; // Источник случайных чисел
    static std::mt19937 gen(rd()); // Генератор случайных чисел
    std::normal_distribution<> dist(mean, stddev); // Нормальное распределение с заданным средним и отклонением
    return dist(gen); // Возвращает случайное число
}

int main() {
    // Вектор для хранения пиков инфицированных для каждой итерации
    std::vector<double> peak_infections;

    // Начало цикла Монте-Карло для моделирования эпидемии
    for (int iter = 0; iter < ITERATIONS; ++iter) {
        // Генерация случайного значения R₀ с учетом эффективности контроля
        double R0 = random_normal(R0_MEAN, R0_STD) * (1 - CONTROL_EFFECTIVENESS);

        // Вычисление скорости передачи инфекции (β)
        double beta = R0 / INFECTION_DURATION;

        // Скорость выздоровления (γ)
        double gamma = 1 / INFECTION_DURATION;

        // Инициализация начальных значений S (восприимчивые), I (инфицированные), R (выздоровевшие)
        double susceptible = POPULATION - INITIAL_INFECTED; // Восприимчивое население
        double infected = INITIAL_INFECTED;                // Инфицированные на начальный момент
        double recovered = 0;                              // Выздоровевшие (начальное значение 0)

        // Вектор для хранения числа инфицированных по дням
        std::vector<double> daily_infected;

        // Моделирование на каждый день
        for (int day = 0; day < DAYS; ++day) {
            // Расчёт новых инфицированных за день
            double new_infected = beta * susceptible * infected / POPULATION;

            // Расчёт выздоровевших за день
            double new_recovered = gamma * infected;

            // Обновление числа восприимчивых, инфицированных и выздоровевших
            susceptible -= new_infected;      // Уменьшение восприимчивых
            infected += new_infected - new_recovered; // Обновление числа инфицированных
            recovered += new_recovered;      // Увеличение выздоровевших

            // Сохранение текущего числа инфицированных
            daily_infected.push_back(infected);
        }

        // Нахождение максимального числа инфицированных (пик) за период моделирования
        double peak = *std::max_element(daily_infected.begin(), daily_infected.end());
        peak_infections.push_back(peak); // Сохранение пика для данной итерации
    }

    // Расчёт среднего пика инфицированных
    double peak_mean = std::accumulate(peak_infections.begin(), peak_infections.end(), 0.0) / peak_infections.size();

    // Расчёт стандартного отклонения пиков
    double sq_sum = std::inner_product(peak_infections.begin(), peak_infections.end(), peak_infections.begin(), 0.0);
    double peak_std = std::sqrt(sq_sum / peak_infections.size() - peak_mean * peak_mean);

    // Вывод результатов
    std::cout << "Средний пик инфицированных: " << peak_mean << std::endl; // Средний пик
    std::cout << "Стандартное отклонение пиков: " << peak_std << std::endl; // Стандартное отклонение

    return 0;
}