#ifndef COMMAND_H
#define COMMAND_H

#include "helper.h"
#include "record.h"


enum cmd_type {//command type
	CMD_NONE, QUIT, STOP, SELECT, INSERT, DELETE
};

enum cond_type {//conditions
	COND_NONE, EQ, NE, LT, GT, LE, GE, LIKE
};

enum op_type {//operation type
	OP_NONE, AND, OR
};



class command : public record {
public:
	cmd_type type = CMD_NONE;
	cond_type c_name = COND_NONE;
	cond_type c_phone = COND_NONE;
	cond_type c_group = COND_NONE;
	op_type operation = OP_NONE;
	bool understandable = 0;


	command() : record() { };
	~command() {}
	void print_cmd() {
		switch (type)
		{
		case CMD_NONE:
			break;
		case QUIT:
			printE("quit;\n");
			break;
		case STOP:
			printE("stop;\n");
			break;
		case SELECT:
			printE("select *");
			if (c_name + c_phone + c_group) {
				printE(" where ");
				int check = 0;
				if (c_name) {
					printE("name ");
					print_cond(c_name);
					printE("%s", get_name());
					if (!check && operation) {
						printE(" "); print_op(operation); printE(" "); check = 1;
					}
				}

				if (c_phone) {
					printE("phone ");
					print_cond(c_phone);
					printE("%lld", get_phone());
					if (!check && operation) {
						printE(" "); print_op(operation); printE(" "); check = 1;
					}
				}
				if (c_group) {
					printE("group ");
					print_cond(c_group);
					printE("%d", get_group());
					if (!check && operation) {
						printE(" "); print_op(operation); printE(" "); check = 1;
					}
				}

			}
			//printf(";\n");
			printE(";");
			break;
		case INSERT:
			//printf("insert (%s, %lld, %d);\n", get_name(), get_phone(), get_group());
			printE("insert (%s, %lld, %d);", get_name(), get_phone(), get_group());
			break;
		case DELETE:
			printE("delete");
			if (c_name + c_phone + c_group) {
				printE(" where ");
				int check = 0;
				if (c_name) {
					printE("name ");
					print_cond(c_name);
					printE("%s", get_name());
					if (!check && operation) {
						printE(" "); print_op(operation); printE(" "); check = 1;
					}
				}
				if (c_phone) {
					printE("phone ");
					print_cond(c_phone);
					printE("%lld", get_phone());
					if (!check && operation) {
						printE(" "); print_op(operation); printE(" "); check = 1;
					}
				}
				if (c_group) {
					printE("group ");
					print_cond(c_group);
					printE("%d", get_group());
					if (!check && operation) {
						printE(" "); print_op(operation); printE(" "); check = 1;
					}
				}
			}
			//printf(";\n");
			printE(";");
			break;
		default:
		printE("\n");
			break;
		}
	}

	void reverse() {
		switch (c_name)
		{
		case EQ: c_name = NE;
			break;
		case NE: c_name = EQ;
			break;
		case LT: c_name = GE;
			break;
		case GT: c_name = LE;
			break;
		case LE: c_name = GT;
			break;
		case GE: c_name = LT;
			break;
		case LIKE:
			break;
		default:
			break;
		}
		operation = AND;
	}
	cmd_type get_type() const { return type; }
	op_type get_op() const { return operation; }

	void print_cond(cond_type cond) {
		switch (cond)
		{
		case COND_NONE:

			break;
		case EQ:	printE("= ");
			break;
		case NE:	printE("<> ");
			break;
		case LT:	printE("< ");
			break;
		case GT:	printE("> ");
			break;
		case LE:	printE("<= ");
			break;
		case GE:	printE(">= ");
			break;
		case LIKE:	printE("like ");
			break;
		default:
			break;
		}
	}

	void print_op(op_type op) {
		switch (op)
		{
		case OP_NONE:
			break;
		case AND:	printE("and");
			break;
		case OR:	printE("or");
			break;
		default:
			break;
		}
	}

	void nullify_all() {
		//set_name(""); //care
		set_phone(0);
		set_group(0);
		type = CMD_NONE;
		c_name = COND_NONE;
		c_phone = COND_NONE;
		c_group = COND_NONE;
		operation = OP_NONE;
		understandable = 0;
	}

	bool is_understandable() { return understandable; }

	void print() {
		record::print();
		printD("%d %d %d %d\n", type, c_phone, c_group, operation);
		printD("understanded: %d\n", understandable);
	}
};
#endif // !COMMAND_H