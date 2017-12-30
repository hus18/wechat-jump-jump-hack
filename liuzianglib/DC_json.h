#pragma once
#ifndef liuzianglib_json
#define liuzianglib_json
#include <vector>
#include <limits>
#include <type_traits>
#include <functional>
#include <stack>
#include "liuzianglib.h"
#include "DC_STR.h"
#include "DC_var.h"
//Version 2.4.21V46
//20171202

namespace DC {

	namespace Web {

		namespace json {

			namespace jsonSpace {

				struct PosPair {
					PosPair() = default;

					PosPair(const DC::pos_type& input1, const DC::pos_type& input2) :first(input1), second(input2) {}

					DC::pos_type first, second;
				};

				using ObjTable = std::vector<PosPair>;

				inline bool comparePosPairfirst(const PosPair& input0, const PosPair& input1) {//�Ƚ�����PosPair�Ŀ�ʼλ��
																							   //sortʱʹ�ã���С������ǰ��
					return input0.first < input1.first;
				}

				inline bool comparePosPairsecond(const PosPair& input0, const PosPair& input1) {//�Ƚ�����PosPair�Ľ���λ��
																								//sortʱʹ�ã���С������ǰ��
					return input0.second < input1.second;
				}

				inline bool SybolValid(const std::vector<std::size_t>& AllStartSymbol, const std::vector<std::size_t>& EndStartSymbol) {//�жϿ�ʼ���źͽ������������Ƿ�һ��
					return AllStartSymbol.size() == EndStartSymbol.size();
				}

				std::vector<PosPair> getSymbolPair(const std::string& str, const char* const StartSymbol, const char* const EndSymbol) {//�ҳ���Եķ��ţ�������Խ���((2+2)*(1+1)*6)���ֶ�����
																																		//����ֵ��std::vector<PosPair>��ÿһ��PosPair����һ�Է��ŵ�λ�ã�PosPair<0>�ǿ�ʼ���ŵ�λ�ã�PosPair<1>�ǽ������ŵ�λ��
																																		//��֧��StartSymbol==EndSymbol�����
					std::vector<std::size_t> AllStartSymbol = DC::STR::find(str, StartSymbol).places, AllEndSymbol = DC::STR::find(str, EndSymbol).places;
					std::vector<PosPair> returnvalue;

					if (!SybolValid(AllStartSymbol, AllEndSymbol)) throw DC::DC_ERROR("invalid string", "symbols can not be paired");//�жϿ�ʼ���źͽ������������Ƿ�һ��				
																																	 //����㷨�������ڡ�����AllStartSymbol�е����һ��Ԫ�������������AllEndSymbolԪ�ر�Ȼ������֮��ԡ���
					for (auto i = AllStartSymbol.rbegin(); i != AllStartSymbol.rend(); i = AllStartSymbol.rbegin()) {
						std::size_t minimal = INT_MAX;//int�������ֵ
						auto iter = AllEndSymbol.end();
						for (auto i2 = AllEndSymbol.begin(); i2 != AllEndSymbol.end(); i2++) {
							if ((!(*i2 < *i)) && (*i2 - *i) < minimal) { minimal = *i2 - *i; iter = i2; }//�ҳ��͵�ǰ��ʼ��������Ľ�������
						}
						if (iter == AllEndSymbol.end())
							throw DC::DC_ERROR("undefined behavior", 0);//������Ӧ�ò����׳�������
						returnvalue.emplace_back(*i, *iter);
						DC::vector_fast_erase_no_return(AllStartSymbol, --i.base());
						DC::vector_fast_erase_no_return(AllEndSymbol, iter);
					}
					return returnvalue;
				}

				inline std::string GetJsStr(const std::string& input) {
					return "\"" + input + "\"";
				}

				class base {
				public:
					base() = default;

					base(const base&) = default;

					base(base&&) = default;

					virtual ~base() = default;

				protected:
					template <typename T, class = typename std::enable_if<!std::is_same<typename std::decay<T>::type, base>::value, T>::type>
					base(T&& inputstr) :rawStr(std::forward<T>(inputstr)) {
						static_assert(std::is_same<std::string, typename std::decay<T>::type>::value, "input type must be std::string");
					}

				public:
					virtual void set(const std::string& input) = 0;

					virtual void set(std::string&& input) = 0;

				protected:
					virtual void RemoveOutsideSymbol() = 0;

					virtual void refresh() = 0;

				protected:
					template<typename T>
					inline void setRawStr(T&& input)noexcept {
						static_assert(std::is_same<std::string, typename std::decay<T>::type>::value, "input type should be std::string");
						rawStr = std::forward<T>(input);
					}

					inline std::size_t findNeXTchar(std::size_t startpos)const {//�ҵ���һ���ַ�������"name:  s"����5��ʼ�ң��ҵ�s����������;�����и�ʽ���Ʒ�
																				//�Ҳ������쳣
						if (rawStr.empty()) throw false;
						for (; startpos < rawStr.size(); startpos++)
							if (rawStr[startpos] != ' '&&rawStr[startpos] != '\n'&&rawStr[startpos] != '\r'&&rawStr[startpos] != '\t')
								return startpos;
						throw false;
					}

					inline std::size_t findNeXTchar(const char& findthis, std::size_t startpos)const {//�ҵ���һ���ַ�
																									  //�Ҳ������쳣
						if (rawStr.empty()) throw false;
						for (; startpos < rawStr.size(); startpos++)
							if (rawStr[startpos] == findthis)
								return startpos;
						throw false;
					}

				protected:
					std::string rawStr;
				};

			}

			class value;
			class number;
			class array;
			class object;

			class transparent final :public jsonSpace::base {
				friend class object;
			public:
				transparent() :withTable(false) {}

				transparent(const std::string&);

				transparent(std::string&&);

				transparent(const transparent&) = delete;

				transparent(transparent&&) = default;

			public:
				virtual inline void set(const std::string& input)override {
					setRawStr(input);
					withTable = false;
				}

				virtual inline void set(std::string&& input)override {
					setRawStr(std::move(input));
					withTable = false;
				}

				inline bool is_empty()const {
					return rawStr.empty();
				}

				object&& to_object();

				value& as_value();

				value&& to_value();

				number& as_number();

				number&& to_number();

				array&& to_array();

			protected:
				virtual inline void RemoveOutsideSymbol() {}

				virtual inline void refresh()noexcept {}

			private:
				template<typename T>
				T& as_something() {
					ptr.reset(new T(rawStr));
					return *static_cast<T*>(ptr.get());
				}

				template<typename T>
				T&& to_something() {
					ptr.reset(new T(std::move(rawStr)));
					return std::move(*static_cast<T*>(ptr.get()));
				}

			private:
				std::unique_ptr<jsonSpace::base> ptr;
				bool withTable;
			};

			class value final :public jsonSpace::base {
			public:
				value() = default;

				value(const value& input) {
					setRawStr(input.rawStr);
					isStr = input.isStr;
				}

				value(value&& input)noexcept {
					setRawStr(std::move(input.rawStr));
					isStr = input.isStr;
				}

				value(const std::string& input) {
					set(input);
				}

				value(std::string&& input) {
					set(std::move(input));
				}

				virtual ~value() = default;

			public:
				value& operator=(const value& input) {
					if (this == &input)
						return *this;
					setRawStr(input.rawStr);
					isStr = input.isStr;
					return *this;
				}

				value& operator=(value&& input)noexcept {
					if (this == &input)
						return *this;
					setRawStr(std::move(input.rawStr));
					isStr = input.isStr;
					return *this;
				}

			public:
				virtual void set(const std::string& input)override {
					setRawStr(input);
					refresh();
					if (!is_bool() && !is_null() && !is_string() && !is_empty()) {
						rawStr.clear();
						throw DC::DC_ERROR("value", "bad value");
					}
				}

				virtual void set(std::string&& input)override {
					setRawStr(std::move(input));
					refresh();
					if (!is_bool() && !is_null() && !is_string() && !is_empty()) {
						rawStr.clear();
						throw DC::DC_ERROR("value", "bad value");
					}
				}

				inline bool is_bool()const noexcept {
					return rawStr == "true" || rawStr == "false";
				}

				inline bool is_null()const noexcept {
					return rawStr == "null";
				}

				inline bool is_string()const noexcept {
					return isStr;
				}

				inline bool is_empty()const noexcept {
					return !this->is_string() && rawStr.empty();
				}

				inline bool as_bool()const {
					if (rawStr == "true") return true;
					if (rawStr == "false") return false;
					throw DC::DC_ERROR("value::as_bool", "can not as bool");
				}

				inline DC::var as_var()const {
					return DC::var(rawStr);
				}

				inline DC::var to_var() {
					isStr = false;
					return DC::var(std::move(rawStr));
				}

				inline std::string as_string()const {
					if (!is_string())
						throw DC::DC_ERROR("value::as_string", "can not as string");
					return DC::STR::replace(rawStr, DC::STR::find(rawStr, "\\\""), "\"");
				}

				inline std::string to_string() {
					if (!is_string())
						throw DC::DC_ERROR("value::as_string", "can not as string");
					isStr = false;
					auto temp = std::move(rawStr);
					return DC::STR::replace(temp, DC::STR::find(temp, "\\\""), "\"");
				}

			protected:
				inline virtual void RemoveOutsideSymbol() {
					bool flag = false;

					for (auto i = rawStr.begin(); i != rawStr.end(); i++) {
						if (*i == '"') {
							rawStr.erase(i);
							flag = true;
							break;
						}
					}

					if (flag != true) return;

					for (auto i = rawStr.rbegin(); i != rawStr.rend(); i++) {
						if (*i == '"') {
							rawStr.erase(--i.base());
							flag = false;
							break;
						}
					}

					if (flag == true)
						throw DC::DC_ERROR("value::RemoveOutsideSymbol", "can not find end symbol");
				}

				virtual inline void refresh()noexcept {
					isStr = false;
					if (rawStr.empty() || !makeIsStr()) return;
					isStr = true;
					RemoveOutsideSymbol();
				}

			private:
				std::size_t findNeXTcharReverse(std::size_t startpos) {//�ҵ���һ���ַ�������"name:  s"����5��ʼ�ң��ҵ�s����������;�����и�ʽ���Ʒ�
																	   //�Ҳ������쳣
					if (rawStr.empty()) throw false;
					while (true) {
						if (rawStr[startpos] != ' '&&rawStr[startpos] != '\n'&&rawStr[startpos] != '\r'&&rawStr[startpos] != '\t')
							return startpos;
						if (startpos == 0) break;
						startpos--;
					}
					throw false;
				}

				std::size_t findNeXTcharReverse(const char& findthis, std::size_t startpos) {//�ҵ���һ���ַ�������"name:  s"����5��ʼ�ң��ҵ�s����������;�����и�ʽ���Ʒ�
																							 //�Ҳ������쳣
					if (rawStr.empty()) throw false;
					while (true) {
						if (rawStr[startpos] == findthis)
							return startpos;
						if (startpos == 0) break;
						startpos--;
					}
					throw false;
				}

				inline bool makeIsStr()noexcept {
					std::size_t firstChar = 0, lastChar = 0;

					//�жϺϲ��Ϸ�
					try {
						firstChar = this->findNeXTchar(0);
						lastChar = this->findNeXTcharReverse(rawStr.size() - 1);
						if (firstChar > lastChar || firstChar == lastChar) throw false;
						if (rawStr[lastChar] != '"' || rawStr[firstChar] != '"') throw false;
					}
					catch (...) {
						return false;
					}
					return true;
				}

			private:
				bool isStr;//ͨ��makeIsStr�ж��Ƿ�Ϊ�ַ���
			};

			class number final :public jsonSpace::base {
			public:
				number() = default;

				number(const number& input) {
					setRawStr(input.rawStr);
				}

				number(number&& input)noexcept {
					setRawStr(std::move(input.rawStr));
				}

				number(const std::string& input) {
					set(input);
				}

				number(std::string&& input) {
					set(std::move(input));
				}

				virtual ~number() = default;

			public:
				number& operator=(const number& input) {
					if (this == &input)
						return *this;
					setRawStr(input.rawStr);
					return *this;
				}

				number& operator=(number&& input)noexcept {
					if (this == &input)
						return *this;
					setRawStr(std::move(input.rawStr));
					return *this;
				}

				bool operator==(const number& input)const {
					try {
						return this->as_double() == input.as_double();
					}
					catch (...) {
						throw DC::DC_ERROR("number::operator==", "can not as number");
					}
				}

				bool operator>(const number& input)const {
					try {
						return this->as_double() > input.as_double();
					}
					catch (...) {
						throw DC::DC_ERROR("number::operator>", "can not as number");
					}
				}

				bool operator<(const number& input)const {
					try {
						return this->as_double() < input.as_double();
					}
					catch (...) {
						throw DC::DC_ERROR("number::operator<", "can not as number");
					}
				}

				bool operator>=(const number& input)const {
					try {
						return this->as_double() >= input.as_double();
					}
					catch (...) {
						throw DC::DC_ERROR("number::operator>=", "can not as number");
					}
				}

				bool operator<=(const number& input)const {
					try {
						return this->as_double() <= input.as_double();
					}
					catch (...) {
						throw DC::DC_ERROR("number::operator<=", "can not as number");
					}
				}

			public:
				virtual inline void set(const std::string& input)override {
					setRawStr(input);
				}

				virtual inline void set(std::string&& input)override {
					setRawStr(std::move(input));
				}

				inline bool is_double()const noexcept {
					try {
						return DC::STR::find(rawStr, ".").places.size() == 1;
					}
					catch (...) {
						return false;
					}
				}

				inline bool is_null()const noexcept {
					return rawStr == "null";
				}

				inline bool is_int32()const noexcept {
					return !is_double() && !is_null();
				}

				inline bool is_empty()const noexcept {
					return rawStr.empty();
				}

				inline int32_t as_int32()const {
					try {
						return DC::STR::toType<int32_t>(rawStr);
					}
					catch (...) {
						throw DC::DC_ERROR("number::as_int32", "can not as int32");
					}
				}

				inline double as_double()const {
					try {
						return DC::STR::toType<double>(rawStr);
					}
					catch (...) {
						throw DC::DC_ERROR("number::as_double", "can not as double");
					}
				}

				inline DC::var as_var()const {
					return DC::var(rawStr);
				}

				inline DC::var to_var() {
					return DC::var(std::move(rawStr));
				}

			protected:
				virtual inline void RemoveOutsideSymbol() {}

				virtual inline void refresh()noexcept {}
			};

			class object :public jsonSpace::base {
				friend class transparent;
			public:
				object() :RemoveOutsideSymbolFlag(false) {}

				object(const object& input) :ObjectSymbolPair(input.ObjectSymbolPair), ArraySymbolPair(input.ArraySymbolPair), StringSymbolPair(input.StringSymbolPair), RemoveOutsideSymbolFlag(input.RemoveOutsideSymbolFlag) {
					setRawStr(input.rawStr);
				}

				object(object&& input)noexcept : ObjectSymbolPair(std::move(input.ObjectSymbolPair)), ArraySymbolPair(std::move(input.ArraySymbolPair)), StringSymbolPair(std::move(input.StringSymbolPair)), RemoveOutsideSymbolFlag(input.RemoveOutsideSymbolFlag) {
					setRawStr(std::move(input.rawStr));
				}

				object(const std::string& input) :RemoveOutsideSymbolFlag(false) {
					set(input);
				}

				object(std::string&& input) :RemoveOutsideSymbolFlag(false) {
					set(std::move(input));
				}

				virtual ~object() = default;

			protected:
				template <typename strT, typename T>
				object(strT&& inputrawStr, T&& inputOS, T&& inputAS, T&& inputSS) : jsonSpace::base(std::forward<strT>(inputrawStr)), ObjectSymbolPair(std::forward<T>(inputOS)), ArraySymbolPair(std::forward<T>(inputAS)), StringSymbolPair(std::forward<T>(inputSS)), RemoveOutsideSymbolFlag(false) {
					static_assert(std::is_same<std::vector<jsonSpace::PosPair>, typename std::decay<T>::type>::value, "input type must be std::vector<jsonSpace::PosPair>");
					static_assert(std::is_same<std::string, typename std::decay<strT>::type>::value, "input type must be std::string");
					//���ű����õĹ��캯��
				}

			public:
				object& operator=(const object& input) {
					if (this == &input)
						return *this;
					setRawStr(input.rawStr);
					ObjectSymbolPair = input.ObjectSymbolPair;
					ArraySymbolPair = input.ArraySymbolPair;
					StringSymbolPair = input.StringSymbolPair;
					return *this;
				}

				object& operator=(object&& input)noexcept {
					if (this == &input)
						return *this;
					setRawStr(std::move(input.rawStr));
					ObjectSymbolPair = std::move(input.ObjectSymbolPair);
					ArraySymbolPair = std::move(input.ArraySymbolPair);
					StringSymbolPair = std::move(input.StringSymbolPair);
					return *this;
				}

			public:
				inline virtual void set(const std::string& input)override {
					setRawStr(input);
					try {
						RemoveOutsideSymbol();
						refresh();
					}
					catch (const DC::DC_ERROR& ex) {
						if (!rawStr.empty()) rawStr.clear();
						throw ex;
					}
				}

				inline virtual void set(std::string&& input)override {
					setRawStr(std::move(input));
					try {
						RemoveOutsideSymbol();
						refresh();
					}
					catch (const DC::DC_ERROR& ex) {
						if (!rawStr.empty()) rawStr.clear();
						throw ex;
					}
				}

				transparent at(const std::string& key)const {
					//����key
					auto findResult_Full = DC::STR::find(rawStr, jsonSpace::GetJsStr(key));
					auto findResult = findResult_Full.places;
					auto findNeXTcharReverse = [this](const DC::pos_type& from) {
						if (from <= 0 || this->rawStr.size() == 0 || from + 1 > this->rawStr.size()) throw false;//û�ҵ�
						for (auto i = from - 1; i >= 0; i--) {
							if (this->rawStr[i] != ' '&&this->rawStr[i] != '\n'&&this->rawStr[i] != '\r'&&this->rawStr[i] != '\t')
								return i;
						}
						throw false;
					};

					//�ж�key�ڱ��㣨�Ȳ����ַ����ڣ��ֲ������������ڣ�ͬʱҲ������һ��value��
					for (auto i = findResult.begin(); i != findResult.end();) {
						auto isValue = [&findNeXTcharReverse, this](const DC::pos_type& input) {//�Ƿ�Ϊһ��value(��ð��֮���
							std::result_of<decltype(findNeXTcharReverse)(const DC::pos_type&)>::type result;//���������findNeXTcharReverse�ķ���ֵ����							
							try {
								if (input - 1 < 0 || input - 1 + 1 > this->rawStr.size()) return false;//�±�Ƿ���
								if (this->rawStr[input - 1] == ':') return true;
								result = findNeXTcharReverse(input - 1);
								if (result<0 || result + 1>this->rawStr.size()) return false;//�±�Ƿ���
								if (this->rawStr[result] == ':') return true;
							}
							catch (bool&) {
								return false;//û�ҵ�ǰһ���ַ���Ҳ˵������value
							}
							return false;
						};

						if (isInsideStr(*i) || isInsideObj(*i) || isInsideArr(*i) || isValue(*i)) {
							i = DC::vector_fast_erase(findResult, i);
							continue;
						}
						i++;
					}
					//����ж��key�����쳣
					if (findResult.size() < 1) {
						throw DC::DC_ERROR("object::at", "cant find key");
					}
					if (findResult.size() > 1) {
						throw DC::DC_ERROR("object::at", "too much key");
					}
					std::size_t startpos = 0, endpos = 0, temp;
					//��key֮���һ��ð��
					try {
						temp = findNeXTchar(':', *findResult.begin() + findResult_Full.remove_length);
					}
					catch (...) {
						throw DC::DC_ERROR("object::at", "can not find separator");//�Ҳ���ð��
					}
					//��ð��֮���һ�������ַ�������ַ�����value�Ŀ�ʼ
					try {
						startpos = findNeXTchar(temp + 1);
					}
					catch (...) {
						//����Ϊ��
						startpos = temp + 1;
					}
					//��value�Ľ��������value���Է��ŶԿ�ʼ�ģ�����value��һ�����󣩣���ôֱ����֮ǰ���ɺõķ��ű����ҵ����ŶԵĽ��������value�����Է��ŶԿ�ʼ�ģ���������Ľ�������
					switch (rawStr[startpos]) {
					case '{': {
						for (const auto& p : ObjectSymbolPair) {
							if (p.first == startpos) {
								endpos = p.second + 1;
								break;
							}
						}
					}break;
					case '[': {
						for (const auto& p : ArraySymbolPair) {
							if (p.first == startpos) {
								endpos = p.second + 1;
								break;
							}
						}
					}break;
					case '\"': {
						for (const auto& p : StringSymbolPair) {
							if (p.first == startpos) {
								endpos = p.second + 1;
								break;
							}
						}
					}break;
					default: {
						//�ֱ��ҳ����ֽ������������
						std::size_t close0 = INT_MAX, close1 = INT_MAX, close2 = INT_MAX;
						int32_t flag = 0;
						try {
							close0 = findNeXTchar('}', startpos);
							close0--;
						}
						catch (...) {
							flag++;
						}
						try {
							close1 = findNeXTchar(']', startpos);
							close1--;
						}
						catch (...) {
							flag++;
						}
						try {
							close2 = findNeXTchar(',', startpos);
							close2--;
						}
						catch (...) {
							flag++;
						}

						//���������û��
						if (flag == 3) {
							for (endpos = startpos; endpos < rawStr.size(); endpos++) {
								if (rawStr[endpos] == ' ' || rawStr[endpos] == '\n' || rawStr[endpos] == '\r' || rawStr[endpos] == '\t') break;
							}
							break;
						}

						//������С�Ĵ���rv��
						endpos = close0;
						if (close1 < endpos) endpos = close1;
						if (close2 < endpos) endpos = close2;
						endpos++;
					}break;
					}

					//�жϺϷ�
					if (startpos > endpos)
						throw DC::DC_ERROR("object::at", "substring length illegal");//�Ӵ����Ȳ��Ϸ�
																					 //��ȡ�Ӵ�
																					 /*this�ڵĺ�����STR::getSub()�����߼���ͬ������ĳЩ�ط�������+1��-1�Ĺ�����
																					 this�ڵĺ�����startposִ�е�endpos�����а���startpos��endpos��ָ���λ�ñ���
																					 ��getSub��ֻ���startpos��endpos֮��Ľ��в�����������posָ���λ�ñ�������startpos==endposʱ��getSub���᷵��һ���մ�*/

					auto check = std::bind(&object::has_table_to_transport, this, std::placeholders::_1, startpos - 1, endpos);
					if (check(ObjectSymbolPair) || check(ArraySymbolPair) || check(StringSymbolPair))
						return get_child(jsonSpace::PosPair(startpos - 1, endpos));
					return transparent(STR::getSub(rawStr, startpos - 1, endpos));
				}

			protected:
				virtual void RemoveOutsideSymbol() {//ɾ��������ķ��Ŷ�
					if (RemoveOutsideSymbolFlag) return;
					RemoveOutsideSymbolFlag = true;

					bool flag = false;

					for (auto i = rawStr.begin(); i != rawStr.end(); i++) {
						if (*i == '{') {
							rawStr.erase(i);
							flag = true;
							break;
						}
					}

					if (flag != true) return;

					for (auto i = rawStr.rbegin(); i != rawStr.rend(); i++) {
						if (*i == '}') {
							rawStr.erase(--i.base());
							flag = false;
							break;
						}
					}

					if (flag == true)
						throw DC::DC_ERROR("object::RemoveOutsideSymbol", "can not find end symbol");
				}

				virtual void refresh() {
					try {
						StringSymbolPair = this->getStringSymbolPair(rawStr);
						ObjectSymbolPair = getSymbolPair("{", "}");
						ArraySymbolPair = getSymbolPair("[", "]");
					}
					catch (const DC::DC_ERROR& ex) {
						clear_except_rawStr();
						throw ex;
					}
				}

			protected:
				inline bool isInsideStr(const DC::pos_type& input)const {//inputλ���Ƿ���js�ַ���������js�û��ַ�������
					for (const auto& p : StringSymbolPair)
						if (input > p.first && input < p.second) return true;
					return false;
				}

				inline bool isInsideObj(const DC::pos_type& input)const {//inputλ���Ƿ���js�ַ���������js�û��ַ�������
					for (const auto& p : ObjectSymbolPair)
						if (input > p.first && input < p.second) return true;
					return false;
				}

				inline bool isInsideArr(const DC::pos_type& input)const {//inputλ���Ƿ���js�ַ���������js�û��ַ�������
					for (const auto& p : ArraySymbolPair)
						if (input > p.first && input < p.second) return true;
					return false;
				}

				inline bool is_Obj_or_Arr_find_if_pred_func(const jsonSpace::PosPair& something_inside, const jsonSpace::PosPair& input)const {
					return input.first == something_inside.first - 1 && input.second == something_inside.second + 1;
				}

				inline bool is_Obj_or_Arr(const jsonSpace::PosPair& input, const std::vector<jsonSpace::PosPair>& symbols)const {//�ж�input�Ƿ�Ϊsymbols��ĳһ����Ա���жϺ���Ϊis_Obj_or_Arr_find_if_pred_func
					return std::find_if(symbols.begin(), symbols.end(), std::bind(&object::is_Obj_or_Arr_find_if_pred_func, this, std::placeholders::_1, std::cref(input))) != symbols.end();
				}

				std::vector<jsonSpace::PosPair> getStringSymbolPair(const std::string str)const {//�ҳ���Ե�""
																								 //����Ƕ��Ŷ
																								 //��һ�β�����������
					auto tstr = DC::STR::replace(str, DC::STR::find(str, R"(\")"), "  ");//��\"���������ո񣬼ȱ�֤�˳��Ȳ��䣬�ֱ�֤��ȥ���û��ַ������������
					std::vector<std::size_t> AllSymbol = std::move(DC::STR::find(tstr, "\"").places);
					std::vector<jsonSpace::PosPair> returnvalue;

					if (AllSymbol.size() % 2 != 0)
						throw DC::DC_ERROR("invalid string", "string symbols \"\" can not be paired");

					for (std::size_t p = 0; p < AllSymbol.size(); ++p) {
						auto temp1 = AllSymbol[p];//ȷ��hapens-before
						++p;
						returnvalue.emplace_back(temp1, AllSymbol[p]);
					}

					return returnvalue;
				}

				std::vector<jsonSpace::PosPair> getSymbolPair(const char* const StartSymbol, const char* const EndSymbol)const {//��ֹjs�ַ�����ķ���Ӱ��
					std::vector<std::size_t> AllStartSymbolRaw = DC::STR::find(rawStr, StartSymbol).places, AllEndSymbolRaw = DC::STR::find(rawStr, EndSymbol).places;
					std::vector<std::size_t> AllStartSymbol, AllEndSymbol;
					std::vector<jsonSpace::PosPair> returnvalue;

					for (auto i = AllStartSymbolRaw.begin(); i != AllStartSymbolRaw.end(); i++) {
						if (isInsideStr(*i)) {
							continue;
						}
						AllStartSymbol.emplace_back(std::move(*i));
					}
					for (auto i = AllEndSymbolRaw.begin(); i != AllEndSymbolRaw.end(); i++) {
						if (isInsideStr(*i)) {
							continue;
						}
						AllEndSymbol.emplace_back(std::move(*i));
					}

					//ע�͵���������Ϊ JSON �ַ���������{}��[]
					//if (!jsonSpace::SybolValid(AllStartSymbol, AllEndSymbol)) throw DC::DC_ERROR("invalid string", "symbols can not be paired", 0);//�жϿ�ʼ���źͽ������������Ƿ�һ��				
					//����㷨�������ڡ�����AllStartSymbol�е����һ��Ԫ�������������AllEndSymbolԪ�ر�Ȼ������֮��ԡ���
					for (auto i = AllStartSymbol.rbegin(); i != AllStartSymbol.rend(); i = AllStartSymbol.rbegin()) {
						std::size_t minimal = INT_MAX;//int�������ֵ
						auto iter = AllEndSymbol.end();
						for (auto i2 = AllEndSymbol.begin(); i2 != AllEndSymbol.end(); i2++) {
							if ((!(*i2 < *i)) && (*i2 - *i) < minimal) { minimal = *i2 - *i; iter = i2; }//�ҳ��͵�ǰ��ʼ��������Ľ�������
						}
						if (iter == AllEndSymbol.end())
							throw DC::DC_ERROR("undefined behavior", 0);//������Ӧ�ò����׳������ǷŽ������ַ����������﷨
						returnvalue.emplace_back(*i, *iter);
						DC::vector_fast_erase_no_return(AllStartSymbol, --i.base());
						DC::vector_fast_erase_no_return(AllEndSymbol, iter);
					}
					return returnvalue;
				}

				inline void clear_except_rawStr() {
					if (!StringSymbolPair.empty()) StringSymbolPair.clear();
					if (!ObjectSymbolPair.empty()) ObjectSymbolPair.clear();
					if (!ArraySymbolPair.empty()) ArraySymbolPair.clear();
				}

				inline bool has_table_to_transport(const std::vector<jsonSpace::PosPair>& table, const DC::pos_type& start, const DC::pos_type& end)const {
					auto findfunc = [&start, &end](const jsonSpace::PosPair& res) {
						if (res.first > start&&res.first < end) return true;
						return false;
					};
					if (std::find_if(table.begin(), table.end(), findfunc) == table.end()) return false;
					return true;
				}

				std::vector<jsonSpace::PosPair> get_transport_table(const std::vector<jsonSpace::PosPair>& table, const jsonSpace::PosPair& pos)const {
					std::vector<jsonSpace::PosPair> rv;
					for (const auto& p : table)
						if (p.first > pos.first + 1 && p.first < pos.second - 1) rv.emplace_back(p.first - pos.first - 2, p.second - pos.first - 2);
					return rv;
				}

				transparent get_child(const jsonSpace::PosPair& pos)const;

			protected:
				std::vector<jsonSpace::PosPair> ObjectSymbolPair, ArraySymbolPair, StringSymbolPair;
				bool RemoveOutsideSymbolFlag;
			};

			class array final :public object{
				friend class transparent;
				friend class object;
			public:
				array() : object() {}

				array(const array& input) {
					ArraySymbolPair = input.ArraySymbolPair;
					StringSymbolPair = input.StringSymbolPair;
					ObjectSymbolPair = input.ObjectSymbolPair;
					setRawStr(input.rawStr);
					RemoveOutsideSymbolFlag = input.RemoveOutsideSymbolFlag;
				}

				array(array&& input)noexcept {
					ArraySymbolPair = std::move(input.ArraySymbolPair);
					StringSymbolPair = std::move(input.StringSymbolPair);
					ObjectSymbolPair = std::move(input.ObjectSymbolPair);
					setRawStr(std::move(input.rawStr));
					RemoveOutsideSymbolFlag = input.RemoveOutsideSymbolFlag;
				}

				array(const std::string& input) : object() {
					set(input);
				}

				array(std::string&& input) : object() {
					set(std::move(input));
				}

				using size_type = std::size_t;

			protected:
				template <typename strT, typename T>
				array(strT&& inputrawStr, T&& inputOS, T&& inputAS, T&& inputSS) : object(std::forward<strT>(inputrawStr), std::forward<T>(inputOS), std::forward<T>(inputAS), std::forward<T>(inputSS)) {
					static_assert(std::is_same<std::vector<jsonSpace::PosPair>, typename std::decay<T>::type>::value, "input type must be std::vector<jsonSpace::PosPair>");
					static_assert(std::is_same<std::string, typename std::decay<strT>::type>::value, "input type must be std::string");
					//���ű����õĹ��캯��
				}

			public:
				array& operator=(const array& input) {
					if (this == &input)
						return *this;
					setRawStr(input.rawStr);
					ObjectSymbolPair = input.ObjectSymbolPair;
					ArraySymbolPair = input.ArraySymbolPair;
					StringSymbolPair = input.StringSymbolPair;
					return *this;
				}

				array& operator=(array&& input)noexcept {
					if (this == &input)
						return *this;
					setRawStr(std::move(input.rawStr));
					ObjectSymbolPair = std::move(input.ObjectSymbolPair);
					ArraySymbolPair = std::move(input.ArraySymbolPair);
					StringSymbolPair = std::move(input.StringSymbolPair);
					return *this;
				}

				inline transparent operator[](const std::size_t& index)const {
					if (index > ObjectSymbolPair.size() - 1) throw DC::DC_ERROR("array::operator[]", "index out of range");//size_t�޷��Ų�����С�������Բ�����С��0
					return DC::STR::getSub(rawStr, ObjectSymbolPair[index].first - 1, ObjectSymbolPair[index].second + 1);
				}

			public:
				inline virtual void set(const std::string& input)override {
					setRawStr(input);
					try {
						RemoveOutsideSymbol();
						refresh();
					}
					catch (const DC::DC_ERROR& ex) {
						if (!rawStr.empty()) rawStr.clear();
						throw ex;
					}
				}

				inline virtual void set(std::string&& input)override {
					setRawStr(std::move(input));
					try {
						RemoveOutsideSymbol();
						refresh();
					}
					catch (const DC::DC_ERROR& ex) {
						if (!rawStr.empty()) rawStr.clear();
						throw ex;
					}
				}

				inline bool is_empty()const {
					return ObjectSymbolPair.empty();
				}

				inline size_type size()const {
					return ObjectSymbolPair.size();
				}

				inline transparent at(const std::size_t& index)const {
					if (index > ObjectSymbolPair.size() - 1) throw DC::DC_ERROR("array::operator[]", "index out of range");//size_t�޷��Ų�����С�������Բ�����С��0
					return DC::STR::getSub(rawStr, ObjectSymbolPair[index].first - 1, ObjectSymbolPair[index].second + 1);
				}

			protected:
				virtual void RemoveOutsideSymbol()override {//ɾ��������ķ��Ŷ�
					if (RemoveOutsideSymbolFlag) return;
					RemoveOutsideSymbolFlag = true;

					bool flag = false;

					for (auto i = rawStr.begin(); i != rawStr.end(); i++) {
						if (*i == '[') {
							rawStr.erase(i);
							flag = true;
							break;
						}
					}

					if (flag != true) return;

					for (auto i = rawStr.rbegin(); i != rawStr.rend(); i++) {
						if (*i == ']') {
							rawStr.erase(--i.base());
							flag = false;
							break;
						}
					}

					if (flag == true)
						throw DC::DC_ERROR("object::RemoveOutsideSymbol", "can not find end symbol");
				}

				virtual void refresh()override {
					try {
						StringSymbolPair = this->getStringSymbolPair(rawStr);
						ObjectSymbolPair = getSymbolPair("{", "}");
						ArraySymbolPair = getSymbolPair("[", "]");

						for (auto i = ObjectSymbolPair.begin(); i != ObjectSymbolPair.end();) {
							if (isInsideStr(i->first) || isInsideObj(i->first) || isInsideArr(i->first)) {
								i = DC::vector_fast_erase(ObjectSymbolPair, i);
								//i = ObjectSymbolPair.erase(i);
								continue;
							}
							i++;
						}

						std::reverse(ObjectSymbolPair.begin(), ObjectSymbolPair.end());
					}
					catch (const DC::DC_ERROR& ex) {
						clear_except_rawStr();
						throw ex;
					}
				}
			};

			transparent::transparent(const std::string& input) :withTable(false) {
				set(input);
			}

			transparent::transparent(std::string&& input) : withTable(false) {
				set(std::move(input));
			}

			inline object&& transparent::to_object() {
				if (withTable) {
					//��̬ת��������ָ�룬ʧ�����쳣
					auto ptr_with_type(dynamic_cast<object*>(ptr.get()));
					if (!ptr_with_type) throw DC::Exception("json::transparent::to_object", "bad cast");

					//��һЩ����Ȼ�󷵻�
					ptr_with_type->RemoveOutsideSymbol();
					return std::move(*ptr_with_type);
				}
				return to_something<json::object>();
			}

			inline value& transparent::as_value() {
				return as_something<json::value>();
			}

			inline value&& transparent::to_value() {
				return to_something<json::value>();
			}

			inline number& transparent::as_number() {
				return as_something<json::number>();
			}

			inline number&& transparent::to_number() {
				return to_something<json::number>();
			}

			inline array&& transparent::to_array() {
				if (withTable) {
					//��̬ת��������ָ�룬ʧ�����쳣
					auto ptr_with_type(dynamic_cast<array*>(ptr.get()));
					if (!ptr_with_type) throw DC::Exception("json::transparent::to_array", "bad cast");

					//��һЩ����Ȼ�󷵻�
					ptr_with_type->RemoveOutsideSymbol();
					for (auto i = ptr_with_type->ObjectSymbolPair.begin(); i != ptr_with_type->ObjectSymbolPair.end();) {
						if (ptr_with_type->isInsideStr(i->first) || ptr_with_type->isInsideObj(i->first) || ptr_with_type->isInsideArr(i->first)) {
							i = DC::vector_fast_erase(ptr_with_type->ObjectSymbolPair, i);
							continue;
						}
						i++;
					}
					std::sort(ptr_with_type->ObjectSymbolPair.begin(), ptr_with_type->ObjectSymbolPair.end(), [](const jsonSpace::PosPair& first, const jsonSpace::PosPair& second) {
						return first.first < second.first;
					});
					return std::move(*ptr_with_type);
				}
				return to_something<json::array>();
			}

			inline transparent object::get_child(const jsonSpace::PosPair& pos)const {
				transparent rv;
				rv.rawStr = DC::STR::getSub(rawStr, pos.first, pos.second);
				const auto& lookA = is_Obj_or_Arr(pos, this->ArraySymbolPair);
				const auto& lookO = is_Obj_or_Arr(pos, this->ObjectSymbolPair);
				if (lookA&&lookO) throw DC::Exception("json::object::get_child", "pos illegal");//A��Oֻ����һ��Ϊ�棬���������Ϊ�棬�ǲ�Ӧ�÷����ģ��˴��׳��쳣������Ϊʲô��������Ϊ��ʱ�����쳣������Ϊ���get�Ķ�����һ��value����number���Ǿͻᶼ��Ϊ�棬���������������
				if (lookO)
					rv.ptr.reset(new object(rv.rawStr, get_transport_table(ObjectSymbolPair, pos), get_transport_table(ArraySymbolPair, pos), get_transport_table(StringSymbolPair, pos)));
				if (lookA)
					rv.ptr.reset(new array(rv.rawStr, get_transport_table(ObjectSymbolPair, pos), get_transport_table(ArraySymbolPair, pos), get_transport_table(StringSymbolPair, pos)));
				//������Ե�ʱ��ptr����dynamic_cast��array������Ϊarray����private�̳У������ǿ�����������Ϣ�ģ����Բ���������ת����͸������Ԫ�����Կ�����͸����ת
				rv.withTable = true;
				return rv;
			}

		}

	}

}

#endif
