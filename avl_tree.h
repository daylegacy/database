#ifndef AVL_TREE_H
#define AVL_TREE_H
#define AVL_MAX_HEIGHT 35
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
		//printD("avl node destructor\n");
		link[0] = link[1] = nullptr; balance = 0; pdata = nullptr; }
    nodeT & move(nodeT & other){
        if(this==&other){
            return *this;
        }
		pdata = other.pdata;
		other.pdata = 0;
        link[0]= other.link[0];
        other.link[0]=0;
        link[1]= other.link[1];
        other.link[1]=0;
        balance = other.balance;
        other.balance=0;
        return *this;
    }

	void swap_data(nodeT & other) {
		auto tmp = pdata;
		pdata = other.pdata;
		other.pdata = tmp;
	}
    void print(){
		pdata->print();
		printf("balance = %d, p = %p\n", balance, this);
    }
	inline bool is_leaf() {
		return link[0] || link[1] ? 0 : 1;
	}
    friend avl_tree<T>;
};

template<class T>
class avl_tree{
    typedef avl_node<T> nodeT;
    typedef avl_tree<T> treeT;
public:
    nodeT * root;
    nodeT * curr;
	nodeT* pre_root;
	int avl_size=0;
public:
	avl_tree() { 
		root = curr = nullptr;
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
        root = curr=nullptr;
		avl_size = 0;
    }
    void gotoLeft(){
        curr=curr->link[0];
    }
    void gotoRight(){
        curr=curr->link[1];
    }
    int insert_here(nodeT * a, nodeT * here){
        if(!root){
            curr = root = a;
			//pre_root->link[0] = root;
            return 0;
        }
        else{
            if(*a->pdata<*here->pdata){
                if(!here->link[0]){
                    here->link[0] = a;
                    here->balance--;
                    if(here->balance<0){
                        return 1;
                    }
                    return 0;
                }
                else{
                    if(insert_here(a, here->link[0])){
                        here->balance--;
                        if(here->balance == -2){
                            if(here->link[0]->balance==-1){
                                one_line_left_rotation(here);
                            }
                            else if(here->link[0]->balance==1){
                                left_right_rotation(here);
                            }//a esli 0? etogo ne bivaet
                            return 0;
                        }
                        if(here->balance<0){
                            return 1;
                        }
                    }
                    return 0;
                }
            }
            else{
                if(!here->link[1]){
                    here->link[1] = a;
                    here->balance++;

                    if(here->balance>0){
                        return 1;
                    }
                    return 0;
                }
                else{
                    if(insert_here(a, here->link[1])){
                        here->balance++;
                        if(here->balance>1){
                            if(here->link[1]->balance>0){
                                one_line_right_rotation(here);
                            }
                            else if(here->link[1]->balance<0){
                                right_left_rotation(here);
                            }
                            return 0;
                        }

                        if(here->balance>0){
                            return 1;
                        }
                    }
                    return 0;
                }
            }
        }
    }

	nodeT * search(T * a, nodeT * here) {
		if (!here)	return nullptr;
		int cmp = a->cmp_name(*here->pdata);
		if (cmp>0) {
			return search(a, here->link[1]);
		}
		if (cmp < 0) {
			return search(a, here->link[0]);
		}
		if (cmp == 0) {
			return here;
		}
	}

    int one_line_left_rotation(nodeT * node){
		int tmp_balance = node->link[0]->balance;
		nodeT* tmplr = node->link[0]->link[1];
		node->link[0]->balance = 0;
		node->balance = 0;
		nodeT* tmpr = node->link[1];
		node->swap_data(*node->link[0]);
		node->link[1] = node->link[0];
		node->link[0] = node->link[0]->link[0];
		node->link[1]->link[0] = tmplr;
		node->link[1]->link[1] = tmpr;
		if (tmp_balance == -1) {
			node->link[1]->balance = 0;
			node->balance = 0;
			return 1;
		}
		else if (tmp_balance == 0) {
			node->link[1]->balance = -1;
			node->balance = 1;
			return 0;
		}
		else {
			assert(false);
			return 0;
		}
		//retrace(node);

    }
    int one_line_right_rotation(nodeT * node){
		int tmp_balance = node->link[1]->balance;
		
        nodeT* tmpl=node->link[0];
        nodeT* tmprl=node->link[1]->link[0];
		node->swap_data(*node->link[1]);
        node->link[0]=node->link[1];
        node->link[1]=node->link[1]->link[1];
        node->link[0]->link[1]=tmprl;
        node->link[0]->link[0]=tmpl;
		if (tmp_balance == 1) {
			node->link[0]->balance = 0;
			node->balance = 0;
			return 1;//h decreased by 1
		}
		else if (tmp_balance == 0) {
			node->link[0]->balance = 1;
			node->balance = -1;
			return 0;
		}
		else {
			assert(false);
			return 0;
		}
        //retrace(node);
    }
    void right_left_rotation(nodeT * node){
        nodeT* tmp=node->link[0];
        nodeT* tmpr=node->link[1]->link[0]->link[0];
		int tmp_balance = node->link[1]->link[0]->balance;
		node->swap_data(*node->link[1]->link[0]);
        node->link[0]=node->link[1]->link[0];
        node->link[1]->link[0] = node->link[0]->link[1];
        node->link[0]->link[0]=tmp;
        node->link[0]->link[1]=tmpr;
		if (tmp_balance == 1) {
			node->link[1]->balance = 0;
			node->link[0]->balance = -1;
		}
		else if (tmp_balance == -1) {
			node->link[1]->balance = 1;
			node->link[0]->balance = 0;
		}
		else {
			node->link[1]->balance = 0;
			node->link[0]->balance = 0;
		}
		node->balance = 0;
        //retrace(node);
    }

    void left_right_rotation(nodeT * node){
		
        nodeT* tmp=node->link[1];
        nodeT* tmpr=node->link[0]->link[1]->link[1];
		int tmp_balance = node->link[0]->link[1]->balance;
		node->swap_data(*node->link[0]->link[1]);
        //node->T::swap(*(node->link[0]->link[1]));
        node->link[1]=node->link[0]->link[1];
        node->link[0]->link[1] = node->link[1]->link[0];
        node->link[1]->link[1]=tmp;
        node->link[1]->link[0]=tmpr;
		if (tmp_balance == 1) {
			node->link[1]->balance = 0;
			node->link[0]->balance = -1;
		}
		else if(tmp_balance ==-1) {
			node->link[1]->balance = 1;
			node->link[0]->balance = 0;
		}
		else {
			node->link[1]->balance = 0;
			node->link[0]->balance = 0;
		}
		node->balance = 0;
        //retrace(node);
    }

	int delete_tree_node(T * node_data, nodeT* here, list<record> * storage) {
		if (!here) { 
			//printD("no res\n");
			return 0; }
		int cmp = node_data->cmp(*here->pdata);
		if (cmp > 0) { 
			int delta = delete_tree_node(node_data, here->link[1], storage);
			//if (delta) printD("1do smth\n");
			if (delta == -1) {

				printD("1it ll be nullptr");
				here->link[1]->print();//
				//print(here);
				here->link[1] = nullptr;
				here->balance--;
				if (here->balance > -2) { return 0; }
				else {
					if (here->link[0]->balance <= 0) {
						return one_line_left_rotation(here);
					}
					else if (here->link[0]->balance > 0) {
						left_right_rotation(here); return 1;
					}
				}
			}
			if (delta == 1) {
				here->balance--;
				if (here->balance > -2) { return 0; }
				else {
					if (here->link[0]->balance <= 0) {
						return one_line_left_rotation(here);
					}
					else if (here->link[0]->balance > 0) {
						left_right_rotation(here); return 1;
					}
				}
			}
			if (delta == 0) {
				return 0;//nothing
			}
		}
		else if (cmp < 0) {
			int delta = delete_tree_node(node_data, here->link[0], storage);
			if (delta) printD("2do smth\n");
			if (delta == -1) {
				printD("2 it ll be nullptr");
				here->link[0]->print();//
				//print(here);
				here->link[0] = nullptr;
				here->balance++;
				if (here->balance < 2) { return 0; }
				else {
					if (here->link[1]->balance >= 0) {
						return one_line_right_rotation(here);
					}
					else if (here->link[1]->balance < 0) {
						right_left_rotation(here); return 1;
					}
				}
			}
			if (delta == 1) {
				here->balance++;
				if (here->balance < 2) { return 0; }
				else {
					if (here->link[1]->balance >= 0) {
						 return one_line_right_rotation(here);
					}
					else if (here->link[1]->balance < 0) {
						right_left_rotation(here); return 1;
					}
				}
			}
			if (delta == 0) {
				return 0;//nothing
			}
		}
		else {
			here->print();
			if (here->is_leaf()) {
				printD("leaf\n");
				
				if(storage)storage->delete_list_node(here->pdata, nullptr);
				printD("alive1\n");
				delete here;
				printD("alive2\n");
				return -1;
			}
			else {
				printD("not leaf\n");
				print(here);
				nodeT *to_swap = nullptr;
				if (here->balance >= 0) {
					to_swap = here->link[1];
					printD("3swap to this ");//
					curr = here->link[1];

					while (curr = curr->link[0]) {
						to_swap = curr;
					}
					to_swap->print();//
					here->pdata = to_swap->pdata;
					print(here);//
					printD("3once more\n");//
					int ret = delete_tree_node(here->pdata, here->link[1], storage);
					printD("3al\n");//
					if (ret == -1) {
						printD("3it ll be nullptr\n");
						//here->link[1]->print();//
						
						here->link[1] = nullptr;
						print(here);
						here->balance--;
						if (here->balance == 0) return 1;
						if (here->balance == -1) { return 0; }
						else {
							if (here->link[0]->balance <= 0) {
								return one_line_left_rotation(here);
							}
							else if (here->link[0]->balance > 0) {
								left_right_rotation(here); return 1;
							}
						}
					}
					if (ret == 1) {
						here->balance--;
						if (here->balance > -2) { return 0; }
						else {
							if (here->link[0]->balance <= 0) {
								return one_line_left_rotation(here);
							}
							else if (here->link[0]->balance > 0) {
								left_right_rotation(here); return 1;
							}
						}
					}
					printD("3 0 retuernde\n");
					return 0;
				}
				else {
					to_swap = here->link[0];
					printD("4swap to this ");//
					curr = here->link[0];

					while (curr = curr->link[1]) {
						to_swap = curr;
					}
					to_swap->print();//
					here->pdata = to_swap->pdata;
					print(here);//
					printD("4once more\n");//
					int ret = delete_tree_node(here->pdata, here->link[0], storage);
					if (ret == -1) {
						printD("4 it ll be nullptr");
						//here->link[0]->print();//
						//print(here);
						here->link[0] = nullptr;
						print(here);
						here->balance++;
						if (here->balance < 2) { return 0; }
						else {
							if (here->link[1]->balance >= 0) {
								return one_line_right_rotation(here);
							}
							else if (here->link[1]->balance < 0) {
								right_left_rotation(here); return 1;
							}
						}
					}
					if (ret == 1) {
						here->balance++;
						if (here->balance < 2) { return 0; }
						else {
							if (here->link[1]->balance >= 0) {
								return one_line_right_rotation(here);
							}
							else if (here->link[1]->balance < 0) {
								right_left_rotation(here); return 1;
							}
						}
					}
					//if (ret != 0) return ret;
					printD("4 0 retuernde\n");
					return 0;
				}
			}
		}
		assert(false);
	}

	bool is_it_avl() {
		root = rroot();
		printf("total number=%d \n", go_around(this->root));
		printf("total number satis=%d\n", go_around_check(this->root));
		retrace(this->root);
		printf("total number satis(retraced)=%d\n", go_around_check(this->root));
		return 1;
	}

	int go_around(nodeT * root) {
		if (!root) return 0;
		return go_around(root->link[0])+go_around(root->link[1])+1;
	}

	int go_around_check(nodeT * root) {
		if (!root) return 0;
		if (abs(root->balance) <= 1) {
			if (root->link[0] && root->pdata->cmp(*root->link[0]->pdata) < 0 ||
				root->link[1] && root->pdata->cmp(*root->link[1]->pdata) > 0) {
				print(root);
				return go_around_check(root->link[0]) + go_around_check(root->link[1]);
			}
			return go_around_check(root->link[0]) + go_around_check(root->link[1])+1;
		}
		print(root);
		return go_around_check(root->link[0]) + go_around_check(root->link[1]);
	}

	void * avl_delete(const T * item)
	{
		//printD("avl_delete started\n");
		/* Stack of nodes. */
		avl_node<T> *pa[AVL_MAX_HEIGHT];	 /* Nodes. */
		unsigned char da[AVL_MAX_HEIGHT];    /* |link[]| indexes. */
		int k;                               /* Stack pointer. */

		avl_node<T> *p;		  /* Traverses tree to find node to delete. */
		int cmp;              /* Result of comparison between |item| and |p|. */

		assert(root != nullptr && item != nullptr);

		k = 0;
		//p = pre_root;
		p = (avl_node<T> *) &root;
		//printf("r =%p, 0 =%p, 1= %p\n", root, root->link[0], root->link[1]);
		//item->print();
		p->link[0] = root;
		//p->print();
		//printf("\n p =%p, 0 =%p, 1= %p\n", p, p->link[0], p->link[1]);
		//print();
		//auto preroot = new nodeT;
		//preroot->link[0] = root;
		//p = preroot;
		for (cmp = -1; cmp != 0; cmp = item->cmp(*p->pdata))
		{
			int dir = cmp > 0;

			pa[k] = p;
			da[k++] = dir;

			p = p->link[dir];
			//printD("cmp = %d\n", cmp);
			//if (p == nullptr) printf("preroot\n");
			//else p->print();

			if (p == nullptr)
				return nullptr;
		}

	
		item = p->pdata;//
		//p->print();
		//printD("ch 1\n");
		if (p->link[1] == nullptr) {
			assert(k - 1 >= 0);
			if (pa[k - 1]->link[da[k - 1]] == root) {
				root = p->link[0];
			}
			pa[k - 1]->link[da[k - 1]] = p->link[0];
		}
		else
		{
			printD("ch 2\n");
			avl_node<T> *r = p->link[1];
			if (r->link[0] == nullptr)
			{
				//printD("ch 2a\n");
				//printf("check1\n");
				r->link[0] = p->link[0];
				r->balance = p->balance;
				if (root == pa[k - 1]->link[da[k - 1]]) {
					//printf("skljfddf43\n");
					root = r;
				}
				pa[k - 1]->link[da[k - 1]] = r;
				da[k] = 1;
				if (root == pa[k++]) {
					root = r;
				}
				pa[k++] = r;
			}
			else
			{
				avl_node<T> *s;
				//printf("check2\n");
				int j = k++;

				for (;;)
				{
					da[k] = 0;
					if (root == pa[k++]) {
						root = r;
					}
					pa[k++] = r;

					s = r->link[0];
					if (s->link[0] == nullptr)
						break;
					if (root == r) {
						root = s;
					}
					r = s;
				}

				s->link[0] = p->link[0];
				r->link[0] = s->link[1];
				s->link[1] = p->link[1];
				s->balance = p->balance;
				if (pa[j - 1]->link[da[j - 1]] == root) {
					//printf("opa\n");
					root = s;
				}
				pa[j - 1]->link[da[j - 1]] = s;
				da[j] = 1;
				pa[j] = s;

			}
		}
		//printf("avl_delete this ");
		//if (p == nullptr) printf("preroot\n");
		//else p->print();
		delete p;

		assert(k > 0);
		while (--k > 0)
		{
			avl_node<T> *y = pa[k];
			//printf("k= %d rechfg\n", k);
			if (da[k] == 0)
			{
				
				
				y->balance++;
				//printf("rechfg5 y= ");
				//y->print();
				if (y->balance == +1)
					break;
				else if (y->balance == +2)
				{
					//printf("rech\n");
					avl_node<T> *x = y->link[1];
					if (x->balance == -1)
					{
						avl_node<T> *w;
						if (y == root) root = w;
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
						else /* |w->balance == -1| */
							x->balance = +1, y->balance = 0;
						w->balance = 0;
						if (pa[k - 1]->link[da[k - 1]] == root) {
							root = w;
						}
						pa[k - 1]->link[da[k - 1]] = w;
						
					}
					else
					{
						if (y == root) root = x;
						y->link[1] = x->link[0];
						x->link[0] = y;
						if (pa[k - 1]->link[da[k - 1]] == root) {
							root = x;
						}
						pa[k - 1]->link[da[k - 1]] = x;
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
				else {
					//printf("bal =0\n");
				}
			}
			else
			{
				y->balance--;
				//printf("rechfg6 y= ");
				//y->print();
				if (y->balance == -1)
					break;
				else if (y->balance == -2)
				{
					avl_node<T> *x = y->link[0];
					if (x->balance == +1)
					{
						avl_node<T> *w;
						if (y == root) root = w;
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
						else /* |w->balance == +1| */
							x->balance = -1, y->balance = 0;
						w->balance = 0;
						if (pa[k - 1]->link[da[k - 1]] == root) {
							root = w;
						}
						pa[k - 1]->link[da[k - 1]] = w;
						
					}
					else
					{
						//printf("Check 2skf\n");
						//printf("root=%p\n", root);
						if (y == root) root = x;
						//x->print();
						y->link[0] = x->link[1];
						x->link[1] = y;
						//printf("pa[k-1]=%p, pa[k - 1]->link[da[k - 1]]=%p\n", pa[k - 1], pa[k - 1]->link[da[k - 1]]);
						if (pa[k - 1]->link[da[k - 1]] == root) {
							//printf("df\n");
							root = x;
						}
						pa[k - 1]->link[da[k - 1]] = x;
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
		//printf("tree\n");
		//print();

		avl_size--;
		//if (avl_size == 0) root = curr = nullptr;
		//tree->avl_generation++;
		return (void *)item;
	}

	nodeT* rroot() { return pre_root->link[0]; }

	nodeT * avl_probe(T * item)
	{
		printD("insert started :");
		//item->print(); printD("\n");

		avl_node<T> *y, *z; /* Top node to update balance factor, and parent. */
		avl_node<T> *p, *q; /* Iterator, and parent. */
		avl_node<T> *n;     /* Newly inserted node. */
		avl_node<T> *w;     /* New root of rebalanced subtree. */
		int dir;                /* Direction to descend. */

		unsigned char da[AVL_MAX_HEIGHT]; /* Cached comparison results. */
		int k = 0;              /* Number of cached results. */

		assert(item != nullptr);
		//z = (avl_node<T> *) &root;
		z = pre_root;
		//root = z->link[0];
		//z->link[0] = root;
		y = rroot();
		dir = 0;
		for (q = z, p = y; p != nullptr; q = p, p = p->link[dir])
		{
			//int cmp = tree->avl_compare(item, p->pdata, tree->avl_param);
			//p->print();
			int cmp = item->cmp(*p->pdata);
			printD("cmp = %d\n", cmp);
			if (cmp == 0) {
				printD("cmp = 0!\n");
				return nullptr;
			}

			if (p->balance != 0)
				z = q, y = p, k = 0;
			da[k++] = dir = cmp > 0;
		}
		if (q != (avl_node<T> *) &root) {
			printD("q = ");
			//q->print(); 
		}
		n = q->link[dir] = new nodeT;
			//tree->avl_alloc->libavl_malloc(tree->avl_alloc, sizeof *n);
		if (n == nullptr)
			return nullptr;

		avl_size++;
		n->pdata = item;
		n->link[0] = n->link[1] = nullptr;
		n->balance = 0;
		if (y == nullptr)
			return n;
		printD("check1\n");
		for (p = y, k = 0; p != n; p = p->link[da[k]], k++)
			if (da[k] == 0)
				p->balance--;
			else
				p->balance++;
		printD("y = ");
		//y->print();
		if (y->balance == -2)
		{
			printD("check -2\n");
			avl_node<T> *x = y->link[0];
			//if (y == root) root = w;
			if (x->balance == -1)
			{
				w = x;
				y->link[0] = x->link[1];
				x->link[1] = y;
				x->balance = y->balance = 0;
			}
			else
			{
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
				else /* |w->balance == +1| */
					x->balance = -1, y->balance = 0;
				w->balance = 0;
			}
		}
		else if (y->balance == +2)
		{
			printD("check +2\n");
			avl_node<T> *x = y->link[1];
			printD("x = ");
			//x->print();
			//if (y == root) root = w;
			if (x->balance == +1)
			{
				w = x;
				y->link[1] = x->link[0];
				x->link[0] = y;
				x->balance = y->balance = 0;
			}
			else
			{
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
				else /* |w->balance == -1| */
					x->balance = +1, y->balance = 0;
				w->balance = 0;
			}
		}
		else {
			printD("check2\n");
			return n;
		}
		printD("check3\n");
		z->link[y != z->link[0]] = w;

		return n;
	}

    int abs(int a){
		return a>0?a: -a;
    }
    int getHeight(nodeT * root)
    {
       if (!root)
			return 0;
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
		return getHeight(root);
	}
    int  task1(nodeT * root, int k, int &counter){
		if(!root) return 0;
		int tmp = task1(root->link[0], k, counter)+task1(root->link[1], k, counter)+1;
		if(tmp<=k) ++counter;
		return tmp;
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
		//max=(max<abs(rHeight-lHeight))?abs(rHeight-lHeight): (max);
		max++;
		return (lHeight > rHeight)? (lHeight+1): (rHeight+1);
    }
	T** insert(T * a) {
		//if (avl_size == 0) root = curr = nullptr;

		//avl_size++;
		//insert_here(a, root);
		auto ret = avl_probe(a);
		if (!root) root = ret;
		else if (!ret) printD("insert failed\n");
		return &(ret)->pdata;
	}


    void print(){

        curr = root=rroot();
		if (!curr) return;
        print_();
        curr = root;
    }
	void print(nodeT * node) {
		curr = node;
		print_();
		curr = root;
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
