//
// Created by wjw on 2024/3/7.
//

#ifndef WEBSERVER_THREADPOOL_H
#define WEBSERVER_THREADPOOL_H

#include <cstring>
#include <thread> // std::thread
#include <future>
#include <vector>
#include <queue>
#include <functional>
namespace WebServer {
    namespace ThreadPool {
        class threadpool
        {
            public:
            explicit threadpool(std::size_t threads);
            ~threadpool();
            template <class F,class... Args>
            std::future<typename std::result_of<F(Args...)>::type>enqueue(F&&f,Args&&... args);
            private:
            std::vector<std::thread>workers;
            std::queue<std::function<void()>>tasks;
            std::mutex queue_mutex;
            std::condition_variable condition;
            bool stop;
        };

        inline threadpool::threadpool(std::size_t threads):stop(false) {
            for(std::size_t i = 0; i < threads; ++i){
                workers.emplace_back(
                  [this]{
                      while(1)
                      {
                          std::function<void()>task;
                          {
                              std::unique_lock<std::mutex>lock(this->queue_mutex);
                              this->condition.wait(lock,
                                                   [this]{return this->stop||!this->tasks.empty();});
                               if(this->stop&&this->tasks.empty())return ;
                               task=std::move(this->tasks.front());
                               this->tasks.pop();
                          }
                          task();
                      }
                  }

                    );
            }
        }

        inline threadpool::~threadpool() {
            {
                std::unique_lock<std::mutex>lock(this->queue_mutex);
                stop=true;
            }
            condition.notify_all();
            for(std::thread &worker:workers)worker.join();
        }

        template<class F, class... Args>
        auto threadpool::enqueue(F&& f, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type>
        {
            using return_type = typename std::result_of<F(Args...)>::type;

            auto task = std::make_shared< std::packaged_task<return_type()> >(
                    std::bind(std::forward<F>(f), std::forward<Args>(args)...)
            );

            std::future<return_type> res = task->get_future();
            {
                std::unique_lock<std::mutex> lock(queue_mutex);

                // don't allow enqueueing after stopping the pool
                if(stop)
                    throw std::runtime_error("enqueue on stopped ThreadPool");

                tasks.emplace([task](){ (*task)(); });
            }
            condition.notify_one();
            return res;
        }
        }
    }
#endif //WEBSERVER_THREADPOOL_H
