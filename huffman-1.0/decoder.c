/* ***************************************************************
 * Huffman Decoder V1.0 
 * 02.04.2006 hannenz@freenet.de
 *
 * Decodes a given file encodeed with the Huffman Encoder V1.0
 * 
 * if an output file is given as second parameter, the decodeed data
 * will be written into that file, else it is writen to stdout 
 * (useful for encoded text files)
 * 
 * refer to the encoder source code for more detail about the file
 * structure, file-header and the like...

 * compiles with cc65 v2.11.0 to c64 target
 * should compile to all other targets as well, but i didn't try...
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

struct node {
	unsigned data;		//the actual symbol (byte value)
	unsigned prob;		//its propability (occurance)
	struct node *left;	//pointer to left child in binary tree 
	struct node *right;	//pointer to right...
	struct node *next;	//pointer to next list element if used in input list
};
typedef struct node NODE;

FILE *fp,*fpout;	
unsigned occ[256];		//this table holds the occurances for each byte (symbol)

NODE *base = NULL;		//root for binary tree
NODE *root = NULL;		//base for input list

int insert_in_list(unsigned char d,unsigned p,NODE *l,NODE *r);

/*allocates memory for a new node and returns a pointer to it */
NODE* allocate_node(){
	NODE *n;
	if ((n = (NODE*)malloc(sizeof(NODE)))==NULL){
		puts("out of memory");
		exit(-1);
	}
	return n;
}

/* this one creates the huffman tree from the input list */
NODE* create_huffman_tree(){
	unsigned prob;
	NODE *lst,*tree1,*tree2,*del;

	while (base->next){			//as long as the list has more than one entry...
		lst = base;			//get first list element
		tree1 = allocate_node();	//create a new node
		tree1->data = lst->data;	//and copy the list entry into it
		tree1->prob = lst->prob;
		tree1->left = lst->left;
		tree1->right = lst->right;
		del = lst;			//delete list entry
		free(del);
		
		lst = lst->next;		//get next list entry
		tree2 = allocate_node();	//see above...
		tree2->data = lst->data;
		tree2->prob = lst->prob;
		tree2->left = lst->left;
		tree2->right = lst->right;
		
		base = lst->next;		//delete this list entry as well
		free(lst);
		prob = tree1->prob + tree2->prob;	//add propabilities of the two entries
		
		insert_in_list('+',prob,tree1,tree2);	//and insert the parent of them in the list again
	}
	return base;		//when finished, the list base (last entry) is the root of the bibnary tree, so return it
}

/* this one inserts an entry into the list, sorted by its propability */
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

/* bit input */
unsigned char get_bit(){
	unsigned char bit;
	static unsigned char byte = 0;
	static unsigned char count = 0;

	/* every 8th bit, get a new byte from the file */
	if ((count & 7) == 0)
		byte = fgetc(fp);
	
	bit = byte & 0x80; 	//get bit 7 (leftmost bit)
	byte = byte << 1;	//and shift the whole byte onen position to the left
	++count;		//increase counter
	return bit;
}

/* recursive function to travers the tree according to the input bits 
 * if a zero bit is encountered we use the left path of the tree, or else we go right until a leaf is found, 
 * then we return this leaf, since it holds the symbol corresponding to the read bit sequence. */
unsigned char decode(NODE *p){
	if (p->left == NULL || p->right == NULL)	//we found a leaf, so return its value
		return p->data;
	else{
		if (get_bit())
			decode(p->right);		//note recursion
		else 
			decode(p->left);		
	}
}

int main(int argc, char **argv){
	unsigned i;
	unsigned char c,n;
	
	/* bla bla... */
	if (argc < 2 || argc > 3){
		puts("usage: decode infile [outfile]\nif no outfile is given, output goes to stdout.");
		exit(0);
	}
	
	/* open input file */
	if ((fp = fopen(argv[1],"rb"))==NULL){
		puts("can't open input file");
		exit(-1);
	}

	/* read its header: first byte of header contains number of used symbols: n*/
	n = fgetc(fp);

	/* now read n times: one byte: symbol, one integer: its occurance (propability) */
	for (i=0;i!=n;++i){
		c = fgetc(fp);
		fread(&occ[c],sizeof(int),1,fp);
	}
	
	/* here we do the same as the encoder does: creating an input list accoring to the occurances... */
	for (i=0;i!=256;++i){
		if (occ[i])
			insert_in_list(i,occ[i],NULL,NULL);
	}
	/* .... then create the huffman tree... */
	root = create_huffman_tree();

	
	/* ... care about the output meanwhile... if a file is given open it, else output to stdout */
	if (argc ==3){
		if ((fpout = fopen(argv[2],"wb"))==NULL){
			puts("can't open output file");
			argc = 2;
		}
	}

	/* we just need to call the decode function with the tree root, that's it... quite cool, eh?! */
	while(!feof(fp)){
		if (argc == 2)
			putc(decode(root),stdout);
		else
			fputc(decode(root),fpout);
	}
	
	/* close the files & clean up */
	fclose(fp);
	fclose(fpout);
}

