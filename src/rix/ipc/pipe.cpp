#include "rix/ipc/pipe.hpp"

namespace rix {
namespace ipc {

/**< TODO */ 
std::array<Pipe, 2> Pipe::create() {
    int pipefd[2];

    if (::pipe(pipefd) == -1) {
        return { Pipe(), Pipe() };
    }

    return {
        Pipe(pipefd[0], true), //read end
        Pipe(pipefd[1], false) //write end
    };
}

Pipe::Pipe() : File(), read_end_(false) {}

/**< TODO */
Pipe::Pipe(const Pipe &other) 
    : File(other), //start with no fd
    read_end_(other.read_end_) //copy read/write role
{}

/**< TODO */
Pipe &Pipe::operator=(const Pipe &other) {
    if (this == &other) {
        return *this;
    }
    File::operator=(other);
    read_end_ = other.read_end_;
    return *this;
    }


Pipe::Pipe(Pipe &&other) : File(std::move(other)), read_end_(other.read_end_) {}

Pipe &Pipe::operator=(Pipe &&other) {
    if (this == &other) {
        return *this;
    }
    File::operator=(std::move(other));
    read_end_ = other.read_end_;

    return *this;
}

Pipe::~Pipe() = default;

bool Pipe::is_read_end() const { return read_end_; }

bool Pipe::is_write_end() const { return !read_end_; }

Pipe::Pipe(int fd, bool read_end) : File(fd), read_end_(read_end) {}

}  // namespace ipc
}  // namespace rix