#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert> 
using namespace std;


enum player { ME=1,ADV=-1};

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


 

  inline void get_min_dist(int *a,int *b) const
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
  }
  //private:
  int dist_mat[15][15] = {}; //0 init
};



struct fact_status_t
{
  int id,play,nb,prod;
};

struct troop_status_t
{
  int id,play,st,end,nb_cy,nb_dest;
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
	    cerr << "fact" << endl;
	    fact[entityId] = fact_status_t{entityId,arg1,arg2,arg3};
	    nb_fact++;
	  }
	else
	  {
	    assert(nb_troop  < 500);
	    
	    cerr << "troop" << endl;
	    troop[nb_troop++] = troop_status_t{entityId,arg1,arg2,arg3,arg4,arg5};
	  }
      }
  }

  inline bool get_min_dist_other(int a,int *b) const
  {
    int min = 100000;
    
    
      for(int j = 0;j<15;++j)
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
	if(fact[i].nb > max)
	  {
	    max = fact[i].nb;
	    ret_val = &fact[i];
	  }
      }
    return ret_val;
    //return max_element(begin(fact), end(fact),
    //		       [](fact_status_t a, fact_status_t b){ return (a.nb < b.nb) && (a.play == ME && b.play == ME); });
  }
private:

  const map_game_t& mgt;

  fact_status_t fact[15];
  troop_status_t troop[500]; //?
  int nb_troop = 0;
  int nb_fact = 0;
  int entity_count;
};


int main()
{
 
  map_game_t map_game;

  game_stat_t game_stat(map_game);

  while (1)
    {
 
      game_stat.update_status();


      const fact_status_t* max_fct = game_stat.get_max_fact_id();

      int min_dist_id;
      if(game_stat.get_min_dist_other(max_fct->id,&min_dist_id))
	cout << "MOVE " <<  max_fct->id << " " << min_dist_id << " " << max_fct->nb/2 <<  endl;
      else
	cout << "WAIT" << endl;
    }
}
