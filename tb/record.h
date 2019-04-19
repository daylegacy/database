#ifndef RECORD_H
#define RECORD_H
#include "helper.h"
#include "vector.h"
class record {
	vector<char> name;
	long long  phone = 0;
	int group = 0;
public:
	record() { ; }
	record(const char * name, long long phone, int group)
		:phone(phone), group(group) 
	{
		set_name(name);
	}
	~record() {
		printD("record destructor\n");
		//name.release();
	}
	record(const record & other) :name(other.name), phone(other.phone), group(other.group) {
		//printf("copy cons\n");
	}
	inline char * get_name() const{
		return name;
	}
	inline long long get_phone() const{
		return phone;
	}
	inline int get_group() const{
		return group;
	}
	void set_name(const char * str) {
		int len = strlen(str);
		int delta = len + 1 - name.getcapacity();
		name.setsize(len+1);
		if (delta > 0 ) {
			name.reallocate(len + 1);
		}
		strcpy(name, str);
	}
	void set_phone(long long phone) {
		this->phone = phone;
	}
	void set_group(int group) {
		this->group = group;
	}
	void print() const{
		printf("  %-17s%-7lld  %-3d\n", (char*)name, phone, group);
	}
	int scan(FILE *fp) {
		char buf_n[LEN];
		long long buf_p;
		int buf_g;
		if (fscanf(fp, "%s %lld %d", buf_n, &buf_p, &buf_g) == 3) {
			set_name(buf_n);
			set_group(buf_g);
			set_phone(buf_p);
			return 1;
		}
		return 0;
	}
	bool operator==(const record & other) const{
		return (strcmp(this->name, other.name) == 0 && this->group == other.group && this->phone == other.phone) ? 1 : 0;
	}
	__attribute__((always_inline)) inline int cmp(const record & other) const {
		int cmp_name = strcmp(this->name, other.name);
		if (cmp_name > 0) return 1;
		else if (cmp_name < 0) return -1;
		else {
			int cmp_phone = this->phone - other.phone;
			if (cmp_phone>0) return 1;
			else if (cmp_phone < 0) return -1;
			else {
				return this->group - other.group;
			}
		}
	}
	__attribute__((always_inline)) inline int cmp_name(const record & other) const {
		return strcmp(this->name, other.name);
	}
	__attribute__((always_inline)) inline int cmp_phone(const record & other) const {
		return this->phone - other.phone;
	}
	__attribute__((always_inline)) inline int cmp_sec(const record & other) const {
		int cmp_phone = this->phone - other.phone;
		if (cmp_phone > 0) return 1;
		else if (cmp_phone < 0) return -1;
		else {
			int cmp_name = strcmp(this->name, other.name);
			if (cmp_name > 0) return 1;
			else if (cmp_name < 0) return -1;
			else {
				return this->group - other.group;
			}
		}
	}
	inline bool operator>(const record & other) const {
		return cmp(other) == 1 ? 1:0;
	}
	__attribute__((always_inline)) inline bool operator<(const record & other) const {
		return cmp(other) < 0 ? 1 : 0;
	}
	inline bool operator>=(const record & other) const {
		return cmp(other) != -1 ? 1 : 0;
	}
	inline bool operator<=(const record & other) const {
		return cmp(other) != 1 ? 1 : 0;
	}
};

#endif // !RECORD_H
