/**
 * @author Markus Bader <markus.bader@mx-robotics.com>
 * @date 15th of February 2021
 **/

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/joystick.h>
#include <future>
#include <mx/joystick.h>

using namespace mx;

Joystick::Joystick() : js_(-1), update_events_(false) {
}

Joystick::Joystick(const std::string &device) : js_(-1), update_events_(false) {
    open(device);
}

int Joystick::open(const std::string &device) {
    js_ = ::open(device.c_str(), O_RDONLY);
    if (js_ == -1) {
        return js_;
    }

    values_.axes.resize(get_axis_count());
    values_.buttons.resize(get_button_count());
    return js_;
}


void Joystick::close() {
    if(js_ != -1)  ::close(js_);
}

Joystick::~Joystick() {
    close();
}

size_t Joystick::get_axis_count(){
    uint8_t nr_of_axes;
    if (ioctl(js_, JSIOCGAXES, &nr_of_axes) == -1) return 0;
    return nr_of_axes;
}

size_t Joystick::get_button_count(){
    uint8_t nr_of_buttons;
    if (ioctl(js_, JSIOCGBUTTONS, &nr_of_buttons) == -1)  return 0;
    return nr_of_buttons;
}



int Joystick::read_events() {
    fd_set set;
    struct timeval timeout;
    int rv;
    
  
    struct js_event event;
    size_t axis;
    size_t bytes;
    while (update_events_)
    {
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        FD_ZERO(&set);      
        FD_SET(js_, &set); 
        rv = select(js_ + 1, &set, NULL, NULL, &timeout);
        if(rv == -1) {
            // perror("select");
            return -1;
        } else if(rv == 0) {
            //printf("timeout");
        } else {
            /// A signal handling needs to be implemented to interrupt read
                bytes = read(js_, &event, sizeof(event));
            if (bytes != sizeof(event) )  return -1;
            values_.event_id++;
            std::scoped_lock<std::mutex> lock(mutex_values_);
            switch (event.type) {
                case JS_EVENT_BUTTON:
                    values_.buttons[event.number] = event.value;
                    break;
                case JS_EVENT_AXIS:
                    values_.axes[event.number] = event.value;
                    break;
            }
        }
    }
    return 0;
}

void Joystick::start() {
    update_events_ = true;
    future_events_ = std::async(std::launch::async, &Joystick::read_events, this);
}

void Joystick::stop() {
    update_events_ = false;
}

Joystick::Values Joystick::values() const {
    // a mutex can help if there are problems
    return values_;
}
Joystick::Values &Joystick::values(Joystick::Values &des) const {
    std::scoped_lock<std::mutex> lock(mutex_values_);
    des = values_;
    return des;
}
int Joystick::get(Joystick::Values &des) const {
    std::scoped_lock<std::mutex> lock(mutex_values_);
    int diff = values_.event_id - des.event_id;
    des = values_;
    return diff;
}

const std::future<int> &Joystick::get_future() {
    return future_events_;
}
