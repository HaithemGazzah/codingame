import sys
import math
from collections import namedtuple
from collections import deque
import random



Enti = namedtuple('Enti', ['id', 'coord', 'type','state','val'])
#Status = namedtuple('Status_map', ['explor', 'num_phan', 'num_bust'])


class Plateform_system:
    def __init__(self,num_agent_,team_id_):
        self.num_agent = num_agent_
        self.team_id = team_id_
        self.agents = {}


    def connect_agent(self,ag_):
        self.agents[ag_.play_id] = ag_

    def broadcast_msg(self,msg):
        for ag in self.agent:
            ag.queue.append(msg)

    def update_entities(self,en_l):
        #first update the status of the bust
        for e in en_l:
            if e.type == self.team_id:
                self.agents[e.id].bust = e
                self.agents[e.id].view_ents['phan'] = []
                self.agents[e.id].view_ents['opo'] = []

        print(self.agents)
        #now vievable object
        for e in en_l:
            if e.type == self.team_id: continue
            for id_ag in self.agents:
                ag = self.agents[id_ag]
                if dist(e.coord,ag.bust.coord) <= 2200:#viewable
                    if e.type == -1:
                        ag.view_ents['phan'].append(e)
                    else:
                        ag.view_ents['opo'].append(e) 
            
class Agent:
    def __init__(self,pf_,play_id_):
        self.pf = pf_
        self.play_id = play_id_
        self.pf.connect_agent(self)
        self.queue = deque()
        self.view_ents = {'phan': [], 'opo': []}
        self.bust = 0
        
    def broadcast_msg(self,msg):
        self.pf.broadcast(msg)

    def send_msg(self,msg,bot_id):
        bot_id.append(msg)

        
    def get_avail_msg(self):
        return self.queue.pop()



    def print_action(self):
        print("action")

        
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

#class instance
map_ex = Map_explore(my_team_id)


pls = Plateform_system(busters_per_player,my_team_id)
#if team_id = 0 -> player id begins at 0, else begin at play 
agents = [Agent(pls,busters_per_player*my_team_id + x) for x in range(busters_per_player)]


# game loop
base = (0,0)

if my_team_id == 1:
    base = (16000,9000)

    



    
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


    pls.update_entities(enti)
    map_ex.print_map()



    for ag in agents:
        print(ag.bust, ag.view_ents)

    quit()
    #*******************************************
    
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
