# include <mutex>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <algorithm>

std::mutex report_mutex;
std::mutex report_mutex1;
std::mutex report_mutex2;
std::mutex report_mutex3;

// Структура, описывающая гостя.
struct visitor {
    int user_name;
    // Чекер, проверяющий был ли гость уже в гостинице.
    bool visited;

    explicit visitor(int i) {
        this->user_name = i;
        this->visited = false;
    }
};

bool CheckIn(int i);

void CheckOut(int i);

// Количество номеров в гостинице.
int number = 30;
// Время сна.
int sleep_time = 5;

// Метод, печатающий информацию в консоль.
void printRes(const std::string &command) {
    std::lock_guard<std::mutex> locked(report_mutex3);
    std::cout << command;
}

void HotelManager(int user_name) {
    auto *i = new visitor(user_name);
    while (!i->visited) {
        if (CheckIn(i->user_name)) {
            printRes("Гость № " + std::to_string(i->user_name) + " спит в гостинице\n");
            std::this_thread::sleep_for(std::chrono::seconds(sleep_time));
            CheckOut(i->user_name);
            i->visited = true;
        } else {
            printRes("Гость № " + std::to_string(i->user_name) + " спит на улице\n");
            std::this_thread::sleep_for(std::chrono::seconds(sleep_time));
            // Гости ожидают некоторое время, чтобы гости с гостиницы спели выселиться.
            printRes("Гость № " + std::to_string(i->user_name) + " ждет осовбождения номеров\n");
            std::this_thread::sleep_for(std::chrono::seconds(sleep_time));
        }
    }
}

// Изменение числа свободных номеров.
int changeNumber(int size, int delta) {
    std::lock_guard<std::mutex> locked(report_mutex1);
    return size + delta;
}

// Выселение человека с номером i.
void CheckOut(int i) {
    std::lock_guard<std::mutex> locked(report_mutex2);
    number = changeNumber(number, 1);
    printRes("Гость № " + std::to_string(i) + " покинул гостиницу. Свободно мест - " + std::to_string(number) + "\n");
}

// Заселение гостя №i.
bool CheckIn(int i) {
    std::lock_guard<std::mutex> locked(report_mutex);
    if (number > 0) {
        number = changeNumber(number, -1);
        printRes("Гость № " + std::to_string(i) + " заселился. Свободно мест - " + std::to_string(number) + "\n");
        return true;
    } else {
        printRes("Гостиница заполнена. Места для гостя № " + std::to_string(i) + " нет\n");
        return false;
    }
}

int main() {
    srand(time(NULL));
    int count = rand() % 1000 + 1;
    std::vector<std::thread> threadList;
    std::cout << "Общее число гостей: " << count << "\n" << "\n";
    for (int i(0); i < count; ++i)
        threadList.emplace_back(HotelManager, i);//тут передаем функцию и аргументы по надобности
    std::for_each(threadList.begin(), threadList.end(), std::mem_fn(&std::thread::join));
}
