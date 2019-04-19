#ifndef LIKE_H
#define LIKE_H
#include <cstring>
#include "substr.h"
const char *  task_6m(const char * substr, int substr_size, const char * string, int string_size)
{//is beginning of string fully match substr?
	auto mask = substr;
	int mask_size = substr_size;
	bool spcl_smb_ON = 1;
	char word[LEN];
	strcpy(word, string);
	*(word + string_size) = '\0';

	//printf("%s", spcl_smbs);
	int k = 0;//iterate chars in word
	int i = 0;//iterate chars in mask

	while (i < mask_size) {
		if (spcl_smb_ON) {
			if (mask[i] == '\\') {
				//printf("1\n");
				spcl_smb_ON = 0; i++;
			}
			else if (mask[i] == '_') {
				if (word[k] == '\0') return nullptr;
				else { k++; i++; }
			}
			else {
				if (mask[i] != word[k]) {
					return nullptr;
				}
				else {
					i++; k++;
				}
			}
		}
		else {
			if (mask[i] != word[k]) {
				return nullptr;
			}
			else {
				i++; k++; spcl_smb_ON = 1;
			}
		}
	}

	return &string[k];//points to beginning of next
}

const char * find_first_next(const char * lexema, int lexema_size, const char * word, int word_size, const char ** start)
{
	int i = 0;//iterator in word
	//int word_size = strlen(word);
	const char* res = nullptr;

	while (i < word_size)
	{
		res = task_6m(lexema, lexema_size, word + i, word_size);
		if (res) { *start = word + i;  return res; }
		else {
			i++;
		}
	}
	return nullptr;
}

const char * find_last_next(const char * lexema, int lexema_size, const char * word, int word_size, const char ** start)
{
	int i = 0;//iterator in word
	//int word_size = strlen(word);
	const char* res = nullptr;
	const char* tmp = nullptr;

	while (i <= word_size - lexema_size)
	{
		tmp = task_6m(lexema, lexema_size, word + i, word_size);
		if (tmp) {
			res = tmp;
			*start = word + i;
		}
		i++;

	}
	return res;
}

bool isProtected(const char * start, const char * smb)
{

	const char * ptr = smb - 1;
	int i = 0;//number of '\'
	while (ptr >= start && *ptr == '\\') {
		i++;
		ptr--;
	}
	return (i % 2);
}

bool isUnprotected(const char * start, const char * smb)
{
	return !(isProtected(start, smb));
}

bool like(const char * word, const char * lexema)
{
	/*char tmp[LEN] = "";
	strcpy(tmp, "HH");
	const char * mask_lexema = strcat(tmp, lexema) + 2;*/
	char mask_lexema[LEN];
	strcpy(mask_lexema, lexema);

	std::size_t mask_size = strlen(mask_lexema);
	std::size_t word_size = strlen(word);
	assert(word_size > 0);
	char right_ended_mask[LEN];//for searching from right
	strcpy(right_ended_mask, mask_lexema);
	substr mask(mask_lexema, mask_size);
	substr left_mask(mask_lexema, 0);
	substr right_mask(mask_lexema + mask_size - 1, 0);

	substr curr_word(word, word_size);
	substr left_word(word, 0);
	substr right_word(word + word_size, 0);

	bool check_l = 0, check_r = 0;
	int left_or_right = 1;//1 = start, -1 = end
	//printf("current word:\"%s\"\n", word);

	while (left_mask.end <= right_mask.start) {

		if (left_or_right > 0) {//check from beginning
			if (*left_mask.start == '%') {//it can only happen at the beginning
				left_mask.move_ends(1);//move by one in mask
				check_l = 1;
			}
			else {
				auto found = strchr(left_mask.start, '%');//find unprotected %

				while (found && isProtected(left_mask.start, found)) {
					found = strchr(found + 1, '%');
				}

				left_mask.set_end(found ? found : mask.end);//if found set it to the end else left mask end is unchanged

				const char * found_part_start = nullptr;
				//find part of word that match left mask
				auto found_part_end = find_first_next(left_mask.start, left_mask.len, left_word.end, right_word.start - left_word.end, &found_part_start);

				//printf("found_part_start:\"%s\"\n", found_part_start);
				if (!found_part_end) return false;//no parts matches with left mask

				if (!check_l) {//did we met %?
					if (found_part_start != left_word.start) return false;
				}

				if (found_part_start < left_word.end || found_part_end > right_word.start) {
					return false;
				}

				left_word.set_start(found_part_start);
				left_word.set_end(found_part_end);
				left_mask = { left_mask.end + 1, left_mask.end + 1 , 0 };//update mask
				//left_word.print();
				check_l = 1;
				//left_mask.set_start(left_mask.start + 1);

				//printf("mask_len_of_sbstr=%d, next ='%c'\n", mask_len_of_sbstr, *found_part_end);
			}
		}
		else {//check from ending
			if (*(right_mask.start) == '%' && isUnprotected(left_mask.end, right_mask.start)) {
				right_mask.move_ends(-1);
				check_r = 1;
			}
			else {
				*(right_ended_mask + (right_mask.end - mask.start)) = '\0';
				auto found = strrchr(right_ended_mask, '%');//find unprotected %

				while (found && isProtected(mask.start, found)) {
					*found = '\0';
					found = strrchr(right_ended_mask, '%');
				}

				right_mask.set_start(found ? mask.start + (found - right_ended_mask) + 1 : mask.start);//if found set it to the end else left mask end is unchanged

				const char * found_part_start = nullptr;
				//find part of word that match left mask
				auto found_part_end = find_last_next(right_mask.start, right_mask.len + 1, left_word.end, right_word.start - left_word.end, &found_part_start);
				//if (found_part_start < left_mask.end) return false;

				//printf("found_part_start:\"%s\"\n", found_part_start);
				if (!found_part_end) return false;//no parts matches with right mask

				if (!check_r) {//did we met %?
					if (found_part_end != right_word.end) return false;
				}
				if (found_part_start < left_word.end || found_part_end > right_word.start) {
					return false;
				}

				right_word.set_start(found_part_start);
				right_word.set_end(found_part_end);
				right_mask = { right_mask.start - 2 , right_mask.start - 2 , 0 };
				check_r = 1;
				//left_word.print();
				//left_mask.set_start(left_mask.start + 1);

				//printf("mask_len_of_sbstr=%d, next ='%c'\n", mask_len_of_sbstr, *found_part_end);
			}

		}
		left_or_right *= -1;
	}

	if (right_word.start == curr_word.end && isUnprotected(mask.start, mask.end - 1) && *(mask.end - 1) == '%') {
		return true;
	}
	if (left_word.end < curr_word.end && (right_word.start == curr_word.end)) {
		return false;
	}
	return true;
}

#endif