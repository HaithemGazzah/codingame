//Copyright Haithem Gazzah & Matthieu Ospici


#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

#include <cmath>
#define MAX_SIZE 10
using namespace std;

class GameState;

typedef enum act_t {MOVE, THROW} act_t;
typedef enum en_t {WIZARD, OPPONENT_WIZARD ,SNAFFLE,BLUDGER} en_t;
std::uint8_t ent_mask[4]={0x01,0x02,0x04,0x80};

struct Coordinates
{
public:
  float x;
  float y;
  Coordinates (float _x, float _y):x(_x),y(_y){}
  Coordinates(){}
  static Coordinates vector_from_points(Coordinates c1,Coordinates c2) 
  { 
    Coordinates c; 
    c.x = c2.x - c1.x; 
    c.y = c2.y - c1.y; 
    return c; 
  }
 
  static float comp_dist(Coordinates c1,Coordinates c2) 
  { 
    return sqrt(pow(c1.x-c2.x,2) + pow(c1.y - c2.y,2)); 
  } 

 
  //return normalized vector from 2 points 
  static Coordinates compute_norm_vect(Coordinates c1,Coordinates c2) 
  { 
    Coordinates cord = vector_from_points(c1,c2); 
    float norm_v = comp_dist(c1,c2); 
    cord.x = (float)cord.x/norm_v; 
    cord.y = (float)cord.y/norm_v; 
    return cord; 
  } 
	 
 
};


class Action
{	
public:
  act_t type;
  Coordinates c;
  int arg;//thrust for move, power for throw, id for spell
  void print()
  {
    switch(this->type)
      {
      case MOVE:
	cout<<"MOVE "<<this->c.x<<" "<<this->c.y<<" "<<this->arg<<endl;
	break;
      case THROW:
	cout<<"THROW "<<this->c.x<<" "<<this->c.y<<" "<<this->arg<<endl;
	break;
		default:
		cerr<<"NULL ACTION NOTHING TO PRINT"<<endl;
      }
  }
};



class Entity
{
public:
  Coordinates c;
  int id;
  int vx;
  int vy;
  float weight;
  float friction;
  int state;
  en_t type;
  Entity()
  {
  }
  void setProperties(int _id, en_t _type, int _x, int _y, int _vx, int _vy,int _state)
  {
    this->id = _id;
    this->c.x=_x;
    this->c.y=_y;
    this->vx=_vx;
    this->vy=_vy;
    this->type=_type;
    this->state=_state;
    switch(_type)
      {
      case OPPONENT_WIZARD:
      case BLUDGER:
	
      case WIZARD:
	this->weight = 1;
	this->friction = 0.75;
	break;
      case SNAFFLE:
	this->weight= 0.5;
	this->friction=0.75;
	break;
      }

  }
	/*
		type_mask : used to filter on more than on type of entity
		id_mask_tab :  is used to exclude some ids can be replaced by boolean table 
		here we are using bit shifting but may prove problematic with endianees
	*/
  int get_closest_entity(std::uint8_t type_mask,GameState *game_state,std::uint16_t id_mask_tab,float *out_dist);
  void print()
  {
    cerr<<"ID = "<<id<<" X = "<<c.x<<" Y ="<<c.y<<" vX="<<vx<<" vY="<<vy<<endl;
  }
};


class GameState 
{
public:
  Entity list_ent[13];
  int num_ents;
  int list_sna[8];
  int num_sna;
  int list_wiz[2]; //id des sorcier
  int list_op[2]; //id des oponant
  static int team_id;

  inline Entity& get_sna(int sna_id)
  {
    return list_ent[list_sna[sna_id]];
  }
  GameState()
  {
  }
  void create_entity_from_input()
  {
    cin >> num_ents; cin.ignore();
    int wiz_count=0;
    int op_count=0;
    int sna_count=0;
    for (int i = 0; i < num_ents; i++) {
      int entityId; // entity identifier
      string entityType; // "WIZARD", "OPPONENT_WIZARD" or "SNAFFLE" (or "BLUDGER" after first league)
      int x; // position
      int y; // position
      int vx; // velocity
      int vy; // velocity
      int state; // 1 if the wizard is holding a Snaffle, 0 otherwise
      cin >> entityId >> entityType >> x >> y >> vx >> vy >> state; cin.ignore();
   
      if(entityType.compare("SNAFFLE") == 0 )
	{
	  list_ent[i].setProperties(entityId, SNAFFLE ,x ,y ,vx ,vy ,state);
	  list_sna[sna_count++]=i;
	}
      else if (entityType.compare("WIZARD") == 0)
	{
	  list_ent[i].setProperties(entityId, WIZARD ,x ,y ,vx ,vy ,state);
	  list_wiz[wiz_count++]=i;
	}
      else
	{
	  list_ent[i].setProperties(entityId, OPPONENT_WIZARD ,x ,y ,vx ,vy ,state);
	  list_op[op_count++]=i;
	}
    }
    num_sna = sna_count;
  }
  void print_entities()
  {
    for(int i=0; i<num_ents; i++)
      list_ent[i].print();

  }


  float eval() //evaluation fct, compute the score of the board
  {
    //first approach, good score if snafles are at the right side
    // if 0 you need to score on the right of the map, if 1 you need to score on the left

    int num_sna = 0;
    if(team_id == 0)
      {
	for(int i = 0;i<num_sna;++i)
	  {
	    if(get_sna(i).c.x >8000) ++num_sna;
	  }
      }
    else
      {
	for(int i = 0;i<num_sna;++i)
	  {
	    if(get_sna(i).c.x <8000) ++num_sna;
	  }
      }


    return num_sna;
    
  }
	
};

int Entity::get_closest_entity(std::uint8_t type_mask,GameState *game_state,std::uint16_t id_mask_tab,float *out_dist)
 {

	*out_dist = 50000;
	int index_min =-1;
	std::uint16_t id_mask = 0;
    for(int i=0;i<game_state->num_ents; i++)
	{
		
		id_mask = 0;
		id_mask |=(1u << i); 
		if (id_mask & id_mask_tab && ent_mask[game_state->list_ent[i].type] & type_mask)
		{
			float dist = Coordinates::comp_dist(this->c,game_state->list_ent[i].c);
			if(dist < *out_dist)
			{
				*out_dist = dist;
				index_min=i;
			}
		}
	}
	return index_min;
}
	
class Simulator 
{
public:
  void predict_entity_state(Entity *entity, Action *action, int *out_x, int *out_y, int *out_vx,int *out_vy)
  {
    switch(action->type)
      {
      case MOVE:
	Coordinates cn = Coordinates::compute_norm_vect(entity->c,action->c);
	cn.x = cn.x*(float)action->arg;
	cn.y = cn.y*(float)action->arg;
				
	float vx_n = entity->vx + cn.x;
	float vy_n = entity->vy + cn.y;
				
	*out_x = round(entity->c.x+vx_n);
	*out_y = round(entity->c.y+vy_n);
				
	*out_vx = round(vx_n*entity->friction/entity->weight);
	*out_vy = round(vy_n*entity->friction/entity->weight);
      }
  }
  GameState * predict_state(GameState* game_state)
  {
  }
};
struct gamestate_queue
{
	
  char size;
  GameState * g_array[MAX_SIZE];
  char front = 0;
  char rear = -1 ;
	
  void insert(GameState *gameState)
  {
    if(rear==(MAX_SIZE-1))
      {
	delete g_array[front];
	front = (front+1)%MAX_SIZE;
			
      }
    rear = (rear+1)%MAX_SIZE;
    g_array[rear] = gameState;
  }
  GameState* get(char offset)
  {
    if(rear==-1) return NULL;
    if(offset ==0)return g_array[rear];
    size=(MAX_SIZE-front+rear)%MAX_SIZE;
    if (offset>size) return NULL;
    char index = (size-offset+front)%MAX_SIZE;
    return g_array[index];
  }
};
class IA_engine
{
public:
  Simulator simulator;
  Action get_action(int entity_index,GameState *game_state,gamestate_queue *history)
  {
  
		Action a;
		if(game_state->list_ent[entity_index].state==0)
		{
			//get closest SNAFFLE or BLUDGER 
			uint8_t type_mask = ent_mask[SNAFFLE] | ent_mask[BLUDGER];
			//And exclude the first snaffel from the search
			uint16_t id_mask = 0xFFFF;
			id_mask &=~(1u<< game_state->list_sna[0]);
			float out_distance=0;
			cerr<<"id_mask = "<<id_mask<<endl;
			int index_closest = game_state->list_ent[entity_index].get_closest_entity(type_mask,game_state,id_mask,&out_distance);
			if(index_closest>-1)
			{
				a.type = MOVE;
				a.c.x = game_state->list_ent[index_closest].c.x;
				a.c.y = game_state->list_ent[index_closest].c.y;
				a.arg = 100;
			}
			else
			{
			    cerr<<"nothing close found"<<endl;
			}
		}
		else
		{
				if (GameState::team_id == 0)
				{
					a.type = THROW;
					a.c.x = 16000;
					a.c.y = 3700;
					a.arg = 500;
				}
				else
				{
					a.type = THROW;
					a.c.x = 0;
					a.c.y = 3700;
					a.arg = 500;
				}

		}
		return a;
  }
	
};

/**
 * Grab Snaffles and try to throw them through the opponent's goal!
 * Move towards a Snaffle and use your team id to determine where you need to throw it.
 **/
int GameState::team_id =0;
int main()
{
  // if 0 you need to score on the right of the map, if 1 you need to score on the left
  cin >> GameState::team_id; cin.ignore();

  IA_engine ia;
  gamestate_queue history;
  GameState *game_state;
  // game loop
  while (1) {
    game_state = new GameState();
    game_state->create_entity_from_input();
    game_state->print_entities();
    for (int i = 0; i < 2; i++)
      {
		Action a = ia.get_action(game_state->list_wiz[i],game_state,&history);	
		a.print();

      }


    history.insert(game_state);
  }
}
