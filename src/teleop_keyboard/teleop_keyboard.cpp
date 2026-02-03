#include <teleop_keyboard/teleop_keyboard.hpp>

TeleopKeyboard::TeleopKeyboard(std::unique_ptr<rix::ipc::interfaces::IO> input,
                               std::unique_ptr<rix::ipc::interfaces::IO> output, double linear_speed,
                               double angular_speed)
    : input(std::move(input)), output(std::move(output)), linear_speed(linear_speed), angular_speed(angular_speed) {}

void TeleopKeyboard::spin(std::unique_ptr<rix::ipc::interfaces::Notification> notif) {
    
    /* TODO */

    uint32_t seq = 0;

    while (true) {
        if (notif && notif->is_ready()) {
            break;
        }

        //read one charactor from input
        uint8_t key;
        ssize_t n = input->read(&key, 1);

        //EOF so exit
        if (n <= 0) {
            return;
        }

        geometry::Twist2DStamped msg;
        bool valid = true;

        //mpa key to twist
        switch (key) {
            
            case 'w':
            msg.twist.vx = +linear_speed;
            break;

            
            case 'a':
            msg.twist.vy = +linear_speed;
            break;

            
            case 's':
            msg.twist.vx = -linear_speed;
            break;

            
            case 'd':
            msg.twist.vy = -linear_speed;
            break;

            
            case 'q':
            msg.twist.wz = +angular_speed;
            break;

            
            case 'e':
            msg.twist.wz = -angular_speed;
            break;

            case ' ':
            break;

            default:
            valid = false;
        }

        if (!valid) {
            continue;
        }

        msg.header.seq = seq++;
        msg.header.frame_id = "mbot";

        auto now = Time::now();
        msg.header.stamp.sec = now.to_milliseconds();

        //serelize msg
        size_t msg_size = msg.size();

        standard::UInt32 size_msg;
        size_msg.data = static_cast<uint32_t>(msg_size);

        //4 bytes 
        uint8_t size_buf[sizeof(uint32_t)];
        size_t offset = 0;
        size_msg.serialize(size_buf, offset);
        output->write(size_buf, sizeof(uint32_t));

        //write sereialized twist2dstamped 
        std::vector<uint8_t> msg_buf(msg_size);
        offset = 0;
        msg.serialize(msg_buf.data(), offset);
        output->write(msg_buf.data(), msg_buf.size());
        

    }
}