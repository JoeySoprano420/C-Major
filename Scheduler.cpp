#include <iostream>
#include <queue>
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <vector>
#include <mutex>
#include <condition_variable>

class Scheduler {
public:
    using Task = std::function<void()>;
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;

    struct ScheduledTask {
        TimePoint time;
        Task task;
        size_t id;
        bool repeat;
        std::chrono::milliseconds interval;

        bool operator>(const ScheduledTask& other) const {
            return time > other.time;
        }
    };

    Scheduler() : running(false), nextId(1) {}

    // Schedule a one-time task after a delay
    size_t schedule(Task task, std::chrono::milliseconds delay) {
        return scheduleImpl(std::move(task), delay, false, std::chrono::milliseconds(0));
    }

    // Schedule a repeating task
    size_t scheduleRepeating(Task task, std::chrono::milliseconds interval) {
        return scheduleImpl(std::move(task), interval, true, interval);
    }

    // Cancel a scheduled task
    void cancel(size_t id) {
        std::lock_guard<std::mutex> lock(mutex);
        cancelled.insert(id);
        cv.notify_all();
    }

    // Start the scheduler loop (blocking)
    void run() {
        running = true;
        while (running) {
            std::unique_lock<std::mutex> lock(mutex);
            if (tasks.empty()) {
                cv.wait(lock, [&] { return !tasks.empty() || !running; });
            }
            if (!running) break;

            auto now = Clock::now();
            if (!tasks.empty() && tasks.top().time <= now) {
                auto task = tasks.top();
                tasks.pop();
                if (cancelled.count(task.id) == 0) {
                    lock.unlock();
                    task.task();
                    lock.lock();
                    if (task.repeat && cancelled.count(task.id) == 0) {
                        ScheduledTask nextTask = task;
                        nextTask.time = now + task.interval;
                        tasks.push(nextTask);
                        cv.notify_all();
                    }
                }
            } else if (!tasks.empty()) {
                cv.wait_until(lock, tasks.top().time);
            }
        }
    }

    // Stop the scheduler loop
    void stop() {
        running = false;
        cv.notify_all();
    }

    ~Scheduler() {
        stop();
    }

private:
    std::priority_queue<ScheduledTask, std::vector<ScheduledTask>, std::greater<>> tasks;
    std::mutex mutex;
    std::condition_variable cv;
    std::atomic<bool> running;
    std::atomic<size_t> nextId;
    std::unordered_set<size_t> cancelled;

    size_t scheduleImpl(Task task, std::chrono::milliseconds delay, bool repeat, std::chrono::milliseconds interval) {
        std::lock_guard<std::mutex> lock(mutex);
        size_t id = nextId++;
        ScheduledTask st{Clock::now() + delay, std::move(task), id, repeat, interval};
        tasks.push(std::move(st));
        cv.notify_all();
        return id;
    }
};

// Example usage:
int main() {
    Scheduler scheduler;

    // Schedule a one-time task
    scheduler.schedule([] {
        std::cout << "Hello from one-time task!\n";
    }, std::chrono::milliseconds(1000));

    // Schedule a repeating task
    size_t repeatId = scheduler.scheduleRepeating([] {
        std::cout << "Hello from repeating task!\n";
    }, std::chrono::milliseconds(500));

    // Run the scheduler in a separate thread
    std::thread schedThread([&scheduler] { scheduler.run(); });

    // Let it run for 3 seconds
    std::this_thread::sleep_for(std::chrono::seconds(3));
    scheduler.cancel(repeatId);
    scheduler.stop();
    schedThread.join();

    std::cout << "Scheduler stopped.\n";
    return 0;
}
