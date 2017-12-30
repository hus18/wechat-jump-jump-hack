#pragma once
#ifndef liuzianglib_timer
#define liuzianglib_timer
#include <time.h>
//Version 2.4.21V30
//20170914

namespace DC {

	class timer {//����������ͣʱҲ��
				 //���̰߳�ȫ
	public:
		timer() {
			status = false;
			reset();
		}

	public:
		inline void start() {
			if (status == true) return;
			record = clock();
			status = true;
		}

		inline void stop() {
			if (status == false) return;
			res += (clock() - record);
			status = false;
		}

		inline void reset() {
			status = false;
			res = 0;
		}

		inline time_t getsecond()const {
			return getclock() / CLOCKS_PER_SEC;
		}

		inline time_t getms()const {//�����ƽ̨��أ���VS15��clock_t���Ǻ���,����ƽ̨������Ҫ�޸�
			return getclock();
		}

	private:
		inline clock_t getclock()const {
			if (status == false)
				return res;
			decltype(res) temp = res;
			temp += (clock() - record);
			return temp;
		}

	private:
		bool status;//true==running,false==stop
		clock_t record, res;
	};

}

#endif
