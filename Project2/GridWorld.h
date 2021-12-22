#ifndef _GRID_WORLD_H
#define _GRID_WORLD_H

#include <vector>
#include <iostream>

using std::vector;
using namespace std;

class districtN {
	public:
		districtN* next;
		districtN* prev;
		int id;
		districtN(int id, districtN* _next = nullptr, districtN* _prev = nullptr)
		{
			this->id = id;
			next = _next;
			prev = _prev;
		}
};

struct district {
	districtN* head = nullptr;
	districtN* tail = nullptr;
	int count = 0;
};

class pArray {
	public:
		int rows;
		int identity;
		int cols;
		districtN* temp;
		bool alive = false;
		pArray(int row, int col, int id, districtN* temp1, bool alive) {
			this->rows = row;
			this->cols = col;
			this->identity = id;
			this->temp = temp1;
			this->alive = alive;
		}
};

class GridWorld {

  private:
    // private stuff goes here!
    //   typedefs
    //   data members
    //   private helper functions
    //   etc.
	
	
	int rows = 0;
	int cols = 0;
	int numPopulation = 0;
	int ID;
	bool isAlive = false;
	vector<pArray> people;
	district** pDist;
	district l1;

	districtN* push_back(district* l1, int id)
    {
        districtN* tmp = new districtN(id, nullptr, nullptr);
		districtN* curr = l1->tail;
        l1->count += 1;

        if (curr == nullptr)
        {
			l1->tail = tmp;
			l1->head = l1->tail;
        }
        else
        {
            curr->next = tmp;
            tmp->prev = curr;
            l1->tail = tmp;
        }

        return tmp;
    }
	
	bool push_back_2(district* l1, districtN** node) {
		
        if (l1->head == nullptr)
        {
			l1->tail = (*node);
			l1->head = l1->tail;
			(*node)->next = (*node)->prev = nullptr;
        }
        else
        {
            l1->tail->next = (*node);
            (*node)->prev = l1->tail;
            l1->tail = (*node);
			(*node)->next = nullptr;
        }
		l1->count++;
		return true;
	}
	
	bool check(district* l1, districtN* node)
    {
        if (l1->head == nullptr)
        {
            return false;
        }

        if (node == l1->head && node == l1->tail)
        {
			l1->head = nullptr;
			l1->tail = nullptr;
        }
        else if (node == l1->head)
        {
            l1->head = node->next;
            l1->head->prev = nullptr;
        }
        else if (node == l1->tail)
        {
            l1->tail->prev->next = nullptr;
            l1->tail = node->prev;
        }
        else
        {
            node->prev->next = node->next;
            node->next->prev = node->prev;
        }

        node->next = nullptr;
        node->prev = nullptr;
        l1->count--;
        return true;
    }
	
	int move(district* l1, districtN* node, district* l2) {
		
		if(!check(l1,node)) 
			return 0;
		
		if(push_back_2(l2,&node)) {
			return 1;
		}
		return 0;
	}
	
	
  public:
  
    /**
    * constructor:  initializes a "world" with nrows and
    *    ncols (nrows*ncols districtcs) in which all 
    *    districtricts are empty (a wasteland!).
    */
	
    GridWorld(unsigned nrows, unsigned ncols) {
		this->rows = nrows;
		this->cols = ncols;
		pDist = new district*[nrows];
		for(int i = 0; i < rows; i++) 
		{
			pDist[i] = new district[ncols];
		}
	}
	
    ~GridWorld() {
		for(int x = 0; x < rows; x++)
			delete[] pDist[x];
		delete[] pDist;
    }
	
    /*
    * function: birth
    * description:  if row/col is valid, a new person is created
    *   with an ID according to rules in handout.  New person is
    *   placed in district (row, col)
    *
    * return:  indicates success/failure
    */
	
    bool birth(int row, int col, int &id){
		if((row < 0 || col < 0) || (row > rows || col > cols))
			return false;
		else {
			if(l1.count == 0) {
				push_back(&pDist[row][col],numPopulation);
				id = numPopulation;
				people.push_back(pArray(row,col,id,pDist[row][col].tail,true));
				numPopulation++;
			}
			else {
				districtN* tmp = l1.head;
				id = tmp->id;
				push_back(&pDist[row][col],l1.head->id);
				people[id] = pArray(row,col,id,pDist[row][col].tail,true);
				if(!check(&l1,tmp))
					return false;
				delete tmp;
			}
		}
		return true;
    }

    /*
    * function: death 
    * description:  if given person is alive, person is killed and
    *   data structures updated to reflect this change.
    *
    * return:  indicates success/failure
    */
	
    bool death(int personID){
		if(personID < 0)
			return false;
		
		else {
			pArray* current = &people[personID];
				
			if(current->alive) {
				if(!check(&pDist[current->rows][current->cols],current->temp))
					return false;
				delete current->temp;
				push_back(&l1, current->identity);
				current->alive = false;
				return true;
			}
		}
		return false;
    }

    /*
    * function: whereis
    * description:  if given person is alive, his/her current residence
    *   is reported via reference parameters row and col.
    *
    * return:  indicates success/failure
    */
	
    bool whereis(int id, int &row, int &col)const{
		if(id < 0)
			return false;
		else {
			if(people[id].alive) {
				row = people[id].rows;
				col = people[id].cols;
				return true;
			}
		}
      return false;
    }

    /*
    * function: move
    * description:  if given person is alive, and specified target-row
    *   and column are valid, person is moved to specified district and
    *   data structures updated accordingly.
    *
    * return:  indicates success/failure
    *
    * comment/note:  the specified person becomes the 'newest' member
    *   of target district (least seniority) --  see requirements of members().
    */
	
    bool move(int id, int targetRow, int targetCol){
		if(id < 0 || (targetRow < 0 || targetCol < 0) || (targetRow > rows || targetCol > cols))
			return false;
		
		else {
			pArray* tmp = &people[id];
			
			if(tmp->alive == false) 
				return false;
			
			if(move(&pDist[tmp->rows][tmp->cols],tmp->temp,&pDist[targetRow][targetCol]) == 1) {
				tmp->rows = targetRow;
				tmp->cols = targetCol;
			}
		}
		
		return true;
    }

    std::vector<int> * members(int row, int col)const{
		vector<int>* data = new vector<int>();
		
		if((row < 0 || col < 0) || (row > rows || col > cols))
			return data;
		
		else {
			districtN* tmp = pDist[row][col].head;
			
			while(tmp){
				data->push_back(tmp->id);
				tmp = tmp->next;
			}
		}
		
		return data;
    }

    /*
    * function: population
    * description:  returns the current (living) population of the world.
    */
	
    int population()const{
		return numPopulation - l1.count;
    }
    
    /*
    * function: population(int,int)
    * description:  returns the current (living) population of specified
    *   district.  If district does not exist, zero is returned
    */
	
    int population(int row, int col)const{
		if((row < 0 || col < 0) || (row > rows || col > cols))
			return 0;
		return pDist[row][col].count;
    }

    /*
    * function: num_rows
    * description:  returns number of rows in world
    */
	
    int num_rows()const {
		return rows;
    }

    /*
    * function: num_cols
    * description:  returns number of columns in world
    */
	
    int num_cols()const {
		return cols;
    }



};

#endif