#include "rix/ipc/signal.hpp"

#include <iostream>
#include <fcntl.h>



namespace rix {
namespace ipc {

std::array<Signal::SignalNotifier, 32> Signal::notifier = {};
//initilize the static array aof motifiers

/**< TODO */
Signal::Signal(int signum) : signum_(signum - 1) {
    //validate signal number
    if (signum < 1 || signum > 32) {
        throw std::invalid_argument("signal number must be between 1 and 32");
    }
        SignalNotifier &n = notifier[signum_];

        //initialize the notifier pipe once
        if (!n.is_init) {
            n.pipe = Pipe::create();
            // Set the read end to non-blocking mode
            int flags = fcntl(n.pipe[0].fd(), F_GETFL, 0);
            fcntl(n.pipe[0].fd(), F_SETFL, flags | O_NONBLOCK);
            n.is_init = true;

            //register the static signal handler
            ::signal(signum, Signal::handler);
        } else {
            // Signal is already being managed by another Signal object
            throw std::invalid_argument("signal number is already in use");
        }
}

/**< TODO */  //destructor
Signal::~Signal() {
    if (signum_ >= 0 && signum_ < 32) {
        SignalNotifier &n = notifier[signum_];

        if(n.is_init) {
            //reset signal abehavior to default
            ::signal(signum_ + 1, SIG_DFL);
            n.is_init = false;
            //rest pipe
            n.pipe = {};
            //

        }
    }
}

Signal::Signal(Signal &&other) : signum_(-1) { std::swap(signum_, other.signum_); }

Signal &Signal::operator=(Signal &&other) {
    if (signum_ >= 0 && signum_ < 32) {
        if (notifier[signum_].is_init) {
            notifier[signum_].is_init = false;
            notifier[signum_].pipe = {};
            ::signal(signum_+1, SIG_DFL);
        }
    }
    signum_ = -1;
    std::swap(signum_, other.signum_);
    return *this;
}

/**< TODO */
bool Signal::raise() const { 
    if (signum_ < 0 || signum_ >= 32) 
    return false;


    return ::raise(signum_ + 1) == 0; 
}

/**< TODO */
bool Signal::kill(pid_t pid) const {
    if (signum_ < 0 || signum_ >= 32) 
    return false;
    return ::kill(pid, signum_ + 1) == 0;
}

int Signal::signum() const { return (signum_ < 0 || signum_ >= 32) ? -1 : signum_ + 1; }

/**< TODO */
bool Signal::wait(const rix::util::Duration &d) const {
    if (signum_ < 0 || signum_ >= 32) 
    return false;

    const SignalNotifier &n = notifier[signum_];
    if (!n.is_init) return false;

    //read from the pipe with timeout
    uint8_t buf;
    int fd = n.pipe[0].fd();
    auto start = std::chrono::steady_clock::now();
    long timeout_ms = d.to_milliseconds();

    while(true) {
        ssize_t bytes = ::read(fd, &buf, 1);
        if (bytes == 1) return true;
        if (bytes < 0 && errno != EAGAIN && errno != EWOULDBLOCK) return false;

        auto elapsed = std::chrono::steady_clock::now() - start;
        auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
        
        if (elapsed_ms >= timeout_ms) {
            return false;
        }
        usleep(1000);
    }
}

/**< TODO */
void Signal::handler(int signum) 
{
    if (signum < 1 || signum > 32) return;

    SignalNotifier &n = notifier[signum - 1];

    if (n.is_init) {
        uint8_t b = 1;
        ssize_t result = ::write(n.pipe[1].fd(), &b, 1);
        (void) result;
    }
}

}  // namespace ipc
}  // namespace rix