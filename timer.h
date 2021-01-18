#pragma once


class Timer {

    bool clear = false;
public:
    template<typename Function>
    void setInterval(Function function, int interval);

    void stop();

};