#ifndef PARSER_H
#define PARSER_H
#define	TOKEN_SIZE (30)
#define N_OF_TOKENS (40)
#define LEN (1200)
#include <cstring>
#include <cstdlib>
#include <cassert>
#include "command.h"


class sql_parser:public command {
	char **tokens;
	char cur_token[TOKEN_SIZE + 1] = { 0 };
	int token_n = 0;
	int tokens_size = 0;
	int smb_n = 0;
	//command command; //to be generated
	char reserved_words[TOKEN_SIZE][N_OF_TOKENS] = { "select", "insert", "where", "quit", "delete", "like", "from", "and", "or",";",  "EOF" };
	char reserved_words_l[TOKEN_SIZE][N_OF_TOKENS] = { ";", ")","(", "EOF" };
	//friend class command;
public:
	sql_parser()
		:command()
	{
		tokens_size = N_OF_TOKENS;
		tokens = new char*[tokens_size];
		for (int i = 0; i < tokens_size; i++) {
			tokens[i] = new char[TOKEN_SIZE + 1];
		}
	}
	~sql_parser() {
		for (int i = 0; i < tokens_size; i++) {
			delete[] tokens[i];
		}
		delete[] tokens;
	}
	void split_to_tokens(char * string) {
		this->token_n = 0;
		this->smb_n = 0;
		int k = 0;
		while (string[k]) {
			char smb = string[k];
			char next_smb = string[k + 1];
			switch (smb)
			{
			case '\n':
			case ' ':	add_token(); break;
			case ';':	add_token(); add_smb(';'); add_token(); break;
			case ',':	add_token(); add_smb(','); add_token(); break;
			case '(':	add_token(); add_smb('('); add_token(); break;
			case ')':	add_token(); add_smb(')'); add_token(); break;
			case '*':	add_token(); add_smb('*'); add_token(); break;
			case '=':	add_token(); add_smb('='); add_token(); break;
			case '>':	add_token(); add_smb('>');
				if (next_smb == '=') { add_smb('='); k++; }
				add_token(); break;
			case '<':	add_token(); add_smb('<');
				if (next_smb == '=') { add_smb('='); k++; }
				else if (next_smb == '>') { add_smb('>'); k++; }
				add_token(); break;
			default:	add_smb(smb);
				break;
			}
			k++;
		}
		add_token();
		add_smb('E'); add_smb('O'); add_smb('F');
		add_token();
	}

	void add_token() {
		assert(token_n < N_OF_TOKENS);
		if (cur_token && cur_token[0] != 0) {
			strcpy(tokens[token_n], cur_token);
			token_n++;
			nullify_token();
		}
	}
	void add_smb(char c) {
		assert(smb_n < TOKEN_SIZE);
		cur_token[smb_n] = c;
		smb_n++;
	}
	void nullify_token() {
		memset((void *)cur_token, 0, (TOKEN_SIZE + 1) * sizeof(char));
		smb_n = 0;
	}

	void make_command(char * string) {
		nullify_all(); //clear prev
		split_to_tokens(string);
		understandable = parse(tokens);
	}

	int parse(char ** list_of_tokens) {
		int i = 0;
		//nullify_all();
		if (strcmp(list_of_tokens[i], "EOF") != 0) {
			if (strcmp(list_of_tokens[i], "select") == 0) {
				type = SELECT;
				i++;
				int n_args = get_arguments(list_of_tokens + i);
				if (!n_args) return 0;
				i += n_args;
				if (strcmp(list_of_tokens[i], "where") == 0) {
					i++;
					int conds = get_conditions(list_of_tokens + i);
					if (!conds) return 0;
					i += conds;
				}
				if (strcmp(list_of_tokens[i], ";") == 0) {
					i++;
					return 1;
				}
				return 0;
			}
			else if (strcmp(list_of_tokens[i], "quit") == 0) {
				type = QUIT; i++;
				if (strcmp(list_of_tokens[i], ";") == 0) {
					i++;
					return 1;
				}
				return 0;
			}
			else if (strcmp(list_of_tokens[i], "stop") == 0) {
				type = STOP; i++;
				if (strcmp(list_of_tokens[i], ";") == 0) {
					i++;
					return 1;
				}
				return 0;
			}
			else if (strcmp(list_of_tokens[i], "delete") == 0) {
				type = DELETE; i++;
				if (strcmp(list_of_tokens[i], "where") == 0) {
					i++;
					int conds = get_conditions(list_of_tokens + i);
					if (!conds) return 0;
					i += conds;
				}
				if (strcmp(list_of_tokens[i], ";") == 0) {
					i++;
					return 1;
				}
				return 0;
			}
			else if (strcmp(list_of_tokens[i], "insert") == 0) {
				type = INSERT; i++;
				if (strcmp(list_of_tokens[i], "(") == 0) {
					i++;
					int n_args = get_values(list_of_tokens + i);
					if (!n_args) return 0;
					i += n_args;
					if (strcmp(list_of_tokens[i], ")") == 0) {
						i++;
						if (strcmp(list_of_tokens[i], ";") == 0) {
							i++;
							return 1;
						}
					}
				}
				return 0;
			}
			return 0;

		}
		return 0;
	}

	int get_arguments(char ** list_of_args) {
		int i = 0;
		while (strcmp(list_of_args[i], "EOF") != 0) {
			if (is_reserved_word(list_of_args[i])) {
				return i;
			}
			if (strcmp(list_of_args[i], "name") == 0) {
				i++;
			}
			else if (strcmp(list_of_args[i], "phone") == 0) {
				i++;
			}
			else if (strcmp(list_of_args[i], "group") == 0) {
				i++;
			}
			else if (strcmp(list_of_args[i], "*") == 0) {
				i++;
			}
			else if (i&&strcmp(list_of_args[i], ",") == 0) {
				i++;
			}
			else {
				return 0;
			}
		}
		return 0;
	}
	int get_conditions(char ** list_of_args) {
		int i = 0;
		while (strcmp(list_of_args[i], "EOF") != 0) {
			if (is_reserved_word(list_of_args[i])) {
				return i;
			}

			int shift = get_condition(list_of_args + i);
			if (!shift) return i;
			i += shift;
			if (strcmp(list_of_args[i], "and") == 0) {
				operation = AND;
				i++;
			}
			else if (strcmp(list_of_args[i], "or") == 0) {
				operation = OR;
				i++;
			}
			else {
				return i;
			}
		}
		return i;
	}

	int get_values(char ** list_of_values) {
		int i = 0;
		while (strcmp(list_of_values[i], "EOF") != 0) {
			if (is_reserved_word_l(list_of_values[i])) {
				return i;
			}
			if (i == 0) { set_name(list_of_values[i]); }
			else if (i == 2) { set_phone((long long)atof(list_of_values[i])); }
			else if (i == 4) { set_group((int)atoi(list_of_values[i])); }
			else if (strcmp(list_of_values[i], ",") != 0) {
				return 0;
			}
			i++;

		}
		return i;
	}

	int get_condition(char ** condition) {
		int i = 0;
		if (strcmp(condition[i], "EOF") != 0 && strcmp(condition[i + 1], "EOF") != 0 && strcmp(condition[i + 2], "EOF") != 0) {
			if (is_reserved_word(condition[i])) {
				return i;
			}
			else if (strcmp(condition[i], "name") == 0) {
				i++;
				c_name = get_cond_type(condition[i]);
				i++;
				set_name(condition[i]);
				i++;
				return i;
			}
			else if (strcmp(condition[i], "phone") == 0) {
				i++;
				c_phone = get_cond_type(condition[i]);
				i++;
				set_phone((long long)atof(condition[i]));
				i++;
				return i;
			}
			else if (strcmp(condition[i], "group") == 0) {
				i++;
				c_group = get_cond_type(condition[i]);
				i++;
				set_group(atoi(condition[i]));
				i++;
				return i;
			}
			return 0;
		}
		return 0;
	}

	cond_type get_cond_type(char * cond) {
		if (strcmp(cond, "<>") == 0) {
			return NE;
		}
		else if (strcmp(cond, "<=") == 0) {
			return LE;
		}
		else if (strcmp(cond, ">=") == 0) {
			return GE;
		}
		else if (strcmp(cond, ">") == 0) {
			return GT;
		}
		else if (strcmp(cond, "<") == 0) {
			return LT;
		}
		else if (strcmp(cond, "=") == 0) {
			return EQ;
		}
		else if (strcmp(cond, "like") == 0) {
			return LIKE;
		}
		return COND_NONE;
	}

	bool is_reserved_word(char * token) {
		int i = 0;
		while (strcmp(reserved_words[i], "EOF") != 0) {
			if (strcmp(reserved_words[i], token) == 0) {
				return true;
			}
			i++;
		}
		return false;
	}
	bool is_reserved_word_l(char * token) {
		int i = 0;
		while (strcmp(reserved_words_l[i], "EOF") != 0) {
			if (strcmp(reserved_words_l[i], token) == 0) {
				return true;
			}
			i++;
		}
		return false;
	}
};
#endif // !PARSER_H

