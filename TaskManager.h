#include <iostream>
#include <queue>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <future>

#pragma once

class SafeQueue
{
private:
    std::queue<std::function<void()>> q;
    std::mutex mtx;

public:
    SafeQueue()
    {
    }
    bool Push(std::function<void()> item)
    {
        std::lock_guard<std::mutex> lock(mtx);
        try
        {
            q.push(move(item));
            return true;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
            return false;
        }
    }
    std::function<void()> Pop(){
        std::lock_guard<std::mutex> lock(mtx);
        if(q.empty()){
            return{};
        }
        auto f = q.front();
        q.pop();
        return f;
    }
    bool isEmpty(){
        std::lock_guard<std::mutex> lock(mtx);
        if(q.empty()) return true;
        return false;
    }

};
class ThreadPool{
    private:
    std::vector<std::thread> workers;
    SafeQueue work;
    std::condition_variable cv;
    std::atomic<bool> stopFlag;
    std::mutex mtx;
    public:
    
    ThreadPool(int NumberOfThreads){
        stopFlag=false;
        for(int i =0;i<NumberOfThreads;i++){
            workers.push_back(std::thread([this](){
                while(true){
                    std::unique_lock<std::mutex> lock(mtx);
                    cv.wait(lock, [this]{ return stopFlag || !work.isEmpty();});
                    if(stopFlag && work.isEmpty()){
                        return;
                    }
                    auto task=work.Pop();
                    lock.unlock();
                    task();
                }
            }));
        }
    }

    template <typename Func, typename... Args>
    auto Enqueue(Func&& func, Args&&... args) -> std::future<std::invoke_result_t<Func, Args...>>{
        using type = std::invoke_result_t<Func, Args...>;
        auto task = std::make_shared<std::packaged_task<type()>>(std::bind(std::forward<Func>(func),std::forward<Args>(args)...));
        std::future<type> res = task->get_future();
        std::function<void()> disguise = [task](){(*task)();};
        work.Push(disguise);
        cv.notify_one();
        return res;
    }
    ~ThreadPool(){
        stopFlag=true;
        cv.notify_all();
        for(int i = 0; i<workers.size();i++){
            workers[i].join();
        }
    }

};