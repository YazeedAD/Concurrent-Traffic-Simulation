#include <iostream>
#include <random>
#include <cstdlib>
#include <ctime>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function.

    // lock
    std::unique_lock<std::mutex> u_lock(_mutex);
    // wait for new messages
    _cond.wait(u_lock, [this] { return !_queue.empty(); });
    //receives new messages and pull them from the queue using move semantics
    T msg = std::move(_queue.back());
    _queue.pop_back();

    return msg;

}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.


    // locks to avoid data race
    std::lock_guard<std::mutex> u_lock(_mutex);
    // adds a new message to the queue
    _queue.push_back(std::move(msg));
    // sends a notification
    _cond.notify_one();
}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    while (_messageQueue.receive() != TrafficLightPhase::green) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method
    //„simulate“ is called. To do this, use the thread queue in the base class.
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));

}

// virtual function which is executed in a thread
[[noreturn]] void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles.

    /*pseudo-random*/
    srand (time(0));     // random seed
    double cycleDuration = rand() % 4000 + 2000;

    // records start time
    auto startTime = std::chrono::system_clock::now();

    while (true)
    {
        // wait 1ms between two cycles
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        // record end time
        auto endTime = std::chrono::system_clock::now();
        // time difference
        double diffTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

        if (diffTime >= cycleDuration)
        {
            // toggles the current phase of the traffic light between red and green
            _currentPhase = static_cast<TrafficLightPhase>(!_currentPhase);
            // rand for next iteration
            cycleDuration = rand() % 4000 + 2000;
            // sends an update method  to the message queue using move semantics
            _messageQueue.send(std::move(_currentPhase));

        }






    }

}

