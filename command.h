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
			printf("quit;\n");
			break;
		case STOP:
			printf("stop;\n");
			break;
		case SELECT:
			printf("select *");
			if (c_name + c_phone + c_group) {
				printf(" where ");
				int check = 0;
				if (c_name) {
					printf("name ");
					print_cond(c_name);
					printf("%s", get_name());
					if (!check && operation) {
						printf(" "); print_op(operation); printf(" "); check = 1;
					}
				}

				if (c_phone) {
					printf("phone ");
					print_cond(c_phone);
					printf("%lld", get_phone());
					if (!check && operation) {
						printf(" "); print_op(operation); printf(" "); check = 1;
					}
				}
				if (c_group) {
					printf("group ");
					print_cond(c_group);
					printf("%d", get_group());
					if (!check && operation) {
						printf(" "); print_op(operation); printf(" "); check = 1;
					}
				}

			}
			//printf(";\n");
			printf(";");
			break;
		case INSERT:
			//printf("insert (%s, %lld, %d);\n", get_name(), get_phone(), get_group());
			printf("insert (%s, %lld, %d);", get_name(), get_phone(), get_group());
			break;
		case DELETE:
			printf("delete");
			if (c_name + c_phone + c_group) {
				printf(" where ");
				int check = 0;
				if (c_name) {
					printf("name ");
					print_cond(c_name);
					printf("%s", get_name());
					if (!check && operation) {
						printf(" "); print_op(operation); printf(" "); check = 1;
					}
				}
				if (c_phone) {
					printf("phone ");
					print_cond(c_phone);
					printf("%lld", get_phone());
					if (!check && operation) {
						printf(" "); print_op(operation); printf(" "); check = 1;
					}
				}
				if (c_group) {
					printf("group ");
					print_cond(c_group);
					printf("%d", get_group());
					if (!check && operation) {
						printf(" "); print_op(operation); printf(" "); check = 1;
					}
				}
			}
			//printf(";\n");
			printf(";");
			break;
		default:
			break;
		}
	}
	cmd_type get_type() const { return type; }
	op_type get_op() const { return operation; }

	void print_cond(cond_type cond) {
		switch (cond)
		{
		case COND_NONE:

			break;
		case EQ:	printf("= ");
			break;
		case NE:	printf("<> ");
			break;
		case LT:	printf("< ");
			break;
		case GT:	printf("> ");
			break;
		case LE:	printf("<= ");
			break;
		case GE:	printf(">= ");
			break;
		case LIKE:	printf("like ");
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
		case AND:	printf("and");
			break;
		case OR:	printf("or");
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