#include <iostream>
#include <thread>
#include <chrono>
#include <future>
#include <mutex>
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv;
bool th1_done = false;
bool th2_done = false;

void bubble_sort(float* arr, int n,bool& done_flag)
{
    if (!arr || n <= 1)
    {
        return;
    }
    //std::lock_guard<std::mutex> lock(mtx);//raiideom

    std::cout << "thread id: " << std::this_thread::get_id() << "\n";

    auto start = std::chrono::system_clock::now();

    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            mtx.lock();
            std::cout << std::this_thread::get_id() << " " << arr[i] << " " << arr[j] << "\n";
            mtx.unlock();
            if (arr[j] > arr[j + 1]) {
                float tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
            }
        }
    }
    auto end = std::chrono::system_clock::now();
    double duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    /*mtx.lock();
    std::cout << "ellapsed time: " << duration << "ms" << std::endl;
    mtx.unlock();*/
    //std::this_thread::sleep_for(std::chrono::seconds(5));
    std::lock_guard <std::mutex> lock(mtx);
    done_flag = true;
    cv.notify_one();
}




void print(float* arr, int n)
{
    if (!arr || n <= 0)
    {
        return;
    }
    for (int i = 0; i < n; i++)
    {
        std::cout << arr[i] << " ";
    }
    std::cout << "\n";
}

int main()
{
    int n;
    std::cin >> n;

    if (n < 1)
    {
        return 1;
    }
    float* arr = new float[n];

    if (!arr)
    {
        return 1;
    }
    for (int i = 0; i < n; i++)
    {
        arr[i] = 5.0 + (static_cast<float>(rand()) / RAND_MAX) * 25;
        arr[i] = i < n / 2 ? arr[i] : -arr[i];
    }
    int mid = n / 2;

    //std::future<void> f1 = std::async(std::launch::async, bubble_sort, arr, mid);
    //std::future<void> f2 = std::async(std::launch::async, bubble_sort, arr + mid, n - mid);

    //f1.get();
    //f2.get();
    //print(arr, n);

    //std::future<void> f3 = std::async(std::launch::async, bubble_sort, arr, n);
    //f3.get();
    //print(arr, n);
    std::thread t1(bubble_sort, arr,       mid,std::ref(th1_done));
    std::thread t2(bubble_sort, arr + mid, mid, std::ref(th2_done));

    std::unique_lock<std::mutex> lk(mtx);
    cv.wait(lk, 
        [] 
        {
            return th1_done && th2_done;
        }
    );
    //lk.unlock();
    /*t1.join();
    t2.join();*/
    t1.detach();
    t2.detach();


    std::cout << "t1 and t2 done, continue\n";
    print(arr, n);
    


    std::thread t3(bubble_sort, arr, n, std::ref(th1_done));
    t3.join();
    std::cout << "final sort:\n";
    print(arr, n);

}

