#pragma once

namespace challenge {

// assume the calling thread already has ownership of the mutex 
struct adopt_lock_t {
    explicit adopt_lock_t() = default;
};

template< class Mutex >
class lock_guard {
public:
    typedef Mutex mutex_type;

    // c-tors according https://en.cppreference.com/w/cpp/thread/lock_guard/lock_guard
    explicit lock_guard( mutex_type& m ) : _m(m) {
        m.lock(); 
    }

    lock_guard( mutex_type& m, adopt_lock_t) : _m(m) {}

    // disable copy constructor and operator=
    lock_guard(const lock_guard&) = delete;
    lock_guard&  operator=(const lock_guard&) = delete;


    ~lock_guard() {
        _m.unlock();
    }
private:
    mutex_type& _m; 
};
}