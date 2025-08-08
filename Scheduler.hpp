#pragma once
#include <queue>
#include <string>
#include <functional>

struct Task {
    std::string name;
    std::function<void()> fn;
};

class Scheduler {
    std::queue<Task> q;
public:
    void submit(const std::string& name, std::function<void()> fn){ q.push({name,std::move(fn)}); }
    void run(){
        while(!q.empty()){
            auto t = q.front(); q.pop();
            t.fn();
        }
    }
};
