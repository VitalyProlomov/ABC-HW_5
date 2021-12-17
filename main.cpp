#include <iostream>
#include <thread>
#include <ctime>
#include <atomic>


std::atomic<int> total_usharpened_needles(0);
std::atomic<int> total_unqualified_needles(0);

std::atomic<int> checked_undefected_needles(0);
std::atomic<int> sharpened_unqualified_needles(0);

std::atomic<int> total_needles_produced(0);
// Goal of the 1st worker - checking needle`s curve.
void CheckNeedle(int amount_of_needles) {
    // Probability of random needle being defected. (such needles get thrown away immediately.
    const double DEFECT_PROBABILITY = 0.01;

    // The most responsible worker - he doesn`t distract and work all the time, passing checked needles to the next worker.
    for (int i = 0; i < amount_of_needles; ++i) {
        double outcome = 1 + rand() % 100;
        // Needle has a defect.
        if (outcome/100 == DEFECT_PROBABILITY) {
            --total_usharpened_needles;
            --total_unqualified_needles;

            std::cout << "WORKER1: needle was defected, so I threw it away" << std::endl;
        } else {
            ++checked_undefected_needles;
            std::cout << "WORKER1: needle is fine, passing to worker2." << std::endl;
        }
    }
}

// Task of the 2nd worker - he is sharpening the needle.
void SharpenNeedle() {
    while (total_usharpened_needles > 0) {
        if (checked_undefected_needles > 0) {
            --total_usharpened_needles;
            --checked_undefected_needles;
            ++sharpened_unqualified_needles;

            std::cout << "WORKER2: Needle is sharpened, passing to worker3" << std::endl;
        } else {
            // If the worker can`t find a needle by his hand he gets distracted for some time and starts observing the ceiling.
            std::cout << "WORKER2[GOT DISTRACTED]: What a nice ceiling.." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(3));
        }
    }
}

// Task of the 3rd worker - quality assurance.
void EvaluateNeedleQuality() {
    const int LEAST_ACCEPTED_QUALITY_MARK = 4;

    while (total_unqualified_needles > 0) {
        if (sharpened_unqualified_needles > 0) {
            --total_unqualified_needles;
            --sharpened_unqualified_needles;

            int quality_mark = 1 + rand() % 100;

            // Checking quality of a needle and throwing it away if its quality mark is not high enough.
            if (quality_mark > LEAST_ACCEPTED_QUALITY_MARK) {
                ++total_needles_produced;
                std::cout << "WORKER3: Needle is good" << std::endl;
            } else {
                std::cout << "WORKER3: This needle is crap, we are not taking it." << std::endl;
            }
        } else {
            // 3rd worker is even more lazy than the 2nd one and spends even more time being distracted, but unlike his partner,
            // he is observing his shoes instead of ceiling.
            std::cout << "WORKER3[GOT DISTRACTED]: I shouldn`t have bought these shoes.." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "There must be 1 argument - it could be '0' for random generation, or any other"
                     " positive integer number for input from the console.";
        return 1;
    }
    std::string parameters(argv[1]);
    int input;
    try {
        input = std::stoi(parameters);
    } catch (std::exception ex) {
        std::cout << "Please enter a valid number";
        return 2;
    }
    if (input < 0) {
        std::cout << "integer must be > 0";
        return 3;
    }

    std::srand(std::time(nullptr));
    int amount_of_needles;
    if (input == 0) {
        // Random generating of needles amount. (multiplying is needed for less predictable generating
        // of values). Adding 1, so that amount of needles is always positive.
        amount_of_needles = 1 + (rand() * 9000) % 10000;
    } else {
        amount_of_needles = input;
    }

    total_usharpened_needles = amount_of_needles;
    total_unqualified_needles = amount_of_needles;

    checked_undefected_needles = 0;
    sharpened_unqualified_needles = 0;

    std::thread worker1(CheckNeedle, amount_of_needles);
    std::thread worker2(SharpenNeedle);
    std::thread worker3(EvaluateNeedleQuality);

    worker1.join();
    worker2.join();
    worker3.join();

    std::cout << "Initial amount of needles: " << amount_of_needles << std::endl;
    std::cout << "Needles that underwent all stages: " << total_needles_produced;
    return 0;
}
