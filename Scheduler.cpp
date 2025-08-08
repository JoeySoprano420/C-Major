#include <iostream>
#include <queue>
#include <vector>
#include <set>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <chrono>
#include <functional>
#include <cstdint>
#include <string>

struct CapsuleMeta {
    std::string name;
    std::string origin;
    std::vector<std::string> qualifiers;
};

struct Task {
    std::function<void()> invoke;
};

struct TimePoint {
    int64_t ticks_ms;
};

TimePoint now_ms() {
    using namespace std::chrono;
    return TimePoint{ duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count() };
}

TimePoint add_ms(const TimePoint& tp, int64_t delta) {
    return TimePoint{ tp.ticks_ms + delta };
}

struct MutationEvent {
    uint64_t taskId;
    std::string type;
    TimePoint at;
};

struct ScheduledTask {
    TimePoint time;
    Task task;
    uint64_t id;
    bool repeat;
    int64_t interval_ms;
    CapsuleMeta meta;
};

struct Earlier {
    bool operator()(const ScheduledTask& a, const ScheduledTask& b) const {
        return a.time.ticks_ms > b.time.ticks_ms; // min-heap
    }
};

class Scheduler {
    std::priority_queue<ScheduledTask, std::vector<ScheduledTask>, Earlier> _pq;
    std::set<uint64_t> _cancelled;
    std::vector<MutationEvent> _mutationLog;
    std::mutex _mtx;
    std::condition_variable _cv;
    std::atomic<bool> _running{ false };
    std::atomic<uint64_t> _nextId{ 1 };

public:
    uint64_t schedule(const Task& task, int64_t delay_ms, const CapsuleMeta& meta) {
        return scheduleImpl(task, delay_ms, false, 0, meta);
    }

    uint64_t scheduleRepeating(const Task& task, int64_t interval_ms, const CapsuleMeta& meta) {
        return scheduleImpl(task, interval_ms, true, interval_ms, meta);
    }

    void cancel(uint64_t id) {
        std::lock_guard<std::mutex> lock(_mtx);
        _cancelled.insert(id);
        _cv.notify_all();
        _mutationLog.push_back({ id, "cancelled", now_ms() });
    }

    void run() {
        _running.store(true);
        while (_running.load()) {
            std::unique_lock<std::mutex> lock(_mtx);
            if (_pq.empty()) {
                _cv.wait(lock, [this] { return !_pq.empty() || !_running.load(); });
                if (!_running.load()) break;
            }
            auto now = now_ms();
            if (!_pq.empty() && _pq.top().time.ticks_ms <= now.ticks_ms) {
                auto cur = _pq.top();
                _pq.pop();
                if (_cancelled.find(cur.id) == _cancelled.end()) {
                    lock.unlock();
                    cur.task.invoke();
                    lock.lock();
                    _mutationLog.push_back({ cur.id, "executed", now_ms() });
                    if (cur.repeat && _cancelled.find(cur.id) == _cancelled.end()) {
                        ScheduledTask nextTask = cur;
                        nextTask.time = add_ms(now, cur.interval_ms);
                        _pq.push(nextTask);
                        _mutationLog.push_back({ cur.id, "rescheduled", now_ms() });
                        _cv.notify_all();
                    }
                }
            } else if (!_pq.empty()) {
                auto wake_at = _pq.top().time.ticks_ms;
                _cv.wait_until(lock, std::chrono::steady_clock::time_point(std::chrono::milliseconds(wake_at)));
            }
        }
    }

    void stop() {
        _running.store(false);
        std::lock_guard<std::mutex> lock(_mtx);
        _cv.notify_all();
    }

    std::vector<ScheduledTask> getPendingTasks() {
        std::lock_guard<std::mutex> lock(_mtx);
        std::vector<ScheduledTask> out;
        auto tmp = _pq;
        while (!tmp.empty()) {
            out.push_back(tmp.top());
            tmp.pop();
        }
        return out;
    }

    std::vector<MutationEvent> getMutationHistory() {
        std::lock_guard<std::mutex> lock(_mtx);
        return _mutationLog;
    }

    std::string renderTimeline() {
        std::lock_guard<std::mutex> lock(_mtx);
        std::string sb;
        for (const auto& e : _mutationLog) {
            sb += "Task " + std::to_string(e.taskId) + " " + e.type + " at " + std::to_string(e.at.ticks_ms) + "ms\n";
        }
        return sb;
    }

private:
    uint64_t scheduleImpl(const Task& task, int64_t delay_ms, bool repeat, int64_t interval_ms, const CapsuleMeta& meta) {
        std::lock_guard<std::mutex> lock(_mtx);
        uint64_t id = _nextId.fetch_add(1);
        ScheduledTask st{ add_ms(now_ms(), delay_ms), task, id, repeat, interval_ms, meta };
        _pq.push(st);
        _mutationLog.push_back({ id, "scheduled", now_ms() });
        _cv.notify_all();
        return id;
    }
};

// Example usage
void hello_once() {
    std::cout << "Hello from one-time task!" << std::endl;
}

void hello_repeat() {
    std::cout << "Hello from repeating task!" << std::endl;
}

int main() {
    Scheduler sched;

    CapsuleMeta onceMeta{ "one-shot", "demo", { "example" } };
    CapsuleMeta repMeta{ "ticker", "demo", { "repeat", "example" } };

    uint64_t onceId = sched.schedule({ hello_once }, 1000, onceMeta);
    uint64_t repId = sched.scheduleRepeating({ hello_repeat }, 500, repMeta);

    std::thread t([&sched] { sched.run(); });

    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    sched.cancel(repId);
    sched.stop();
    t.join();

    std::cout << "Scheduler stopped." << std::endl;
    std::cout << sched.renderTimeline();

    return 0;
}
