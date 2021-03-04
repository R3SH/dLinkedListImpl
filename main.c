#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define ST_SYMBOLS 16
#define TIME_SYMBOLS 3

struct TimeCont
{
	char hrs[TIME_SYMBOLS], mins[TIME_SYMBOLS];					/*Hours, minutes*/
};

struct Trains
{
	int number;
	char stName[ST_SYMBOLS];
	struct TimeCont depTime, travTime;			/*Departre time, travel time*/
	char avTick;
};

typedef struct Trains DataType;

struct Node
{
	DataType data;
	struct Node* next;		/*Pointer to next Node*/
	struct Node* prev;		/*Pointer to previous Node*/
};

struct List					/*List descriptor*/
{
	struct Node* begin, * end;
};

typedef struct List* list;

DataType input_train(void);
struct List read_file(char* filename, struct List);
struct List new_node_org(struct List, DataType);
struct List new_node_create(struct List, DataType);
struct List new_node_start(struct List, DataType);
struct List new_node_insert(struct Node*, DataType);
struct List new_node_end(struct List, DataType);
struct List delete_node(struct List);
struct List delete_node_beg(struct List);
struct List delete_node_end(struct List);
void delete_node_mid(list);
int write_file(char* filename, struct List);
int timeContToInt(struct TimeCont);
void nullTheList(list plist);
void delete_list(list);
void edit_list(list);
void print_list(struct List);
void print_list_structure(struct List);
void search_list(struct List);

int main(int argc, char* argv[])
{
	char filename[80];
	struct List trains;
	char mnCh = NULL;
	int tmp, n, i;

	nullTheList(&trains);
	trains = read_file(argv[1], trains);

	do
	{
		system("CLS");
		puts("1. Organized add\n2. Show\n3. Search\n4. Delete\n5. Insert(!CORRUPTS LIST'S ORGANIZATION)\n6. Insert to beginning(!CORRUPTS LIST'S ORGANIZATION)\n7. Insert to the end(!CORRUPTS LIST'S ORGANIZATION)\n8. Exit");
		mnCh = getchar(); getchar();

		switch (mnCh)
		{
		case '1': trains = new_node_org(trains, input_train()); break;
		case '2': print_list(trains); break;
		case '3': search_list(trains); break;
		case '4': trains = delete_node(trains); break;
		case '5': trains = new_node_create(trains, input_train()); break;
		case '6': trains = new_node_start(trains, input_train()); break;
		case '7': trains = new_node_end(trains, input_train());
		}
	} while (mnCh != '8');

	system("CLS");
	puts("Do you want to save entered list?(y/n)");
	mnCh = getchar(); getchar();
	if (mnCh == 'y' || mnCh == 'Y')
	{
		do
		{
			system("CLS");
			puts("Create new file(1) or write to the origin(2)?(1/2)");
			mnCh = getchar(); getchar();
			if (mnCh == '1')
			{
				puts("Enter name for new file: ");
				scanf("%s", &filename);
				write_file(filename, trains);
			}
			else if (mnCh == '2')
				write_file(argv[1], trains);
			else
			{
				puts("ERROR: Invalid input");
				getchar();
			}
		} while (mnCh != '1' && mnCh != '2');
	}

	return 0;
}

DataType input_train(void)
{
	DataType train;
	puts("Train number:");
	scanf("%d", &train.number); getchar();
	puts("Station name:");
	gets(train.stName);
	puts("Departure time hours:");
	gets(train.depTime.hrs);
	puts("Departure time minutes:");
	gets(train.depTime.mins);
	puts("Travel time hours:");
	gets(train.travTime.hrs);
	puts("Travel time minutes:");
	gets(train.travTime.mins);
	puts("Available avTick(Y/N):");
	scanf("%c", &train.avTick); getchar();

	return train;
}

struct List read_file(char* filename, struct List plist)
{
	FILE* f1;
	char fname[80];
	DataType inp_train;
	struct List cur;
	if (filename != NULL)
	{
		if ((f1 = fopen(filename, "rb")) == NULL)
		{
			perror("ERROR: Unable to open file to read");
			getchar();
			return plist;
		}
	}
	else
	{
		printf("Unable to open %s", filename);
		printf("\nEnter filename: ");
		scanf("%s", &fname);
		if ((f1 = fopen(fname, "rb")) == NULL)
			printf("\nWasn't able to open file");

		return plist;
	}

	if (fread(&inp_train, sizeof(inp_train), 1, f1))
		plist = new_node_start(plist, inp_train);

	cur = plist;

	while (fread(&inp_train, sizeof(inp_train), 1, f1))
	{
		cur = new_node_end(cur, inp_train);
		cur.begin = cur.begin->next;
	}

	plist.end = cur.begin;

	fclose(f1);
	return plist;
}

int write_file(char* filename, struct List plist)
{
	FILE* f1;

	if ((f1 = fopen(filename, "wb+")) == NULL)
	{
		perror("ERROR: Unable to open/create file to write to");
		getchar();
		return 0;
	}
	while (plist.begin)
	{
		if (fwrite(&plist.begin->data, sizeof(DataType), 1, f1))
			plist.begin = plist.begin->next;
		else
		{
			perror("ERROR: Corrupted list data");
			getchar();
			return 0;
		}
	}

	return 1;
}

struct List new_node_org(struct List plist, DataType new_train)			/*Organized add to organized doubly linked list*/
{
	struct List tempList = plist;

	if (plist.begin == NULL)
		plist = new_node_start(plist, new_train);
	else
	{
		while (tempList.begin != NULL && new_train.number >= tempList.begin->data.number)
			tempList.begin = tempList.begin->next;

		if (tempList.begin == NULL)
			plist = new_node_end(plist, new_train);
		else
		{
			tempList.begin = tempList.begin->prev;
			new_node_insert(tempList.begin, new_train);
		}
	}

	return plist;
}

struct List new_node_create(struct List plist, DataType new_train)
{
	struct List tempList = plist;
	int kNumb;

	if (plist.begin == NULL)
	{
		plist = new_node_start(plist, new_train);
		return plist;
	}
	else
	{
		print_list_structure(plist);
		printf("Enter after what node to insert:\n");
		scanf("%d", &kNumb);
		while (tempList.begin->data.number != kNumb && tempList.begin != NULL)
			tempList.begin = tempList.begin->next;

		if (plist.begin != NULL)
		{
			if (plist.begin->next == NULL)
				plist = new_node_end(plist, new_train);
			else
				new_node_insert(tempList.begin, new_train);
		}
		else
			printf("\nINVALID VALUE\n");
	}

	return plist;
}

struct List new_node_start(struct List plist, DataType new_train)		/*Inserts function to the beginning of the list, returns updated list descriptor*/
{
	struct Node* temp = (struct Node*)malloc(sizeof(struct Node));

	temp->data = new_train;
	temp->next = plist.begin;
	temp->prev = NULL;
	if (plist.begin)				/*If list is not empty then second node's pointer to prev asserted with address of temp*/
		plist.begin->prev = temp;
	else							/*else if list is empty temp is last node*/
		plist.end = temp;
	plist.begin = temp;

	return plist;
}

struct List new_node_insert(struct Node* plist, DataType new_train)
{
	struct Node* temp = (struct Node*)malloc(sizeof(struct Node));

	temp->data = new_train;
	temp->prev = plist;
	plist = plist->next;
	temp->next = plist;		/*TODO: read accesss violation error			!!!pointer to prev is fubar*/
	plist->next = temp;
	if (temp->next)
		temp->next->prev = temp;
	else
	{
		perror("ERROR: TRYING TO ACCESS INVALID MEMORY(new_node_insert)");
		getchar();
	}
}

struct List new_node_end(struct List plist, DataType new_train)		/*Appends a new node to the end of the list, returns updated list descriptor*/
{
	struct Node* temp = (struct Node*)malloc(sizeof(struct Node));
	temp->data = new_train;
	temp->next = NULL;

	if (plist.end)						/*check if linked list is not empty*/
		plist.end->next = temp;
	else								/*else if linked list is empty, makes the new node as the head*/
		plist.begin = temp;

	temp->prev = plist.end;				/*asigns pointer to prev node*/
	plist.end = temp;

	return plist;
}

void nullTheList(list plist)
{
	plist->begin = plist->end = NULL;
}

void print_list(struct List cur)		/*This function prints contents of linked list*/
{
	char dr = NULL;
	int i, k = 0, offset = 0;

	system("CLS");
	if (cur.begin == NULL)
	{
		puts("List is empty");
		getchar();
		return;
	}

	while (cur.begin != NULL)
	{
		printf("|%4d|%16s| %2s:%2s  |  %2s:%2s  |    %c    |\n", cur.begin->data.number, cur.begin->data.stName,
			cur.begin->data.depTime.hrs, cur.begin->data.depTime.mins, cur.begin->data.travTime.hrs, cur.begin->data.travTime.mins, cur.begin->data.avTick);
		cur.begin = cur.begin->next;		/*cur.begin->prev*/

		offset++;
	}

	/*
	puts("Enter number of objects on one page:");
	scanf("%d", &k); getchar();
	while (offset <= k && cur != NULL)

	{
		puts("|  N  |  Station name  |Dep.time|Trav.time|Av.tickets|");
		puts("------------------------------------------------------");
		for (i = 0; i < k && cur != NULL; i++)
		{
			printf("|%4d|%16s| %5s  |  %5s  |    %c    |\n", cur->data.number, cur->data.stName,
				cur->data.depTime, cur->data.travTime, cur->data.avTick);
			cur = cur->prev;

			offset++;
		}
		if (cur == NULL && cur->prev != NULL);
			puts("<l--------------------------------------------------r>");
	}*/
	getchar();
}

void print_list_structure(struct List plist)
{
	system("CLS");
	printf("List's structure:\n");
	while (plist.begin != NULL)
	{
		printf("<|%4d|>", plist.begin->data.number);
		plist.begin = plist.begin->next;
	}

	printf("\nList's structure(End to beginning)\n");
	while (plist.end != NULL)
	{
		printf("<|%4d|>", plist.end->data.number);
		plist.end = plist.end->prev;
	}

	getchar();
}

void search_list(struct List cur)
{
	char dst[ST_SYMBOLS];
	int i, k = 0, bound1 = 0, bound2 = 0, idepTime = 0;
	struct TimeCont tb1, tb2;

	if (cur.begin == NULL)
	{
		puts("List is empty");
		getchar();
		return;
	}

	system("CLS");
	puts("Enter destination city");
	gets(dst);
	puts("Enter time interval\nHours from:");
	gets(tb1.hrs);
	puts("\nMinutes from:");
	gets(tb1.mins);
	puts("To:\nHours to:");
	gets(tb2.hrs);
	puts("To:\nMinutes to:");
	gets(tb2.mins);

	bound1 = timeContToInt(tb1);
	bound2 = timeContToInt(tb2);

	system("CLS");
	puts("|  N  |  Station name  |Dep.time|Trav.time|Av.tickets|");
	puts("------------------------------------------------------");

	while (cur.begin != NULL)
	{
		if (strcmp(cur.begin->data.stName, dst) == 0)
		{
			idepTime = timeContToInt(cur.begin->data.depTime);
			if (idepTime != 0)
			{
				if (idepTime > bound1 && idepTime < bound2)
				{
					printf("|%4d|%16s| %2s:%2s  |  %2s:%2s  |    %c    |\n", cur.begin->data.number, cur.begin->data.stName,
						cur.begin->data.depTime.hrs, cur.begin->data.depTime.mins, cur.begin->data.travTime.hrs, cur.begin->data.travTime.mins,
						cur.begin->data.avTick);
				}
			}

			k++;
		}

		cur.begin = cur.begin->next;
	}

	if (k == 0)
	{
		system("CLS");
		puts("No matching results");
	}

	getchar();
}

int timeContToInt(struct TimeCont tConv)			/*Converts time from TimeCont structs to integer*/
{
	int i, tmp = 0;

	for (i = 0; i < TIME_SYMBOLS; i++)
	{
		if (isdigit(tConv.hrs[i]))
		{
			tmp *= 10;
			tmp += tConv.hrs[i] - 48;
		}
	}
	for (i = 0; i < TIME_SYMBOLS; i++)
	{
		if (isdigit(tConv.mins[i]))
		{
			tmp *= 10;
			tmp += tConv.mins[i] - 48;
		}
	}

	return tmp;
}

struct List delete_node(struct List plist)
{
	struct List tmp = plist;
	int i, kNumb;

	system("CLS");
	printf("List's structure:\n");
	print_list_structure(plist);
	printf("\nEnter number you want to delete: ");
	scanf("%d", &kNumb);
	while (tmp.begin != NULL && kNumb != tmp.begin->data.number)
		tmp.begin = tmp.begin->next;

	if (tmp.begin)
	{
		if (tmp.begin->data.number == kNumb)
		{
			if (tmp.begin->next == NULL)
				plist = delete_node_end(plist);
			else if (tmp.begin->prev == NULL)
				plist = delete_node_beg(plist);
			else
				delete_node_mid(tmp.begin);
		}
	}
	else
	{
		system("CLS");
		printf("Such node wasn't found");
	}

	return plist;
}

struct List delete_node_beg(struct List plist)			/*Delete's node in the beginning*/
{
	list tmp;

	if (plist.begin)
	{
		tmp = plist.begin;
		plist.begin = plist.begin->next;
		plist.begin->prev = NULL;
		free(tmp);

		return plist;
	}
	else
	{
		perror("CAN'T DELETE NON-EXISTENT NODE");
		getchar();
	}
}

struct List delete_node_end(struct List plist)				/*Delete's node in the end of the list*/
{
	list tmp;

	if (plist.end)
	{
		tmp = plist.end;
		plist.end = plist.end->prev;
		plist.end->next = NULL;			/*TODO: ERRROR CHECK*/
		free(tmp);

		return plist;
	}
	else
	{
		perror("CAN'T DELETE NON-EXISTENT NODE");
		getchar();
	}
}

void delete_node_mid(struct Node* pnode)
{
	pnode->next->prev = pnode->prev;
	pnode->prev->next = pnode->next;
	free(pnode);
}

/*void delete_node(list begin)
{
	struct node* temp = begin;
	int posn, i;

	if (begin == NULL)
	{
		system("CLS");
		puts("Can't delete nodes from non-existent list.");
		getchar();
		return;
	}

	system("CLS");
	puts("Enter the position you want to delete");
	scanf("%d", &posn);

	if (posn == 1)
	{

	}

	for(i = 0; i < posn && temp != NULL; i++)
		temp = temp->next;

	if (temp == NULL)
	{
		system("CLS");
		puts("Can't delete non-existent node.");
		getchar();
		return;
	}
	else
	{


		temp = begin;
		begin = begin->next;

		free(temp);
	}
}*/