#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cstring>
using namespace std;


enum player { ME=1,ADV=-1};

// class action
const int MAX_ACTIONS = 5;

class action
{
public:
  virtual void do_action() const = 0;

};

class a_move:public action
{
public:
  
  virtual void do_action() const final 
  {
    cout << "MOVE " <<  src  << " " << dest << " " << cy_count <<  endl;
  }
  a_move(int src_,int dest_,int cy_):src(src_),dest(dest_),cy_count(cy_){}
  int src,dest,cy_count;
};

class a_bomb:public action
{
public:
  virtual void do_action() const final
  {
    cout << "BOMB " <<  src  << " " << dest << endl;
  }

  a_bomb(int s_,int d_):src(s_),dest(d_){}
  int src,dest;
};

class a_inc:public action
{
public:
  virtual void do_action() const final
  {
    cout << "INC " << fact_id << endl;
  }
  a_inc(int fi_):fact_id(fi_){}
  int fact_id;
};

class a_msg:public action
{
public:
  virtual void do_action() const final
  {
    cout << "MSG " << msg_p << endl;
  }

  a_msg(const string &ms_):msg_p(ms_){}
  string msg_p;
};

class a_wait:public action
{
public:
  virtual void do_action() const final
  {
    cout << "WAIT" << endl;
  }
};

struct actions_list_t
{
  action *act_list[MAX_ACTIONS];
};


// ******************************* end action def
class map_game_t
{
public:
  map_game_t()
  {
    int factoryCount; // the number of factories
    cin >> factoryCount; cin.ignore();
    int linkCount; // the number of links between factories
    cin >> linkCount; cin.ignore();
    for (int i = 0; i < linkCount; i++)
      {
        int factory1;
        int factory2;
        int distance;
        cin >> factory1 >> factory2 >> distance; cin.ignore();
	
	dist_mat[factory1][factory2] = distance;
	dist_mat[factory2][factory1] = distance;
      }
  }

  inline int get_dist(int a,int b) const
  {
    return dist_mat[a][b];
  }


 

  /*inline void get_min_dist(int *a,int *b) const
  {
    int min = 100000;
    
    for(int i = 0;i<15;++i)
      for(int j = 0;j<15;++j)
	{
	  if(dist_mat[i][j] < min)
	    {
	      min = dist_mat[i][j];
	      *a = i;
	      *b = j;
	    }
	}
	}*/
  //private:
  int dist_mat[15][15] = {}; //0 init
};


struct entity
{
 
};

struct fact_status_t
{
  int id,play,nb_cy,curr_prod,turn_prod;
};

struct troop_status_t
{
  int id,play,st,end,nb_cy,nb_dest;
};

struct bomb_status_t
{
  int id,play,st,end,nb_dest;
};

class game_stat_t
{
public:
  game_stat_t(const map_game_t& mgt_):mgt(mgt_){}
  void update_status()
  {
   
    cin >> entity_count; cin.ignore();
    
    nb_troop = 0;
    nb_fact = 0;
    nb_bombs = 0;
    
    for (int i = 0; i < entity_count; i++)
      {
	int entityId;
	string entityType;
	int arg1;
	int arg2;
	int arg3;
	int arg4;
	int arg5;
	cin >> entityId >> entityType >> arg1 >> arg2 >> arg3 >> arg4 >> arg5; cin.ignore();
	if(entityType == "FACTORY")
	  {
	    //cerr << "fact" << endl;
	    fact[entityId] = fact_status_t{entityId,arg1,arg2,arg3,arg4};
	    nb_fact++;
	  }
	else if(entityType == "TROOP")
	  {
	    assert(nb_troop  < 500);
	    
	    //cerr << "troop" << endl;
	    troop[nb_troop++] = troop_status_t{entityId,arg1,arg2,arg3,arg4,arg5};
	  }
	else
	  {
	    assert(nb_bombs  < 2);
	    bombs[nb_bombs++] = bomb_status_t{entityId,arg1,arg2,arg3,arg4};
	  }
      }
  }


  


  inline bool get_min_dist_other(int a,int *b) const
  {
    int min = 100000;
    
    
      for(int j = 0;j<nb_fact;++j)
	{
	  
	  if(fact[j].play != ME && mgt.dist_mat[a][j] > 0 && mgt.dist_mat[a][j] < min )
	    {
	      min = mgt.dist_mat[a][j];
	      *b = j;
	    }
	}
      return min != 100000;
  }
  
  inline const fact_status_t* get_max_fact_id() const
  {

    int max = 0;
    const fact_status_t *ret_val;
    for(int i=0;i<nb_fact;++i)
      {
	if(fact[i].play != ME) continue;
	if(fact[i].nb_cy > max)
	  {
	    max = fact[i].nb_cy;
	    ret_val = &fact[i];
	  }
      }
    return ret_val;
    //return max_element(begin(fact), end(fact),
    //		       [](fact_status_t a, fact_status_t b){ return (a.nb < b.nb) && (a.play == ME && b.play == ME); });
  }
  //private:

  const map_game_t& mgt;

  fact_status_t fact[15];
  troop_status_t troop[500]; //?
  bomb_status_t bombs[4]; //4 bomb max
  int nb_troop = 0;
  int nb_fact = 0;
  int nb_bombs = 0;
  int entity_count = 0;
  
};


class simulate_game
{
public:
  template<typename T>
  void up_entity(const T* orig_stat,T* new_stat,int size)
  {
    //decrease the count to end and rearange the arrays (count = 0 at the end of the array)
    int nb_arr = 0;
    for(int i = 0;i<size;++i)
      {
	if(orig_stat[i].nb_dest == 1)
	  ++nb_arr;
      }
    
    for(int i = 0;i<size;++i)
      {
	if(orig_stat[i].nb_dest == 1)
	  {
	    new_stat[i + size - nb_arr] = orig_stat[i];
	    --(new_stat[i + size - nb_arr].nb_dest);
	  }
	else
	  {
	  
	    new_stat[i] = orig_stat[i];
	    --(new_stat[i].nb_dest);
	  }
	

      }

 
  }
  
  void one_step(actions_list_t &ap1,actions_list_t &ap2,const game_stat_t& orig_stat, game_stat_t& new_stat)
  {
    //https://github.com/CodinGame/ghost-in-the-cell/blob/master/Referee.java
    // Move troops and bombs

    //-- troops
    up_entity<troop_status_t>(orig_stat.troop,new_stat.troop,orig_stat.nb_troop);
    new_stat.nb_troop =  orig_stat.nb_troop;


    //bombs
    up_entity<bomb_status_t>(orig_stat.bombs,new_stat.bombs,orig_stat.nb_bombs);
    new_stat.nb_bombs = orig_stat.nb_bombs;
    

    
    // Decrease disabled countdown
    //copy the factory to the new state
    memcpy(new_stat.fact,orig_stat.fact,orig_stat.nb_fact*sizeof(fact_status_t));
    new_stat.nb_fact = orig_stat.nb_fact;

    for(int i = 0;i<new_stat.nb_fact;++i)
      {
	if(new_stat.fact[i].turn_prod > 0)
	  --(new_stat.fact[i].turn_prod);
      }
    
    
    //   iterate factory, remove 1 if disabled
    
    // Execute orders for each player
    //  send bombs, send troops, increase
    
    //production usine

    
      
  }
};

int main()
{
 
  map_game_t map_game;

  game_stat_t game_stat(map_game);

  while (1)
    {
 
      game_stat.update_status();

      //action act;

      
      const fact_status_t* max_fct = game_stat.get_max_fact_id();

      int min_dist_id;
      if(game_stat.get_min_dist_other(max_fct->id,&min_dist_id))
	{
	  a_move(max_fct->id,min_dist_id,max_fct->nb_cy/2).do_action();
	  //act->do_action();
	  //cout << "MOVE " <<  max_fct->id << " " << min_dist_id << " " << max_fct->nb/2 <<  endl;
	}
      else
	{
	  a_wait().do_action();

	}
      //cout << "WAIT" << endl;


    }
}
