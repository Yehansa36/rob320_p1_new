#include "rix/ipc/fifo.hpp"

namespace rix {
namespace ipc {

/**< TODO */
Fifo::Fifo(const std::string &pathname, Mode mode, bool nonblocking) 
    : pathname_(pathname), mode_(mode)
{   //create fifo file if it doesnt exist
    mkfifo(pathname.c_str(), 0666);
    
    int flags = 0;
    if (mode == Mode::READ) {
        flags = O_RDONLY;
    } else {
        flags = O_WRONLY;
    }

    if (nonblocking) {
        flags |= O_NONBLOCK;
    }

    int fd = ::open(pathname.c_str(), flags);
    if (fd < 0) { //open failed meaning invalid fifo
        fd_ = -1;
        pathname_ = "";
        mode_ = Mode::READ;
    } else {
        fd_ = fd; //pass fd to base file
    }
}

Fifo::Fifo() {}
//Fifo::Fifo() : File(), mode_(Mode::READ), pathname_("") {}

/**< TODO */
Fifo::Fifo(const Fifo &other) 
    : File(other), //file duplicates fd
    pathname_(other.pathname_),
    mode_(other.mode_) 
{}

/**< TODO */ //copy assignment
Fifo &Fifo::operator=(const Fifo &other) {
    if (this == &other) return*this;

    File::operator=(other);
    pathname_ = other.pathname_;
    mode_ = other.mode_;
    return *this;
}

Fifo::Fifo(Fifo &&other)
    : File(std::move(other)),
      pathname_(std::move(other.pathname_)),
      mode_(std::move(other.mode_)) {
}

Fifo &Fifo::operator=(Fifo &&other) {
    if (fd_ > 0) {
        ::close(fd_);
        fd_ = -1;
        pathname_ = "";
    }
    std::swap(fd_, other.fd_);
    pathname_ = std::move(other.pathname_);
    mode_ = std::move(other.mode_);
    return *this;
}

Fifo::~Fifo() {}

std::string Fifo::pathname() const { return pathname_; }

Fifo::Mode Fifo::mode() const { return mode_; }

}  // namespace ipc
}  // namespace rix