#include "rix/ipc/file.hpp"
#include <iostream>
namespace rix {
namespace ipc {


/**< TODO */
bool File::remove(const std::string &pathname) {
   int num = ::unlink(pathname.c_str()); //.c_str to convert the string to char bc unlink() takes in const chars
  
   if (num == 0) {
       return true;
   } else {
   return false;
}
}


File::File() : fd_(-1) {}


File::File(int fd) : fd_(fd) {}


/**< TODO */
File::File(std::string pathname, int creation_flags, mode_t mode)
:fd_(-1)
{
   if (creation_flags & O_CREAT) {
   fd_ = ::open(pathname.c_str(), creation_flags, mode);
   } else {
   fd_ = ::open(pathname.c_str(), creation_flags);
   }
}


/**< TODO */
File::File(const File &src) : fd_(-1) {
   if (src.fd_ >= 0) {
   fd_ = ::dup(src.fd_);
   }
}


/**< TODO */
File &File::operator=(const File &src) {
   if(this != &src) {
       if (fd_ >= 0) {
           ::close(fd_);
       }
       if (src.fd_ >= 0) {
           fd_ = ::dup(src.fd_);
       } else {
           fd_ = -1;
       }
   }


   return *this;
}


File::File(File &&src) : fd_(-1) { std::swap(fd_, src.fd_); }


File &File::operator=(File &&src) {
   if (this != &src) { ///new edits here
       if (fd_ >= 0) {
           ::close(fd_);
       }
       fd_ = -1;
       std::swap(fd_, src.fd_);
   }
   return *this;
   /*if (fd_ >= 0) {
       ::close(fd_);
       fd_ = -1;
   }
   std::swap(fd_, src.fd_);
   return *this;
   */
}


/**< TODO */
File::~File() {
   if (fd_ >= 0) {
   ::close(fd_);
   }
}


/**< TODO */
ssize_t File::read(uint8_t *buffer, size_t size) const {
   if (fd_ < 0) {
       return -1;
   }
      return::read(fd_, buffer, size);
}
       //printf("size %ul\n", size);


/**< TODO */
ssize_t File::write(const uint8_t *buffer, size_t size) const {
   if (fd_ < 0) {
       return -1;
   }
   return::write(fd_, buffer, size);


}


int File::fd() const { return fd_; }


/**< TODO */
bool File::ok() const {
   return fd_ >= 0;
  
}


/**< TODO */
void File::set_nonblocking(bool status) {
   if (fd_ < 0) {


       return;
   }
   int flag = ::fcntl(fd_, F_GETFL); //read the current flag


   if (flag == -1) { //on error
       return;


   }
   if (status) {
       flag |= O_NONBLOCK; //enable non blocking
          


   } else {
       flag &= ~O_NONBLOCK; //disable non-blocking
             




   }
   ::fcntl(fd_, F_SETFL, flag);
}






/**< TODO */
bool File::is_nonblocking() const {
   if (fd_ < 0) {
       return false;
   }
   int flag = ::fcntl(fd_, F_GETFL); //check current status
   if (flag == -1) {
       return false;
   }
   return (flag & O_NONBLOCK) != 0;
  
}


/**< TODO */
bool File::wait_for_writable(const util::Duration &duration) const {
   if (fd_ < 0) {
       return false;
   }
   struct pollfd pw;  //a pollfd struct contains a file descriptor, events (input), and revents (output)
       pw.fd = fd_;
       pw.events = POLLOUT; //pollout for writing, telling kernel what your looking for
       pw.revents = 0;


   int time = duration.to_milliseconds();


   int num = ::poll(&pw, 1, time);


   if (num > 0) {
       if (pw.revents & POLLOUT) {
           return true;
       } else {
           return false;
       }
   } else {
       return false;
   }
}


/**< TODO */
bool File::wait_for_readable(const util::Duration &duration) const {
   if (fd_ < 0) {
       return false;
   }
   struct pollfd p;  //a pollfd struct contains a file descriptor, events (input), and revents (output)
       p.fd = fd_;
       p.events = POLLIN; //pollout for writing, telling kernel what your looking for
       p.revents = 0;


   int time = duration.to_milliseconds();


   int num = ::poll(&p, 1, time);
   if (num > 0) {
       if (p.revents & POLLIN) {
           return true;
       } else {
           return false;
       }
   } else {
       return false;
   }
}


}  // namespace ipc
}  // namespace rix



