#include <teleop_keyboard/teleop_keyboard.hpp>

TeleopKeyboard::TeleopKeyboard(std::unique_ptr<rix::ipc::interfaces::IO> input,
                               std::unique_ptr<rix::ipc::interfaces::IO> output, double linear_speed,
                               double angular_speed)
    : input(std::move(input)), output(std::move(output)), linear_speed(linear_speed), angular_speed(angular_speed) {}

void TeleopKeyboard::spin(std::unique_ptr<rix::ipc::interfaces::Notification> notif) {
    
    /* TODO */

    uint32_t seq = 0; //sequence number for msgs, increments with each new message

    //infinite loop - runs until notification signals SIGINT or EOF
    while (true) {
        if (notif && notif->is_ready()) {
            break; //exit clean without sending anything else
        }

        //read one charactor from input stream
        uint8_t key;
        ssize_t n = input->read(&key, 1); //n = number of bytes read

        //if EOF or errors, exit function
        if (n <= 0) {
            return;
        }

        //create new twist2dstamped msg to store the command
        geometry::Twist2DStamped msg;
        //flag to rack if the key pressed maps to a valid command
        bool valid = true;

        //map the key to corresponding twist command
        switch (key) {
            case 'w': //forward
            msg.twist.vx = +linear_speed; //move forward in x
            break;

            case 'a': //left
            msg.twist.vy = +linear_speed; //move left in y
            break;

            case 's': //backward
            msg.twist.vx = -linear_speed; //move back in x
            break;
         
            case 'd': //right
            msg.twist.vy = -linear_speed; //move right in y
            break;
            
            case 'q': //rotate counterclockwise
            msg.twist.wz = +angular_speed; //positive angular velocity
            break;

            case 'e': //rotate clockwise
            msg.twist.wz = -angular_speed; //neg angular velocity
            break;

            case ' ': //space = stop
            break;

            default: //ignore any other key
            valid = false;
        }

        if (!valid) { //skip iteration if key is invalid
            continue;
        }
        //msg header
        msg.header.seq = seq++; //incrementing sequence number
        msg.header.frame_id = "mbot"; //frame name for robot

        //set timestamp in msec using current line
        auto now = Time::now();
        msg.header.stamp.sec = now.to_milliseconds();

        //serelize msg to byte array
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