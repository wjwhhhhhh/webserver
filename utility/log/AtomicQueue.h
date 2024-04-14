//
// Created by wjw on 2024/4/13.
//

#ifndef INC_1111_ATOMICQUEUE_H
#define INC_1111_ATOMICQUEUE_H
#include<atomic>
#include<iostream>
#include<string>
#include<memory>
namespace WebServer::Atomic
{
    using std::string;
    class AtomicQueue //适用于多个生产者和一个消费者
    {
        struct AtomicNode
        {
            explicit AtomicNode(const string &a=""):next_(nullptr),value_(a)
            {
            }
            std::atomic<AtomicNode*> next_;
             string value_;
        };
        public:
        //设置别称
        using Node=AtomicNode;
        AtomicQueue():head_(new Node()),tail_(head_.load()),is_close_(false),Ptr(nullptr)
        {
        }
        ~AtomicQueue()
        {
            close();
            std::cout<<"ok"<<std::endl;
            string a;
            while(pop_front(a));
            delete head_.load();
        }
        bool push_back(const std::string &val)
        {
            auto new_node_ = new Node(val);
            Node* old_tail = nullptr;
            do {
                old_tail = tail_.load(); // 获取当前尾节点
            } while (!std::atomic_compare_exchange_strong(&(old_tail->next_), &Ptr, new_node_)); // 将新节点连接到尾节点的next

            std::atomic_compare_exchange_strong(&tail_, &old_tail, new_node_); // 更新尾节点为新节点
            return true;
        }
        bool pop_front(std::string &val)
        {
            Node*next=nullptr;
            while(1)
            {
                next=head_.load()->next_;
                if(next== nullptr)
                {
                    if(is_close_)return false;
                    else continue;
                }
                val=next->value_;
                delete head_;
                head_.store(next);
                break;
            }
            return true;
        }
        void close()
        {
            is_close_=true;
        }
        private:
        Node* Ptr;
        std::atomic<bool> is_close_;
       std::atomic<AtomicNode*>head_;
        std::atomic<AtomicNode*>tail_;
    };
};
#endif //INC_1111_ATOMICQUEUE_H
