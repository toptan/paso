#include <chrono>
#include <functional>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace std::chrono;

const vector<string> prezimena{
    "Јовановић", "Петровић",   "Николић",    "Марковић",
    "Ђорђевић",  "Стојановић", "Илић",       "Станковић",
    "Павловић",  "Милошевић",  "Танасковић", "Васић",
    "Милићевић", "Бранковић",  "Поповић",    "Живановић"};

const vector<string> imena{
    "Јелена",  "Марија",     "Мирјана", "Драгана", "Снежана",
    "Љиљана",  "Ивана",      "Гордана", "Милан",   "Дејан",
    "Душан",   "Горан",      "Марко",   "Милош",   "Никола",
    "Зоран",   "Александар", "Ана",     "Милица",  "Драган",
    "Топлица", "Даница",     "Дарко",   "Лука",    "Невена"};

int main(int argc, char *argv[]) {
    default_random_engine index_year_engine(
        system_clock::now().time_since_epoch().count());
    default_random_engine index_number_engine(
        system_clock::now().time_since_epoch().count());
    default_random_engine year_engine(
        system_clock::now().time_since_epoch().count());

    uniform_int_distribution<int> index_year_distribution(1996, 2015);
    uniform_int_distribution<int> index_number_distribution(1, 600);
    uniform_int_distribution<int> year_distribution(1, 5);

    auto index_year_dice = bind(index_year_distribution, index_year_engine);
    auto index_number_dice =
        bind(index_number_distribution, index_number_engine);
    auto year_dice = bind(year_distribution, year_engine);

    for (auto prezime = 0; prezime < prezimena.size(); prezime++) {
        for (auto ime = 0; ime < imena.size(); ime++) {
            auto index_year = index_year_dice();
            auto index_number = index_number_dice();
            auto year = year_dice();
            if (index_year + year > 2016) {
                year = 2016 - index_year;
            }
            cout << index_year << "/" << setfill('0') << setw(4) << index_number
                 << "," << prezimena[prezime] << "," << imena[ime] << ",,"
                 << year << endl;
        }
    }
    return 0;
}
