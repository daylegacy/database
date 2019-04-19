#ifndef AVL_TREE_H
#define AVL_TREE_H
#define AVL_MAX_HEIGHT 30
#define MAX_PRINT 3
//#include "database.h"
#include "record.h"
#include "list.h"


int max = 0;

template<class T>
class avl_tree;
template<class T>
class avl_node{
public:
	typedef avl_node<T> nodeT;
	T * pdata;
	nodeT * link[2];
    int balance;
public:
	avl_node() { link[0] = link[1] = 0; balance = 0; pdata = nullptr; }
	avl_node(T * pdata) { link[0] = link[1] = 0; balance = 0; this -> pdata = pdata; }
	~avl_node() { 
		link[0] = link[1] = nullptr; balance = 0; pdata = nullptr; }
    void print(){
		pdata->print();
		//printf("balance = %d, p = %p\n", balance, this);
    }
    friend avl_tree<T>;
};
template<class T>
inline void right_left_r(avl_node<T> * & w, avl_node<T>* & x, avl_node<T>* &y) {
	assert(x->balance == -1);
	w = x->link[0];
	x->link[0] = w->link[1];
	w->link[1] = x;
	y->link[1] = w->link[0];
	w->link[0] = y;
	if (w->balance == +1)
		x->balance = 0, y->balance = -1;
	else if (w->balance == 0)
		x->balance = y->balance = 0;
	else
		x->balance = +1, y->balance = 0;
	w->balance = 0;
}
template<class T>
inline void left_right_r(avl_node<T> * & w, avl_node<T>* & x, avl_node<T>* &y) {
	assert(x->balance == +1);
	w = x->link[1];
	x->link[1] = w->link[0];
	w->link[0] = x;
	y->link[0] = w->link[1];
	w->link[1] = y;
	if (w->balance == -1)
		x->balance = 0, y->balance = +1;
	else if (w->balance == 0)
		x->balance = y->balance = 0;
	else 
		x->balance = -1, y->balance = 0;
	w->balance = 0;
}
template<class T>
inline void left_r(avl_node<T> * & w, avl_node<T>* & x, avl_node<T>* &y) {
	w = x;
	y->link[0] = x->link[1];
	x->link[1] = y;
	x->balance = y->balance = 0;
}
template<class T>
inline void right_r(avl_node<T> * & w, avl_node<T>* & x, avl_node<T>* &y) {
	w = x;
	y->link[1] = x->link[0];
	x->link[0] = y;
	x->balance = y->balance = 0;
}
template<class T>
class avl_tree{
    typedef avl_node<T> nodeT;
    typedef avl_tree<T> treeT;
public:
    nodeT * curr;
	nodeT* pre_root;
	int avl_size=0;
public:
	avl_tree() { 
		curr = nullptr;
		pre_root = new nodeT;
		avl_size = 0; }

    void delete_tree(nodeT * root){
		if (!root) return;
		delete_tree(root->link[0]);
		delete_tree(root->link[1]);
		delete root;
    }
    ~avl_tree(){
        delete_tree(pre_root);
		pre_root = nullptr;
		curr = nullptr;
		avl_size = 0;
    }
    void gotoLeft(){
		curr = curr->link[0];
    }
    void gotoRight(){
		curr = curr->link[1];
    }

	__attribute__((always_inline)) inline nodeT * find(const void * item, int (record::*compare)(const record&)const) {
		int cmp;
		auto p = pre_root;
		const record *rec = (const record*)item;
		for (cmp = -1; cmp != 0; cmp = (rec->*compare)(*p->pdata))
		{
			p = p->link[cmp > 0];
			if (p == nullptr)
				return nullptr;
		}
		return p;
	}

	void print_no_f(nodeT *node){
		if(!node) return;
		node->pdata->print();
		print_no_f(node->link[0]);
		print_no_f(node->link[1]);
	}

	__attribute__((always_inline)) inline const T * avl_delete(const T * item, int (record::*compare)(const record&)const)
	{
		avl_node<T> *pa[AVL_MAX_HEIGHT];	
		unsigned char dirs[AVL_MAX_HEIGHT];    
		int k;                              

		avl_node<T> *p;		 
		int cmp;              

		assert(item != nullptr);

		k = 0;
		p = pre_root;
		for (cmp = -1; cmp != 0; cmp = (item->*compare)(*p->pdata))
		{
			int dir = cmp > 0;

			pa[k] = p;
			dirs[k++] = dir;

			p = p->link[dir];
			if (p == nullptr)
				return nullptr;
		}
	
		item = p->pdata;
		if (p->link[1] == nullptr) {
			pa[k - 1]->link[dirs[k - 1]] = p->link[0];
		}
		else
		{
			avl_node<T> *r = p->link[1];
			if (r->link[0] == nullptr)
			{
				r->link[0] = p->link[0];
				r->balance = p->balance;
				pa[k - 1]->link[dirs[k - 1]] = r;
				dirs[k] = 1;
				pa[k++] = r;
			}
			else
			{
				avl_node<T> *s;
				int j = k++;

				for (;;)
				{
					dirs[k] = 0;
					pa[k++] = r;

					s = r->link[0];
					if (s->link[0] == nullptr)
						break;
					r = s;
				}
				s->link[0] = p->link[0];
				r->link[0] = s->link[1];
				s->link[1] = p->link[1];
				s->balance = p->balance;
				pa[j - 1]->link[dirs[j - 1]] = s;
				dirs[j] = 1;
				pa[j] = s;
			}
		}
		delete p;

		assert(k > 0);
		while (--k > 0)
		{
			avl_node<T> *y = pa[k];
			if (dirs[k] == 0)
			{
				y->balance++;
				if (y->balance == +1)
					break;
				else if (y->balance == +2)
				{
					avl_node<T> *x = y->link[1];
					if (x->balance == -1)
					{
						avl_node<T> *w;
						right_left_r(w, x, y);
						pa[k - 1]->link[dirs[k - 1]] = w;
						
					}
					else
					{ //right_rotation
						y->link[1] = x->link[0];
						x->link[0] = y;
						pa[k - 1]->link[dirs[k - 1]] = x;
						if (x->balance == 0)
						{
							x->balance = -1;
							y->balance = +1;
							break;
						}
						else
							x->balance = y->balance = 0;
					}
				}
			}
			else
			{
				y->balance--;
				if (y->balance == -1)
					break;
				else if (y->balance == -2)
				{
					avl_node<T> *x = y->link[0];
					if (x->balance == +1)
					{
						avl_node<T> *w;
						left_right_r(w, x, y);
					}
					else
					{ //left rotation
						y->link[0] = x->link[1];
						x->link[1] = y;
						pa[k - 1]->link[dirs[k - 1]] = x;
						if (x->balance == 0)
						{
							x->balance = +1;
							y->balance = -1;
							break;
						}
						else
							x->balance = y->balance = 0;
					}
				}
			}
		}

		avl_size--;
		return item;
	}

	

	inline nodeT* r_root() { return pre_root->link[0]; }

	__attribute__((always_inline)) inline nodeT * avl_insert(T * item, int (record::*compare)(const record&)const)
	{
		avl_node<T> *y = nullptr, *z = nullptr; 
		avl_node<T> *p = nullptr, *q = nullptr; 
		avl_node<T> *n = nullptr;     
		avl_node<T> *w = nullptr;     
		int dir;               

		unsigned char dirs[AVL_MAX_HEIGHT]; 
		int k = 0;              
		assert(item != nullptr);
		z = pre_root;
		y = r_root();
		dir = 0;
		for (q = z, p = y; p != nullptr; q = p, p = p->link[dir])
		{
			int cmp = (item->*compare)(*p->pdata);
			if (cmp == 0) {
				return nullptr;
			}

			if (p->balance != 0)
				z = q, y = p, k = 0;
			dirs[k++] = dir = cmp > 0;
		}
		n = q->link[dir] = new nodeT(item);
		if (n == nullptr)
			return nullptr;
		avl_size++;
		if (y == nullptr)
			return n;
		for (p = y, k = 0; p != n; p = p->link[dirs[k]], k++)
			if (dirs[k] == 0)
				p->balance--;
			else
				p->balance++;
		if (y->balance == -2)
		{
			avl_node<T> *x = y->link[0];
			if (x->balance == -1)
			{
				left_r(w, x, y);
			}
			else
			{
				left_right_r(w, x, y);
			}
		}
		else if (y->balance == +2)
		{
			avl_node<T> *x = y->link[1];
			if (x->balance == +1)
			{
				right_r(w, x, y);
			}
			else
			{
				right_left_r(w, x, y);
			}
		}
		else {
			return n;
		}
		z->link[y != z->link[0]] = w;
		return n;
	}

	bool is_it_avl(int (record::*compare)(const record&)const) {
		printf("total number=%d \n", go_around(this->r_root()));
		printf("total number satisf=%d\n", go_around_check(this->r_root(), compare));
		retrace(this->r_root());
		printf("total number satisf(retraced)=%d\n", go_around_check(this->r_root(), compare));
		return 1;
	}

	int go_around(nodeT * root) {
		if (!root) return 0;
		return go_around(root->link[0]) + go_around(root->link[1]) + 1;
	}

	int go_around_check(nodeT * root, int (record::*compare)(const record&)const) {
		if (!root) return 0;
		if (abs(root->balance) <= 1) {
			if ((root->link[0] && (root->pdata->*compare)(*root->link[0]->pdata) < 0) ||
				(root->link[1] && (root->pdata->*compare)(*root->link[1]->pdata) > 0)) {
				print(root);
				return go_around_check(root->link[0], compare) + go_around_check(root->link[1], compare);
			}
			return go_around_check(root->link[0], compare) + go_around_check(root->link[1], compare) + 1;
		}
		print(root);
		return go_around_check(root->link[0], compare) + go_around_check(root->link[1], compare);
	}

    int abs(int a){
		return a > 0 ? a : -a;
    }
    int getHeight(nodeT * root)
    {
		if (!root)return 0;
		else
		{
			int lHeight = getHeight(root->link[0]);
			int rHeight = getHeight(root->link[1]);

			return (lHeight > rHeight)? (lHeight+1): (rHeight+1);
		}
    }
	int getminHeight(nodeT * root)
	{
		if (!root)
			return 0;
		else
		{
			int lHeight = getminHeight(root->link[0]);
			int rHeight = getminHeight(root->link[1]);

			return (lHeight < rHeight) ? (lHeight + 1) : (rHeight + 1);
		}
	}
	int height() {
		return getHeight(r_root());
	}
    void retrace(nodeT * root){
        //int max=0;
        retrace_help(root, max);
		
    }
    int retrace_help(nodeT * root, int &max)
    {
		if (!root) return 0;
		int lHeight = retrace_help(root->link[0], max);
		int rHeight = retrace_help(root->link[1],max);
		root->balance=rHeight-lHeight;
		max++;
		return (lHeight > rHeight)? (lHeight+1): (rHeight+1);
    }


    void print(){

        curr =r_root();
		if (!curr) return;
        print_();
        curr = r_root();
    }
	void print(nodeT * node) {
		curr = node;
		print_();
		curr = r_root();
	}
    void print_(int isLeft=-1, int level=0){
		int i = 0; nodeT * tmp=0;
		if (level>MAX_PRINT) return;
		for (i=0;i<level;i++){
			printf("| ");
		}
		if(isLeft==1){
			printf("L:" );
		}
		if(isLeft==0){
			printf("R:" );
		}
		curr->print();
		tmp = curr;
		if(curr->link[0]) {gotoLeft(); print_(1, level+1);}
		curr = tmp;
		if(curr->link[1]){gotoRight();print_(0,level +1);}
    }

};



#endif // !AVL_TREE_H
