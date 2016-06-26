import sys
import math
from collections import namedtuple
import random



Enti = namedtuple('Enti', ['id', 'coord', 'type','state','val'])
#Status = namedtuple('Status_map', ['explor', 'num_phan', 'num_bust'])


class Map_explore:
    def __init__(self,team_id):
        self.map =   [{'explor':0,'num_phan':0,'num_bust':0}  for x in range(8*5)]
        if team_id == 0:
            self.map[0] = {'explor':1,'num_phan':0,'num_bust':0} 
        #    self.map[1] = {'explor':1,'num_phan':0,'num_bust':0} 
        #    self.map[16] = {'explor':1,'num_phan':0,'num_bust':0} 
            
        else:
            self.map[7*5] = {'explor':1,'num_phan':0,'num_bust':0} 
        #    self.map[15*8-1] = {'explor':1,'num_phan':0,'num_bust':0} 
        #    self.map[14*8] = {'explor':1,'num_phan':0,'num_bust':0}
            
    def print_map(self):
        for y in range(5):
            for x in range(8):
               elt = self.map[y*8+x]
               print('E',elt['explor'],'N',elt['num_phan'], end=";", file=sys.stderr)
            print("", file=sys.stderr)
            
    def coord2zone(self,c):
        x = int(c[0]/2001)  #round to lower int
        y = int(c[1]/2001)

        return self.map[y*8+x]


    def default_coord(self,c):
        #1. find the closest zone with phantom
        #2. find the closest unexplored zone
        
        min_empt = 999999
        zon_empt = -1
        min_phan = 999999
        zon_phan = -1
        
        for x in range(8):
            for y in range(5):
                zone_coord = (x*2001+1000,y*2001+1000)
                distance = dist(c,zone_coord)
                if self.coord2zone(zone_coord)['num_phan'] > 0 and distance < min_phan:
                   min_phan = distance
                   zon_phan = zone_coord
                elif self.coord2zone(zone_coord)['explor'] == 0 and distance < min_empt:
                    min_empt = distance
                    zon_empt = zone_coord
                   
        #if zon_phan != -1: return zon_phan
        if zon_empt != -1: return zon_empt
        return (0,0)
        #raise Exception('Heuuuuu')
        
        
def move_v(c):
    return "MOVE " + str(c[0]) + " " + str(c[1])


def dist(c1,c2):
    return int(math.sqrt((c1[0]-c2[0])**2 + (c1[1]-c2[1])**2))

# Send your busters out into the fog to trap ghosts and bring them home!

busters_per_player = int(input())  # the amount of busters you control
ghost_count = int(input())  # the amount of ghosts on the map
my_team_id = int(input())  # if this is 0, your base is on the top left of the map, if it is one, on the bottom right

# game loop
base = (0,0)
eval_points = [(16000,0),(16000,2000),(16000,4000),(16000,6000),(16000,8000),(14000,9000),(11000,9000),(9000,9000),(6000,9000),(2000,9000)]

if my_team_id == 1:
    base = (16000,9000)
    eval_points = [(0,0),(0,2000),(0,4000),(0,6000),(0,8000),(14000,0),(11000,0),(9000,0),(6000,0),(2000,0)]

map_ex = Map_explore(my_team_id)


    
while True:
    entities = int(input())  # the number of busters and ghosts visible to you
    enti = []
    
    for i in range(entities):
        # entity_id: buster id or ghost id
        # y: position of this buster / ghost
        # entity_type: the team id if it is a buster, -1 if it is a ghost.
        # state: For busters: 0=idle, 1=carrying a ghost.
        # value: For busters: Ghost id being carried. For ghosts: number of busters attempting to trap this ghost.
        entity_id, x, y, entity_type, state, value = [int(j) for j in input().split()]
        enti.append(Enti(entity_id, (x, y), entity_type, state, value))

        #update zone
        if entity_type == -1:
            map_ex.coord2zone((x,y))['num_phan'] += 1
        if entity_type == my_team_id: #mon bus
            map_ex.coord2zone((x,y))['explor'] = 1 #on a explore

    map_ex.print_map()
            
    #find the closer buster
    closer_ph = {}
    for eb in enti:
        #print("tt ",eb, file=sys.stderr)
        if eb.type == -1: #my_team_id  ok b
            min = 1000000
            closest_id = -1
            for ep in enti:
               if ep.type == my_team_id: #-1:
                   if dist(ep.coord,eb.coord) < min:
                       closest_id = ep.id
                       min = dist(ep.coord,eb.coord) 
            if closest_id != -1: closer_ph[closest_id] = eb
           
    #for to in closer_ph:
    #   print(to, closer_ph[to], file=sys.stderr)
        
    for en in enti:
        if en.type != my_team_id: continue
        if en.state == 1: #on a un phantome
            if dist(en.coord,base) < 1600:
                print("RELEASE")
            else:
                print("MOVE " + str(base[0]) + " " + str(base[1]))
        elif en.id in closer_ph:
            #on il voit un phantome
            dist_b_p = dist(en.coord,closer_ph[en.id].coord)
            if dist_b_p < 1760 and dist_b_p > 900:
                #on prend le phanto mais avant on met a jour la map
                map_ex.coord2zone(closer_ph[en.id].coord)['num_phan'] -= 1
                
                
                print("BUST " + str(closer_ph[en.id].id))
            else:
                #on sapproche
                print("MOVE " + str(closer_ph[en.id].coord[0]) + " " + str(closer_ph[en.id].coord[1]))
                
            print(dist_b_p,file=sys.stderr)

            
        else:    
            # Write an action using print
            # To debug: print("Debug messages...", file=sys.stderr)

        # MOVE x y | BUST id | RELEASE
            #print(move_v(random.choice(eval_points)))
            mc = map_ex.default_coord(en.coord)
            print(move_v(mc)) 
