#ifndef DATABASE_H
#define DATEBASE_H
#include "parser.h"
#include "avl_tree.h"
#include "list.h"
#include "like.h"

#include <sys/time.h>

template<class T>
class db_avl;

template<class T>
class db_list : public list<T>{
public:
	int select_with_cmd(const command & cmd) {
		int i = 0;
		list_node<T> *node = this->gethead();
		while (node) {
			if (satisfy(*node, cmd)) {
				//node->print();
				i++;
			}
			node = node->next;
		}
		return i;
	}

	int delete_with_cmd(const command & cmd, avl_tree<list_node<T>> * search_tree) {
		int i = 0;
		list_node<T> *node = this->gethead();
		list_node<T> *tmp;
		while (node) {
			tmp = node->next;
			if (satisfy(*node, cmd)) {
				node->print(); printf("\n ");
				int size = search_tree->avl_size;
				if (size < 10) {
					printf("d = %d\n ", size);
					search_tree->print();
				}
				
				if (search_tree) {
					search_tree->avl_delete(node);
				}
				this->delete_list_node(node);
				
				i++;
			}
			node = tmp;
		}
		if (search_tree) {
			search_tree->print();
			search_tree->is_it_avl();
		}
		return i;
	}
};

template<class T>
class db_avl : public avl_tree<T> {
public:
	int counter;
	int checked;

	int select_with_cmd(const command & cmd) {
		int i = 0;
		avl_node<T> *node = nullptr;
		counter = 0;
		checked = 0;
		const list_node<command> ln_cmd(cmd);
		if (cmd.c_name == EQ) search_eq(this->root, ln_cmd);
		else search_gt(this->root, ln_cmd);
		return counter;
	}


	void search_eq(avl_node<T> * node, const list_node<command> & cmd) {
		if (!node) return;
		checked++;
		int cmp = cmd.cmp(*(record*)node->pdata);
		if		(cmp > 0) { search_eq(node->link[1], cmd); }
		else if (cmp < 0) { search_eq(node->link[0], cmd); }
		else {
			
			search_eq(node->link[0], cmd);
			search_eq(node->link[1], cmd);
			if (satisfy(*node->pdata, cmd, 1)) {
				counter++;
				//node->pdata->print();
			}
			//printf("-----\n");
		}
	}

	int delete_eq(avl_node<T> * node, const list_node<command> & cmd, list<record>* storage) {
		if (!node) return 0;
		
		int cmp = node->pdata->cmp_name(cmd);
		if (cmp > 0) { delete_eq(node->link[0], cmd, storage); }
		else if (cmp < 0) { delete_eq(node->link[1], cmd, storage); }
		else {

			/*delete_eq(node->link[0], cmd, storage);
			delete_eq(node->link[1], cmd, storage);*/
			if (satisfy(*node->pdata, cmd, 1)) {
				counter++;
				checked++;
				this->avl_delete(node->pdata);
				storage->delete_list_node(node->pdata, nullptr);
			}
			//printf("-----\n");
		}
		return counter;
	}


	void search_gt(avl_node<T> * node, const list_node<command> & cmd) {
		if (!node) return;
		checked++;
		switch (cmd.c_name)
		{
		case GT:
			if (node->pdata->cmp_name(cmd) > 0) {
				if (satisfy(*node->pdata, cmd, 1)) {
					counter++;
					//node->pdata->print();
				}
				print_and_count(node->link[1], cmd);
				search_gt(node->link[0], cmd);
			}
			else {
				search_gt(node->link[1], cmd);
			}
			return;
		case LT:
			if (node->pdata->cmp_name(cmd) < 0) {
				if (satisfy(*node->pdata, cmd, 1)) {
					counter++;
					//node->pdata->print();
				}
				print_and_count(node->link[0], cmd);
				search_gt(node->link[1], cmd);
			}
			else {
				search_gt(node->link[0], cmd);
			}
			return;
		case LE:
			if (node->pdata->cmp_name(cmd) <= 0) {
				if (satisfy(*node->pdata, cmd, 1)) {
					counter++;
					//node->pdata->print();
				}
				print_and_count(node->link[0], cmd);
				search_gt(node->link[1], cmd);
			}
			else {
				search_gt(node->link[0], cmd);
			}
			return;
		case GE:
			if (node->pdata->cmp_name(cmd) >= 0) {
				if (satisfy(*node->pdata, cmd, 1)) {
					counter++;
					//node->pdata->print();
				}
				print_and_count(node->link[1], cmd);
				search_gt(node->link[0], cmd);
			}
			else {
				search_gt(node->link[1], cmd);
			}
			return;
		default:
			return;
		}
		
	}

	void  print_and_count(avl_node<T> * node, const list_node<command> & cmd) {
		if (!node) return;
		if (satisfy(*node->pdata, cmd, 1)) {
			counter++;
			//node->pdata->print();
		}
		print_and_count(node->link[0], cmd);
		print_and_count(node->link[1], cmd);
	}

	void  delete_and_count(avl_node<T> * node, const list_node<command> & cmd, list<record>* storage) {
		if (!node) return;
		if (satisfy(*node->pdata, cmd, 1)) {
			checked++;
			counter++;
			this->avl_delete(node->pdata);
			storage->delete_list_node(node->pdata, nullptr);
		}
		delete_and_count(node->link[0], cmd, storage);
		delete_and_count(node->link[1], cmd, storage);
	}

	void delete_gt(avl_node<T> * node, const list_node<command> & cmd, list<record>* storage) {
		if (!node) return;
		switch (cmd.c_name)
		{
		case GT:
			if (node->pdata->cmp_name(cmd) > 0) {
				
				delete_and_count(node->link[1], cmd, storage);
				delete_gt(node->link[0], cmd, storage);
				if (satisfy(*node->pdata, cmd, 1)) {
					counter++; checked++;
					this->avl_delete(node->pdata);
					storage->delete_list_node(node->pdata, nullptr);
				}
			}
			else {
				delete_gt(node->link[1], cmd, storage);
			}
			return;
		case LT:
			if (node->pdata->cmp_name(cmd) < 0) {
				
				delete_and_count(node->link[0], cmd, storage);
				delete_gt(node->link[1], cmd, storage);
				if (satisfy(*node->pdata, cmd, 1)) {
					counter++; checked++;
					this->avl_delete(node->pdata);
					storage->delete_list_node(node->pdata, nullptr);
				}
			}
			else {
				delete_gt(node->link[0], cmd, storage);
			}
			return;
		case LE:
			if (node->pdata->cmp_name(cmd) <= 0) {
				
				delete_and_count(node->link[0], cmd, storage);
				delete_gt(node->link[1], cmd, storage);
				if (satisfy(*node->pdata, cmd, 1)) {
					counter++; checked++;
					this->avl_delete(node->pdata);
					storage->delete_list_node(node->pdata, nullptr);
				}
			}
			else {
				delete_gt(node->link[0], cmd, storage);
			}
			return;
		case GE:
			if (node->pdata->cmp_name(cmd) >= 0) {
				
				delete_and_count(node->link[1], cmd, storage);
				delete_gt(node->link[0], cmd, storage);
				if (satisfy(*node->pdata, cmd, 1)) {
					counter++; checked++;
					this->avl_delete(node->pdata);
					storage->delete_list_node(node->pdata, nullptr);
				}
			}
			else {
				delete_gt(node->link[1], cmd, storage);
			}
			return;
		default:
			return;
		}

	}

	
	
	int delete_with_cmd(const command & cmd, list<record> *list) {
		int i = 0;
		avl_node<T> *node = nullptr;
		counter = 0;
		checked = 0;
		const list_node<command> ln_cmd(cmd);
		if (cmd.c_name == EQ) { 
			do {
				;
			} while (delete_eq(this->root, ln_cmd, list));
			
		}
		printf("hei=%d, minh = %d\n", this->height(), this->getminHeight(this->root));
		this->is_it_avl();
		return checked;
	}
};

template < class T >
class Database {
private:
	//Private data
	db_list<T> storage;
	db_avl<list_node<T>> search_tree;//name

	//Private methods
	Database(const Database & db);
	Database(Database && db);

public:
	//Public methods
	//Constructors
	Database() :storage(){}
	//~Database();
	

	int init_from_file(const char * filename) {
		FILE * fp = fopen(filename, "r");
		if (!fp) { printf("sdf\n"); return 0; }
		T data;
		while (data.scan(fp)) {
			//storage.push(data);
			if (!search_tree.insert((storage.push(data)))) {
				//data.print();
			}
			//printf("tree\n");
			//search_tree.print();
			//printf("------------------\n");
		}
		//printf("counter=%d\n", max);
		fclose(fp);
		search_tree.print();
		
		printf("hei=%d, minh = %d\n", search_tree.height(),search_tree.getminHeight(search_tree.rroot()));
		search_tree.is_it_avl();
		return 1;
		//return storage.init_from_file(filename);
	}
	int execute_cmd(const command &cmd) {
		switch (cmd.type)
		{
		case INSERT:
			if (!storage.find(*(const T*)&cmd)) {
				search_tree.insert(storage.push(*(const T*)&cmd));
			}
			break;
		case SELECT:
			if(cmd.c_name && cmd.c_name!=LIKE)	return search_tree.select_with_cmd(cmd);
			else return storage.select_with_cmd(cmd);
			//return storage.select_with_cmd(cmd);
			break;
		case DELETE:
			/*if (cmd.c_name && cmd.c_name == EQ)	return search_tree.delete_with_cmd(cmd, &storage);
			else */
				return storage.delete_with_cmd(cmd, &search_tree);
			break;
		default:
			break;
		}
		return 0;
	}
	void run_shell() {
		char string[LEN];
		sql_parser parser;
		//printf("sql_query: ");
		while (fgets(string, LEN, stdin)) {
			parser.make_command(string);
			//printD("\n");
			//parser.print();
			if (parser.is_understandable()) {
				//int res = execute_cmd(*(command*)&parser);
				struct timeval start;
				struct timeval end;
				search_tree.counter = 0; search_tree.checked = 0;
				gettimeofday(&start, nullptr);

				int res = execute_cmd(*(command*)&parser);
				gettimeofday(&end, nullptr);
				double delta = ((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec) / 1000000.0;
					//0;
				printf("Elapsed: %.4lf,  res = %d, counter = %d, size = %d\n", delta, res, search_tree.counter, search_tree.avl_size);
				//parser.print_cmd();
				//printf(" %d\n", res);
				if (parser.get_type() == QUIT) break;
			}
			//printf("sql_query: ");
		}
	}

};
template<class T>
__attribute__((always_inline)) inline bool satisfy(const T & sample, const command &cmd, int except_name=0) noexcept{
	//we gain 3 points we win
	if (cmd.c_group + cmd.c_name + cmd.c_phone == 0) return true;
	int points = 0;

	if (cmd.c_group) {
		int group_diff = sample.get_group() - cmd.get_group();
		switch (cmd.c_group)
		{
		case EQ:	points += (group_diff == 0);
			break;
		case NE:	points += (group_diff != 0);
			break;
		case LT:	points += (group_diff < 0);
			break;
		case GT:	points += (group_diff > 0);
			break;
		case LE:	points += (group_diff <= 0);
			break;
		case GE:	points += (group_diff >= 0);
			break;
		default:
			points += 1;
			break;//like included
		}
	}
	else points++;
	
	if (cmd.c_phone) {
		long long phone_diff = sample.get_phone() - cmd.get_phone();
		switch (cmd.c_phone)
		{
		case EQ:	points += (phone_diff == 0);
			break;
		case NE:	points += (phone_diff != 0);
			break;
		case LT:	points += (phone_diff < 0);
			break;
		case GT:	points += (phone_diff > 0);
			break;
		case LE:	points += (phone_diff <= 0);
			break;
		case GE:	points += (phone_diff >= 0);
			break;
		default:
			points += 1;
			break;//like included
		}
	}
	else points++;
	

	if (points == 3 || (points == 2 && cmd.get_op() == OR)) return 1;
	if (cmd.c_name && !except_name) {
		int name_diff = strcmp(sample.get_name(), cmd.get_name());
		switch (cmd.c_name)
		{
		case EQ:	points += (name_diff == 0);
			break;
		case NE:	points += (name_diff != 0);
			break;
		case LT:	points += (name_diff < 0);
			break;
		case GT:	points += (name_diff > 0);
			break;
		case LE:	points += (name_diff <= 0);
			break;
		case GE:	points += (name_diff >= 0);
			break;
		case LIKE:  points += like(sample.get_name(), cmd.get_name());
			break;
		default:
			points += 1;
			break;
		}
	}
	else {
		points++;
	}
	//assert(points <= 3);
	if (points == 3 || (points == 2 && cmd.get_op()==OR)) return 1;
	return 0;
}

#endif