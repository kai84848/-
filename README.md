# C++ Thread Pool

一个基于 C++17/20 实现的高性能线程池，支持固定模式和动态增长模式，采用生产者-消费者模型设计。

## 特性

- **双模式支持**：支持固定线程数模式 (`MODE_FIXED`) 和动态增长模式 (`MODE_CACHED`)
- **类型安全**：使用 `std::any` 实现任务返回值的类型安全传递
- **线程安全**：基于 `std::mutex` 和 `std::condition_variable` 实现线程同步
- **智能指针管理**：使用 `std::shared_ptr` 管理任务生命周期，避免内存泄漏
- **任务队列阈值**：支持设置任务队列最大容量，防止内存无限增长
- **超时机制**：任务提交支持超时检测，避免无限阻塞

## 项目结构

```
.
├── CMakeLists.txt    # CMake 构建配置
├── main.cpp          # 示例程序
├── threadpool.h      # 线程池头文件
└── threadpool.cpp    # 线程池实现
```

## 环境要求

- C++17 或更高版本（推荐 C++20）
- CMake 3.10+
- 支持 C++17 的编译器（GCC 7+、Clang 5+、MSVC 2017+）

## 快速开始

### 编译

```bash
mkdir build && cd build
cmake ..
make
```

### 使用示例

```cpp
#include "threadpool.h"
#include <iostream>
#include <any>

// 1. 继承 Task 类，实现自定义任务
class MyTask : public Task {
public:
    std::any run() override {
        // 任务逻辑
        int a = 10, b = 20;
        int sum = a + b;
        
        // 设置返回值
        ret_ = sum;
        return sum;
    }
};

int main() {
    // 2. 创建线程池
    ThreadPool pool;
    
    // 3. 设置模式（可选，默认为固定模式）
    pool.setMode(PoolMode::MODE_FIXED);
    
    // 4. 启动线程池，指定线程数量
    pool.start(4);
    
    // 5. 创建并提交任务
    auto myTask = std::make_shared<MyTask>();
    pool.submitTask(myTask);
    
    // 6. 获取任务返回值
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::any res = myTask->getRet();
    int val = std::any_cast<int>(res);
    std::cout << "Result: " << val << std::endl;
    
    return 0;
}
```

## API 文档

### ThreadPool 类

| 方法 | 说明 |
|------|------|
| `ThreadPool()` | 构造函数，初始化线程池 |
| `~ThreadPool()` | 析构函数 |
| `setMode(PoolMode mode)` | 设置线程池工作模式 |
| `setTaskQueMaxThreshHold(int threshold)` | 设置任务队列最大容量 |
| `submitTask(std::shared_ptr<Task> sp)` | 提交任务到线程池 |
| `start(int initThreadSize = 4)` | 启动线程池 |

### PoolMode 枚举

| 模式 | 说明 |
|------|------|
| `MODE_FIXED` | 固定线程数模式，线程数量始终保持不变 |
| `MODE_CACHED` | 动态增长模式，根据任务量自动调整线程数 |

### Task 类

自定义任务需要继承 `Task` 类，并重写 `run()` 方法：

```cpp
class MyTask : public Task {
public:
    std::any run() override {
        // 实现任务逻辑
        ret_ = /* 返回值 */;
        return ret_;
    }
};
```

## 设计亮点

1. **RAII 原则**：使用智能指针管理资源，确保异常安全
2. **条件变量**：使用 `notFull_` 和 `notEmpty_` 实现高效的生产者-消费者同步
3. **原子操作**：使用 `std::atomic_int` 保证任务计数线程安全
4. **非拷贝设计**：禁用拷贝构造函数和赋值运算符，避免资源管理问题

## 注意事项

- 任务提交后，需要等待任务执行完成才能获取返回值
- 线程池析构时需要确保所有任务已完成（当前版本待完善）
- 使用 `std::any_cast` 获取返回值时，类型必须匹配

