#pragma GCC optimize (3)

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <list>
//#include <random>

using namespace std;

template<class T>
class list_optim
{
public:
  list_optim():index(0){}

  inline int size() const {return index;}

  inline void push_back(T e)
  {
    
    if(index >= 20) throw string("heuuu sup 20");
    tab[index++] = e;
  }

  inline const T& get_elem(int i) const
  {
    return tab[i];
  }
private:
  int index;
  T tab[20];
};

struct pair_g
{
  pair_g():ca(-1),cb(-1),rot(-1){}
  pair_g(int ca_,int cb_):ca(ca_),cb(cb_),rot(0){}
  int ca,cb,rot;
};


class planed_block
{
public:
  planed_block(std::istream &cin2)
  {
    for (int i = 0; i < 8; i++) {
      int colorA; // color of the first block
      int colorB; // color of the attached block
      cin2 >> colorA >> colorB; cin2.ignore();
      pair_g tm(colorA,colorB);
      pb[i] = tm;
    }  
  }


  
  void print() const
  {
    for(const pair_g &p:pb)
      {
	std::cerr << "(" << p.ca << ',' << p.cb <<')';
      }
    std::cerr << std::endl;
  }
  //private:
  pair_g pb[8];
};
class grid
{
public:

  grid(std::istream &cin2)
  {
    //contruct from stdin (!)
    for (int i = 0; i < 12; i++)
      {
	string row;
	cin2 >> row; cin2.ignore();
	for(int j=0;j<6;++j)
	  {
	    if(row[j] == '.')
	      g_raw[i][j] = 99;
	    else g_raw[i][j] = row[j]-48;
	  }
      }

    //update top_col
    for(int i=0;i<6;++i)
      {
	top_col[i] = find_top_col_compute(*this,i);
      }
  }


  void evaluate_position(const planed_block& pb,
			 const grid& g,
			 int depth,
			 int *max_score,
			 int *col,
			 int *orientation,
			 int depth_0_col,
			 int depth_0_orient,
			 int max_depth,
			 int max_depth_aleat)  //return une colonne et une orientation pour le 0th truc qui arrive
  {

    //    if(depth == max_depth)
    if(depth == max_depth_aleat)
      return;


  
    int min_i,max_i,min_j,max_j; //big bricolage...

    if(depth < max_depth)
      {
	min_i = 0;
	max_i = 4;
	min_j = 0;
	max_j = 6;
      }
    else
      {
	min_i = rand() % 4;
	max_i = min_i + 1;

	min_j = rand() % 6;
	max_j=min_j + 1;
      }
    
    for(int i=min_i;i<max_i;++i)
      for(int j=min_j;j<max_j;++j)
	{
	  if(j == 5 && i == 0) continue ;
	  if(j == 0 && i == 2) continue; //bad orientation / line

	  if(i == 0 && (find_top_col(g,j) < 0 || find_top_col(g,j + 1) < 0)) continue;
	  if(i == 2 && (find_top_col(g,j) < 0 || find_top_col(g,j - 1) < 0)) continue;
	  if((i == 1 || i == 3) && (find_top_col(g,j) < 1)) continue;
			
	  grid step_grid = g;
	  int score;

	  if(depth == 0)
	    {
	      depth_0_col = j;
	      depth_0_orient = i;
	      
	    }

	  //  	  cerr << " GRID AVANT"<< endl;
	  //step_grid.print_grid();
	  
	  compute_configuration_score_and_grid(pb.pb[depth],
					       i,
					       j,
					       step_grid,
					       &score);
	  //  cerr << " GRID APRES"<< endl;
	  //step_grid.print_grid();
	  
	  //  cerr << " SCORE " << score << " " << pb.pb[depth].ca << ", " << pb.pb[depth].cb << " col " << j << " or " << i << endl;
	  

	  if(score > *max_score)
	    {
	      *max_score = score;
	      *col = depth_0_col;
	      *orientation = depth_0_orient;

	    }

	  evaluate_position(pb,
			    step_grid,
			    depth + 1,
			    max_score,
			    col,
			    orientation,
			    depth_0_col,
			    depth_0_orient,
			    max_depth,
			    max_depth_aleat);
	}
  }
  
  

  
  void compute_configuration_score_and_grid(const pair_g& pg,int orientation,int col,grid &grid_tmp,int *score) const
  {

    //grid grid_tmp = *this;

    int top,topA,topB;
    *score = 0;

    switch(orientation)
      {
      case 0:
	if(col == 5) throw logic_error("col 5, orient 0 !!");

	topA = find_top_col(grid_tmp,col + 1);
	topB = find_top_col(grid_tmp,col);


	  
	grid_tmp.g_raw[topA][col+1] = pg.cb;
	grid_tmp.g_raw[topB][col] = pg.ca;


	--(grid_tmp.top_col[col+1]);
	--(grid_tmp.top_col[col]);

	*score += min(grid_tmp.top_col[col],grid_tmp.top_col[col+1]);

	
	break;
      
      case 1:
	top = find_top_col(grid_tmp,col);
      
	grid_tmp.g_raw[top][col] = pg.ca;
	grid_tmp.g_raw[top-1][col] = pg.cb;

	grid_tmp.top_col[col] -= 2;

	*score +=grid_tmp.top_col[col];
	
	break;
      case 2:
	if(col == 0) throw logic_error("col 0, orient 2 !!");

	topA = find_top_col(grid_tmp,col - 1);
	topB = find_top_col(grid_tmp,col);

	grid_tmp.g_raw[topA][col-1] = pg.cb;
	grid_tmp.g_raw[topB][col] = pg.ca;
      
	--(grid_tmp.top_col[col-1]);
	--(grid_tmp.top_col[col]);

	*score += min(grid_tmp.top_col[col],grid_tmp.top_col[col-1]);
	break;
      case 3:
	top = find_top_col(grid_tmp,col);
      
	grid_tmp.g_raw[top][col] = pg.cb;
	grid_tmp.g_raw[top-1][col] = pg.ca;

	grid_tmp.top_col[col] -= 2;
	*score += grid_tmp.top_col[col];
	break;
	
      }

    //  *score *= 100;
    //    cerr << "bef ful scor comp " << endl;
    *score +=  full_score_computation_and_update(grid_tmp);
   
   //grid_tmp.print_grid();
   
    
  }




  struct coord_b
  {
    coord_b(short line_,short col_):line(line_),col(col_){}
    short line,col;
  };
  
  int search_rec(const grid& g,short color,short line,short col, short visited[12][6],int visited_code) const
  {

    if(g.g_raw[col][line] != color || visited[col][line] > 0)
      return 0;
    
    visited[col][line] = visited_code;
    
    
    int nb_block = 1;
    
    if(line - 1 >= 0)
      nb_block += search_rec(g,color,line-1,col,visited,visited_code);
    if(line + 1 <6)
      nb_block += search_rec(g,color,line+1,col,visited,visited_code);
    if(col + 1 < 12)
      nb_block += search_rec(g,color,line,col+1,visited,visited_code);
    if(col - 1 >= 0)
      nb_block += search_rec(g,color,line,col-1,visited,visited_code);

    
    return nb_block;
  }

 
  int full_score_computation_and_update(grid &g) const
  {
    bool found;
    int chain_number = 0;
    int global_score = 0;

    do
      {
	int step_score;


	found = search_and_update_one_step(g,chain_number,&step_score);
	//cerr << "bef seaiuons " << chain_number << endl;
	global_score += step_score;
	++chain_number;
      } while (found);


    return global_score;
  }



 

  struct group_info
  {
    group_info(short nb,short c,short vc):num_block(nb),color(c),visited_code(vc){}

     group_info():num_block(0),color(0),visited_code(0){}
    
    int compute_bonus_group() const
    {
      if(num_block <= 10)
	return num_block - 4;
      else
	return 8;
    }
    
    short int num_block;
    short int color;
    short int visited_code;
  };  


  
  bool search_and_update_one_step(grid &g,int step_number,int *score)  const //true if a group is found
  {
    //    std::list<group_info> lgi;
    list_optim<group_info> lgi;
    short visited[12][6] = {0};
    int visited_code = 0; //unique code for each visited
    
    for(int i=0;i<12;++i)
      for(int j=0;j<6;++j)
	{
	  if(g.g_raw[i][j] == 99 || visited[i][j] > 0 || g.g_raw[i][j]==0) continue;
	  
	  short color = g.g_raw[i][j];
	  
	  int block_found = search_rec(g,color,j,i,visited,++visited_code);

	  //cerr << "bfound " << block_found << "color : " << color <<  endl;
	  if(block_found >= 4) //ok on a trouvé un group a destroy
	    {
	      lgi.push_back(group_info(block_found,color,visited_code));
	      //   cerr << "block found ! " << endl;
	      //on destroy aussi les element a 0 a coté
	      
	      
	    }
	  
	}

    //ok maintenant faut degager tous les visited qui on un visited code dans la liste
    if(lgi.size() > 0) //on a found au moin un groupe, donc on met a jour la grid et on compute le score
      {
	for(int i=0;i<12;++i)
	  for(int j=0;j<6;++j)
	    {
	      //for(const group_info& gi:lgi)
	      for(int x=0;x<lgi.size();++x)
		{
		  const group_info &gi = lgi.get_elem(x);
		  if(visited[i][j] == gi.visited_code)
		    {
		      if(i+1 < 12 && g.g_raw[i+1][j] == 0)  visited[i+1][j] = gi.visited_code;
		      if(j+1 < 6 && g.g_raw[i][j+1] == 0)  visited[i][j+1] = gi.visited_code;
		      //cerr << "on erase " << i << " , " << j << endl;
		      erase_one_elt(g,i,j);
		      break;
		    }
		}
	    }

	//et on compute le score de cette etape !

	//nb block
	
	int nb_block=0;
	int nb_diff_color = 0;
	int bonus_group = 0;
	const int COL_MAX = 10;
	int color[COL_MAX] = {0}; //10 color max ?
	//	for(const group_info&gi:lgi)
	  for(int x=0;x<lgi.size();++x)
	  {
	    const group_info &gi = lgi.get_elem(x);
	    nb_block += gi.num_block;
	    color[gi.color] = 1;
	    bonus_group += gi.compute_bonus_group();
	  }

	for(int i:color)
	  nb_diff_color += i;

	int bonus_color;
	
	//compute bonus color
	switch(nb_diff_color)
	  {
	  case 1:
	    bonus_color = 0;
	    break;

	  case 2:
	    bonus_color = 2;
	    break;

	  case 3:
	    bonus_color = 4;
	    break;

	  case 4:
	    bonus_color = 8;
	    break;
	    
	  case 5:
	    bonus_color = 16;
	    break;

	  default:
	    throw string("heuu, bonus color");
	
	  }

	//--- chain power
	//	cerr << " nb vblock " << nb_block << endl;

	int chain_power = 0;
	if(step_number > 0) chain_power = pow(2,step_number + 2);


	int score_partial = chain_power + bonus_color + bonus_group;
	if(score_partial > 999)  score_partial = 999;
	if(score_partial == 0) score_partial = 1;
	
	//cerr << "part " << score_partial << endl;
	
	*score = 10*nb_block * (score_partial);


	//== ******compmute some bonnus and malus heuristique

	return true;
      }
    else //on a rien trouver, pas de score et false
      {
	*score = 0;
	return false;
      }

    
  }

  
  void erase_one_elt(grid& g,short col,short line) const //on erase l'element qui est en coord c
  {
    for(int i=col;i>0;--i)
      {
	g.g_raw[i][line] = g.g_raw[i-1][line];
	g.g_raw[i-1][line] = 99;
	--(g.top_col[line]);
      }
  }

  int find_top_col(const grid &g,int col) const
  {
    return g.top_col[col];
  }
  
  int find_top_col_compute(const grid &g,int col) const //if coll is full, -1 is returned, if not, the col is turned
  {
    for(int i=0;i<12;++i)
      {
	if(g.g_raw[i][col] != 99)
	  {
	    if(i == 0)
	      return -1; //this col is full !!
	    else
	      return i - 1;
	  }
      }
    return 11; //coll is empty
  }
  
  void print_grid()
  {
    for(int i=0;i<12;++i)
      {
	for(int j=0;j<6;++j)
	  {
	    if(g_raw[i][j] == 99)
	      std::cerr << '.';
	    else
	      std::cerr << g_raw[i][j];
	  }
	std::cerr << std::endl;
      }
  }
private:
  short int g_raw[12][6];
  short int top_col[6];
};
/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{
  srand (time(NULL));
    
  int rot=0;
  // game loop
  while (1) {                      
    planed_block pbl(cin);
    grid g_m(cin);
    grid g_adv(cin);
  

    /*  cerr << " ******************  print " << endl;
    g_m.print_grid();

    int score;
    g_m.search_and_update_one_step(g_m,0,&score);

    cerr << "score : " << score << endl;

    cerr << " ******************  print 2222222222 " << endl;
    g_m.print_grid();
    //pbl.print();


    return 0;*/
   int orien = (int)((rot++)%4);
    //g_m.create_new_grid(pbl.pb[0],orien,2);


    int max_score,col,orientation;
    max_score = 0;
    col = 0;
    orientation = 0;

    //-- eval adversaire

    
    g_adv.evaluate_position(pbl,
			  g_adv,
			  0,
			  &max_score,
			  &col,
			  &orientation,
			  0,
			  0,
			  1,
			  1);

    int max_d = 4;
    int max_d_al = 8;

    cerr << "ene skull " << (float)max_score / (float)70 << endl;
	
    if((float)max_score / (float)70 >=6)
      {
	//on contre
	max_d = 1;
	max_d_al = 1;
      }
    
    max_score = 0;
    g_m.evaluate_position(pbl,
			  g_m,
			  0,
			  &max_score,
			  &col,
			  &orientation,
			  0,
			  0,
			  max_d,
			  max_d_al);
    
    cerr << "col " << col << " or " << orientation << " for value " << max_score << endl;
    // Write an action using cout. DON'T FORGET THE "<< endl"
    // To debug: cerr << "Debug messages..." << endl;

    cout << col << " " << orientation << endl;

    // "x rotation": the column in which to drop your pair of blocks folowed by its rotation (0, 1, 2 or 4)
    //    cout << "2 " << orien << endl;
    return 1;
  }
}
