#include "stdafx.h"
#include "CppUnitTest.h"
#include <shared_mutex>
#include <iostream>
#include <string>
#include <thread>
#include <condition_variable>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace cpp_14
{
	TEST_CLASS(MUTEX)
	{
	public:
		TEST_METHOD(SHARED_TIME_MUTEX)
		{
			std::shared_timed_mutex a;

			std::unique_lock<std::shared_timed_mutex> lock(a, std::chrono::milliseconds(3));
		}

		TEST_METHOD(ADOPT_LOCK)
		{
			std::mutex a, b;

			std::lock(a, b);

			// 이미 락이 걸린 상태인 뮤텍스에 대해서 영역 락
			std::unique_lock<std::mutex> lock1(a, std::adopt_lock);
			std::unique_lock<std::mutex> lock2(b, std::adopt_lock);

			// 락 풀림
		}

		TEST_METHOD(DEFER_LOCK)
		{
			std::mutex a, b;

			// 락을 나중에 하도록함
			std::unique_lock<std::mutex> lock1(a, std::defer_lock);
			std::unique_lock<std::mutex> lock2(b, std::defer_lock);
			
			std::lock(lock1, lock2);
			// 락 걸린 상태

		}

		TEST_METHOD(TRY_TO_LOCK)
		{
			std::mutex a;

			// lock 을 시도해봄
			std::unique_lock<std::mutex> lock1(a, std::try_to_lock);

			if (lock1)
			{
				// 락 획득
			}
		}

		TEST_METHOD(TIMED_MUTEX)
		{
			std::timed_mutex a;

			// 3ms동안 락 시도
			std::unique_lock<std::timed_mutex> lock(a, std::chrono::milliseconds(3));

		}

		TEST_METHOD(SHARED_MUTEX)
		{
			std::shared_mutex mutex;

			{
				//read lock
				std::shared_lock<std::shared_mutex> rlock(mutex);
			}
			{
				//write lock
				std::unique_lock<std::shared_mutex> wlock(mutex);
			}
			/*
			{
			std::shared_mutex mutex;
			//
			boost::upgrade_lock<boost::shared_mutex> uLock(m_mutex);
			// read lock
			{
			boost::upgrade_to_unique_lock<boost::shared_mutex> uLock2(uLock);
			// write lock
			}
			}
			*/
		}
	};

	std::mutex m;
	std::condition_variable cv;
	std::string data;
	bool ready = false;
	bool processed = false;

	TEST_CLASS(CONDITION)
	{
		static
			void worker_thread()
		{
			// ready 될때까지 대기
			{
				std::unique_lock<std::mutex> lk(m);
				cv.wait(lk, [] {
					std::cout << " - ready 될때까지 대기\n";
					return ready;
				});

				std::cout << " - 락을 걸고 data를 수정\n";
				data = "완료";

				// 처리 완료됨으로 처리
				processed = true;
			}
			std::cout << " - 처리 완료되었다고 알려줌\n";

			// lock 을 풀어서 깨어난후 바로 일할수 있게 해줌
			cv.notify_one();
		}

		TEST_METHOD(A)
		{
			std::thread worker(worker_thread);

			data = "받아라~";
			{
				std::lock_guard<std::mutex> lk(m);
				ready = true;
				std::cout << " - 처리하라고 알려줌\n";
			}
			cv.notify_one();

			{
				// 다 처리될 때까지 기다리는중

				std::unique_lock<std::mutex> lk(m);
				cv.wait(lk, [] {
					std::cout << " - 끝났어?\n";
					return processed; });
				std::cout << " - 처리 완료 받은 데이터:" << data << '\n';
			}

			worker.join();
		}
	};
}