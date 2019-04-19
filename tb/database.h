#ifndef DATABASE_H
#define DATEBASE_H
#include "parser.h"
#include "avl_tree.h"
#include "list.h"
#include "like.h"
#include "record.h"

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
				node->print();
				i++;
			}
			node = node->next;
		}
		return i;
	}

	int delete_with_cmd(const command & cmd, db_avl<list_node<T>> * search_tree,
			db_avl<list_node<T>> * secondary_tree) {
		

		if (cmd.c_name==EQ && cmd.operation!=OR){
			list_node<T>* node_to_delete;
			while ((node_to_delete = search_tree->find_cmd(&cmd, &record::cmp_name))) {
				search_tree->avl_delete(node_to_delete, &record::cmp);
				secondary_tree->avl_delete(node_to_delete, &record::cmp_sec);
				this->delete_list_node(node_to_delete);
			}
		}
		else if (cmd.c_phone == EQ && cmd.operation != OR) {
			list_node<T>* node_to_delete;
			while ((node_to_delete = secondary_tree->find_cmd(&cmd, &record::cmp_phone))) {
				search_tree->avl_delete(node_to_delete, &record::cmp);
				secondary_tree->avl_delete(node_to_delete, &record::cmp_sec);
				this->delete_list_node(node_to_delete);
			}
		}
		else {
			list_node<T> *node = this->gethead();
			list_node<T> *tmp;
			while (node) {
				tmp = node->next;
				if (satisfy(*node, cmd)) {
					search_tree->avl_delete(node, &record::cmp);
					secondary_tree->avl_delete(node, &record::cmp_sec);
					this->delete_list_node(node);
				}
				node = tmp;
			}
		}
		
		return 0;
	}
};

template<class T>
class db_avl : public avl_tree<T> {
public:
	int counter;
	int checked;

	int select_with_cmd(const command & cmd, int (record::*compare)(const record&)const) {
		counter = 0;
		checked = 0;
		const list_node<command> ln_cmd(cmd);
		if ((cmd.c_name == EQ && compare == &record::cmp_name) || 
			(cmd.c_phone == EQ && compare == &record::cmp_phone))
			search_eq(this->r_root(), ln_cmd, compare);
		else if ((cmd.c_name == NE && compare == &record::cmp_name) ||
				(cmd.c_phone == NE && compare == &record::cmp_phone)) {
			sel_NE2(cmd, this->r_root(), compare);
		}
		else search_gt(this->r_root(), ln_cmd, compare);
		return counter;
	}


	void search_eq(avl_node<T> * node, const list_node<command> & cmd, int (record::*compare)(const record&)const) {
		if (!node) return;
		int cmp = (cmd.*compare)(*(record*)node->pdata);
		if		(cmp > 0) { search_eq(node->link[1], cmd, compare); }
		else if (cmp < 0) { search_eq(node->link[0], cmd, compare); }
		else {
			search_eq(node->link[0], cmd, compare);
			search_eq(node->link[1], cmd, compare);
			if (satisfy(*node->pdata, cmd)) {
				node->pdata->print();
			}
		}
	}


	void search_gt(avl_node<T> * node, const list_node<command> & cmd, int (record::*compare)(const record&)const) {
		if (!node) return;
		switch (compare==&record::cmp_name?cmd.c_name:cmd.c_phone)
		{
		case GT:
			if ((node->pdata->*compare)(cmd) > 0) {
				if (satisfy(*node->pdata, cmd)) {
					node->pdata->print();
				}
				print_and_count(node->link[1], cmd);
				search_gt(node->link[0], cmd, compare);
			}
			else {
				search_gt(node->link[1], cmd, compare);
			}
			return;
		case LT:
			if ((node->pdata->*compare)(cmd) < 0) {
				if (satisfy(*node->pdata, cmd)) {
					node->pdata->print();
				}
				print_and_count(node->link[0], cmd);
				search_gt(node->link[1], cmd, compare);
			}
			else {
				search_gt(node->link[0], cmd, compare);
			}
			return;
		case LE:
			if ((node->pdata->*compare)(cmd) <= 0) {
				if (satisfy(*node->pdata, cmd)) {
					node->pdata->print();
				}
				print_and_count(node->link[0], cmd);
				search_gt(node->link[1], cmd, compare);
			}
			else {
				search_gt(node->link[0], cmd, compare);
			}
			return;
		case GE:
			if ((node->pdata->*compare)(cmd) >= 0) {
				if (satisfy(*node->pdata, cmd)) {
					node->pdata->print();
				}
				print_and_count(node->link[1], cmd);
				search_gt(node->link[0], cmd, compare);
			}
			else {
				search_gt(node->link[1], cmd, compare);
			}
			return;
		default:
			return;
		}
		
	}

	void  print_and_count(avl_node<T> * node, const list_node<command> & cmd) {
		if (!node) return;
		if (satisfy(*node->pdata, cmd)) {
			node->pdata->print();
		}
		print_and_count(node->link[0], cmd);
		print_and_count(node->link[1], cmd);
	}
	void  just_print(avl_node<T> * node) {
		if (!node) return;
		node->pdata->print();
		just_print(node->link[0]);
		just_print(node->link[1]);
	}

	void sel_NE2(const command & cmd, avl_node<T> * node, int (record::*compare)(const record&)const, int checked = 0) {
		if (!node) return;
		int cmp = (cmd.*compare)(*node->pdata);
		if (cmp != 0) {
			if (checked) { print_and_count(node, cmd); return; }
			if (satisfy(*node->pdata, cmd)) {
				node->pdata->print();
			}
			print_and_count(node->link[cmp < 0], cmd);
			sel_NE2(cmd, node->link[cmp > 0], compare);
			return;
		}
		if (cmp == 0) {
			sel_NE2(cmd, node->link[0], compare, 1);
			sel_NE2(cmd, node->link[1], compare, 1);
		}
	}

	int sel_OR(command & cmd, db_avl<T> * secondary_tree) {
		select_with_cmd(cmd, &record::cmp_name);
		((command *)&cmd)->reverse();
		secondary_tree->select_with_cmd(cmd, &record::cmp_phone);
		return 1;
	}

	T* find_cmd(const command * cmd, int (record::*compare)(const record&)const) {
		int cmp;
		avl_node<T>* p = this->pre_root;
		for (cmp = -1;; cmp = (cmd->*compare)(*p->pdata))
		{
			if (cmp == 0 && satisfy(*p->pdata, *cmd)) {
				return p->pdata;
			}
			p = p->link[cmp > 0];
			if (p == nullptr)
				return nullptr;

		}
		return nullptr;
	}
};

template < class T >
class Database {
private:
	//Private data
	db_list<T> storage;
	db_avl<list_node<T>> search_tree;//name
	db_avl<list_node<T>> secondary_tree;//phone

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
			list_node<T> *node = storage.push(data);
			search_tree.avl_insert(node, &record::cmp);
			secondary_tree.avl_insert(node, &record::cmp_sec);
			//printf("tree\n");
			//search_tree.print();
			//printf("------------------\n");
		}
		//printf("counter=%d\n", max);
		fclose(fp);
		//search_tree.print();
		//secondary_tree.print();
		
		/*printf("hei=%d, minh = %d\n", search_tree.height(),search_tree.getminHeight(search_tree.r_root()));
		search_tree.is_it_avl(&record::cmp);
		printf("hei=%d, minh = %d\n", secondary_tree.height(), secondary_tree.getminHeight(secondary_tree.r_root()));
		secondary_tree.is_it_avl(&record::cmp_sec);*/
		return 1;
		//return storage.init_from_file(filename);
	}
	int execute_cmd(command &cmd) {
		switch (cmd.type)
		{
		case INSERT:
			if (!secondary_tree.find(&cmd, &record::cmp_sec)) {
				list_node<T> *node = storage.push(*(const T*)&cmd);
				search_tree.avl_insert(node, &record::cmp);
				secondary_tree.avl_insert(node, &record::cmp_sec);
			}
			break;
		case SELECT:
			if (cmd.c_phone && cmd.operation != OR && 
				cmd.c_phone != NE) return secondary_tree.select_with_cmd(cmd, &record::cmp_phone);
			// if (cmd.c_phone == NE && cmd.operation == 0) 
			// 	return secondary_tree.sel_NE(&cmd, &record::cmp_phone);	

			if (cmd.c_name && cmd.c_name!=LIKE && cmd.operation != OR && 
				cmd.c_name != NE)	return search_tree.select_with_cmd(cmd, &record::cmp_name);
			if (cmd.c_name && cmd.c_name != LIKE && cmd.operation == OR && cmd.c_phone) {
				return search_tree.sel_OR(cmd, &secondary_tree);
			}
			// if (cmd.c_name == NE && cmd.operation == 0) 
			// 	return search_tree.sel_NE(&cmd, &record::cmp_name);

			return storage.select_with_cmd(cmd);
			break;
		case DELETE:
			return storage.delete_with_cmd(cmd, &search_tree, &secondary_tree);
			break;
		default:
			break;
		}
		return 0;
	}
	void run_shell() {
		char string[LEN];
		sql_parser parser;
		while (fgets(string, LEN, stdin)) {
			parser.make_command(string);
			if (parser.is_understandable()) {
				//int res = execute_cmd(*(command*)&parser);
				struct timeval start;
				struct timeval end;
				search_tree.counter = 0; search_tree.checked = 0;
				gettimeofday(&start, nullptr);
				execute_cmd(*(command*)&parser);
				gettimeofday(&end, nullptr);
				double delta = ((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec) / 1000000.0;
					//0;
				//parser.print_cmd();
				//fprintf(stderr, "Elapsed: %.6lf, counter = %d, size = %d\n", delta, search_tree.counter, search_tree.avl_size);
				
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