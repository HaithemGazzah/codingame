//Copyright Haithem Gazzah & Matthieu Ospici


//thanks to magus for some functions related to collisions, http://files.magusgeek.com/csb/csb.html

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>
#include <limits>   
#include <cmath>
#define MAX_SIZE 10
using namespace std;

class GameState;

typedef enum act_t {MOVE, THROW} act_t;
typedef enum en_t {WIZARD, OPPONENT_WIZARD ,SNAFFLE,BLUDGER} en_t;


typedef enum wall_t {R_WALL, L_WALL,T_WALL,B_WALL,WALL_NONE} wall_t;

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

  //thanks to magus for this function, http://files.magusgeek.com/csb/csb.html
  Coordinates closest(const Coordinates &a,  const Coordinates &b) const
  {
    float da = b.y - a.y;
    float db = a.x - b.x;
    float c1 = da*a.x + db*a.y;
    float c2 = -db*this->x + da*this->y;
    float det = da*da + db*db;
    float cx = 0;
    float cy = 0;

    if (det != 0)
      {
        cx = (da*c1 - db*c2) / det;
        cy = (da*c2 + db*c1) / det;
      }
    else
      {
        cx = this->x;
        cy = this->y;
      }

    return Coordinates(cx, cy);
}

  
  static float comp_dist(const Coordinates &c1,const Coordinates &c2) 
  { 
    return sqrt(pow(c1.x-c2.x,2) + pow(c1.y - c2.y,2)); 
  } 


  static float comp_dist2(const Coordinates &c1,const Coordinates &c2) 
  { 
    return (c1.x-c2.x)*(c1.x-c2.x) + (c1.y - c2.y)*(c1.y - c2.y); 
  } 
 
  //return normalized vector from 2 points 
  static Coordinates compute_norm_vect(const Coordinates &c1,const Coordinates &c2) 
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
	cout<<"MOVE "<<(int)this->c.x<<" "<<(int)this->c.y<<" "<<this->arg<<endl;
	break;
      case THROW:
	cout<<"THROW "<<(int)this->c.x<<" "<<(int)this->c.y<<" "<<this->arg<<endl;
	break;
      }
  }

  void print_debug()
  {
    switch(this->type)
      {
      case MOVE:
	cerr<<"MOVE "<<(int)this->c.x<<" "<<(int)this->c.y<<" "<<this->arg<<endl;
	break;
      case THROW:
	cerr<<"THROW "<<(int)this->c.x<<" "<<(int)this->c.y<<" "<<this->arg<<endl;
	break;
      }
  }
  
};

class Entity;

struct Collision
{
public:
  Collision():a(NULL),b(NULL),time(0){}
  
  Collision(Entity* a_, wall_t wt_,float t_):
    a(a_),b(NULL),time(t_),wt(wt_){}
  Collision(Entity* a_, Entity* b_,float t_):a(a_),b(b_),time(t_){}
  //private:
  Entity* a;
  Entity *b;
  float time;
  wall_t wt; //if a wall....
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
  float radius;
  int state;
  en_t type;
  //Entity *sna_hosted_by = NULL; //id of the wiz which hosts the sna if type == 1
  Entity *sna = NULL; //id of the sna for the wiz
  bool no_colision = false; //a sna attached to a snaffle has no col
  Entity()
  {
  }

  Entity(en_t et_):type(et_){}

  
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
	this->weight = 1;
	this->friction = 0.75;
	this->radius = 400;
	break;
      case BLUDGER:
	this->weight = 8;
	this->friction = 0.9;
	this->radius = 200;
	break;

      case WIZARD:
	this->weight = 1;
	this->friction = 0.75;
	this->radius = 400;
	break;
      case SNAFFLE:
	this->weight= 0.5;
	this->friction=0.75;
	this->radius = 150;
	break;
      default:
	cerr << "HEUUUU ERROR !!!" << endl;
      }

  }
	
  /*  int get_closest_entity(en_t targetType,GameState *game_state,float *out_dist)
  {
    //TODO
    }*/
	
  void print()
  {
    cerr<<"ID = "<<id<<" X = "<<c.x<<" Y ="<<c.y<<" vX="<<vx<<" vY="<<vy<<endl;

  }


  float comp_dist_to_2(const Entity& e) const
  {
    return Coordinates::comp_dist2(c,e.c);
  }


  //check if a collision happen between this and a wall
  bool check_collision_walls(Collision &col_out)
  {

    if(this->no_colision)
      return false; //one of the ent is a sna captured by a wiz --> no col !
    //this is a wall !!
    Coordinates lc;
    lc = c;
    
    lc.x += this->vx;
    lc.y += this->vy;
    
    
    float total_dist = Coordinates::comp_dist(c,lc);
    
    float time = 2;
    
    
    // cerr << total_dist << "  " << lc.x <<" " <<  " " <<lc.y << endl;
    if(lc.x - this->radius < 0 )
      {
	//left wall
	float dist_impact = Coordinates::comp_dist(c,Coordinates(this->radius,lc.y));
	//cerr << " LEFT " << " " << total_dist << " " << dist_impact <<endl;
	time = dist_impact/total_dist;
	col_out = Collision(this,L_WALL , time);
	
	
      }
    
    if(lc.x + this->radius > 16001)
      {
	//right wall
	
	float dist_impact = Coordinates::comp_dist(c,Coordinates(16001 - this->radius,lc.y));
	//cerr << " RIGHT " << " " << total_dist << " " << dist_impact <<endl;
	if(dist_impact/total_dist < time)
	  {
	    time = dist_impact/total_dist;
	    
	    col_out = Collision(this,R_WALL , time);
	  }
      }
    
    if(lc.y - this->radius < 0 )
      {
	//top wall

	float	dist_impact = Coordinates::comp_dist(c,Coordinates(lc.x,this->radius));
//		cerr << " TOP " << " " << total_dist << " " << dist_impact <<endl;
	if(dist_impact/total_dist < time)
	  {
	    time = dist_impact/total_dist;
	    col_out = Collision(this,T_WALL , time);
	  }
      }
    
    if(lc.y + this->radius > 7501)
      {
	//bottom wall
	float dist_impact = Coordinates::comp_dist(c,Coordinates(lc.x,7501 -this->radius));
	
	//cerr << " BOTO " << " " << total_dist << " " << dist_impact <<endl;
	if(dist_impact/total_dist < time)
	  {
	    time = dist_impact/total_dist;
	    col_out = Collision(this,B_WALL , time);
	  }
      }


    if(time > 1)
      return false; //no collision with a wall
    else
      return true;
      
  }

  //check if a collision can happen between this and u
  bool collision(Entity &u,Collision &col_out) 
  {



    if(u.no_colision || this->no_colision)
      return false; //one of the ent is a sna captured by a wiz --> no col !
    
    // Distance carré
    float dist = comp_dist_to_2(u);
    float sr;

    //somme rayons carrés


    
    if((this->type == OPPONENT_WIZARD || this->type == WIZARD) && u.type == SNAFFLE)
      sr = (this->radius)*(this->radius); //point collision if snaffle
    else if (this->type == SNAFFLE && (u.type == OPPONENT_WIZARD || u.type == WIZARD))
      sr = (u.radius)*(u.radius); //point collision if snaffle
    else
      sr = (this->radius + u.radius)*(this->radius + u.radius); //non wiz - snafle collision
    

    // On prend tout au carré pour éviter d'avoir à appeler un sqrt inutilement. C'est mieux pour les performances

    if (dist < sr)
      {
        // Les objets sont déjà l'un sur l'autre. On a donc une collision immédiate
   	col_out = Collision(this, &u, 0.0);
	return true;
      }

    // Optimisation. Les objets ont la même vitesse ils ne pourront jamais se rentrer dedans
    if (this->vx == u.vx && this->vy == u.vy)
      {
	return false;
      }

    // On se met dans le référentiel de u. u est donc immobile et se trouve sur le point (0,0) après ça
    float lx = this->c.x - u.c.x;
    float ly = this->c.y - u.c.y;
    Coordinates myp = Coordinates(lx, ly);
    float lvx = this->vx - u.vx;
    float lvy = this->vy - u.vy;
    Coordinates up = Coordinates(0, 0);

    // On cherche le point le plus proche de u (qui est donc en (0,0)) sur la droite décrite par notre vecteur de vitesse
    Coordinates p = up.closest(myp, Coordinates(lx + lvx, ly + lvy));

    // Distance au carré entre u et le point le plus proche sur la droite décrite par notre vecteur de vitesse
    float pdist = Coordinates::comp_dist2(up,p);

    // Distance au carré entre nous et ce point
    //    float mypdist = myp.comp_dist_to_2(p);
    float mypdist = Coordinates::comp_dist2(myp,p);
    // Si la distance entre u et cette droite est inférieur à la somme des rayons, alors il y a possibilité de collision
    if (pdist < sr)
      {
        // Notre vitesse sur la droite
        float length = sqrt(lvx*lvx + lvy*lvy);

        // On déplace le point sur la droite pour trouver le point d'impact
        float backdist = sqrt(sr - pdist);
        p.x = p.x - backdist * (lvx / length);
        p.y = p.y - backdist * (lvy / length);

        // Si le point s'est éloigné de nous par rapport à avant, c'est que notre vitesse ne va pas dans le bon sens
        if ( Coordinates::comp_dist2(myp,p)> mypdist)
	  {
	    return false;
	  }
	pdist = Coordinates::comp_dist(p,myp);
	
        // Le point d'impact est plus loin que ce qu'on peut parcourir en un seul tour
        if (pdist > length)
	  {
            return false;
	  }

        // Temps nécessaire pour atteindre le point d'impact
        float t = pdist / length;


	col_out = Collision(this, &u, t);
	return true;
      }

    //    return null;
    return false;
  }


  void compute_collision_effect_wall(wall_t wal)
  {
    switch(wal)
      {
      case R_WALL:
	vx = -vx;
	break;
      case L_WALL:
	vx = -vx;
	break;
      case T_WALL:
	vy = -vy;
	break;
      case B_WALL:
	vy = -vy;
	break;
      case WALL_NONE:
	cerr << "************* ERRRRERU WALL " << endl;
      }
  }

  void compute_collision_effect(Entity &u)
  {
    if (u.type == SNAFFLE && (this->type == WIZARD ||this->type == OPPONENT_WIZARD ))
      {
	u.c  = this->c;
	u.vx = this->vx;
	u.vy = this->vy;
	u.no_colision = true;
	this->sna = &u;

	this->state = 1;
	
      }
    else if ((u.type == WIZARD ||u.type == OPPONENT_WIZARD ) && this->type == SNAFFLE)
      {
	this->c  = u.c;
	this->vx = u.vx;
	this->vy = u.vy;

	this->no_colision = true;
	u.sna = this;

	u.state = 1;
	
      }
    else
      {

        float m1 = this->weight;
        float m2 = u.weight;

        // Si les masses sont égales, le coefficient sera de 2. Sinon il sera de 11/10
        float mcoeff = (m1 + m2) / (m1 * m2);

        float nx = this->c.x - u.c.x;
        float ny = this->c.y - u.c.y;

        // Distance au carré entre les 2 pods. Cette valeur pourrait être écrite en dure car ce sera toujours 800²
        float nxnysquare = nx*nx + ny*ny;

        float dvx = this->vx - u.vx;
        float dvy = this->vy - u.vy;

        // fx et fy sont les composantes du vecteur d'impact. product est juste la pour optimiser
        float product = nx*dvx + ny*dvy;
        float fx = (nx * product) / (nxnysquare * mcoeff);
        float fy = (ny * product) / (nxnysquare * mcoeff);

        // On applique une fois le vecteur d'impact à chaque pod proportionnellement à sa masse
        this->vx -= fx / m1;
        this->vy -= fy / m1;
        u.vx += fx / m2;
        u.vy += fy / m2;

      
        float impulse = sqrt(fx*fx + fy*fy);
        cerr << "impu " << impulse << endl;
        if (impulse < 100.0)
	  {
            fx = fx * 100.0 / impulse;
            fy = fy * 100.0 / impulse;
	  }

        // On applique une deuxième fois le vecteur d'impact à chaque pod proportionnellement à sa masse
        this->vx -= fx / m1;
        this->vy -= fy / m1;
        u.vx += fx / m2;
        u.vy += fy / m2;
      }
  }



  //update speed vector
  void prepare_move(const Action& act)
  {


    //assert(act.type == MOVE);
    assert(this->type == WIZARD);

    Coordinates cn = Coordinates::compute_norm_vect(this->c,act.c);

    if(act.type == MOVE)
      {
	if(this->sna != NULL)
	  {
	    //we lost sna
	    this->sna->no_colision = false;
	    this->sna = NULL;
	    this->state = 0;
	
	  }

    
	
	cn.x = cn.x*(float)act.arg/weight;
	cn.y = cn.y*(float)act.arg/weight;


	this->vx += cn.x;
	this->vy += cn.y;
      }
    else
      {
	//action throw !!
	assert(this->sna != NULL);


	cn.x = cn.x*(float)act.arg/this->sna->weight;
	cn.y = cn.y*(float)act.arg/this->sna->weight;


	this->sna->vx += cn.x;
	this->sna->vy += cn.y;
	
	this->sna->no_colision = false;
	this->sna = NULL;
	this->state = 0;

	
	  

      }

  }


  void execute_move(float t)
  {


      

    this->c.x += this->vx * t;
    this->c.y += this->vy * t;
      
  }




  void finish_action()
  {
    this->c.x = round(this->c.x);
    this->c.y = round(this->c.y);
    this->vx = round(this->vx * friction);
    this->vy = round(this->vy * friction);

    /*   if(this->type == SNAFFLE && this->no_colision)
      {
	this->no_colision = false; //after a turn, snaffle are released
	this->sna_hosted_by = NULL;
      }

    if((this->type == OPPONENT_WIZARD || this->type == WIZARD) && this->sna != NULL)
      {
	this->sna = NULL; //after a turn, sna released
	
	}*/
      
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

  inline const Entity& get_sna(int sna_id) const
  {
    return list_ent[list_sna[sna_id]];
  }

  inline  const Entity& get_wiz(int wiz_id) const
  {
    return list_ent[list_wiz[wiz_id]];
  }

  
  inline  Entity& get_wiz(int wiz_id)
  {
    return list_ent[list_wiz[wiz_id]];
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
	  //check if a snaffle belongs to a WIZARD
	  for(int ii=0;ii<4;++ii)
	    {
	      if(list_ent[ii].state == 1 && list_ent[ii].vx == list_ent[i].vx && list_ent[ii].vy == list_ent[i].vy)
		{
		  //ok le snaffle est attaché
		  //list_ent[i].sna_hosted_by = &list_ent[ii];
		  list_ent[ii].sna = &list_ent[i];
		  list_ent[ii].no_colision = true;
		}
	    }
	}
      else if (entityType.compare("WIZARD") == 0)
	{
	  list_ent[i].setProperties(entityId, WIZARD ,x ,y ,vx ,vy ,state);
	  list_wiz[wiz_count++]=i;
	}
      else if (entityType.compare("OPPONENT_WIZARD") == 0)
	{
	  list_ent[i].setProperties(entityId, OPPONENT_WIZARD ,x ,y ,vx ,vy ,state);
	  list_op[op_count++]=i;
	}
      if (entityType.compare("BLUDGER") == 0)
	{
	  list_ent[i].setProperties(entityId, BLUDGER ,x ,y ,vx ,vy ,state);

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


    // better to have sna in my zone
    float num_sna_l = 0;
    if(team_id == 0)
      {
	for(int i = 0;i<num_sna;++i)
	  {
	    if(get_sna(i).c.x >13000 && get_sna(i).c.y >1750 && get_sna(i).c.y<5750) ++num_sna_l;
	  }
      }
    else
      {
	for(int i = 0;i<num_sna;++i)
	  {
	    if(get_sna(i).c.x <3000 && get_sna(i).c.y >1750 && get_sna(i).c.y<5750 ) ++num_sna_l;
	  }
      }


    //very good to put sna on goal !
    float sna_goal = 0;
    for(int i =0;i<num_sna;++i)
      {
	//predict the dest :
	Coordinates c = get_sna(i).c;

	c.x += get_sna(i).vx;
	c.y += get_sna(i).vy;

	if(c.y >1750 && c.y<5750)
	  {
	    if(team_id == 0 && c.x >=16000)
	      {
		sna_goal += 20;
	      }
	    else if(team_id == 1 && c.x <= 0)
	      {
		sna_goal += 20;
	      }
	  }
      }


    //Good score si on se raproche d'un sna

    float close_sna_score = 0;

    float dist[2] = {0,0};
    
    for(int i=0;i<2;++i)
      {
	float clos = 100000;
	for(int j=0;j<num_sna;++j)
	  {
	    float lc = Coordinates::comp_dist2(get_wiz(i).c,get_sna(j).c);
	    if(lc < clos)
	      {
		clos = lc;
		dist[i] = lc;
	      }
	  }
      }

    close_sna_score = dist[0] + dist[1];
    return num_sna_l + sna_goal + close_sna_score;
    
  }

  
	
};


class Simulator 
{
public:


  Action generate_move_act(const Entity& wiz_ent,int cur_act,int num_act)
  {

    assert(wiz_ent.type == WIZARD);

    
    
    Action act;
 
    
    //  (a+rcos(θ+ϕ),b+rsin(θ+ϕ))
   
    float section = (2.0*3.14)/num_act;

    float circle = 1000;
    Coordinates c;
    act.c.x = wiz_ent.c.x + circle*cos(section*cur_act);
    act.c.y = wiz_ent.c.y + circle*sin(section*cur_act);


    if(wiz_ent.state == 1)
      {
	act.arg = 500;
	act.type = THROW;
      }
    else
      {
	act.arg = 150;
	act.type = MOVE;
      }
    //act.print_debug();
    return act;

  }



  
  GameState predict_state(const GameState &game_state, const Action &a,const Action &b)
  {
    GameState game_state_n = game_state;

    Entity &wiz1 = game_state_n.get_wiz(0);
    Entity &wiz2 = game_state_n.get_wiz(1);

    wiz1.prepare_move(a);
    wiz2.prepare_move(b);


    //simulation of collisions
    const int num_ent = game_state_n.num_sna + 4 + 2;
    float t = 0.0;

    while (t < 1.0)
      {
        Collision firstCollision(NULL,NULL,18.0);


        for (int i = 0; i < num_ent; ++i)
	  {
	    //check i with wall
	    Collision col_out;
	    if(game_state_n.list_ent[i].check_collision_walls(col_out))
	      if(col_out.time != 0 && col_out.time + t < 1.0 && col_out.time < firstCollision.time)
		firstCollision = col_out;
		  
	      
	    for (int j = i + 1; j < num_ent; ++j)
	      {
                Collision col_out;
		if(game_state_n.list_ent[i].collision(game_state_n.list_ent[j],col_out))
		  if(col_out.time != 0 && col_out.time + t < 1.0 && col_out.time < firstCollision.time)
		    firstCollision = col_out;   


	      }

           
        }

        if (firstCollision.time == 18) //no collision
	  {

            for (int i = 0; i < num_ent; ++i)
	      {
                game_state_n.list_ent[i].execute_move(1.0 - t);
	      }
	    

            t = 1.0;
        } else
	  {

            for (int i = 0; i < num_ent; ++i)
	      {

		game_state_n.list_ent[i].execute_move(firstCollision.time - t);
	      }

            // On joue la collision
	    if(firstCollision.b != NULL)
	      firstCollision.a->compute_collision_effect(*firstCollision.b);
	    else
	      firstCollision.a->compute_collision_effect_wall(firstCollision.wt);

	    
            t += firstCollision.time;
        }
    }

    for (int i = 0; i < num_ent; ++i)
      {
	
	game_state_n.list_ent[i].finish_action();
      }
   
    return game_state_n;
  }



  float get_best_actions(const GameState &game_state,
			 Action &act1_o,
			 Action & act2_o//,
			 /*	 int cur_dep,
				 int max_dep*/)
  {
    const Entity& wiz0 = game_state.get_wiz(0);
    const Entity& wiz1 = game_state.get_wiz(1);
    
    int num_actions = 10;

    float best_eval = -10000; //numeric_limits<float>::min();
   
    Action loc_1,loc2;
    
    for(int i = 0;i<num_actions;++i)
      {
	Action act1 = generate_move_act(wiz0,i,num_actions);
	for(int j=0;j<num_actions;++j)
	  {
	    Action act2 = generate_move_act(wiz1,j,num_actions);
	    GameState gs = predict_state(game_state, act1,act2);

	    
	    // get_best_actions(gs,act1,act2,cur_dep + 1 ,max_dep)
	    
	    
	    float loc_eval = gs.eval();
	   // cerr << "l e " << loc_eval << endl;
	    if(loc_eval > best_eval)
	      {
	        //  cerr << "yo " << endl;
		best_eval = loc_eval;
		/*loc_1 = act1;
		  loc_2 = act2;*/
		act1_o = act1;
		act2_o = act2;
		
	      }
	    
	  }
      }
    return best_eval;
  }
};
/*struct gamestate_queue
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
  Simulator simulator;
  Action get_action(int entity_index,GameState *game_state,gamestate_queue *history)
  {
    //return Action(MOVE, 
  }
	
  };*/

/**
 * Grab Snaffles and try to throw them through the opponent's goal!
 * Move towards a Snaffle and use your team id to determine where you need to throw it.
 **/
int GameState::team_id =0;
int main()
{
  // if 0 you need to score on the right of the map, if 1 you need to score on the left
  cin >> GameState::team_id; cin.ignore();

  // IA_engine ia;
  // gamestate_queue history;
  GameState game_state_input;
  Simulator sim;
  
  // game loop
  while (1) {
    
    game_state_input.create_entity_from_input();
    game_state_input.print_entities();



    cerr << "compute new gm" << endl;

    Action act1;

    Action act2;


 
    sim.get_best_actions(game_state_input,act1,act2);

    act1.print();
    act2.print();
  }
}
