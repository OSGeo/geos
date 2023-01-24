// tut
#include <tut/tut.hpp>
// geos
#include <geos/util/Interrupt.h>
// std
#include <chrono>
#include <functional>
#include <thread>

using geos::util::Interrupt;

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_interrupt_data {
    static void workForever() {
        try {
            std::cerr << "Started " << std::this_thread::get_id() << "." << std::endl;
            while (true) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                GEOS_CHECK_FOR_INTERRUPTS();
            }
        } catch (const std::exception&) {
            std::cerr << "Interrupted " << std::this_thread::get_id() << "." << std::endl;
            return;
        }
    }

    static void interruptNow() {
        Interrupt::request();
    }

    static std::map<std::thread::id, bool>* toInterrupt;

    static void interruptIfRequested() {
        if (toInterrupt == nullptr) {
            return;
        }

        auto it = toInterrupt->find(std::this_thread::get_id());
        if (it != toInterrupt->end() && it->second) {
            it->second = false;
            Interrupt::requestForCurrentThread();
        }
    }
};

std::map<std::thread::id, bool>* test_interrupt_data::toInterrupt = nullptr;

typedef test_group<test_interrupt_data> group;
typedef group::object object;

group test_interrupt_group("geos::util::Interrupt");

//
// Test Cases
//


// Interrupt worker thread via global request from from main thead
template<>
template<>
void object::test<1>
()
{
    std::thread t(workForever);
    Interrupt::request();

    t.join();
}

// Interrupt worker thread via thread-specific request from worker thread using a callback
template<>
template<>
void object::test<2>
()
{
    Interrupt::registerCallback(interruptIfRequested);

    std::thread t1(workForever);
    std::thread t2(workForever);

    // Create map and add entries before exposing it to the interrupt
    // callback that will be acessed from multiple threads. It's OK
    // for multiple threads to modify entries in the map but not for
    // multiple threads to create entries.
    std::map<std::thread::id, bool> shouldInterrupt;
    shouldInterrupt[t1.get_id()] = false;
    shouldInterrupt[t2.get_id()] = false;
    toInterrupt = &shouldInterrupt;

    shouldInterrupt[t2.get_id()] = true;
    t2.join();

    shouldInterrupt[t1.get_id()] = true;
    t1.join();
}

// Register separate callbacks for each thread. Each callback will
// request interruption of itself only.
template<>
template<>
void object::test<3>
()
{
    bool interrupt1 = false;
    int numCalls2 = 0;

    auto cb1 = ([](void* data) {
        if (*static_cast<bool*>(data)) {
            Interrupt::requestForCurrentThread();
        }
    });

    auto cb2 = ([](void* data) {
        if (++*static_cast<int*>(data) > 5) {
            Interrupt::requestForCurrentThread();
        }
    });


    std::thread t1([&cb1, &interrupt1]() {
        Interrupt::registerThreadCallback(cb1, &interrupt1);
    });

    std::thread t2([&cb2, &numCalls2]() {
        Interrupt::registerThreadCallback(cb2, &numCalls2);
    });

    t2.join();

    interrupt1 = true;
    t1.join();
}

} // namespace tut

