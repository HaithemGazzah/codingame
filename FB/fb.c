#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <math.h>
/**
 * Grab Snaffles and try to throw them through the opponent's goal!
 * Move towards a Snaffle and use your team id to determine where you need to throw it.
 **/
typedef struct coord {float x,y;} coord;

typedef enum en_t {WIZARD, OPPONENT_WIZARD ,SNAFFLE,BLUDGER} en_t;

typedef struct entity_t
{
  int entityId;
  en_t et;
  coord c;
  int vx; // velocity
  int vy; // velocity
  int state; // 1 if the wizard is holding a Snaffle, 0 otherwise
} entity_t;

typedef struct game_stat_t
{
  entity_t ents[13];
  int num_ents;
  int list_sna[8];
  int num_sna;
  int list_wiz[2]; //id des sorcier
  int list_op[2]; //id des oponant

  int myTeamId;
} game_stat_t;


float comp_dist(coord c1,coord c2);




coord vector_from_points(coord c1,coord c2)
{
    coord c;
    c.x = c2.x - c1.x;
    c.y = c2.y - c1.y;
    return c;
}

//return normalized vector from 2 points
coord compute_norm_vect(coord c1,coord c2)
{
    coord cv = vector_from_points(c1,c2);
    float norm_v = comp_dist(c1,c2);
    cv.x = cv.x/norm_v;
    cv.y = cv.y/norm_v;
    return cv;
}

float comp_dist(coord c1,coord c2)
{
  return sqrt(pow(c1.x-c2.x,2) + pow(c1.y - c2.y,2));
}

entity_t get_sor(int id,game_stat_t* gs) //0 or 1
{
  return gs->ents[gs->list_wiz[id]];
}


void print_action_move(coord c,int thrust)
{
  printf("MOVE %i %i %i\n",(int)c.x,(int)c.y,thrust);
}

void print_action_throw(coord c,int power)
{
  printf("THROW %i %i %i\n",c.x,c.y,power);
}

entity_t get_closest_sna(entity_t *wiz,game_stat_t* st,int entity_id_mask)
{
  float dist_min = 50000;
  entity_t sna_min;
  for(int i=0;i<st->num_sna;++i)
    {
      int sna_id = st->list_sna[i];
      entity_t sna = st->ents[sna_id];

      if(entity_id_mask != -1 && sna.entityId == entity_id_mask) continue; //mask one entity
      float dist = comp_dist(wiz->c,sna.c);
     // fprintf(stderr,"dist %f\n",dist);
      if(dist < dist_min)
	{
	  dist_min = dist;
	  sna_min = sna;
	}
      
    }
  return sna_min;
}
en_t get_type(char entityType[21])
{
     
  if(strncmp(entityType, "WIZARD", 21) == 0)
    return WIZARD;
  
  if(strncmp(entityType, "OPPONENT_WIZARD", 21) == 0)
    return OPPONENT_WIZARD;
  
  if(strncmp(entityType, "SNAFFLE" , 21) == 0)
    return SNAFFLE;
  
  if(strncmp(entityType, "BLUDGER", 21) == 0)
    return BLUDGER;
  
  fprintf(stderr,"ERRRROR\n");
  return WIZARD;
  
  
}
int main()
{
   // if 0 you need to score on the right of the map, if 1 you need to score on the left
 
  int step = -1;
  game_stat_t stat;
  scanf("%d", &(stat.myTeamId)); 
    
  // game loop
  while (1) {
    int entities; // number of entities still in game
    scanf("%d", &entities);

    stat.num_ents = entities;
    stat.num_sna = 0;

    int num_wiz = 0;
    int num_op = 0;
	fprintf(stderr,"step : %d\n",++step);
	
    for (int i = 0; i < entities; i++) {
      int entityId; // entity identifier
      char entityType[21]; // "WIZARD", "OPPONENT_WIZARD" or "SNAFFLE" (or "BLUDGER" after first league)
      int x; // position
      int y; // position
      int vx; // velocity
      int vy; // velocity
      int state; // 1 if the wizard is holding a Snaffle, 0 otherwise
      scanf("%d%s%d%d%d%d%d", &entityId, entityType, &x, &y, &vx, &vy, &state);

      
      fprintf(stderr,"%d,%s,%d,%d,%d,%d,%d\n", entityId, entityType, x, y, vx, vy, state);
      
      stat.ents[i].entityId = entityId;
      stat.ents[i].et = get_type(entityType);
      stat.ents[i].c.x = x; 
      stat.ents[i].c.y = y; 
      stat.ents[i].vx = vx; 
      stat.ents[i].vy = vy; 
      stat.ents[i].state = state;
      //fprintf(stderr, "nstr et %s\n",entityType);   
      //fprintf(stderr, "nstr seiet %i\n",stat.ents[i].et); 
      //fprintf(stderr, "wizzz %i, %i  %i\n",WIZARD, OPPONENT_WIZARD,SNAFFLE); 
      //fill lists
      if(stat.ents[i].et == WIZARD)
	stat.list_wiz[num_wiz++] = i;
	    
      if(stat.ents[i].et == OPPONENT_WIZARD)
	stat.list_op[num_op++] = i;

      if(stat.ents[i].et == SNAFFLE)	    
	stat.list_sna[stat.num_sna++] = i;

      //fprintf(stderr, "num s,a %i\n",stat.num_sna);
    }
    
    int entity_id_mask = -1;
    for (int i = 0; i < 2; i++)
      {

	// Write an action using printf(). DON'T FORGET THE TRAILING \n
	// To debug: fprintf(stderr, "Debug messages...\n");
	    
	    
	// Edit this line to indicate the action for each wizard (0 ≤ thrust ≤ 150, 0 ≤ power ≤ 500)
	// i.e.: "MOVE x y thrust" or "THROW x y power"

	entity_t wiz = get_sor(i,&stat);
	
	//if j'ai un sna
	if(wiz.state == 1)
	  {
	    if(stat.myTeamId == 0)
	      printf("THROW 16000 3700 500\n");
	    else
	      printf("THROW 0 3700 500\n");  
		
	  }
	else
	  {
	
	//find the closest
	
	
	fprintf(stderr, "wiz %i\n",wiz.entityId);
	entity_t closest_sna =  get_closest_sna(&wiz,&stat,entity_id_mask);
	entity_id_mask = closest_sna.entityId;
	//printf("MOVE 8000 3750 100\n");

    

    coord cn = compute_norm_vect(wiz.c,closest_sna.c);
    
    //fprintf(stderr,"norm %f,%f\n",cn.x,cn.y);
    cn.x = cn.x*(float)150;
    cn.y = cn.y*(float)150;
    
    float vx_n = wiz.vx + cn.x;
    float vy_n = wiz.vy + cn.y;
    
    int pred_cord_x = round(wiz.c.x+vx_n);
    int pred_cord_y = round(wiz.c.y+vy_n);
    
    int pred_vx = round(vx_n*0.75);
    int pred_vy = round(vy_n*0.75);
    fprintf(stderr,"predicted : %i,%i\n",pred_cord_x,pred_cord_y);
	fprintf(stderr,"predicted ve: %i,%i\n",pred_vx,pred_vy);
	
	print_action_move(closest_sna.c,150);
      }
  }
  }

  return 0;
}
