#pragma once
#ifndef liuzianglib_MySQL
#define liuzianglib_MySQL
#include <vector>
#include <mysql.h>
#pragma comment(lib,"libmysql.lib")
//Version 2.4.21V43
//20171026
//ע��:DC_WinSock.h��������DC_MySQL.h����

namespace DC {

	class DC_MySQL {//�������캯����=����������ǿ����˷��������������������Ӿ��������֮����Ҫ�������ӵ���������
	public:
		DC_MySQL() {
			mysql_init(&SQL);
		}

		~DC_MySQL() {
			mysql_close(&SQL);
		}

		DC_MySQL(const DC_MySQL& input) {
			mysql_init(&SQL);
			IP = input.IP;
			username = input.username;
			password = input.password;
			DBname = input.DBname;
			port = input.port;
		}

	public:
		const DC_MySQL& operator=(const DC_MySQL& input) {
			mysql_close(&SQL);
			mysql_init(&SQL);
			IP = input.IP;
			username = input.username;
			password = input.password;
			DBname = input.DBname;
			port = input.port;
			return *this;
		}

		void SetServer(const std::string& inputIP, const std::string& inputusername, const std::string& inputpassword, const std::string& inputDBname, const std::size_t& inputport) {
			IP = inputIP;
			username = inputusername;
			password = inputpassword;
			DBname = inputDBname;
			port = inputport;
		}

		bool VerConnection() {//�ж������Ƿ���á�
							  //�����δȡ�صĽ��Ҳ�᷵�����Ӳ�����
			if (mysql_ping(&SQL) == 0) return true;
			return false;
		}

		bool Connect() {
			mysql_close(&SQL);
			mysql_init(&SQL);
			if (mysql_real_connect(&SQL, IP.c_str(), username.c_str(), password.c_str(), DBname.c_str(), port, NULL, 0) == NULL) return false;
			return true;
		}

		void disconnect() {
			mysql_close(&SQL);
		}

		bool Query(const std::string& que) {//�����ѯ����н�������Ҵ˽��û��ͨ��GetResult()��ȡ����ô���޷��ٴ�ִ������insert�������н������䡱��ֱ����ͨ��GetResult()��֮ǰ�����Ľ��ȡ���ˣ������ٴ�ִ�С��н������䡱��
			if (mysql_query(&SQL, que.c_str()) == 0) return true;
			return false;
		}

		std::vector<std::string> GetResult() {
			MYSQL_RES *SQL_RES;
			MYSQL_ROW SQL_ROW;
			std::vector<std::string> result;
			if ((SQL_RES = mysql_store_result(&SQL)) == NULL) { mysql_free_result(SQL_RES); return result; }
			while ((SQL_ROW = mysql_fetch_row(SQL_RES)) != NULL) {
				for (std::size_t i = 0; i < mysql_num_fields(SQL_RES); i++)
					result.push_back(SQL_ROW[i]);
			}
			mysql_free_result(SQL_RES);
			return result;
		}

		MYSQL* GetHandle() {
			return &SQL;
		}

	private:
		MYSQL SQL;
		std::string IP, username, password, DBname;
		std::size_t port;
	};

}

#endif
