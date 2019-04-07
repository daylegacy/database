#ifndef LIST_H
#define LIST_H
#include "helper.h"
#include "avl_tree.h"
template<class T>
class avl_tree;

template<class T>
struct list_node : public T{
	list_node * next;
	list_node * prev;
	
	list_node() :T(){
		next = nullptr;
		prev = nullptr;
	}

	list_node(const T & data) :T(data){
		prev = nullptr;
		next = nullptr;
		printD("copy cons\n");
	}
	~list_node() {
		//printf("list node destructor\n");
		next = nullptr;
		prev = nullptr;
	}
};

template<class T>
class list {
	list_node<T> *head;
	list_node<T> *tail;
	int size;
public:
	list() {
		head = tail = nullptr;
		size = 0;
	}

	~list() {
		list_node<T> *node = head;
		list_node<T> * tmp;
		while (node) {
			tmp = node->next;
			delete node;
			node = tmp;
		}
		head = tail = nullptr; size = 0;
	}


	list_node<T> * push(const T & data) {
		auto new_node = new list_node<T>(data);
		if (tail) {
			new_node->prev = tail;
			tail->next = new_node;
			tail = new_node;
		}
		else {
			tail = head = new_node;
		}
		size++;
		return new_node;
	}


	void delete_list_node(list_node<T> * node) {
		if (!head) return;
		if (node->prev) {
			node->prev->next = node->next;
		}
		else {
			head = node->next;
		}
		if (node->next) {
			node->next->prev = node->prev;
		}
		else {
			tail = node->prev;
		}
		size--;
		//printf("delete list node ");
		//node->print();
		delete node;
	}

	bool find(const T& sample) {
		list_node<T> * tmp = head;
		while (tmp) {
			if (sample == *tmp) return 1;
			tmp = tmp->next;
		}
		return 0;
	}

	int init_from_file(const char * filename) {
		FILE * fp = fopen(filename, "r");
		if (!fp) return 0;
		T data;
		while (data.scan(fp)) {
			push(data);
		}
		fclose(fp);
		return 1;
	}
	list_node<T> * gethead() {
		return head;
	}
};

#endif // !LIST_H
