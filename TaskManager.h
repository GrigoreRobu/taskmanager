#include <iostream>
#include <queue>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <future>

using namespace std;

class SafeQueue
{
private:
    queue<function<void()>> q;
    mutex mtx;

public:
    SafeQueue()
    {
    }
    bool Push(function<void()> item)
    {
        lock_guard<mutex> lock(mtx);
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
    function<void()> Pop(){
        lock_guard<mutex> lock(mtx);
        if(q.empty()){
            return{};
        }
        auto f = q.front();
        q.pop();
        return f;
    }
    bool isEmpty(){
        lock_guard<mutex> lock(mtx);
        if(q.empty()) return true;
        return false;
    }

};
class ThreadPool{
    private:
    vector<thread> workers;
    SafeQueue work;
    condition_variable cv;
    atomic<bool> stopFlag;
    mutex mtx;
    public:
    
    ThreadPool(int NumberOfThreads){
        for(int i =0;i<NumberOfThreads;i++){
            stopFlag=false;
            workers.push_back(thread([this](){
                while(true){
                    unique_lock<mutex> lock(mtx);
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
    auto Enqueue(Func&& func, Args&&... args) -> future<invoke_result_t<Func, Args...>>{
        using type = invoke_result_t<Func, Args...>;
        auto task = make_shared<packaged_task<type()>>(bind(forward<Func>(func),forward<Args>(args)...));
        future<type> res = task->get_future();
        function<void()> disguise = [task](){(*task)();};
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