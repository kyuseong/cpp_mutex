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

			// �̹� ���� �ɸ� ������ ���ؽ��� ���ؼ� ���� ��
			std::unique_lock<std::mutex> lock1(a, std::adopt_lock);
			std::unique_lock<std::mutex> lock2(b, std::adopt_lock);

			// �� Ǯ��
		}

		TEST_METHOD(DEFER_LOCK)
		{
			std::mutex a, b;

			// ���� ���߿� �ϵ�����
			std::unique_lock<std::mutex> lock1(a, std::defer_lock);
			std::unique_lock<std::mutex> lock2(b, std::defer_lock);
			
			std::lock(lock1, lock2);
			// �� �ɸ� ����

		}

		TEST_METHOD(TRY_TO_LOCK)
		{
			std::mutex a;

			// lock �� �õ��غ�
			std::unique_lock<std::mutex> lock1(a, std::try_to_lock);

			if (lock1)
			{
				// �� ȹ��
			}
		}

		TEST_METHOD(TIMED_MUTEX)
		{
			std::timed_mutex a;

			// 3ms���� �� �õ�
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
			// ready �ɶ����� ���
			{
				std::unique_lock<std::mutex> lk(m);
				cv.wait(lk, [] {
					std::cout << " - ready �ɶ����� ���\n";
					return ready;
				});

				std::cout << " - ���� �ɰ� data�� ����\n";
				data = "�Ϸ�";

				// ó�� �Ϸ������ ó��
				processed = true;
			}
			std::cout << " - ó�� �Ϸ�Ǿ��ٰ� �˷���\n";

			// lock �� Ǯ� ����� �ٷ� ���Ҽ� �ְ� ����
			cv.notify_one();
		}

		TEST_METHOD(A)
		{
			std::thread worker(worker_thread);

			data = "�޾ƶ�~";
			{
				std::lock_guard<std::mutex> lk(m);
				ready = true;
				std::cout << " - ó���϶�� �˷���\n";
			}
			cv.notify_one();

			{
				// �� ó���� ������ ��ٸ�����

				std::unique_lock<std::mutex> lk(m);
				cv.wait(lk, [] {
					std::cout << " - ������?\n";
					return processed; });
				std::cout << " - ó�� �Ϸ� ���� ������:" << data << '\n';
			}

			worker.join();
		}
	};
}