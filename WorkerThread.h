#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include <QThread>

class WorkerThread : public QThread {
    Q_OBJECT

public:
    template <typename Function>
    void doWork(Function&& lambda)
    {
        this->lambda = lambda;
    }

    void run() override
    {
        this->lambda();
    }
signals:
    void progressRangeChanged(int min, int max);
    void progressValueChanged(int value);

private:
    std::function<void()> lambda;
};
#endif // WORKERTHREAD_H
