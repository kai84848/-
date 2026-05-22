# -
基于可变参模板实现的线程池

/*example
// 创建线程任务，从Task中继承run方法
class MyTask : public Task
{
    public:
        std::any run()
        {
        // 线程代码...


        //无返回值
        ret_ =  std::any{};
        return std::any{};

        有返回值
        ret_ = returnNum;
        return returnNum;
        }
}；

// main中创建线程
ThreadPool pool;
pool.start( // 线程数 )
auto myTask = std::make_shared<MyTask>();
pool.submitTask(myTask);
std::this_thread::sleep_for(std::chrono::seconds(1));
std::any res = myTask->getRet();
int val = std::any_cast<int>(res);
std::cout<<"val:"<<val<<std::endl;
*/
