/* ***************************************************************
 * Huffman Encoder V1.0 
 * 02.04.2006 hannenz@freenet.de
 *
 * Encodes a given file using Huffman Codes
 * 
 * writes output file with header:
 * 1 Byte: Number of Symbols: n
 * n times:
 * 	char: 		symbol
 * 	unsigned int:	propability of that symbol
 *
 * compiles with cc65 v2.11.0 to c64 target
 * should compile to all other targets as well, but i didn't try...
 *  
 * the source code for the decoder contains more documentation about
 * creating the input list and the huffman tree. Since this is the
 * same job for encoder and decoder, refer to there for more info.
 * the functions are the same!
 *
 * have fun.
 *
 * do whatever you want with this source... ;)
 *
 * hannenz@freenet.de
 * http://people.freenet.de/hannenz
 * ****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cbm.h>

/* GLOBALS */

struct node {			//data structure for the nodes of the binary tree
	unsigned data;		//the actual symbol (byte value)
	unsigned prob;		//its propability (number of occurance in fact), 
				//i mixed this two terms a little bit, don't get confused...
	struct node *left;	//well, for the tree... pointer to children
	struct node *right;
	struct node *next;	//for the list we need a pointer to the next list element
};
typedef struct node NODE;

unsigned occ[256];		//this table contains the actual number of occurances for each byte (->propability)
unsigned char codelength[256];		//length of the bitcode for each byte value
unsigned int  codes[256];		//generated bitcode for each byte value
unsigned char ministack[sizeof(int)*8];	//this ministack is needed for bit output...
unsigned char sp,maxdepth;		
unsigned inbytes,outbytes;

NODE *base = NULL;		//base for the input list
NODE *root = NULL;		//root for the binary tree

static char nomem[]="out of memory.";

/* PROTOTYPES */

int insert_in_list(unsigned char,unsigned,NODE *,NODE *);
void make_codes(NODE*,unsigned char,unsigned);
NODE* allocate_node(void);
NODE *create_huffman_tree(void);
void push(unsigned char);
unsigned char pull(void);
void bits_out(unsigned,unsigned char);

/* CODE */

/* this function generates the code for each symbol, that occured at least once in the input file
 * it is a recursive function and traverses the tree in preorder, and for each step to the left child 
 * adds a binary 0 to the code and for each step to the right a binary 1. If a leaf is reached the code is
 * complete, so it will be returned 
 * refer to other documents on huffman for detailed information... (search the web!)*/

void make_codes(NODE *t,unsigned char depth,unsigned code){
	/* is ot a leaf? */
	/* then output the code in the table and end recursion */
	if (t->left == NULL && t->right == NULL){
		codes[t->data] = code;
		codelength[t->data] = depth;
		return;
	}
	else{
		code = code << 1;
		// since we store the codes in an integer, max codelength can be 16 (Bit).
		// The codelength equals the tree-depth, so exit if this depth is > 16
		if (++depth > maxdepth){
			maxdepth = depth;	//just for info: determine the maximum codelength
			if (maxdepth > sizeof(int)*8){
				puts("codelength overflow!");
				exit(-1);
			}
		}
		// recursion, adding a binary 0, is not necessary because of left shift automatically shifts in a 0
		make_codes(t->left,depth,code);
		code |= 1;	//add binary 1
		make_codes(t->right,depth,code);
	}
}

//well... should be clear...??!
NODE* allocate_node(void){
	NODE *n;
	if ((n = (NODE*)malloc(sizeof(NODE)))==NULL){
		puts(nomem);
		exit(-1);
	}
	return n;
}

//creates the huffman tree. refer to the decoder sourcecode for more info!
NODE* create_huffman_tree(void){
	unsigned prob;
	NODE *lst,*tree1,*tree2,*del;

	while (base->next){
		lst = base;
		tree1 = allocate_node();
		tree1->data = lst->data;
		tree1->prob = lst->prob;
		tree1->left = lst->left;
		tree1->right = lst->right;
		del = lst;
		free(del);
		
		lst = lst->next;
		tree2 = allocate_node();
		tree2->data = lst->data;
		tree2->prob = lst->prob;
		tree2->left = lst->left;
		tree2->right = lst->right;

		
		base = lst->next;		//removing the entries we have now.
		free(lst);
		prob = tree1->prob + tree2->prob;
		
		insert_in_list('+',prob,tree1,tree2);
	}
	return base;
}


//inserts a node into the input list, refer to the decoder source code for more documentation
int insert_in_list(unsigned char d,unsigned p,NODE *l,NODE *r){
	NODE *ptr,*tmp,*tmp2;
	/* if it is the very first entry, create a base */
	if (base == NULL){
		base = allocate_node();
		ptr = base;
		ptr->next = NULL;
	}
	else{
		/* look for position to sort in */
		ptr = base;	// begin at base
		while (ptr->prob < p){	//compare propabilities
			tmp = ptr;	//strore current pos
			if ((ptr = ptr->next) == NULL) {	//advance in list,
			       	/* if end of list reached: create a new entry*/
				ptr = allocate_node();
				tmp->next = ptr;	//let former last entry point to the new last one
				ptr->next = NULL;	//and mark it as end
				ptr->data = d;
				ptr->prob = p;
				ptr->left = l;
				ptr->right = r;
				return EXIT_SUCCESS;
			}
		}
		/* if it has to be sorted in at the beginning */
		if (ptr == base){
			tmp = base;	//store old base and create a new entry
			ptr = allocate_node();
			ptr->next = tmp;	//link to old base
			base = ptr;		//and set it as new base
		}
		else{
			/* sort in somewhere inbetween */
			tmp2 = ptr;	//store current pointer (that is: pointer to the element with next greater prob
			ptr = allocate_node();
			tmp->next = ptr;	//let the one before point to our new entry
			ptr->next = tmp2;	//and this one points to the element with next greater prob
		}
	}
	ptr->data = d;	//write data to current entry and exit.
	ptr->prob = p;
	ptr->left = l;
	ptr->right = r;
	return EXIT_SUCCESS;
}

//ministack push
void push(unsigned char x){
	ministack[sp] = x;
	++sp;
}
//ministack pull
unsigned char pull(void){
	--sp;
	return ministack[sp];
}

/* we need to output single bits, here we go:
 * bitio could be done very much more efficient if done in plain assembler...
 * but i wanted to dio it in plain C because of compiling for other systems.
 */

void bits_out(unsigned x,unsigned char length){
	unsigned char i;
	static unsigned char outbyte = 0;	//make 'em static, so each call they have their old value!
	static unsigned char count = 0;
	
	sp = 0;					//init stack pointer
	for (i=0;i!=length;++i){		//do as often as codelength is:
		push(x%2);			//push the rightmost bit and shift right
		x = x >> 1;
	}

	for (i=0;i!=length;++i){		//then get em back from stack in reversed order and output them
		outbyte = outbyte << 1;		//shift the output byte one ops left
		outbyte |= pull();		//then add the next bit from stack to the rightmost position
		if (++count == 8){		//every 8th time: output the complete byte, reset
			count = 0;
			cbm_k_bsout(outbyte);
			++outbytes;
			outbyte = 0;
		}
	}
}

void clear_tree(NODE* p){
	if (p->left == NULL || p->right == NULL){
		free (p);
		return;
	}
	clear_tree(p->left);
	clear_tree(p->right);
}

void clear_list(){
	NODE *ptr,*ptr1;
	
	if (base != NULL){
		ptr = base->next;
		while (ptr != NULL){
			ptr1 = base->next->next;
			base->next = ptr1;
			free(ptr->next);
			free(ptr);
			ptr = ptr1;
		}
		free(base->next);
		free(base);
		base = NULL;
	}
}

int main(int argc, char **argv){
	unsigned i;
	unsigned char c,n;
	long perc;
	unsigned char *ram_base,*ram,*ram_end;

	if (argc != 3){
		puts("specify input and output file!");
		exit(-1);
	}
	
	maxdepth = 0;
	memset(codelength,0,sizeof(codelength));	//init codelength table with 0 to see which bytes are used.
		
	i = _heapmaxavail()-0x2000;		//reserve 0x2000 bytes for binary tree & list, i hope this is enough ;)
	ram_base = (unsigned char*)malloc(i);	//allocate mem for input file (we load it to RAM, since we have to
						//read it twice anyway and from RAM is much faster!
						//if you need more memory for tree and the like, you could read 2 times
						//from disk having a very slow performance but lots of RAM
	
	//open ibnput file (read file)
	if ((cbm_open(2,8,2,argv[1]))!=0){
		puts("can't open niput file");
		exit(-1);
	}
	
	/* read file to RAM and count probabilities (occurances)*/
	ram_end = ram_base;
	puts("reading input file.");
	cbm_k_chkin(2);
	while(cbm_k_readst() != 64){
		c = cbm_k_basin();
		*ram_end++ = c;
		++occ[c];
		++inbytes;
		if (ram_end > ram_base + i){
			puts(nomem);
			cbm_close(2);
			exit(-1);
		}
	}
	cbm_close(2);

	/* now insert the non-zero occurances into the input list (sorted!)*/
	for (i=0;i!=256;++i){
		if (occ[i])
			insert_in_list(i,occ[i],NULL,NULL);
	}
	
	/*create the huffman tree */
	root = create_huffman_tree();
	
	/* and generate the codes for each symbol according to the huffman tree */
	make_codes(root,0,0);
	
	/* open output file */
	puts("writing output file.");	
	if ((cbm_open(2,8,CBM_WRITE,strncat("@:",argv[2],16)))!=0){
		puts("can't open write file");
		exit(-1);
	}
	cbm_k_ckout(2);
	// generate & write header
	
	n = 0;
	for (i=0;i!=256;++i){
		if (occ[i])
			++n;
	}
	cbm_k_bsout(n);
	
	for (i=0;i!=256;++i){
		if (occ[i]){
			cbm_k_bsout(i);
			cbm_k_bsout(occ[i]);
			cbm_k_bsout(occ[i]>>8);
			outbytes += 3;
		}
	}
	
	/* write the actual codes */
	/* cc65 doesn't seem to know rewind, what a pity...*/

	ram = ram_base;
	while (ram != ram_end){
		 c = *ram++;
		 bits_out(codes[c],codelength[c]);
	}
        bits_out(0,7);
	
	/* close, info to user & exit */
	
	cbm_close(2);
	perc = (long)outbytes*100/inbytes;
	printf ("\ninput:       %d bytes\noutput:      %d bytes\ncompression: %lu%%\n",inbytes,outbytes,perc);
	printf ("\nmax codelength: %d",maxdepth);
	free(ram_base);
	clear_tree(root);
	clear_list();
}

