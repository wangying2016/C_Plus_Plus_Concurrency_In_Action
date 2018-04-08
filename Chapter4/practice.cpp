bool flag;
std::mutex m;

void wait_for_flag()
{
    std::unique_lock<std::mutex> lk(m);
    wihle(!flag) {
        lk.unlock();    // 1. Ω‚À¯ª•≥‚¡ø
        std::this_thread::sleep_for(std::chrono::milliseconds(100));    // 2. –›√ﬂ 100 ms
        lk.lock();      // 3. ‘ŸÀ¯ª•≥‚¡ø
    }
}

// Listing 4.1 Waiting for data to process with a std::condition_variable
std::mutex mut;
std::queue<data_chunk> data_queue;
std::condition_variable data_cond;

void data_perparation_thread()
{
    while (more_data_to_prepare()) {
        data_chunk const data = prepare_data();
        std::lock_guard<std::mutex> lk(mut);
        data_queue.push(data);
        data_cond.notify_one();
    }
}

void data_processing_thread()
{
    while (true) {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(
            lk, []{return !data_queue.empty();});
        data_chunk data = data_queue.front();
        data_queue.pop();
        lk.unlock();
        process(data);
        if (is_last_chunk(data)) break;
    }
}