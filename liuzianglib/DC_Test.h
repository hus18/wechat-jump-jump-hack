#pragma once
#ifndef liuzianglib_Test
#define liuzianglib_Test
#include <iostream>
#include "liuzianglib.h"
//Version 2.4.21V34
//20171021

namespace DC {

	class UniqueID {
	public:
		UniqueID() :uniqueid(DC::randomer(0, 65535)) {}

		UniqueID(const UniqueID&) = default;

	public:
		inline const int32_t& getUniqueID()const {
			return uniqueid;
		}

	private:
		const int32_t uniqueid;
	};

	class Test :public std::enable_shared_from_this<Test> {
	public:
		Test() {
			PrintInfo();
			std::cout << "Ĭ�Ϲ���\n";
		}

		Test(const Test& input) :uniqueid() {
			input.PrintInfo();
			std::cout << " �� ";
			PrintInfo();
			std::cout << "��������\n";
		}

		Test(Test&& input) :uniqueid() {
			input.PrintInfo();
			std::cout << " �� ";
			PrintInfo();
			std::cout << "�ƶ�����\n";
		}

		~Test() {
			PrintInfo();
			std::cout << "����\n";
		}

	public:
		Test& operator=(const Test& input) {
			input.PrintInfo();
			std::cout << " �� ";
			PrintInfo();
			std::cout << "������ֵ�����\n";
			return *this;
		}

		Test& operator=(Test&& input) {
			input.PrintInfo();
			std::cout << " �� ";
			PrintInfo();
			std::cout << "�ƶ���ֵ�����\n";
			return *this;
		}

	public:
		inline void doNothing() {
			PrintInfo();
			std::cout << "ʲô��û����\n";
		}

	private:
		inline void PrintInfo()const {
			std::cout << "Test(" << uniqueid.getUniqueID() << ") ";
		}

	private:
		UniqueID uniqueid;
	};

}

#endif