#include "mbot_driver/mbot_driver.hpp"
//#include <cstring>
//#include <vector>
//#include "rix/util/time.hpp"


using namespace rix::ipc;
using namespace rix::msg;
//using namespace rix::util;


MBotDriver::MBotDriver(std::unique_ptr<interfaces::IO> input, std::unique_ptr<MBotBase> mbot)
   : input(std::move(input)), mbot(std::move(mbot)) {}


void MBotDriver::spin(std::unique_ptr<interfaces::Notification> notif) {
   /* TODO */
   //1. loop until SIGINT
   //2. read next 4 bytes
   //3. read serialized message
   //4. deserialize into Twist2DStamp
   //5. send the drive command to the Mbot
   //6. repeat until stop
   //7. send drive(0, 0, 0) to stop robot
   standard::UInt32 msg_size; //buffer for reading message size


   while (true) {
       //exit if SIGINT
       if (notif && notif->is_ready()) {
           break;
       }
  
   //read next 4 bytes
   size_t offset = 0;
   uint8_t size_buf[sizeof(uint32_t)];


   ssize_t n = input->read(size_buf, sizeof(uint32_t));
   if (n <= 0) {
       continue;
   }
   //deserialize msg size
   offset = 0;
   if (!msg_size.deserialize(size_buf, sizeof(uint32_t), offset)) {
       continue;
   }
   uint32_t payload_size = msg_size.data;


   //read serialized Twist2DStamped
   std::vector<uint8_t> msg_buf(payload_size);
   size_t bytes_read = 0;


   while (bytes_read < payload_size) {
       ssize_t r = input->read( msg_buf.data() + bytes_read, payload_size - bytes_read);


       if (r <= 0) {
           break;
       }
       bytes_read += r;
   }
   if (bytes_read != payload_size) {
       continue;
   }
   //deserialize into Twist2Dstamped
   geometry::Twist2DStamped twist_msg;
   offset = 0;

   if (!twist_msg.deserialize(msg_buf.data(), msg_buf.size(), offset)) {
       continue;
   }
   //send drive command to mbot
   mbot->drive(twist_msg);
   }

   geometry::Twist2DStamped stop_msg;

   stop_msg.twist.vx = 0.0;
   stop_msg.twist.vy = 0.0;
   stop_msg.twist.wz = 0.0;

   mbot->drive(stop_msg);
}
